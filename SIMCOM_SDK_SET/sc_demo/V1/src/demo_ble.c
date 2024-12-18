#ifdef BT_SUPPORT

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "simcom_os.h"
#include "simcom_ble.h"
#include "simcom_common.h"
#include "simcom_debug.h"
#include "simcom_uart.h"
#include "uart_api.h"

//#define SC_EXT_ADV

typedef enum{
    SC_BLE_DEMO_OPEN                        = 1,
    SC_BLE_DEMO_CLOSE                       = 2,
    SC_BLE_DEMO_SET_ADDRESS                 = 3,
    SC_BLE_DEMO_CREATE_ADV_DATA             = 4,
    SC_BLE_DEMO_SET_ADV_DATA                = 5,
    SC_BLE_DEMO_SET_ADV_PARAM               = 6,
    SC_BLE_DEMO_REGISTER_SERVICE            = 7,
    SC_BLE_DEMO_UNREGISTER_SERVICE          = 8,
    SC_BLE_DEMO_REGISTER_EVENT_HANDLE       = 9,
    SC_BLE_DEMO_ENABLE_ADV                  = 10,
    SC_BLE_DEMO_DISABLE_ADV                 = 11,
    SC_BLE_DEMO_DISCONNECT                  = 12,
    SC_BLE_DEMO_INDICATE                    = 13,
    SC_BLE_DEMO_NOTIFY                      = 14,
    SC_BLE_DEMO_SCAN_START                  = 15,
    SC_BLE_DEMO_SCAN_STOP                   = 16,
    SC_BLE_DEMO_GET_ADDRESS                 = 17,
    SC_BLE_DEMO_PAIR_DEVICE                 = 18,
    SC_BLE_DEMO_ENABLE_PAIR                 = 19,
    SC_BLE_DEMO_GET_PAIR_INFO               = 20,
    SC_BLE_DEMO_DEL_PAIR_INFO               = 21,
    SC_BLE_DEMO_CLEAR_PAIR_INFO             = 22,
    SC_BLE_DEMO_GET_RSSI                    = 23,
    SC_BLE_DEMO_DEVICE_NAME                 = 24,
    #ifdef CUS_DRIVER_BT
    SC_BLE_DEMO_DEVICE_CONFIG               = 25,
    #endif
    SC_BLE_MAX                              = 99,
}SC_BLE_DEMO_TYPE;


extern void PrintfOptionMenu(char *options_list[], int array_size);
extern void PrintfResp(const char *format, ...);


#define CHARACTERISTIC_VALUE_LENGTH             512

static char custom_characteristic_value[CHARACTERISTIC_VALUE_LENGTH]     = {"hello world"};
static SC_ATT_CHARA_CCB_EX_T   custom_client_characteristic_config_value        = {0};


SC_UUID_16_T sc_ble_uuid_primary_service =
{
    .type.type = LE_UUID_TYPE_16,
    .value = LE_ATT_UUID_PRIMARY
};

SC_UUID_16_T sc_ble_uuid_client_charc_config =
{
    .type.type = LE_UUID_TYPE_16,
    .value = LE_ATT_UUID_CLIENT_CHARC_CONFIG
};

SC_UUID_16_T sc_ble_uuid_characteristic =
{
    .type.type = LE_UUID_TYPE_16,
    .value = LE_ATT_UUID_CHARC
};

static SC_UUID_16_T custom_service_declare =
{
    .type.type = LE_UUID_TYPE_16,
    .value = 0xFFF0
};

static SC_BLE_CHARACTERISTIC_16_T custom_characteristic_declare =
{
    .uuid = 0xFFF1,
    .properties = LE_ATT_CHARC_PROP_READ | LE_ATT_CHARC_PROP_WWP | LE_ATT_CHARC_PROP_NOTIFY | LE_ATT_CHARC_PROP_INDICATE
};

static SC_UUID_16_T custom_characteristic_attribute_value =
{
    .type.type = LE_UUID_TYPE_16,
    .value = 0xFFF1
};


