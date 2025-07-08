/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "ble_cts_cent.h"
#include "services/cts/ble_svc_cts.h"
//task
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//fede
#include "host/ble_uuid.h"
#include "uuids.h"
//led
#include "driver/gpio.h"
//gattc
#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/util/util.h"

#include "host/ble_gatt.h"

#define BLE_TAG "ble"
#define MAIN_TAG "main"


#define LED_GPIO GPIO_NUM_48  // Use your chosen pin

static void turnLedOn(bool status){
        // Configure the pin as output
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);

    // Turn on LED (depending on circuit: HIGH = ON or LOW = ON)
    gpio_set_level(LED_GPIO, status);  // Set to 0 or 1 depending on wiring
}


static const char *tag = "NimBLE_CTS_CENT";
static int ble_cts_cent_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t peer_addr[6];

static char *day_of_week[7] = {
    "Unknown"
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};
void ble_store_config_init(void);
static void ble_cts_cent_scan(void);

// Prints the current time from the CTS structure
void printtime(struct ble_svc_cts_curr_time ctime) 
{
    ESP_LOGI(BLE_TAG, "Date : %d/%d/%d %s",
             ctime.et_256.d_d_t.d_t.day,
             ctime.et_256.d_d_t.d_t.month,
             ctime.et_256.d_d_t.d_t.year,
             day_of_week[ctime.et_256.d_d_t.day_of_week]);

    ESP_LOGI(BLE_TAG, "hours : %d minutes : %d",
             ctime.et_256.d_d_t.d_t.hours,
             ctime.et_256.d_d_t.d_t.minutes);

    ESP_LOGI(BLE_TAG, "seconds : %d", ctime.et_256.d_d_t.d_t.seconds);
    ESP_LOGI(BLE_TAG, "fractions : %d", ctime.et_256.fractions_256);
}

// Application callback. Called when the read of the CTS current time characteristic has completed.
static int ble_cts_cent_on_read(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attr,
                                void *arg) 
                                {
    struct ble_svc_cts_curr_time ctime; // store the read time

    ESP_LOGI(BLE_TAG, "Read Current time complete; status=%d conn_handle=%d",
             error->status, conn_handle);

    if (error->status == 0) {
        ESP_LOGI(BLE_TAG, " attr_handle=%d value=", attr->handle);
        print_mbuf(attr->om);
        ESP_LOGI(BLE_TAG, "");
        ble_hs_mbuf_to_flat(attr->om, &ctime, sizeof(ctime), NULL);
        printtime(ctime);
        return 0;
    }

    // Terminate the connection on error
    return ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

// Performs read on the current time characteristic
static int ble_cts_cent_read_time(const struct peer *peer) 
{
    int rc;
    // Subscribe to notifications for the Current Time Characteristic.
    // A central enables notifications by writing two bytes (1, 0) to the characteristic's client-characteristic-configuration-descriptor (CCCD).
    //chr = peer_chr_find_uuid(peer, &uuid_svc, &uuid_chr);
    const struct peer_chr *chr = peer_chr_find_uuid(peer,
        (const ble_uuid_t *)&uuid_svc,
        (const ble_uuid_t *)&uuid_chr);

    if (chr == NULL) {
        ESP_LOGE(BLE_TAG, "Error: Peer doesn't support the CTS characteristic");
        return ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }

    rc = ble_gattc_read(peer->conn_handle, chr->chr.val_handle, ble_cts_cent_on_read, NULL);
    if (rc != 0) {
        ESP_LOGE(BLE_TAG, "Error: Failed to read characteristic; rc=%d", rc);
        return ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }

    return 0;
}

// Called when service discovery of the specified peer has completed.
static void ble_cts_cent_on_disc_complete(const struct peer *peer, int status, void *arg) 
{
    if (status != 0) {
        // Service discovery failed. Terminate the connection.
        ESP_LOGE(BLE_TAG, "Error: Service discovery failed; status=%d conn_handle=%d", status, peer->conn_handle);
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    // Service discovery has completed successfully. 
    // Now we have a complete list of services, characteristics, and descriptors that the peer supports.
    ESP_LOGI(BLE_TAG, "Service discovery complete; status=%d conn_handle=%d", status, peer->conn_handle);
    // Now perform GATT procedure against the peer: read for the cts service.
    ble_cts_cent_read_time(peer);
}

static void ble_cts_cent_scan(void) // Initiates the GAP general discovery procedure.
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;
    int rc;

    // Figure out address to use while advertising (no privacy for now)
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(BLE_TAG, "Error determining address type; rc=%d", rc);
        return;
    }

    // Tell the controller to filter duplicates; we don't want to process repeated advertisements from the same device.
    disc_params.filter_duplicates = 1;
    // Perform a passive scan. I.e., don't send follow-up scan requests to each advertiser.
    disc_params.passive = 1;
    // Use defaults for the rest of the parameters.
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;
    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      ble_cts_cent_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(BLE_TAG, "Error initiating GAP discovery procedure; rc=%d", rc);
    }
}

