
// ble_gatt_demo.h generated from ble_gatt_demo.gatt for BTstack
// it needs to be regenerated when the .gatt file is updated.

// To generate ble_gatt_demo.h:
// tool/compile_gatt.py ble_gatt_demo.gatt ble_gatt_demo.h

// att db format version 1

// binary attribute representation:
// - size in bytes (16), flags(16), handle (16), uuid (16/128), value(...)

#include <stdint.h>

// Reference: https://en.cppreference.com/w/cpp/feature_test
#if __cplusplus >= 200704L
constexpr
#endif
uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // [0] add GATT Service
    // 0x0001 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x01, 0x18, 
    // 0x0002 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_GATT_SERVICE_CHANGED-INDICATE
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x20, 0x03, 0x00, 0x05, 0x2a, 
    // 0x0003 VALUE-ORG_BLUETOOTH_CHARACTERISTIC_GATT_SERVICE_CHANGED-INDICATE-''
    // 
    0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x2a, 
    // 0x0004 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x04, 0x00, 0x02, 0x29, 0x00, 0x00, 
    // [1] add GAP Service

    // 0x0005 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0006 CHARACTERISTIC-GAP_DEVICE_NAME-READ |
    0x0d, 0x00, 0x02, 0x00, 0x06, 0x00, 0x03, 0x28, 0x02, 0x07, 0x00, 0x00, 0x2a, 
    // 0x0007 VALUE-GAP_DEVICE_NAME-READ |-'SC BLE DEMO'
    // READ_ANYBODY
    0x13, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x2a, 0x53, 0x43, 0x20, 0x42, 0x4c, 0x45, 0x20, 0x44, 0x45, 0x4d, 0x4f, 
    // 0x0008 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_GAP_APPEARANCE-READ
    0x0d, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x02, 0x09, 0x00, 0x01, 0x2a, 
    // 0x0009 VALUE-ORG_BLUETOOTH_CHARACTERISTIC_GAP_APPEARANCE-READ-'C0 03'
    // READ_ANYBODY
    0x0a, 0x00, 0x02, 0x00, 0x09, 0x00, 0x01, 0x2a, 0xC0, 0x03, 
    // [2] add Customed Service
    // Counter Service

    // 0x000a PRIMARY_SERVICE-A2345678-B234-C234-D234-E23456789ABC
    0x18, 0x00, 0x02, 0x00, 0x0a, 0x00, 0x00, 0x28, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0xe2, 0x34, 0xd2, 0x34, 0xc2, 0x34, 0xb2, 0x78, 0x56, 0x34, 0xa2, 
    // Counter Characteristic, with read and write and notify
    // 0x000b CHARACTERISTIC-AFAEADAC-ABAA-A9A8-A7A6-A5A4A3A2A1A0-READ | WRITE | NOTIFY | DYNAMIC | INDICATE
    0x1b, 0x00, 0x02, 0x00, 0x0b, 0x00, 0x03, 0x28, 0x3a, 0x0c, 0x00, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 
    // 0x000c VALUE-AFAEADAC-ABAA-A9A8-A7A6-A5A4A3A2A1A0-READ | WRITE | NOTIFY | DYNAMIC | INDICATE-''
    // READ_ANYBODY, WRITE_ANYBODY
    0x16, 0x00, 0x0a, 0x03, 0x0c, 0x00, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 
    // 0x000d CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x0d, 0x00, 0x02, 0x29, 0x00, 0x00, 
    // 0x000e CHARACTERISTIC-12345678-1234-5678-ABCD-BA9876543210-READ | WRITE | NOTIFY | DYNAMIC | INDICATE
    0x1b, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x03, 0x28, 0x3a, 0x0f, 0x00, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xcd, 0xab, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 
    // 0x000f VALUE-12345678-1234-5678-ABCD-BA9876543210-READ | WRITE | NOTIFY | DYNAMIC | INDICATE-''
    // READ_ANYBODY, WRITE_ANYBODY
    0x16, 0x00, 0x0a, 0x03, 0x0f, 0x00, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xcd, 0xab, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 
    // 0x0010 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x10, 0x00, 0x02, 0x29, 0x00, 0x00, 
    // 0x0011 CHARACTERISTIC-8901-READ | WRITE | NOTIFY | DYNAMIC | INDICATE
    0x0d, 0x00, 0x02, 0x00, 0x11, 0x00, 0x03, 0x28, 0x3a, 0x12, 0x00, 0x01, 0x89, 
    // 0x0012 VALUE-8901-READ | WRITE | NOTIFY | DYNAMIC | INDICATE-''
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x0a, 0x01, 0x12, 0x00, 0x01, 0x89, 
    // 0x0013 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x13, 0x00, 0x02, 0x29, 0x00, 0x00, 

    // END
    0x00, 0x00, 
}; // total size 152 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GATT_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GATT_SERVICE_END_HANDLE 0x0004
#define ATT_SERVICE_GATT_SERVICE_01_START_HANDLE 0x0001
#define ATT_SERVICE_GATT_SERVICE_01_END_HANDLE 0x0004
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0005
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0009
#define ATT_SERVICE_GAP_SERVICE_01_START_HANDLE 0x0005
#define ATT_SERVICE_GAP_SERVICE_01_END_HANDLE 0x0009
#define ATT_SERVICE_A2345678_B234_C234_D234_E23456789ABC_START_HANDLE 0x000a
#define ATT_SERVICE_A2345678_B234_C234_D234_E23456789ABC_END_HANDLE 0x0013
#define ATT_SERVICE_A2345678_B234_C234_D234_E23456789ABC_01_START_HANDLE 0x000a
#define ATT_SERVICE_A2345678_B234_C234_D234_E23456789ABC_01_END_HANDLE 0x0013

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_GATT_SERVICE_CHANGED_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_GATT_SERVICE_CHANGED_01_CLIENT_CONFIGURATION_HANDLE 0x0004
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0007
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_GAP_APPEARANCE_01_VALUE_HANDLE 0x0009
#define ATT_CHARACTERISTIC_AFAEADAC_ABAA_A9A8_A7A6_A5A4A3A2A1A0_01_VALUE_HANDLE 0x000c
#define ATT_CHARACTERISTIC_AFAEADAC_ABAA_A9A8_A7A6_A5A4A3A2A1A0_01_CLIENT_CONFIGURATION_HANDLE 0x000d
#define ATT_CHARACTERISTIC_12345678_1234_5678_ABCD_BA9876543210_01_VALUE_HANDLE 0x000f
#define ATT_CHARACTERISTIC_12345678_1234_5678_ABCD_BA9876543210_01_CLIENT_CONFIGURATION_HANDLE 0x0010
#define ATT_CHARACTERISTIC_8901_01_VALUE_HANDLE 0x0012
#define ATT_CHARACTERISTIC_8901_01_CLIENT_CONFIGURATION_HANDLE 0x0013