static int custom_characteristic_write_cb(void *param)
{
    SC_BLE_RW_T *rw = (SC_BLE_RW_T *)param;

    sAPI_Debug("%s: handle %04x, length %d, offset %d\r\n", __func__, rw->handle, rw->length, rw->offset);

    if((rw->offset >= CHARACTERISTIC_VALUE_LENGTH) || (rw->length > CHARACTERISTIC_VALUE_LENGTH))
    {
        sAPI_Debug("Offset specified was past the end of the attribute.\r\n");
        PrintfResp("\r\nOffset specified was past the end of the attribute.\r\n");
        return SC_BLE_ERR_INVALID_OFFSET;
    }

    memset(custom_characteristic_value, 0, CHARACTERISTIC_VALUE_LENGTH);
    memcpy(custom_characteristic_value, rw->data, rw->length);
    sAPI_Debug("%s: receive data:", __func__);
    for(int i=0; i<rw->length; i++)
    {
        sAPI_Debug("%02X", custom_characteristic_value[i]);
    }

    return SC_BLE_ERR_OK;
}

static int custom_characteristic_read_cb(void *param)
{
    SC_BLE_RW_T *rw = (SC_BLE_RW_T *)param;

    sAPI_Debug("%s: handle %d, length %d, offset %d\r\n", __func__, rw->handle, rw->length, rw->offset);

    if((rw->offset >= CHARACTERISTIC_VALUE_LENGTH) || (rw->length > CHARACTERISTIC_VALUE_LENGTH))
    {
        sAPI_Debug("Offset specified was past the end of the attribute.\r\n");
        PrintfResp("\r\nOffset specified was past the end of the attribute.\r\n");
        return SC_BLE_ERR_INVALID_OFFSET;
    }

    unsigned char *p = (unsigned char *)sAPI_Malloc(CHARACTERISTIC_VALUE_LENGTH);
    if(p == NULL)
    {
        sAPI_Debug("%s: malloc fail\r\n", __func__);
        return 0x80;
    }

    memset(p, 0, CHARACTERISTIC_VALUE_LENGTH);
    memcpy(p, custom_characteristic_value, strlen(custom_characteristic_value));
    rw->data = p;

    return strlen(custom_characteristic_value);
}


static int custom_client_char_config_write_cb(void *param)
{
    SC_BLE_RW_T *rw = (SC_BLE_RW_T *)param;

    sAPI_Debug("%s: handle %d, length %d, offset %d\r\n", __func__, rw->handle, rw->length, rw->offset);

    if(rw->data[0] > 0x03)//bit1:bit0 = indication:notification
    {
        sAPI_Debug("%s: written data illegal.\r\n", __func__);
        PrintfResp("\r\n%s: written data illegal.\r\n", __func__);
        return 0x81;
    }

    sAPI_Debug("%s: receive data:0x%02X", __func__, rw->data[0]);

    return SC_BLE_ERR_OK;
}

static SC_BLE_SERVICE_T custom_attrs[] =
{
    //custom service declare
    SC_BLE_DECLARE_PRIMARY_SERVICE(sc_ble_uuid_primary_service, custom_service_declare, LE_UUID_TYPE_16),

    //custom characteristic declare
    SC_BLE_DECLARE_CHARACTERISTIC(sc_ble_uuid_characteristic, custom_characteristic_declare, custom_characteristic_attribute_value, LE_ATT_PM_READABLE|LE_ATT_PM_WRITEABLE,
                              custom_characteristic_write_cb, custom_characteristic_read_cb,
                              custom_characteristic_value, sizeof(custom_characteristic_value)),

    //custom client characteristic config declare
    SC_BLE_DECLARE_CLINET_CHRAC_CONFIG(sc_ble_uuid_client_charc_config, custom_client_characteristic_config_value, custom_client_char_config_write_cb),
};

