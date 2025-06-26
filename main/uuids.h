
//fede

//#include "nimble/ble.h"
#include "host/ble_uuid.h"

//static const ble_uuid_t uuid_svc = BLE_UUID16_INIT(BLE_SVC_CTS_UUID16);
//static const ble_uuid_t uuid_chr = BLE_UUID16_INIT(BLE_SVC_CTS_CHR_UUID16_CURRENT_TIME);

static const ble_uuid16_t uuid_svc =
    BLE_UUID16_INIT(BLE_SVC_CTS_UUID16);
static const ble_uuid16_t uuid_chr =
    BLE_UUID16_INIT(BLE_SVC_CTS_CHR_UUID16_CURRENT_TIME);
