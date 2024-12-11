/**
  ******************************************************************************
  * @file    demo_loc.c
  * @author  SIMCom OpenSDK Team
  * @brief   Source file of location feature from LBS.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 SIMCom Wireless.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "simcom_os.h"
#include "simcom_loc.h"
#include "simcom_common.h"
#include "simcom_debug.h"
#include "simcom_uart.h"
#include "uart_api.h"


extern void PrintfOptionMenu(char *options_list[], int array_size);
extern void PrintfResp(char *format);
int lbsCurrentTime = 1;
int lbsTestTime = 0;
extern void LbsTestDemoInit(void);



sMsgQRef SC_LbsResp_msgq;

/**
  * @brief  LBS demo
  * @param  void
  * @note   Need to insert SIM card and get registered before LBS operation.
  * @retval void
  */
void LbsDemo(void)
{
    INT16 channel = 0;
    INT16 type;
    UINT32 opt = 0;
    SC_LBS_RETURNCODE ret;

    SC_STATUS status;
    char *note =
        "\r\nPlease select an option to test from the items listed below.\r\n\r\nPlease make sure the IMEI has set before LBS(AT+SIMEI=IMEI)\r\n";
    char *options_list[] =
    {
        "1. get longitude and latitude",
        "2. get detail address",
        "3. get error number",
        "4. get longitude latitude and date time",
        "5. test the API for LBS",
        "99. back",
    };

    while (1)
    {
        PrintfResp(note);
        PrintfOptionMenu(options_list, sizeof(options_list) / sizeof(options_list[0]));

        opt = UartReadValue();

        if (SC_LbsResp_msgq == NULL)
        {
            status = sAPI_MsgQCreate(&SC_LbsResp_msgq, "SC_LbsResp_msgq", (sizeof(SIM_MSG_T)), 4, SC_FIFO);
            if (SC_SUCCESS != status)
            {
                sAPI_Debug("osaStatus = [%d],create msg error!", status);
            }
        }
        else
        {
            sAPI_Debug("msg has been created!");
        }

        switch (opt)
        {
            case  SC_LBS_GET_LONLAT:
            {
                type = 1;
                char path[300] = {0};
                ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
                if (ret != SC_LBS_SUCCESS)
                {
                    sAPI_Debug("get location fail!\r\n");
                    PrintfResp("\r\nLBS get longitude and latitude Fail!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("get location success!\r\n");
                    SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};
                    SC_lbs_info_t *sub_data = NULL;
                    SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND);
                    if (SC_SUCCESS != osaStatus)
                    {
                        sAPI_Debug("osaStatus = [%d],recv msg error!", osaStatus);
                        PrintfResp("\r\nLBS get longitude and latitude Fail!\r\n");
                        break;
                    }
                    else
                    {
                        if (msgQ_data_recv.msg_id == SC_SRV_LBS)
                        {
                            sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                            if (sub_data->u8ErrorCode != 0)
                            {
                                snprintf(path, sizeof(path), "\r\nLBS get longitude and latitude Fail!Recv LBS info:errocode:%d\r\n",
                                         sub_data->u8ErrorCode);
                                PrintfResp(path);
                                sAPI_Free(sub_data);

                                break;
                            }
                            else
                            {
                                if((1 == sub_data->u8LngMinusFlag) && (1 == sub_data->u8LatMinusFlag))
                                {
                                    snprintf( path, sizeof(path), "\r\nRecv LBS info:errocode:%d,Lng:-%f,Lat:-%f,Acc:%u\r\n",sub_data->u8ErrorCode,
                                         (float)(sub_data->u32Lng) / 1000000, (float)(sub_data->u32Lat) / 1000000, sub_data->u16Acc); 
                                }
                                else if((1 == sub_data->u8LngMinusFlag) && (0 == sub_data->u8LatMinusFlag))
                                {
                                    snprintf( path, sizeof(path), "\r\nRecv LBS info:errocode:%d,Lng:-%f,Lat:%f,Acc:%u\r\n",sub_data->u8ErrorCode,
                                         (float)(sub_data->u32Lng) / 1000000, (float)(sub_data->u32Lat) / 1000000, sub_data->u16Acc);
                                }
                                else if((0 == sub_data->u8LngMinusFlag) && (1 == sub_data->u8LatMinusFlag))
                                {
                                    snprintf( path, sizeof(path), "\r\nRecv LBS info:errocode:%d,Lng:%f,Lat:-%f,Acc:%u\r\n",sub_data->u8ErrorCode,
                                         (float)(sub_data->u32Lng) / 1000000, (float)(sub_data->u32Lat) / 1000000, sub_data->u16Acc);
                                }
                                else
                                {
                                    snprintf( path, sizeof(path), "\r\nRecv LBS info:errocode:%d,Lng:%f,Lat:%f,Acc:%u\r\n",sub_data->u8ErrorCode,
                                         (float)(sub_data->u32Lng) / 1000000, (float)(sub_data->u32Lat) / 1000000, sub_data->u16Acc);
                                }
                                sAPI_Debug("%s", path);
                                sAPI_Free(sub_data);
                                PrintfResp(path);
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case SC_LBS_GET_DETAILADDRESS:
            {
                type = 2;
                char path[300] = {0};
                ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
                if (ret != SC_LBS_SUCCESS)
                {
                    sAPI_Debug("get location fail!\r\n");
                    PrintfResp("\r\nLBS get detail address Fail!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("get location success!\r\n");
                    SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};
                    SC_lbs_info_t *sub_data = NULL;


                    SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND);
                    if (SC_SUCCESS != osaStatus)
                    {
                        sAPI_Debug("osaStatus = [%d],recv msg error!", osaStatus);
                        PrintfResp("\r\nLBS get detail address Fail!\r\n");
                        break;
                    }
                    else
                    {
                        if (msgQ_data_recv.msg_id == SC_SRV_LBS)
                        {
                            sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                            if (sub_data->u8ErrorCode != 0)
                            {
                                snprintf(path, sizeof(path), "\r\nLBS get detail address Fail!Recv LBS info:errocode:%d\r\n", sub_data->u8ErrorCode);
                                sAPI_Free(sub_data);

                                PrintfResp(path);

                                break;
                            }
                            else
                            {
                                char tempBuf[151];
                                memset(tempBuf, 0, 151);
                                char *p_tmp = tempBuf;
                                UINT16 i;
                                for (i = 0; i < sub_data->u32AddrLen; i++)
                                {
                                    p_tmp += sprintf(p_tmp, "%02x", sub_data->u8LocAddress[i]);
                                    sAPI_Debug("%02x", sub_data->u8LocAddress[i]);
                                }
                                snprintf(path, sizeof(path), "\r\nRecv LBS info:errocode:%d,detail_addr:%s\r\n", sub_data->u8ErrorCode, tempBuf);
                                sAPI_Free(sub_data);
                                PrintfResp(path);
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case SC_LBS_GET_ERRNO:
            {
                type = 3;
                char path[300] = {0};
                ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
                if (ret != SC_LBS_SUCCESS)
                {
                    sAPI_Debug("get location fail!\r\n");
                    PrintfResp("\r\nLBS get error number Fail!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("get location success!\r\n");
                    SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};
                    SC_lbs_info_t *sub_data = NULL;
                    SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND);
                    if (SC_SUCCESS != osaStatus)
                    {
                        sAPI_Debug("osaStatus = [%d],recv msg error!", osaStatus);
                        PrintfResp("\r\nLBS get error number Fail!\r\n");
                        break;
                    }
                    else
                    {
                        if (msgQ_data_recv.msg_id == SC_SRV_LBS)
                        {
                            sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                            if (sub_data->u8ErrorCode != 0)
                            {
                                snprintf(path, sizeof(path), "\r\nLBS get error number Fail!Recv LBS info:errocode:%d\r\n", sub_data->u8ErrorCode);
                                sAPI_Free(sub_data);
                                PrintfResp(path);

                                break;
                            }
                            else
                            {
                                snprintf(path, sizeof(path), "\r\nRecv LBS info:errocode:%d\r\n", sub_data->u8ErrorCode);
                                sAPI_Free(sub_data);

                                PrintfResp(path);

                                break;
                            }
                        }
                    }
                }
                break;
            }
            case SC_LBS_GET_LONLATTIME:
            {
                type = 4;
                char path[300] = {0};
                ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
                if (ret != SC_LBS_SUCCESS)
                {
                    sAPI_Debug("get location fail!\r\n");
                    PrintfResp("\r\nLBS get longitude latitude and date time Fail!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("get location success!\r\n");
                    SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};
                    SC_lbs_info_t *sub_data = NULL;
                    SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND);
                    if (SC_SUCCESS != osaStatus)
                    {
                        sAPI_Debug("osaStatus = [%d],recv msg error!", osaStatus);
                        PrintfResp("\r\nLBS get longitude latitude and date time Fail!\r\n");
                        break;
                    }
                    else
                    {
                        if (msgQ_data_recv.msg_id == SC_SRV_LBS)
                        {
                            sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                            if (sub_data->u8ErrorCode != 0)
                            {
                                snprintf(path, sizeof(path), "\r\nLBS get longitude latitude and date time Fail!Recv LBS info:errocode:%d\r\n",
                                         sub_data->u8ErrorCode);
                                sAPI_Debug("%s", path);
                                sAPI_Free(sub_data);
                                PrintfResp(path);
                                break;
                            }
                            else
                            {
                                if((1 == sub_data->u8LngMinusFlag) && (1 == sub_data->u8LatMinusFlag))
                                {
                                    snprintf( path, sizeof(path), "\r\nRecv LBS info:errocode:%d,Lng:-%f,Lat:-%f,Acc:%u,DateAndTime:%s\r\n",sub_data->u8ErrorCode,
                                         (float)(sub_data->u32Lng) / 1000000, (float)(sub_data->u32Lat) / 1000000, sub_data->u16Acc, sub_data->u8DateAndTime); 
                                }
                                else if((1 == sub_data->u8LngMinusFlag) && (0 == sub_data->u8LatMinusFlag))
                                {
                                    snprintf( path, sizeof(path), "\r\nRecv LBS info:errocode:%d,Lng:-%f,Lat:%f,Acc:%u,DateAndTime:%s\r\n",sub_data->u8ErrorCode,
                                         (float)(sub_data->u32Lng) / 1000000, (float)(sub_data->u32Lat) / 1000000, sub_data->u16Acc, sub_data->u8DateAndTime);
                                }
                                else if((0 == sub_data->u8LngMinusFlag) && (1 == sub_data->u8LatMinusFlag))
                                {
                                    snprintf( path, sizeof(path), "\r\nRecv LBS info:errocode:%d,Lng:%f,Lat:-%f,Acc:%u,DateAndTime:%s\r\n",sub_data->u8ErrorCode,
                                         (float)(sub_data->u32Lng) / 1000000, (float)(sub_data->u32Lat) / 1000000, sub_data->u16Acc, sub_data->u8DateAndTime);
                                }
                                else
                                {
                                    snprintf( path, sizeof(path), "\r\nRecv LBS info:errocode:%d,Lng:%f,Lat:%f,Acc:%u,DateAndTime:%s\r\n",sub_data->u8ErrorCode,
                                         (float)(sub_data->u32Lng) / 1000000, (float)(sub_data->u32Lat) / 1000000, sub_data->u16Acc, sub_data->u8DateAndTime);
                                }
                                sAPI_Free(sub_data);
                                PrintfResp(path);
                                break;
                            }

                        }
                    }
                }
                break;
            }
            case SC_LBS_TEST:
            {
                sAPI_Debug("Start lbs demo test thread!");
                PrintfResp("\r\nPlease input test time.\r\n");

                lbsTestTime = UartReadValue();
                lbsCurrentTime = 1;
                sAPI_Debug("lbsTestTime:%d,lbsCurrentTime:%d", lbsTestTime, lbsCurrentTime);

                LbsTestDemoInit();
                break;
            }
            case SC_LBS_DEMO_MAX:
            {
                return;
            }

            default :
                break;
        }
    }
}