static const uint8_t target_mac[6] = {0xFA, 0xA1, 0x01, 0x98, 0x07, 0x2A};
// Indicates whether we should try to connect to the sender of the specified advertisement. 
// The function returns a positive result if the device advertises connectability and support for the Current Time Service.
static int ble_cts_cent_should_connect(const struct ble_gap_disc_desc *disc)
{
    ESP_LOGI(BLE_TAG, "Found device MAC: %02X:%02X:%02X:%02X:%02X:%02X",
            disc->addr.val[5], disc->addr.val[4], disc->addr.val[3], disc->addr.val[2], disc->addr.val[1], disc->addr.val[0]);

    // Compare the device address with target MAC
    if (memcmp(disc->addr.val, target_mac, 6) == 0) {
        ESP_LOGI(BLE_TAG, "Should connect!");
        return true;  // connect if MAC matches
    }
    ESP_LOGE(BLE_TAG, "Should not connect!");
    return false;     // otherwise don't connect

/*
    // HID Service UUID (16-bit)
    static const ble_uuid16_t hid_uuid = BLE_UUID16_INIT(0x1812);

    // Iterate through advertisement fields to find service UUIDs
    struct ble_hs_adv_fields fields;
    int rc = ble_hs_adv_parse_fields(&fields, disc->data, disc->length_data);
    if (rc != 0) {
        return false;
    }

    // Check 16-bit service UUIDs
    for (int i = 0; i < fields.num_uuids16; i++) {
        const ble_uuid_t *uuid = &fields.uuids16[i].u;
        if (ble_uuid_cmp(uuid, (const ble_uuid_t *)&hid_uuid) == 0) {
            return true;
        }
    }

    return false;
  */
}


// Connects to the sender of the specified advertisement if it looks interesting. 
// A device is "interesting" if it advertises connectability and support for the Current Time service.
static void ble_cts_cent_connect_if_interesting(void *disc)
{
    uint8_t own_addr_type;
    int rc;
    ble_addr_t *addr;

    // Don't do anything if we don't care about this advertiser.
    if (!ble_cts_cent_should_connect((struct ble_gap_disc_desc *)disc)) {
        return;
    }

    #if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
        // Scanning must be stopped before a connection can be initiated.
        rc = ble_gap_disc_cancel();
        if (rc != 0) {
            ESP_LOGD(BLE_TAG, "Failed to cancel scan; rc=%d", rc);
            return;
        }
    #endif

    // Figure out address to use for connect (no privacy for now)
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(BLE_TAG, "error determining address type; rc=%d", rc);
        return;
    }

    // Try to connect to the advertiser. Allow 30 seconds (30000 ms) timeout.
    addr = &((struct ble_gap_disc_desc *)disc)->addr;

    rc = ble_gap_connect(own_addr_type, addr, 30000, NULL, ble_cts_cent_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(BLE_TAG, "Error: Failed to connect to device; addr_type=%d addr=%s; rc=%d",
                 addr->type, addr_str(addr->val), rc);
        return;
    }
}

int blecent_on_disc_chr(uint16_t conn_handle,
                        const struct ble_gatt_error *error,
                        const struct ble_gatt_chr *chr,
                        void *arg)
{
    ESP_LOGI(BLE_TAG, "blecent_on_disc_chr");
    // This is where you detect the characteristic you care about
    //TODO
    /*if (chr->uuid.u.type == BLE_UUID_TYPE_16 &&
        chr->uuid.u16.value == YOUR_CHAR_UUID) {

        uint16_t char_val_handle = chr->val_handle;
        uint16_t cccd_handle = chr->val_handle + 1;

        // Set up subscription here
        ...
    }*/
    return 0;
}

