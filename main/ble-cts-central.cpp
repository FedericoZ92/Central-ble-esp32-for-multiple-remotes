#include "ble-cts-central.h"
#include "util.h"

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
//led
#include "driver/gpio.h"
//gattc
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
//serial
 #include "serial-module.h"


#define LED_GPIO GPIO_NUM_48  // Use your chosen pin

static const ble_uuid16_t uuid_svc = BLE_UUID16_INIT(BLE_SVC_CTS_UUID16);
static const ble_uuid16_t uuid_chr = BLE_UUID16_INIT(BLE_SVC_CTS_CHR_UUID16_CURRENT_TIME);

static void turnLedOn(bool status){
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
    gpio_set_level(LED_GPIO, status);
}

static const char *tag = "NimBLE_CTS_CENT";
int ble_cts_cent_gap_event(struct ble_gap_event *event, void *arg);

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
void ble_cts_cent_scan(void);

void printtime(struct ble_svc_cts_curr_time ctime) {
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

int ble_cts_cent_on_read(uint16_t conn_handle,
                        const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr,
                        void *arg) 
{
    struct ble_svc_cts_curr_time ctime;
    ESP_LOGI(BLE_TAG, "Read Current time complete; status=%d conn_handle=%d", error->status, conn_handle);
    if (error->status == 0) {
        ESP_LOGI(BLE_TAG, " attr_handle=%d value=", attr->handle);
        print_mbuf(attr->om);
        ESP_LOGI(BLE_TAG, "");
        ble_hs_mbuf_to_flat(attr->om, &ctime, sizeof(ctime), NULL);
        printtime(ctime);
        return 0;
    }
    return ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

int ble_cts_cent_read_time(const struct peer *peer) 
{
    int rc;
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

int on_cccd_written(uint16_t conn_handle,
                    const struct ble_gatt_error *error,
                    struct ble_gatt_attr *attr,
                    void *arg) 
{
    if (error->status == 0) {
        ESP_LOGI(BLE_TAG, "Notifications enabled via CCCD");
    } else {
        ESP_LOGE(BLE_TAG, "Failed to write CCCD; status=%d", error->status);
    }
    return 0;
}

#define MAX_CONNECTIONS 4
#define MAX_NOTIFY_CHARS_PER_CONN 5
#define MAX_PEERS 4
static ble_addr_t connected_peers[MAX_PEERS];
static int num_connected_peers = 0;

struct notify_sub {
    uint16_t val_handle;
    uint16_t cccd_handle;
};

struct conn_notify_subs {
    uint16_t conn_handle;
    int notify_sub_count;
    struct notify_sub notify_subs[MAX_NOTIFY_CHARS_PER_CONN];
};

struct conn_notify_subs conn_notify_array[MAX_CONNECTIONS];

struct conn_notify_subs *get_conn_notify(uint16_t conn_handle) 
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (conn_notify_array[i].conn_handle == conn_handle) {
            return &conn_notify_array[i];
        }
    }
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (conn_notify_array[i].conn_handle == 0) {
            conn_notify_array[i].conn_handle = conn_handle;
            conn_notify_array[i].notify_sub_count = 0;
            return &conn_notify_array[i];
        }
    }
    return NULL;
}

#define BLE_GATT_DESC_CLIENT_CHAR_CFG 0x2902
#define MAX_NOTIFY_CHARS 5

struct notify_sub notify_subs[MAX_NOTIFY_CHARS];
int notify_sub_count = 0;

int on_descriptor_discovered(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            uint16_t chr_val_handle,
                            const struct ble_gatt_dsc *dsc,
                            void *arg) 
{
    int idx = (int)(intptr_t)arg;
    struct conn_notify_subs *conn_notify = get_conn_notify(conn_handle);
    if (!conn_notify) {
        ESP_LOGE(BLE_TAG, "No notify_sub slot for conn_handle %d", conn_handle);
        return 0;
    }
    if (error->status != 0 || dsc == NULL) {
        ESP_LOGI(BLE_TAG, "Descriptor discovery complete for notify_sub %d", idx);
        if (conn_notify->notify_subs[idx].cccd_handle != 0) {
            uint8_t cccd_val[] = {0x01, 0x00};
            int rc = ble_gattc_write_flat(conn_handle,
                                          conn_notify->notify_subs[idx].cccd_handle,
                                          cccd_val,
                                          sizeof(cccd_val),
                                          on_cccd_written,
                                          NULL);
            if (rc != 0) {
                ESP_LOGE(BLE_TAG, "Failed to write CCCD for notify_sub %d rc=%d", idx, rc);
            }
        } else {
            ESP_LOGW(BLE_TAG, "No CCCD handle found for notify_sub %d", idx);
        }
        return 0;
    }
    if (dsc->uuid.u16.value == BLE_GATT_DESC_CLIENT_CHAR_CFG) {
        conn_notify->notify_subs[idx].cccd_handle = dsc->handle;
        ESP_LOGI(BLE_TAG, "Found CCCD handle %d for notify_sub %d", dsc->handle, idx);
    }
    return 0;
}

