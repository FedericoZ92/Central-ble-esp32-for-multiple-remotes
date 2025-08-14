#pragma once

#include "host/ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initializes BLE CTS central logic and NimBLE callbacks. 
void ble_cts_cent_init(void);

// BLE host task (runs nimble_port_run). 
void ble_cts_cent_host_task(void *param);

// GAP event handler for NimBLE. 
int ble_cts_cent_gap_event(struct ble_gap_event *event, void *arg);

// Start scanning for devices. 
void ble_cts_cent_scan(void);

void ble_cts_cent_on_reset(int reason);

void ble_cts_cent_on_sync(void);

#ifdef __cplusplus
}
#endif