// The nimble host executes this callback when a GAP event occurs.  
// The application associates a GAP event callback with each connection that is
// established. ble_cts_cent uses the same callback for all connections.
//
// @param event   The event being signalled.
// @param arg     Application-specified argument; unused by ble_cts_cent.
//
// @return 0 if the application successfully handled the event; nonzero on failure.
static int ble_cts_cent_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    struct ble_hs_adv_fields fields;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_DISC");
        rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                     event->disc.length_data);
        if (rc != 0) {
            break;
        }

        // An advertisement report was received during GAP discovery.
        print_adv_fields(&fields);
        // Try to connect to the advertiser if it looks interesting.
        ble_cts_cent_connect_if_interesting(&event->disc);
        break;
    case BLE_GAP_EVENT_LINK_ESTAB:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_LINK_ESTAB");
        // A new connection was established or a connection attempt failed.
        if (event->link_estab.status == 0) {
            // Connection successfully established.
            ESP_LOGI(BLE_TAG, "Connection established!!!!!!!!!!!!!!!!!!!!!!! ");
            turnLedOn(true);

            rc = ble_gap_conn_find(event->link_estab.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);
            ESP_LOGI(BLE_TAG, "\n");

            // Remember peer.
            rc = peer_add(event->link_estab.conn_handle);
            if (rc != 0) {
                ESP_LOGE(BLE_TAG, "Failed to add peer; rc=%d", rc);
                return 0;
            }

            /*#if CONFIG_EXAMPLE_ENCRYPTION
                // Initiate security: Pairing, Bonding, Encryption
                rc = ble_gap_security_initiate(event->link_estab.conn_handle);
                if (rc != 0) {
                    ESP_LOGI(BLE_TAG, "Security could not be initiated, rc = %d", rc);
                    return ble_gap_terminate(event->link_estab.conn_handle,
                                            BLE_ERR_REM_USER_CONN_TERM);
                } else {
                    ESP_LOGI(BLE_TAG, "Connection secured");
                }
            #else*/
                // Perform service discovery
                rc = peer_disc_all(event->link_estab.conn_handle,
                                ble_cts_cent_on_disc_complete, NULL);
                if (rc != 0) {
                    ESP_LOGE(BLE_TAG, "Failed to discover services; rc=%d", rc);
                    return 0;
                }
            //#endif
        } else {
            turnLedOn(false);
            // Connection attempt failed; resume scanning.
            ESP_LOGE(BLE_TAG, "Error: Connection failed; status=%d",
                     event->link_estab.status);
            ble_cts_cent_scan();
        }
        break;
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_CONNECT. Connected to peer");

            // ✅ You call this: Start discovering services or characteristics
            ble_gattc_disc_all_chrs(
                event->connect.conn_handle,
                1,                      // start_handle
                0xffff,                 // end_handle
                blecent_on_disc_chr,    // callback for each characteristic
                NULL                    // optional argument
            );
        } else {
            ESP_LOGE(BLE_TAG, "Connection failed: %d", event->connect.status);
        }
        break;        
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_DISCONNECT");
        turnLedOn(true);
        ESP_LOGI(BLE_TAG, "disconnect; reason=%d ", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);
        ESP_LOGI(BLE_TAG, "\n");
        peer_delete(event->disconnect.conn.conn_handle); // Forget about peer.
        ble_cts_cent_scan(); // Resume scanning.q
        break;
    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_DISC_COMPLETE discovery complete; reason=%d", event->disc_complete.reason);
        break;
    case BLE_GAP_EVENT_ENC_CHANGE:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_ENC_CHANGE");
        // Encryption has been enabled or disabled for this connection.
        ESP_LOGI(BLE_TAG, "encryption change event; status=%d",
                 event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        break;
    case BLE_GAP_EVENT_NOTIFY_RX: // Peer sent us a notification or indication.
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_NOTIFY_RX");
        ESP_LOGI(BLE_TAG, "received %s; conn_handle=%d attr_handle=%d attr_len=%d",
                            event->notify_rx.indication ? "indication" : "notification",
                            event->notify_rx.conn_handle,
                            event->notify_rx.attr_handle,
                            OS_MBUF_PKTLEN(event->notify_rx.om));
        // Attribute data is contained in event->notify_rx.om. Use `os_mbuf_copydata` to copy the data received in notification mbuf.
        break;
    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_MTU");
        ESP_LOGI(BLE_TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle,
                 event->mtu.channel_id,
                 event->mtu.value);
        break;
    default:
        break;
    }
    return 0;
}