int on_characteristic_discovered(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                const struct ble_gatt_chr *chr,
                                void *arg) 
{
    if (error->status != 0 || chr == NULL) {
        ESP_LOGI(BLE_TAG, "Characteristic discovery complete");
        struct conn_notify_subs *conn_notify = get_conn_notify(conn_handle);
        if (!conn_notify) {
            ESP_LOGE(BLE_TAG, "No notify_sub slot for conn_handle %d", conn_handle);
            return 0;
        }
        for (int i = 0; i < conn_notify->notify_sub_count; i++) {
            int rc = ble_gattc_disc_all_dscs(conn_handle,
                                             conn_notify->notify_subs[i].val_handle,
                                             conn_notify->notify_subs[i].val_handle + 5,
                                             on_descriptor_discovered,
                                             (void*)(intptr_t)i);
            if (rc != 0) {
                ESP_LOGE(BLE_TAG, "Descriptor discovery failed for handle %d rc=%d",
                         conn_notify->notify_subs[i].val_handle, rc);
            }
        }
        return 0;
    }
    struct conn_notify_subs *conn_notify = get_conn_notify(conn_handle);
    if (!conn_notify) {
        ESP_LOGE(BLE_TAG, "No notify_sub slot for conn_handle %d", conn_handle);
        return 0;
    }
    if ((chr->properties & BLE_GATT_CHR_PROP_NOTIFY) && 
        conn_notify->notify_sub_count < MAX_NOTIFY_CHARS_PER_CONN) {
        int idx = conn_notify->notify_sub_count;
        conn_notify->notify_subs[idx].val_handle = chr->val_handle;
        conn_notify->notify_subs[idx].cccd_handle = 0;
        ESP_LOGI(BLE_TAG, "Notify characteristic found, handle=%d", chr->val_handle);
        conn_notify->notify_sub_count++;
    }
    return 0;
}

int on_service_discovered(  uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            const struct ble_gatt_svc *service,
                            void *arg) 
{
    if (error->status != 0 || service == NULL) {
        ESP_LOGI(BLE_TAG, "Service discovery complete");
        ble_gattc_disc_all_chrs(conn_handle, 1, 0xFFFF, on_characteristic_discovered, NULL);
        return 0;
    }
    char uuid_str[BLE_UUID_STR_LEN];
    ble_uuid_to_str(&service->uuid.u, uuid_str);
    ESP_LOGI(BLE_TAG, "Service found: UUID=%s", uuid_str);
    return 0;
}

void ble_cts_cent_scan(void) 
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;
    int rc;
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(BLE_TAG, "Error determining address type; rc=%d", rc);
        return;
    }
    disc_params.filter_duplicates = 1;
    disc_params.passive = 1;
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

static int ble_cts_cent_should_connect(const struct ble_gap_disc_desc *disc) 
{
    ESP_LOGI(BLE_TAG, "Found device MAC: %02X:%02X:%02X:%02X:%02X:%02X",
            disc->addr.val[5], disc->addr.val[4], disc->addr.val[3], disc->addr.val[2], disc->addr.val[1], disc->addr.val[0]);
    if (memcmp(disc->addr.val+3, target_mac+3, 3) == 0) {
        ESP_LOGI(BLE_TAG, "Should connect!");
        return true;
    }
    ESP_LOGE(BLE_TAG, "Should not connect!");
    return false;
}

bool already_connected(ble_addr_t *addr) 
{
    for (int i = 0; i < num_connected_peers; i++) {
        if (memcmp(connected_peers[i].val, addr->val, 6) == 0 &&
            connected_peers[i].type == addr->type) {
            return true;
        }
    }
    return false;
}

static void ble_cts_cent_connect_if_interesting(void *disc) 
{
    uint8_t own_addr_type;
    int rc;
    ble_addr_t *addr = &((struct ble_gap_disc_desc *)disc)->addr;
    if (!ble_cts_cent_should_connect((struct ble_gap_disc_desc *)disc)) {
        return;
    }
    if (already_connected(addr)) {
        ESP_LOGI(BLE_TAG, "Already connected to device %s; skipping", addr_str(addr->val));
        return;
    }
    #if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
        rc = ble_gap_disc_cancel();
        if (rc != 0) {
            ESP_LOGD(BLE_TAG, "Failed to cancel scan; rc=%d", rc);
            return;
        }
    #endif
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(BLE_TAG, "error determining address type; rc=%d", rc);
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
    rc = ble_gap_connect(own_addr_type, addr, 30000, NULL, ble_cts_cent_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(BLE_TAG, "Failed to connect to device; addr_type=%d addr=%s; rc=%d",
                 addr->type, addr_str(addr->val), rc);
        return;
    }
}

int blecent_on_disc_chr(uint16_t conn_handle,
                        const struct ble_gatt_error *error,
                        const struct ble_gatt_chr *chr,
                        void *arg) {
    ESP_LOGI(BLE_TAG, "blecent_on_disc_chr");
    return 0;
}