//for sAPI_BleRegisterEventHandle
//static int custom_ble_handle_event(SC_BLE_EVENT_MSG_T *msg)
//for sAPI_BleRegisterEventHandleEx
static void custom_ble_handle_event(void *argv)
{
    char urc[512] = {0};
    SC_BLE_EVENT_MSG_T *msg = (SC_BLE_EVENT_MSG_T *)argv;
    SC_BLE_SCAN_EVENT_T *scan = NULL;

    sAPI_Debug("%s: event_type:%d event_id:%d", __func__, msg->event_type, msg->event_id);
    switch(msg->event_type)
    {
        case EVENT_TYPE_COMMON:
            switch(msg->event_id)
            {
                case COMMON_POWERUP_FAILED:
                    sAPI_Debug("COMMON_POWERUP_FAILED.");
                    PrintfResp("\r\nCOMMON_POWERUP_FAILED.\r\n");
                    //ble_handle_open(-1);//not set afer sdk of date 20230619 
                    break;
                case COMMON_POWERUP_COMPLETE:
                    sAPI_Debug("COMMON_POWERUP_COMPLETE.");
                    PrintfResp("\r\nCOMMON_POWERUP_COMPLETE.\r\n");
                    //ble_handle_open(0);//not set afer sdk of date 20230619 
                    break;
                case COMMON_SLAVE_LE_BOND_COMPLETE:
                    sAPI_Debug("COMMON_SLAVE_LE_BOND_COMPLETE.");
                    PrintfResp("\r\nCOMMON_SLAVE_LE_BOND_COMPLETE.\r\n");
                    break;
                case COMMON_RSSI:
                {
                    signed char *rssi = (signed char *)msg->payload;
                    sprintf(urc, "\r\nCOMMON_RSSI rssi:%d.\r\n",*rssi);
                    sAPI_Debug("COMMON_RSSI %d.",*rssi);
                    PrintfResp(urc);
                    break;
                }
                default:break;
            }
            break;
        case EVENT_TYPE_LE:
            switch(msg->event_id)
            {
                case BLE_SCAN_EVENT:
                {
                    scan = (SC_BLE_SCAN_EVENT_T *)(msg->payload);
                    int i = 0,j = 0;
                    char advData[63] = {0};

                    for(i = 0; i < scan->length; i ++)
                    {
                        j += sprintf(advData + j, "%02x",scan->data[i]);
                    }
                    sAPI_Debug("remote device addr:%02x:%02x:%02x:%02x:%02x:%02x,advData[%s],rssi:%d",
                                    scan->address.bytes[5],scan->address.bytes[4],scan->address.bytes[3],
                                    scan->address.bytes[2],scan->address.bytes[1],scan->address.bytes[0],
                                    advData, scan->rssi);
                    sprintf(urc, "remote device addr:%02x:%02x:%02x:%02x:%02x:%02x,advData[%s],rssi:%d\r\n",
                                    scan->address.bytes[5],scan->address.bytes[4],scan->address.bytes[3],
                                    scan->address.bytes[2],scan->address.bytes[1],scan->address.bytes[0],
                                    advData, scan->rssi);
                    PrintfResp(urc);
                    break;
                }
                case BLE_CONNECT_EVENT:
                    sAPI_Debug("BLE_CONNECT_EVENT.");
                    PrintfResp("\r\nBLE_CONNECT_EVENT.\r\n");
                    break;

                case BLE_DIS_CONNECT_EVENT:
                    sAPI_Debug("BLE_DIS_CONNECT_EVENT.");
                    PrintfResp("\r\nBLE_DIS_CONNECT_EVENT.\r\n");
                    break;

                case BLE_MTU_EXCHANGED_EVENT:
                    sAPI_Debug("BLE_MTU_EXCHANGED_EVENT.");
                   PrintfResp("\r\nBLE_MTU_EXCHANGED_EVENT.\r\n");
                    break;

                case BLE_INDICATE_EVENT:
                    sAPI_Debug("BLE_INDICATE_EVENT.");
                    PrintfResp("\r\nBLE_INDICATE_EVENT.\r\n");
                    break;

                case BLE_CLIENT_MTU_EXCHANGED_EVENT:
                    sAPI_Debug("BLE_CLIENT_MTU_EXCHANGED_EVENT.");
                    PrintfResp("\r\nBLE_CLIENT_MTU_EXCHANGED_EVENT.\r\n");
                    break;

                case BLE_CLIENT_READ_BY_GROUP_TYPE_RSP_EVENT:
                    sAPI_Debug("BLE_CLIENT_READ_BY_GROUP_TYPE_RSP_EVENT.");
                    PrintfResp("\r\nBLE_CLIENT_READ_BY_GROUP_TYPE_RSP_EVENT.\r\n");
                    break;

                case BLE_CLIENT_WRITE_RSP:
                    sAPI_Debug("BLE_CLIENT_WRITE_RSP.");
                    PrintfResp("\r\nBLE_CLIENT_WRITE_RSP.\r\n");
                    break;

                default:
                    sAPI_Debug("%s: default.", __func__);
                    PrintfResp("\r\n: default.\r\n");
                    break;
            }
            break;
        default:break;
    }
}