static void ble_cts_cent_on_reset(int reason)
{
    ESP_LOGE(BLE_TAG, "Resetting state; reason=%d", reason);
}

static void ble_cts_cent_on_sync(void)
{
    int rc;
    // Make sure we have proper identity address set (public preferred)
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);
    ble_cts_cent_scan(); // Begin scanning for a peripheral to connect to.
}

void ble_cts_cent_host_task(void *param)
{
    ESP_LOGI(BLE_TAG, "BLE Host Task Started");
    // This function will return only when nimble_port_stop() is executed
    nimble_port_run();
    nimble_port_freertos_deinit();
}

static int on_notify_cb(uint16_t conn_handle,
                        const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr,
                        void *arg)
{
    if (error->status != 0) {
        ESP_LOGE(BLE_TAG, "Notification error: %d", error->status);
        return 0;
    }

    ESP_LOGD(BLE_TAG, "**********Notification received: handle=%d, len=%d", attr->handle, attr->om->om_len);
    uint8_t *data = attr->om->om_data;
    for (int i = 0; i < attr->om->om_len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");

    return 0;
}

static int blecent_on_disc_chr(const struct ble_gatt_error *error,
                                uint16_t conn_handle,
                                const struct ble_gatt_chr *chr,
                                void *arg)
{
    if (error->status != 0) {
        ESP_LOGE(BLE_TAG, "Characteristic discovery failed: %d", error->status);
        return 0;
    }

    ESP_LOGI(BLE_TAG, "Characteristic discovered: val_handle=%d", chr->val_handle);

    // Store handles
    uint16_t char_val_handle = chr->val_handle;
    uint16_t cccd_handle = chr->val_handle + 1; // assuming CCCD is immediately after

    /*
    // Match your target UUID here
    if (chr->uuid.u.type == BLE_UUID_TYPE_16 &&
        chr->uuid.u16.value == YOUR_CHAR_UUID) {

        ESP_LOGI(BLE_TAG, "Found desired characteristic");

        // Store value handle
        your_val_handle = chr->val_handle;

        // OPTIONAL: If you've already discovered descriptors, get CCCD handle here
        your_cccd_handle = find_cccd_handle_somehow();  // OR just use val_handle + 1 as a guess

        static struct ble_gatt_subscribe_params sub_params;
        memset(&sub_params, 0, sizeof(sub_params));
        sub_params.notify = on_notify_cb;
        sub_params.value_handle = your_val_handle;
        sub_params.cccd_handle = your_cccd_handle;

        int rc = ble_gattc_subscribe(conn_handle, &sub_params);
        if (rc != 0) {
            ESP_LOGE(BLE_TAG, "Subscription failed: %d", rc);
        } else {
            ESP_LOGI(BLE_TAG, "Subscribed to notifications");
        }
    }    
    */

    return 0;
}

//----------------------------------------------------------------------------------

void task_main(void *pvParameters) 
{
    esp_log_level_set(BLE_TAG, ESP_LOG_DEBUG); 
    ESP_LOGI(MAIN_TAG, "Entering main");
    int rc;
    // Initialize NVS — it is used to store PHY calibration data
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(BLE_TAG, "Failed to init nimble %d", ret);
        return;
    }

    // Configure the host.
    ble_hs_cfg.reset_cb = ble_cts_cent_on_reset;
    ble_hs_cfg.sync_cb = ble_cts_cent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    // Initialize data structures to track connected peers.
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);

    // Set the default device name.
    rc = ble_svc_gap_device_name_set("nimble-cts-cent");
    assert(rc == 0);

    // XXX Need to have template for store
    // TODO
    //ble_store_config_init(); // TODO: add this back

    nimble_port_freertos_init(ble_cts_cent_host_task);

    // Main loop with 1 second delay
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1000 ms (1 second)
    }
    ESP_LOGE(MAIN_TAG, "Leaving main");
}

extern "C" void app_main(void) 
{
    xTaskCreate(task_main, "task_main", 4096, NULL, 5, NULL);
}