int ble_cts_cent_gap_event(struct ble_gap_event *event, void *arg) 
{
    char buffer[64];
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
        print_adv_fields(&fields);
        ble_cts_cent_connect_if_interesting(&event->disc);
        break;
    case BLE_GAP_EVENT_LINK_ESTAB: {
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_LINK_ESTAB");
        if (event->link_estab.status == 0) {
            ESP_LOGI(BLE_TAG, "Connection established!");
            turnLedOn(true);
            rc = ble_gap_conn_find(event->link_estab.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);
            bool found = false;
            for (int i = 0; i < num_connected_peers; i++) {
                if (memcmp(connected_peers[i].val, desc.peer_id_addr.val, 6) == 0 &&
                    connected_peers[i].type == desc.peer_id_addr.type) {
                    found = true;
                    break;
                }
            }
            if (!found && num_connected_peers < MAX_PEERS) {
                connected_peers[num_connected_peers++] = desc.peer_id_addr;
                ESP_LOGI(BLE_TAG, "Peer added to connected_peers list");
            } else if (found) {
                ESP_LOGW(BLE_TAG, "Peer already tracked");
            } else {
                ESP_LOGW(BLE_TAG, "Max peers reached");
            }
            rc = peer_add(event->link_estab.conn_handle);
            if (rc != 0) {
                ESP_LOGE(BLE_TAG, "Failed to add peer; rc=%d", rc);
                return 0;
            }
            rc = ble_gattc_disc_all_svcs(event->link_estab.conn_handle, on_service_discovered, NULL);
            if (rc != 0) {
                ESP_LOGE(BLE_TAG, "Failed to discover services; rc=%d", rc);
                return 0;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            ble_cts_cent_scan();
        } else {
            turnLedOn(false);
            ESP_LOGE(BLE_TAG, "Connection failed; status=%d", event->link_estab.status);
            ble_cts_cent_scan();
        }
        break;
    }
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_CONNECT. Connected to peer");
            ble_gattc_disc_all_chrs(event->connect.conn_handle, 1, 0xffff, blecent_on_disc_chr, NULL);
        } else {
            ESP_LOGE(BLE_TAG, "Connection failed: %d", event->connect.status);
        }
        break;        
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_DISCONNECT");
        turnLedOn(false);
        ESP_LOGI(BLE_TAG, "disconnect; reason=%d ", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);
        for (int i = 0; i < num_connected_peers; i++) {
            if (memcmp(connected_peers[i].val, event->disconnect.conn.peer_id_addr.val, 6) == 0 &&
                connected_peers[i].type == event->disconnect.conn.peer_id_addr.type) {
                for (int j = i; j < num_connected_peers - 1; j++) {
                    connected_peers[j] = connected_peers[j + 1];
                }
                num_connected_peers--;
                ESP_LOGI(BLE_TAG, "Removed disconnected peer");
                break;
            }
        }
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (conn_notify_array[i].conn_handle == event->disconnect.conn.conn_handle) {
                memset(&conn_notify_array[i], 0, sizeof(struct conn_notify_subs));
                ESP_LOGI(BLE_TAG, "Cleared notify subscriptions");
                break;
            }
        }
        // Forget about peer in NimBLE
        peer_delete(event->disconnect.conn.conn_handle);
        // Resume scanning for new devices
        ble_cts_cent_scan();
    break;
    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_DISC_COMPLETE discovery complete; reason=%d", event->disc_complete.reason);
        break;
    case BLE_GAP_EVENT_ENC_CHANGE:
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_ENC_CHANGE");
        // Encryption has been enabled or disabled for this connection.
        ESP_LOGI(BLE_TAG, "encryption change event; status=%d", event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        break;
    case BLE_GAP_EVENT_NOTIFY_RX: // Peer sent us a notification or indication.
        ESP_LOGI(BLE_TAG, "BLE_GAP_EVENT_NOTIFY_RX");
        // Use the connection handle to fetch connection info
        rc = ble_gap_conn_find(event->notify_rx.conn_handle, &desc);
        if (rc == 0) {
            // Format the MAC address into the buffer
            snprintf(buffer, sizeof(buffer),
                     "Received notify from remote mac: %02X:%02X:%02X:%02X:%02X:%02X\n",
                     desc.peer_id_addr.val[5],
                     desc.peer_id_addr.val[4],
                     desc.peer_id_addr.val[3],
                     desc.peer_id_addr.val[2],
                     desc.peer_id_addr.val[1],
                     desc.peer_id_addr.val[0]);
            // Use your custom SerialModule::write()
            SerialLine.write(buffer);
        } else {
            printf("Failed to get connection descriptor\n");
        }
        
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

void ble_cts_cent_on_reset(int reason)
{
    ESP_LOGE(BLE_TAG, "Resetting state; reason=%d", reason);
}

void ble_cts_cent_on_sync(void)
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

    return 0;
}