void BLEDemo(void)
{
    int result = 0;
    SC_BLE_ADDR_T address = {0};
    int advDataSize = 0;
    int advRspDataSize = 0;
#ifdef SC_EXT_ADV
    char advData[255] = {0};
    char advRspData[255] = {0};
#else
    char advData[31] = {0};
    char advRspData[31] = {0};
    SC_BLE_ADV_PARAM_T param = {0};
#endif
    unsigned char advData_type_Flags = 0;
    char urc[500] = {0};

    UINT32 opt = 0;
    char *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    char *options_list[] =
    {
        "1. open",
        "2. close",
        "3. set address",
        "4. create adv data",
        "5. set adv data",
        "6. set adv param",
        "7. register service",
        "8. unregister service",
        "9. register event handle",
        "10. enable adv",
        "11. disable adv",
        "12. disconnect",
        "13. indicate",
        "14. notify",
        "15. scan start",
        "16. scan stop",
        "17. get address",
        "18. le pair to device",
        "19. le enable pair",
        "20. get pair info",
        "21. delete pair info",
        "22. clear pair info",
        "23. get RSSI",
        "24. set device name",
        #ifdef CUS_DRIVER_BT
        "25. set ble device config",
        #endif
        "99. back",
    };
    //gap peripheral role execute procedure: 9->1->3->6->4->5->7->10->(establish ble link)->(13/14)->12->8->11->2
    //gap observer role scan procedure: 9->1->15->16

    while(1)
    {
        PrintfResp(note);
        PrintfOptionMenu(options_list,sizeof(options_list)/sizeof(options_list[0]));

        opt = UartReadValue();

        result = 0;
        memset(urc, 0, 50);
        if(opt == 99)
            return;
        switch(opt)
        {
            case SC_BLE_DEMO_OPEN:
                sAPI_BleRegisterEventHandleEx(custom_ble_handle_event);
                sAPI_Debug("register ble event handle finish.");
                PrintfResp("\r\nregister ble event handle finish.\r\n");
                result = sAPI_BleOpen(NULL, 0);
                if(result == 0)
                {
                    sAPI_Debug("open ble device success.");
                    PrintfResp("\r\nopen ble device success.\r\n");
                }
                else
                {
                    sAPI_Debug("open ble device fail. error code = %d", result);
                    sprintf(urc, "\r\nopen ble device fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                sAPI_BleSetGapName("SIMCOM BLE DEMO");
                break;
            case SC_BLE_DEMO_CLOSE:
                sAPI_BleClose();
                sAPI_Debug("close ble device finish.");
                PrintfResp("\r\nclose ble device finish.\r\n");
                break;
            case SC_BLE_DEMO_SET_ADDRESS:
                address.bytes[0] = 0x20;
                address.bytes[1] = 0xA1;
                address.bytes[2] = 0x23;
                address.bytes[3] = 0xB4;
                address.bytes[4] = 0x35;
                address.bytes[5] = 0xC6;
                result = sAPI_BleSetAddress(&address);
                if(result == 0)
                {
                    sAPI_Debug("set address success.");
                    PrintfResp("\r\nset address success.\r\n");
                }
                else
                {
                    sAPI_Debug("set address fail. error code = %d", result);
                    sprintf(urc, "\r\nset address fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_GET_ADDRESS:
                result = sAPI_BleGetAddress(&address);
                if(result == 0)
                {
                    char buffer0[128] = {0};
                    sprintf(buffer0,"\r\n+Get add: \"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
                            address.bytes[0], address.bytes[1], address.bytes[2],
                            address.bytes[3], address.bytes[4], address.bytes[5]);
                    PrintfResp(buffer0);
                }
                else
                {
                    sAPI_Debug("get address fail. error code = %d", result);
                    sprintf(urc, "\r\nget address fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_CREATE_ADV_DATA:
                advData_type_Flags = 0x18;
                advDataSize = 0;
                memset(advData, 0, sizeof(advData));
                result = sAPI_BleCreateAdvData(BLE_ADV_DATA_TYPE_FLAG, advData, sizeof(advData), &advData_type_Flags, 1);
                if(result == -1)
                {
                    advDataSize = -1;
                    sAPI_Debug("create adv data fail. error code = %d", result);
                    sprintf(urc, "\r\ncreate adv data fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                    break;
                }
                advDataSize += result;

                result = sAPI_BleCreateAdvData(BLE_ADV_DATA_TYPE_COMPLETE_NAME, &advData[advDataSize], sizeof(advData)-advDataSize, "SIMCOM BLE DEMO", strlen("SIMCOM BLE DEMO"));
                if(result == -1)
                {
                    advDataSize = -1;
                    sAPI_Debug("create adv data fail. error code = %d", result);
                    sprintf(urc, "\r\ncreate adv data fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                    break;
                }
                advDataSize += result;

#ifndef SC_EXT_ADV
                result = sAPI_BleSetAdvData(advData, advDataSize);
                if(result == 0)
                {
                    sAPI_Debug("set adv data success.");
                    PrintfResp("\r\nset adv data success.\r\n");
                }
                else
                {
                    sAPI_Debug("set adv data fail. error code = %d", result);
                    sprintf(urc, "\r\nset adv data fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }

                result = sAPI_BleCreateAdvData(BLE_ADV_DATA_TYPE_SPECIFIC_DATA, &advRspData[advRspDataSize], sizeof(advRspData)-advRspDataSize, "test response", strlen("test response"));
                result = sAPI_BleSetScanResponseData((unsigned char *)advRspData,result);
#else
                result = sAPI_BleCreateAdvData(BLE_ADV_DATA_TYPE_SPECIFIC_DATA, &advData[advDataSize], sizeof(advData)-advDataSize, 
                "WEIYUANTESTEXTADV0123456789", strlen("WEIYUANTESTEXTADV0123456789"));
                if(result == -1)
                {
                    advDataSize = -1;
                    sAPI_Debug("create adv data fail. error code = %d", result);
                    sprintf(urc, "\r\ncreate adv data fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                    break;
                }
                advDataSize += result;
                result = sAPI_BleSetExtAdvData(advData, advDataSize);
                if(result == 0)
                {
                    sAPI_Debug("set ext adv data success.");
                    PrintfResp("\r\nset ext adv data success.\r\n");
                }
                else
                {
                    sAPI_Debug("set ext adv data fail. error code = %d", result);
                    sprintf(urc, "\r\nset ext adv data fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }

                char resp[50] = {0};
                memset(resp, 0xA5, 50);
                result = sAPI_BleCreateAdvData(BLE_ADV_DATA_TYPE_SPECIFIC_DATA, &advRspData[advRspDataSize], sizeof(advRspData)-advRspDataSize, resp,50);
                result = sAPI_BleSetExtScanResponseData((unsigned char *)advRspData,result);
#endif
                if(result == 0)
                {
                    sAPI_Debug("set adv resp data success.");
                    PrintfResp("\r\nset adv resp data success.\r\n");
                }
                else
                {
                    sAPI_Debug("set adv resp data fail. error code = %d", result);
                    sprintf(urc, "\r\nset adv resp data fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_SET_ADV_DATA:
                if((advDataSize == 0) || (advDataSize == -1))
                {
                    sAPI_Debug("set adv data fail, please create adv data.");
                    PrintfResp("\r\nset adv data fail, please create adv data.\r\n");
                }
                else
                {
#ifndef SC_EXT_ADV
                    result = sAPI_BleSetAdvData(advData, advDataSize);
#else
                    result = sAPI_BleSetExtAdvData(advData, advDataSize);
#endif
                    if(result == 0)
                    {
                        sAPI_Debug("set adv data success.");
                        PrintfResp("\r\nset adv data success.\r\n");
                    }
                    else
                    {
                        sAPI_Debug("set adv data fail. error code = %d", result);
                        sprintf(urc, "\r\nset adv data fail. error code = %d\r\n", result);
                        PrintfResp(urc);
                    }
                }
                break;
            case SC_BLE_DEMO_SET_ADV_PARAM:
#ifndef SC_EXT_ADV
                memset(&param, 0, sizeof(SC_BLE_ADV_PARAM_T));
                param.interval_min = 400;
                param.interval_max = 400;
                param.advertising_type = LE_ADV_TYPE_IND;
                param.own_address_type = LE_ADDRESS_TYPE_RANDOM;
                result = sAPI_BleSetAdvParam(&param);
                if(result == 0)
                {
                    sAPI_Debug("set broadcast parameter success.");
                    PrintfResp("\r\nset broadcast parameter success.\r\n");
                }
                else
                {
                    sAPI_Debug("set broadcast parameter fail. error code = %d", result);
                    sprintf(urc, "\r\nset broadcast parameter fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
#else
                {
                    SC_BLE_EXT_ADV_PARAM_T ext_param = {0};
                    memset(&ext_param, 0, sizeof(SC_BLE_EXT_ADV_PARAM_T));
                    ext_param.properties = 0x41;
                    ext_param.interval_max = 400;
                    ext_param.interval_min = 300;
                    ext_param.channel = 0x07;//37 & 38 & 39
                    ext_param.own_address_type = LE_ADDRESS_TYPE_RANDOM;
                    ext_param.tx_power = 8;
                    ext_param.primary_phy = 0x3;
                    ext_param.secondary_phy = 0x3;
                    result = sAPI_BleSetExtAdvParam(&ext_param);
                    if(result == 0)
                    {
                        sAPI_Debug("set broadcast parameter success.");
                        PrintfResp("\r\nset broadcast parameter success.\r\n");
                    }
                    else
                    {
                        sAPI_Debug("set broadcast parameter fail. error code = %d", result);
                        sprintf(urc, "\r\nset broadcast parameter fail. error code = %d\r\n", result);
                        PrintfResp(urc);
                    }
                }
#endif
                break;
            case SC_BLE_DEMO_REGISTER_SERVICE:
                result = sAPI_BleRegisterService(custom_attrs, sizeof(custom_attrs) / sizeof(SC_BLE_SERVICE_T));
                if(result == 0)
                {
                    sAPI_Debug("service register success.");
                    PrintfResp("\r\nservice register success.\r\n");
                }
                else
                {
                    sAPI_Debug("service register fail. error code = %d", result);
                    sprintf(urc, "\r\nservice register fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_UNREGISTER_SERVICE:
                result = sAPI_BleUnregisterService();
                if(result == 0)
                {
                    sAPI_Debug("service unregister success.");
                    PrintfResp("\r\nservice unregister success.\r\n");
                }
                else
                {
                    sAPI_Debug("service unregister fail. error code = %d", result);
                    sprintf(urc, "\r\nservice unregister fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_REGISTER_EVENT_HANDLE:
                //choose just one api to regiset callback
                //sAPI_BleRegisterEventHandle(custom_ble_handle_event);
                //sAPI_BleRegisterEventHandleEx(custom_ble_handle_event);
                sAPI_Debug("register ble event handle finish.");
                PrintfResp("\r\nregister ble event handle finish.\r\n");
                break;
            case SC_BLE_DEMO_ENABLE_ADV:
#ifndef SC_EXT_ADV
                result = sAPI_BleEnableAdv();
#else
                result = sAPI_BleEnableExtAdv();
#endif
                if(result == 0)
                {
                    sAPI_Debug("enable adv success.");
                    PrintfResp("\r\nenable adv success.\r\n");
                }
                else
                {
                    sAPI_Debug("enable adv fail. error code = %d", result);
                    sprintf(urc, "\r\nenable adv fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_DISABLE_ADV:
#ifndef SC_EXT_ADV
                result = sAPI_BleDisableAdv();
#else
                result = sAPI_BleDisableExtAdv();
#endif
                if(result == 0)
                {
                    sAPI_Debug("disable adv success.");
                    PrintfResp("\r\ndisable adv success.\r\n");
                }
                else
                {
                    sAPI_Debug("disable adv fail. error code = %d", result);
                    sprintf(urc, "\r\ndisable adv fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_DISCONNECT:
                result = sAPI_BleDisconnect();
                if(result == 0)
                {
                    sAPI_Debug("disconnect ble link success.");
                    PrintfResp("\r\ndisconnect ble link success.\r\n");
                }
                else
                {
                    sAPI_Debug("disconnect ble link fail. error code = %d", result);
                    sprintf(urc, "\r\ndisconnect ble link fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_INDICATE:
                sAPI_Debug("start sending indication. gatt_handle:%d", custom_characteristic_declare.handle);
                if(custom_characteristic_declare.handle == 0)
                {
                    sAPI_Debug("send indication fail, please register service.");
                    PrintfResp("\r\nsend indication fail, please register service.\r\n");
                }
                else
                {
                    result = sAPI_BleIndicate(custom_characteristic_declare.handle, "simcom is testing indicate", strlen("simcom is testing indicate"));
                    if(result == 0)
                    {
                        sAPI_Debug("indicate send success.");
                        PrintfResp("\r\nindicate send success.\r\n");
                    }
                    else
                    {
                        sAPI_Debug("indicate send fail. error code = %d", result);
                        sprintf(urc, "\r\nindicate send fail. error code = %d\r\n", result);
                        PrintfResp(urc);
                    }
                }
                break;
            case SC_BLE_DEMO_NOTIFY:
                sAPI_Debug("start sending notification. gatt_handle:%d", custom_characteristic_declare.handle);
                if(custom_characteristic_declare.handle == 0)
                {
                    sAPI_Debug("send notification fail, please register service.");
                    PrintfResp("\r\nsend notification fail, please register service.\r\n");
                }
                else
                {
                    result = sAPI_BleNotify(custom_characteristic_declare.handle, "simcom is testing notify", strlen("simcom is testing notify"));
                    if(result == 0)
                    {
                        sAPI_Debug("notify send success.");
                        PrintfResp("\r\nnotify send success.\r\n");
                    }
                    else
                    {
                        sAPI_Debug("notify send fail. error code = %d", result);
                        sprintf(urc, "\r\nnotify send fail. error code = %d\r\n", result);
                        PrintfResp(urc);
                    }
                }
                break;
            case SC_BLE_DEMO_SCAN_START:
                result = sAPI_BleScan(LE_ACTIVE_SCAN, 0x1F40, 0x1F40, LE_ADDRESS_TYPE_RANDOM);
                if(result == 0)
                {
                    sAPI_Debug("scan start success.");
                    PrintfResp("\r\nscan start success.\r\n");
                }
                else
                {
                    sAPI_Debug("scan start fail. error code = %d", result);
                    sprintf(urc, "\r\nscan start fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_SCAN_STOP:
                result = sAPI_BleScanStop();
                if(result == 0)
                {
                    sAPI_Debug("scan stop success.");
                    PrintfResp("\r\nscan stop success.\r\n");
                }
                else
                {
                    sAPI_Debug("scan stop fail. error code = %d", result);
                    sprintf(urc, "\r\nscan stop fail. error code = %d\r\n", result);
                    PrintfResp(urc);
                }
                break;
            case SC_BLE_DEMO_PAIR_DEVICE:
            {
                 char addrStr[13] = {0};
                 SC_BLE_ADDR_T addr = {0};
                 int j = 6;
                 PrintfResp("\r\nPlease input address to pair, like XXXXXXXXXXXX\r\n");
                 UartReadLine(addrStr, sizeof(addrStr));
                 for(int i = 0;i < 12;i += 2)
                 {
                     char tmp[4] = {0};
                     memcpy(tmp,&addrStr[i],2);
                     addr.bytes[--j] = strtoul(tmp,NULL,16);
                 }
                result = sAPI_BleSmpIniate(addr);
                sAPI_Debug("pair to %2X:%2X:%2X:%2X:%2X:%2X. error code = %d",
                    addr.bytes[5],addr.bytes[4],addr.bytes[3],
                    addr.bytes[2],addr.bytes[1],addr.bytes[0],
                    result);
                sprintf(urc, "pair to %2X:%2X:%2X:%2X:%2X:%2X. error code = %d",
                    addr.bytes[5],addr.bytes[4],addr.bytes[3],
                    addr.bytes[2],addr.bytes[1],addr.bytes[0],
                    result);
                PrintfResp(urc);
                break;
            }
            case SC_BLE_DEMO_ENABLE_PAIR:
                PrintfResp("\r\ninput enable/disable(1/0).\r\n");
                opt = UartReadValue();
                result = sAPI_BleSetPairEnable(opt);
                break;
            case SC_BLE_DEMO_GET_PAIR_INFO:
            {
                 int flag = 0;
                 SC_BLE_DEVICE_INFO_RECORD info = {0};
                 for(int i = 0;i < 10;i ++)
                 {
                     if(sAPI_BleGetPairInfo(i,&info) == 0)
                     {
                         sAPI_Debug("pair info[%d] %2X:%2X:%2X:%2X:%2X:%2X",i,
                             info.address[5],info.address[4],info.address[3],
                             info.address[2],info.address[1],info.address[0]);
                         sprintf(urc, "\r\npair info[%d] %2X:%2X:%2X:%2X:%2X:%2X\r\n",i,
                             info.address[5],info.address[4],info.address[3],
                             info.address[2],info.address[1],info.address[0]);
                         PrintfResp(urc);
                         flag ++;
                     }
                 }
                 if(flag == 0)
                 {
                     sAPI_Debug("NOT pair info");
                     sprintf(urc, "\r\nNOT pair info\r\n");
                     PrintfResp(urc);
                 }
                 break;
            }
            case SC_BLE_DEMO_DEL_PAIR_INFO:
            {
                int flag = 0;
                 PrintfResp("\r\ninput device index(<= 10).\r\n");
                 opt = UartReadValue();
                 sAPI_BleDeleteSinglePairInfo(opt);
                 SC_BLE_DEVICE_INFO_RECORD info = {0};
                 for(int i = 0;i < 10;i ++)
                 {
                     if(sAPI_BleGetPairInfo(i,&info) == 0)
                     {
                         sAPI_Debug("pair info[%d] %2X:%2X:%2X:%2X:%2X:%2X",i,
                             info.address[5],info.address[4],info.address[3],
                             info.address[2],info.address[1],info.address[0]);
                         sprintf(urc, "\r\npair info[%d] %2X:%2X:%2X:%2X:%2X:%2X\r\n",i,
                             info.address[5],info.address[4],info.address[3],
                             info.address[2],info.address[1],info.address[0]);
                         PrintfResp(urc);
                         flag ++;
                     }
                 }
                 if(flag == 0)
                 {
                     sAPI_Debug("NOT pair info");
                     sprintf(urc, "\r\nNOT pair info\r\n");
                     PrintfResp(urc);
                 }
                 break;
            }
            case SC_BLE_DEMO_CLEAR_PAIR_INFO:
                sAPI_BleClearPairInfo();
                break;
            case SC_BLE_DEMO_GET_RSSI:
                sAPI_BleReadRssi();
                break;
            case SC_BLE_DEMO_DEVICE_NAME:
            {
                char name[248] = {0};
                //set name before open BT
                sAPI_BleSetDeviceName("SC_TEST_NAME");
                strcpy(name,sAPI_BleGetDeviceName());
                sAPI_Debug("set name is %s",name);
                sprintf(urc, "\r\nset name is %s\r\n",name);
                PrintfResp(urc);
                break;
            }
#ifdef CUS_DRIVER_BT
            case SC_BLE_DEMO_DEVICE_CONFIG:
            {
                SC_BLE_DEV_CFG_T device;
                PrintfResp("\r\ninput bt_uart id(0<= 3):\r\n");
                device.uart_id.id = UartReadValue();

                PrintfResp("\r\ninput bt_uart baudrate:\r\n");
                device.uart_baudrate.baudrate = UartReadValue();

                PrintfResp("\r\ninput host_wakeup_bt:\r\n");
                device.host_wakeup_bt = UartReadValue();

                PrintfResp("\r\ninput bt_wakeup_host:\r\n");
                device.bt_wakeup_host = UartReadValue();

                PrintfResp("\r\ninput bt_reset:\r\n");
                device.bt_rst = UartReadValue();

                PrintfResp("\r\ninput bt_pwdn:\r\n");
                device.bt_pwdn = UartReadValue();

                sAPI_BleDevConfigInit(&device);
                break;
            }
#endif
            default:break;
        }
    }
}

#endif

