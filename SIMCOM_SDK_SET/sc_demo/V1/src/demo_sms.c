/**
  ******************************************************************************
  * @file    demo_sms.c
  * @author  SIMCom OpenSDK Team
  * @brief   Source file of sms operation.
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
#include "stdio.h"
#include "stdlib.h"
#include "simcom_common.h"
#include "simcom_debug.h"
#include "simcom_sms.h"
#include "simcom_uart.h"
#include "uart_api.h"


#define FALSE 0
#define TRUE 1
#define SMS_URC_RECIVE_TIME_OUT 1000
#define MAX_SMS_INPUT_DATA_LEN 400

static BOOL gSmsDemohasinited = FALSE;
sMsgQRef g_sms_demo_msgQ;
sMsgQRef g_sms_demo_urc_process_msgQ;
sMsgQRef g_sms_demo_urc_rsp_msgQ;
static sTaskRef gSmsUrcProcessTask = NULL;
static UINT8 gSmsUrcProcessTaskStack[1024 * 4];

#if (defined SIMCOM_A7680C_V5_01) || (defined SIMCOM_A7670C_V701) || (defined SIMCOM_A7670C_V702) || defined (SIMCOM_A7680C_V506) || defined (SIMCOM_A7680C_V603)
#define SC_UART    SC_UART4
#else
#define SC_UART    SC_UART
#endif

extern void PrintfOptionMenu(char *options_list[], int array_size);
extern void PrintfResp(char *format);
void sTask_SmsUrcProcessor(void);

char input_data[SC_SMS_MAX_MULTIPLE_MSG_LENGTH + 1];
UINT32 input_data_len;
BOOL input_data_end = TRUE;

typedef enum
{
    SC_SMS_INIT,
    SC_SMS_EXIT
} SC_SMS_INIT_BRANCH;

typedef enum
{
    SC_SMS_DEMO_INIT            = 1,
    SC_SMS_DEMO_WRITE           = 2,
    SC_SMS_DEMO_READ            = 3,
    SC_SMS_DEMO_SEND            = 4,
    SC_SMS_DEMO_SEND_STORAGE    = 5,
    SC_SMS_DEMO_SEND_MULTIPLE   = 6,
    SC_SMS_DEMO_DELETEALL       = 7,
    SC_SMS_DEMO_DELETEONE       = 8,
    SC_SMS_DEMO_SIMPLE_TESET    = 9,
    SC_SMS_DEMO_NEWMSGIND       = 10,
    SC_SMS_DEMO_GETNEWMSGIND    = 11,
    SC_SMS_DEMO_LISTMSG         = 12,
    SC_SMS_DEMO_CHSET           = 13,
    SC_SMS_DEMO_DECODEPDU       = 14,
    SC_SMS_DEMO_ENCODEPDU       = 15,
    SC_SMS_DEMO_FORMAT       = 16,
    SC_SMS_DEMO_DEINIT          = 99,
    SC_SMS_DEMO_MAX

} SC_SMS_DEMO_TYPE;

typedef enum
{
    SC_SMS_PARA_FORMAT,
    SC_SMS_PARA_SMS_LENGTH,
    SC_SMS_PARA_SMS_DATA,
    SC_SMS_PARA_SRCADDR,
    SC_SMS_PARA_SMS_STATE,
    SC_SMS_PARA_CONFIRM,
    SC_SMS_PARA_SMS_INDEX,
    SC_SMS_PARA_SMS_MODE,
    SC_SMS_PARA_MAX
} SC_SMS_PARA_TYPE;

/*remove \r\n
 *since uart transport data to here by max length of 32 byte per package
 *data tansport end flag is \r\n received*/
/**
  * @brief  Data input processor
  * @param  *data_in--pointer
  * @param  data_in_len--data length
  * @note
  * @retval void
  */
BOOL inputDataProcessor(char *data_in, UINT32 data_in_len)
{
    UINT32 in_len = data_in_len;
    BOOL ret = FALSE;
    if (NULL == data_in)
        goto end;
    if (TRUE == input_data_end)
    {
        memset(input_data, 0, SC_SMS_MAX_MULTIPLE_MSG_LENGTH + 1);
        input_data_len = 0;
    }
    sAPI_Debug("%#x%#x%#x", data_in[0], data_in[1], data_in[2]);
    sAPI_Debug("enter: inputDataProcessor, len[%d],input_data_len[%d],input_data[%s],input_data_end[%d]", in_len,
               input_data_len, input_data, input_data_end);
    if ((in_len > 2) && (0x0d == data_in[in_len - 2]) && (0x0a == data_in[in_len - 1]))
    {
        memcpy(input_data + input_data_len, data_in, in_len - 2);
        input_data_len += in_len - 2;
        input_data_end = TRUE;
        ret = TRUE;
    }
    else
    {
        memcpy(input_data + input_data_len, data_in, in_len);
        input_data_len += in_len;
        input_data_end = FALSE;
        ret = FALSE;
    }
end:
    sAPI_Debug("inputDataProcessor end, ret[%d]", ret);
    return ret;
}

/**
  * @brief  SMS demo parameter initial
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode SmsDemoInit(SC_SMS_INIT_BRANCH branch)
{
    sAPI_Debug("enter: SmsDemoInit init");
    SC_SMSReturnCode retval = SC_SMS_SUCESS;
    SC_STATUS status;
    if (TRUE != gSmsDemohasinited)
    {
        status = sAPI_MsgQCreate(&g_sms_demo_msgQ, "g_sms_demo_msgQ", sizeof(SIM_MSG_T), 4, SC_FIFO);
        sAPI_Debug("enter: sAPP_SmsTaskDemo init2");
        if (status != SC_SUCCESS)
        {
            sAPI_Debug("ERROR: message queue creat err!\n");
            retval = SC_SMS_FAIL;
        }

        /*msgQ prepared for urc processor*/
        if (g_sms_demo_urc_process_msgQ == NULL)
            status = sAPI_MsgQCreate(&g_sms_demo_urc_process_msgQ, "g_sms_demo_urc_process_msgQ", sizeof(SIM_MSG_T), 4, SC_FIFO);
        if (status != SC_SUCCESS)
        {
            sAPI_Debug("ERROR: message queue g_sms_demo_urc_process_msgQ creat err!\n");
            retval = SC_SMS_FAIL;
        }

        /*msgQ for urc process response*/
        status = sAPI_MsgQCreate(&g_sms_demo_urc_rsp_msgQ, "g_sms_demo_urc_rsp_msgQ", sizeof(SIM_MSG_T), 4, SC_FIFO);
        if (status != SC_SUCCESS)
        {
            sAPI_Debug("ERROR: message queue g_sms_demo_urc_rsp_msgQ creat err!\n");
            retval = SC_SMS_FAIL;
        }


        if (NULL == gSmsUrcProcessTask)
        {
            /*creat a thread, prepared for */
            status = sAPI_TaskCreate(&gSmsUrcProcessTask, gSmsUrcProcessTaskStack, 1024 * 4, 90, "SmsurcProcesser",
                                     (void *)sTask_SmsUrcProcessor, (void *)0);
            if (status != SC_SUCCESS)
            {
                gSmsUrcProcessTask = NULL;
                sAPI_Debug("Ping-------ERROR, Task Create error!\n");
            }
        }


        gSmsDemohasinited = TRUE;
        retval = SC_SMS_SUCESS;


    }


    /*delete msgQ, end demo process*/
    if (SC_SMS_EXIT == branch)
    {
        sAPI_MsgQDelete(g_sms_demo_msgQ);
        sAPI_MsgQDelete(g_sms_demo_urc_rsp_msgQ);
        gSmsDemohasinited = FALSE;
        retval = SC_SMS_SUCESS;
    }

    sAPI_Debug("%s, init finish, ret=%d", __FUNCTION__, retval);
    return retval;
}

/**
  * @brief  SMS URC processor
  * @param  void
  * @note   Refer to cus_urc.c
  * @retval void
  */
void sTask_SmsUrcProcessor()
{
    char rsp_buff[500] = {0};
    SC_SMSReturnCode ret = SC_SMS_SUCESS;
    SC_STATUS status;
    SIM_MSG_T SmsUrcProcessReq;
    SIM_MSG_T msg_rsp;
    sAPI_Debug("%S, handle init!", __func__);
    while (1)
    {
        sAPI_Debug("----%s, wait for new urc msg!-------", __func__);
        memset(&SmsUrcProcessReq, 0, sizeof(SIM_MSG_T));
        memset(rsp_buff, 0, sizeof(rsp_buff));
        /*msgQ send from cus_urc.c*/
        sAPI_MsgQRecv(g_sms_demo_urc_process_msgQ, &SmsUrcProcessReq, SC_SUSPEND);
        sAPI_Debug("%s, msg_id[%d]", __func__, SmsUrcProcessReq.msg_id);

        /*operations related to a valid cmd type*/
        switch (SmsUrcProcessReq.msg_id)
        {
            case SC_URC_NEW_MSG_IND:
            {
                sAPI_Debug("SC_URC_NEW_MSG_IND, index[%d],msg_id[%d]", SmsUrcProcessReq.arg1, SmsUrcProcessReq.msg_id);
                ret = sAPI_SmsReadMsg(1, SmsUrcProcessReq.arg1, g_sms_demo_urc_rsp_msgQ);
                sAPI_Debug("%s, %d, msg read, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg_rsp, 0, sizeof(msg_rsp));
                    status = sAPI_MsgQRecv(g_sms_demo_urc_rsp_msgQ, &msg_rsp, SMS_URC_RECIVE_TIME_OUT);
                    if (status == SC_SUCCESS)
                    {
                        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\r\nSC_URC_NEW_MSG_IND, New msg come in:\r\n");
                        sAPI_Debug("[sms], sAPI_SmsReadMsg, primID[%d] resultCode[%d],rspStr[%s]", msg_rsp.arg1, msg_rsp.arg2, msg_rsp.arg3);
                        sprintf(rsp_buff, "\r\n\r\nsAPI_SmsReadMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg_rsp.arg1,
                                msg_rsp.arg2, (char *)msg_rsp.arg3);
                        sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff); /*show msg to uart*/
                        /*must free this response buffer*/
                        sAPI_Free(msg_rsp.arg3);
                    }
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tsTask_SmsUrcProcessor SMS read failed!\r\n");
                }
                break;
            }
            case SC_URC_FLASH_MSG:
            {
                /*recv new text msg string here, if sAPI_SmsSetNewMsgInd(1,1,2,0,0,0) has been set when booting*/
                memset(rsp_buff, 0, sizeof(rsp_buff));
                sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\r\nSC_URC_FLASH_MSG, New msg come in:\r\n");
                sprintf(rsp_buff, "%s", (char *)SmsUrcProcessReq.arg3);
                sAPI_Debug("SC_URC_FLASH_MSG,[%s]", rsp_buff);
                sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff); /*show msg to uart*/
                sAPI_Free(SmsUrcProcessReq.arg3);
            }
            break;
        }
    }

}

/**
  * @brief  Write SMS demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smsMsgWritedemo(void)
{
    SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    int len;
    UINT32 opt = SC_SMS_PARA_FORMAT;
    SIM_MSG_T msg;
    char rsp_buff[120];
    UINT8 sms_data[SC_SMS_MAX_CI_MSG_PDU_SIZE + 1];
    SCsmsFormatMode fmatmode = 0;
    UINT16 smslen = 0;
    UINT8 srcAddr[SC_SMS_MAX_ADDRESS_LENGTH + 1];
    UINT8 stat = 0;

    char *note = "\r\nPlease input WRITE sms para in order: \r\n";
    char *options_list[] =
    {
        "\t1. Formatmode: ",
        "\t2. SmsLen: ",
        "\t3. SmsData: ",
        "\t4. SrcAddr: ",
        "\t5. Stat: ",
        "\t6. Confirm to write: \r\n\t\t1. yes\t\t2. no\r\n"
    };
    PrintfResp(note);
    sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);

    while (1)
    {
        len = UartRead(buf, 100, FALSE);

        sAPI_Debug("--smsMsgWritedemo--, operation=[%d] buf = [%s] len = %d", opt, buf,len);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
            continue;
        }
        //sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        /*get para data in order*/
        switch (opt)
        {
            case SC_SMS_PARA_FORMAT:
            {
                fmatmode = atoi(input_data);
                opt = SC_SMS_PARA_SMS_LENGTH;
                break;
            }
            case SC_SMS_PARA_SMS_LENGTH:
            {
                smslen = atoi(input_data);
                opt = SC_SMS_PARA_SMS_DATA;
                break;
            }
            case SC_SMS_PARA_SMS_DATA:
            {
                memset(sms_data, 0, SC_SMS_MAX_CI_MSG_PDU_SIZE + 1);
                memcpy(sms_data, input_data, input_data_len);
                opt = SC_SMS_PARA_SRCADDR;
                break;
            }
            case SC_SMS_PARA_SRCADDR:
            {
                memset(srcAddr, 0, SC_SMS_MAX_ADDRESS_LENGTH + 1);
                memcpy(srcAddr, input_data, input_data_len);
                opt = SC_SMS_PARA_SMS_STATE;
                break;
            }
            case SC_SMS_PARA_SMS_STATE:
            {
                stat = atoi(input_data);
                opt = SC_SMS_PARA_CONFIRM;
                break;
            }
            case SC_SMS_PARA_CONFIRM:
            {
                if (atoi(input_data) == 1)
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\tYES, send msg now.\r\n");
                    sAPI_Debug("%s, format[%d],sms[%s],smslen[%d],sddr[%s]", __func__, fmatmode, sms_data, smslen, srcAddr);
                    if (1 == fmatmode)
                        ret = sAPI_SmsWriteMsg(fmatmode, sms_data, smslen, srcAddr, stat, g_sms_demo_msgQ);
                    else
                        ret = sAPI_SmsWriteMsg(fmatmode, sms_data, smslen, NULL, stat, g_sms_demo_msgQ);
                    sAPI_Debug("%s, %d, msg write, ret=%d!!", __func__, __LINE__, ret);
                    if (ret == SC_SMS_SUCESS)
                    {
                        memset(&msg, 0, sizeof(msg));
                        sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                        sAPI_Debug("[sms], sAPI_SmsWriteMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                        sprintf(rsp_buff, "\r\n\r\nsAPI_SmsWriteMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                                msg.arg2, (char *)msg.arg3);
                        sAPI_Free(msg.arg3);
                        sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                    }
                    return ret;
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nERROR: SMS write CANCEL!!");
                    return SC_SMS_FAIL;
                }
            }
            default:
                break;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");

        sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);

    }

    return ret;
}
/**
  * @brief  Read SMS demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smsMsgReaddemo(void)
{
    SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    int len;
    UINT32 opt = SC_SMS_PARA_FORMAT;
    SCsmsFormatMode fmatmode = {0};
    SIM_MSG_T msg;
    int resp_buff_len = 500;
    char rsp_buff[resp_buff_len];

    char *note = "\r\nPlease input READ sms para in order: \r\n";
    char *options_list[] =
    {
        "\t1. Formatmode: ",
        "\t2. SmsIndex: ",
    };
    PrintfResp(note);
    sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[0]);

    while (1)
    {
        len = UartRead(buf, 99, FALSE);

         sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
            continue;
        }
        //sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        /*get para data in order*/
        switch (opt)
        {
            case SC_SMS_PARA_FORMAT:
            {
                fmatmode = atoi(input_data);
                opt = SC_SMS_PARA_SMS_INDEX;
                break;
            }
            case SC_SMS_PARA_SMS_INDEX:
            {
                ret = sAPI_SmsReadMsg(fmatmode, atoi(input_data), g_sms_demo_msgQ);
                sAPI_Debug("%s, %d, msg read, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsReadMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsReadMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSMS read failed!\r\n");
                }
                return ret;
            }
            default:
                break;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
        sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[1]);

    }

    return ret;
}
/**
  * @brief  Send normal SMS demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smsMsgSenddemo(void)
{
    SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    int len;
    UINT32 opt = SC_SMS_PARA_FORMAT;
    SIM_MSG_T msg;
    int resp_buff_len = 120;
    char rsp_buff[resp_buff_len];
    SCsmsFormatMode fmatmode = 0;
    UINT8 sms_data[SC_SMS_MAX_CI_MSG_PDU_SIZE + 1];
    UINT16 smslen = 0;
    UINT8 srcAddr[SC_SMS_MAX_ADDRESS_LENGTH + 1];

    char *note = "\r\nPlease input SEND sms para in order: \r\n";
    char *options_list[] =
    {
        "\t1. Formatmode: ",
        "\t2. SmsLen: ",
        "\t3. SmsData: ",
        "\t4. SrcAddr: ",
        "\t5. Confirm to send: \r\n\t\t1. yes\t\t2. no\r\n"
    };
    PrintfResp(note);
    sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);

    while (1)
    {
        len = UartRead(buf, 99, FALSE);

        sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
            continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        /*get para data in order*/
        switch (opt)
        {
            case SC_SMS_PARA_FORMAT:
            {
                fmatmode = atoi(input_data);
                opt = SC_SMS_PARA_SMS_LENGTH;
                break;
            }
            case SC_SMS_PARA_SMS_LENGTH:
            {
                smslen = atoi(input_data);
                opt = SC_SMS_PARA_SMS_DATA;
                break;
            }
            case SC_SMS_PARA_SMS_DATA:
            {
                memset(sms_data, 0, SC_SMS_MAX_CI_MSG_PDU_SIZE + 1);
                memcpy(sms_data, input_data, input_data_len);
                opt = SC_SMS_PARA_SRCADDR;
                break;
            }
            case SC_SMS_PARA_SRCADDR:
            {
                memset(srcAddr, 0, SC_SMS_MAX_ADDRESS_LENGTH + 1);
                memcpy(srcAddr, input_data, input_data_len);
                opt = SC_SMS_PARA_SMS_STATE;
                break;
            }
            case SC_SMS_PARA_SMS_STATE:
            case SC_SMS_PARA_CONFIRM:
            {
                if (atoi(input_data) == 1)
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\tYES, send msg now.\r\n");
                    sAPI_Debug("%s, format[%d],sms[%s],smslen[%d],sddr[%s]", __func__, fmatmode, sms_data, smslen, srcAddr);
                    if (1 == fmatmode)
                        ret = sAPI_SmsSendMsg(fmatmode, sms_data, smslen, srcAddr, g_sms_demo_msgQ);
                    else
                        ret = sAPI_SmsSendMsg(fmatmode, sms_data, smslen, NULL, g_sms_demo_msgQ);
                    sAPI_Debug("%s, %d, msg write, ret=%d!!", __func__, __LINE__, ret);
                    if (ret == SC_SMS_SUCESS)
                    {
                        memset(&msg, 0, sizeof(msg));
                        sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                        sAPI_Debug("[sms], sAPI_SmsSendMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                        sprintf(rsp_buff, "\r\n\r\nsAPI_SmsSendMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                                msg.arg2, (char *)msg.arg3);
                        sAPI_Free(msg.arg3);
                        sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                    }
                    else
                    {
                        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: SMS Send msg failed!\r\n");
                    }
                    return ret;
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nSMS write CANCEL!!");
                    return SC_SMS_FAIL;
                }
            }
            default:
                break;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
        sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);

    }

    return ret;
}

/**
  * @brief  Send long SMS demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smsMsgSendLongMsgdemo(void)
{
    SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    int len;
    UINT32 opt = SC_SMS_PARA_FORMAT;
    SIM_MSG_T msg;
    SCsmsFormatMode fmatmode;
    int resp_buff_len = 120;
    char rsp_buff[resp_buff_len];
    UINT8 sms_data[SC_SMS_MAX_MULTIPLE_MSG_LENGTH + 1];
    UINT16 smslen = 0;
    UINT8 srcAddr[SC_SMS_MAX_ADDRESS_LENGTH + 1];

    char *note = "\r\nPlease input SEND sms para in order: \r\n";
    char *options_list[] =
    {
        "\t1. Formatmode: ",
        "\t2. SmsLen: ",
        "\t3. SmsData: ",
        "\t4. SrcAddr: ",
        "\t5. Confirm to send: \r\n\t\t1. yes\t\t2. no\r\n"
    };

    PrintfResp(note);
    sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);

    while (1)
    {
        len = UartRead(buf, 99, FALSE);

        sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
            continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        /*get para data in order*/
        switch (opt)
        {
            case SC_SMS_PARA_FORMAT:
            {
                fmatmode = atoi(input_data);
                sAPI_Debug("--SMSDemo--, smslen=[%d]", fmatmode);
                opt = SC_SMS_PARA_SMS_LENGTH;
                break;
            }
            case SC_SMS_PARA_SMS_LENGTH:
            {
                smslen = atoi(input_data);
                sAPI_Debug("--SMSDemo--, smslen=[%d]", smslen);
                opt = SC_SMS_PARA_SMS_DATA;
                break;
            }
            case SC_SMS_PARA_SMS_DATA:
            {
                memset(sms_data, 0, SC_SMS_MAX_MULTIPLE_MSG_LENGTH + 1);
                memcpy(sms_data, input_data, input_data_len);
                sAPI_Debug("--SMSDemo--, sms_data=[%s]", sms_data);
                opt = SC_SMS_PARA_SRCADDR;
                break;
            }
            case SC_SMS_PARA_SRCADDR:
            {
                memset(srcAddr, 0, SC_SMS_MAX_ADDRESS_LENGTH + 1);
                memcpy(srcAddr, input_data, input_data_len);
                opt = SC_SMS_PARA_SMS_STATE;
                break;
            }
            case SC_SMS_PARA_SMS_STATE:
            case SC_SMS_PARA_CONFIRM:
            {
                if (atoi(input_data) == 1)
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\tYES, send long msg now.\r\n");
                    sAPI_Debug("%s,sms[%s],smslen[%d],sddr[%s]", __func__, sms_data, smslen, srcAddr);
                    sAPI_Debug("%s,sddr[%s]", __func__, srcAddr);
                    ret = sAPI_SmsSendLongMsg(sms_data, smslen, srcAddr, g_sms_demo_msgQ);
                    sAPI_Debug("%s, %d, msg write, ret=%d!!", __func__, __LINE__, ret);
                    if (ret == SC_SMS_SUCESS)
                    {
                        sAPI_Debug("%s,SC_SMS_SUCESS", __func__);
                        memset(&msg, 0, sizeof(msg));
                        sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                        sAPI_Debug("[sms], smsMsgSendLongMsgdemo, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                        sprintf(rsp_buff, "\r\n\r\nsAPI_SmsSendLongMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                                msg.arg2, (char *)msg.arg3);
                        sAPI_Free(msg.arg3);
                        sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                    }
                    else
                    {
                        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: SMS long msg failed!\r\n");
                    }
                    return ret;
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nSMS write CANCEL!!");
                    return SC_SMS_FAIL;
                }
            }
            default:
                break;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
        sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);
    }
    return ret;
}

/**
  * @brief  Send SMS from storage demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smsMsgSendStorageMsgdemo(void)
{
    SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    int len;
    UINT32 opt = SC_SMS_PARA_SMS_INDEX;
    int msgindex = 1;
    SIM_MSG_T msg;
    int resp_buff_len = 120;
    char rsp_buff[resp_buff_len];
    UINT8 srcAddr[SC_SMS_MAX_ADDRESS_LENGTH + 1];

    char *note = "\r\nPlease input SEND STROED sms para in order: \r\n";
    char *options_list[] =
    {
        "\t1. SmsIndex: ",
        "\t2. SrcAddr: ",
    };
	PrintfResp(note);
	sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[0]);

    while (1)
    {
        len = UartRead(buf, 99, FALSE);

        sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
           continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        /*get para data in order*/
        switch (opt)
        {
            case SC_SMS_PARA_SMS_INDEX:
            {
                msgindex = atoi(input_data);
                opt = SC_SMS_PARA_SRCADDR;
                break;
            }
            case SC_SMS_PARA_SRCADDR:
            {
            	sAPI_Debug("strlen(input_data)=%d,msgindex=%d",strlen(input_data),msgindex);
                /*specific case no srcaddr para, just send msg by index*/
                if (strlen(input_data) < 3)
                    ret = sAPI_SmsSendStorageMsg(msgindex, NULL, g_sms_demo_msgQ);
                else
                {
                    /*make sure the input data of API has no ext data*/
                    memset(srcAddr, 0, sizeof(srcAddr));
                    memcpy(srcAddr, input_data, input_data_len);
                    ret = sAPI_SmsSendStorageMsg(msgindex, srcAddr, g_sms_demo_msgQ);
                }
                sAPI_Debug("%s, %d, msg sAPI_SmsSendStorageMsg, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsSendStorageMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsSendStorageMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSMS send stored msg failed!\r\n");
                }
                return ret;
            }
            default:
                break;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
        sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[1]);

    }

    return ret;
}

/**
  * @brief  Delete SMS demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smsMsgDelOnedemo(void)
{
    SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    int len;
    UINT32 opt = SC_SMS_PARA_SMS_INDEX;
    int msgindex = 1;
    SIM_MSG_T msg;
    int resp_buff_len = 120;
    char rsp_buff[resp_buff_len];

    char *note = "\r\nPlease input DELETE sms para in order: \r\n";
    char *options_list[] =
    {
        "\t1. SmsIndex: ",
    };
    PrintfResp(note);
    sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[0]);

    while (1)
    {
        len = UartRead(buf, 99, FALSE);

        sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
            continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        /*get para data in order*/
        switch (opt)
        {
            case SC_SMS_PARA_SMS_INDEX:
            {
                msgindex = atoi(input_data);
                ret = sAPI_SmsDelOneMsg(msgindex, g_sms_demo_msgQ);
                sAPI_Debug("%s, %d, msg del, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsDeleteMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsDeleteMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSMS del failed!\r\n");
                }
                return ret;
            }
            default:
                break;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
    }
    return ret;
}

/**
  * @brief  New SMS indication demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smsSetNewMsgInddemo(void)
{
    SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    int len;
    UINT32 opt = SC_SMS_PARA_SMS_INDEX;

    char *note = "\r\nPlease input DELETE sms para in order: \r\n";
    char *options_list =
        "NOTICE: IF the setting is successful, \r\n    it will take effect for the NEXT received SMS\r\n1. store msg to SIM card: \r\n2. directly report msg by text mode: \r\n3. directly report msg by pdu mode: \r\n";

    PrintfResp(note);
    PrintfResp(options_list);

    while (1)
    {
        len = UartRead(buf, 100, FALSE);

        sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
           continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        /*get para data in order*/
        opt = atoi(input_data);
       // opt = UartReadValue();
        switch (opt)
        {
            case 1:
            {
                ret = sAPI_SmsSetNewMsgInd(1, 2, 1, 0, 0, 0);
                if (ret == SC_SMS_SUCESS)
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tNew msg store to SIM card\r\n");
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSet failed\r\n");
                }
                return ret;
            }
            case 2:
            {
                ret = sAPI_SmsSetNewMsgInd(1, 1, 2, 0, 0, 0);
                if (ret == SC_SMS_SUCESS)
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tDirct report by text mode\r\n");
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSet failed\r\n");
                }
                return ret;
            }
            case 3:
            {
                ret = sAPI_SmsSetNewMsgInd(0, 1, 2, 0, 0, 0);
                if (ret == SC_SMS_SUCESS)
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tDirct report by pdu mode\r\n");
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSet failed\r\n");
                }
                return ret;
            }
            default:
                continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
    }
    return ret;
}


/**
  * @brief  get New SMS indication demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smsGetNewMsgInddemo(void)
{
	SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    UINT32 mode,mt,bm,ds,bfr = 0;
    UINT32 opt = SC_SMS_PARA_SMS_INDEX;

    char *note = "\r\n1.GET CNMI\r\n";

    PrintfResp(note);
    opt = UartReadValue();

    while (1)
    {
        switch (opt)
        {
            case 1:
            {
                ret = sAPI_SmsGetNewMsgInd(&mode,&mt,&bm,&ds,&bfr);
                if(ret == SC_SMS_SUCESS)
                {
                    sAPI_Debug("get NewmsgInd sucess mode[%ld]mt[%ld]bm[%ld]ds[%ld]bfr[%ld]",mode,mt,bm,ds,bfr);
                    sprintf(buf,"Get cnmi success mode[%ld]mt[%ld]bm[%ld]ds[%ld]bfr[%ld]",mode,mt,bm,ds,bfr);
                    PrintfResp(buf);
                }
                else
                {
                    sAPI_Debug("Get cnmi falied!");
                    PrintfResp("\r\nGet cnmi falied!\r\n");
                }
                return ret;
            }
            default:
                break;
        }
		if(opt != 1)
			PrintfResp("INVALID VALUE!\r\n");
		break;
     }
     return ret;
}

/**
  * @brief  LIST SMS demo
  * @param  void
  * @note
  * @retval void
  */

SC_SMSReturnCode smsListMsgdemo(void)
{
	SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    int len;
    UINT32 opt = SC_SMS_PARA_SMS_INDEX;
    INT32 mode = 0;

    //int maxnum = 50;
	char *note = "\r\nPlease input LIST sms para in order: \r\n";
	char *options_list[] =
	{
		"\t1. Mode: ",
	};
	PrintfResp(note);
	sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[0]);

	while (1)
    {
        len = UartRead(buf, 99, FALSE);

         sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
            continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        /*get para data in order*/
        switch (opt)
        {

            case SC_SMS_PARA_SMS_INDEX:
            {
        		mode = atoi(input_data);
	            ret = sAPI_SmsListMsg(mode);
	            sAPI_Debug("%s, %d, msg list, ret=%d!!", __func__, __LINE__, ret);
	            if (ret == SC_SMS_SUCESS)
	            {
	                sAPI_Debug("[sms], sAPI_SmsListMsg, sucess");
                	sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSMS list sucess!\r\n");
	            }
	            else
	            {
	                sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSMS list failed!\r\n");
	            }
                return ret;
            }
            default:
                break;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");

    }

    return ret;
}

/**
  * @brief  set New SMS indication demo
  * @param  void
  * @note
  * @retval void
  */
SC_SMSReturnCode smschsetdemo(void)
{
	SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char buf[100] = {0};
    char tmpbuf[50] = {0};
    UINT8 format;
    UINT32 opt = SC_SMS_PARA_SMS_INDEX;

    char *note = "\r\n1.GET CSCS 2.SET GSM 3.SET IRA 4.SET UCS2\r\n";

    PrintfResp(note);
    opt = UartReadValue();

    while (1)
    {
        switch (opt)
        {
            case 1:
            {
                ret = sAPI_SmsgetChset(&format);
                if(ret == SC_SMS_SUCESS)
                {
					if(format == 0)
						strcpy(tmpbuf,"\"GSM\"");
					else if(format == 1)
						strcpy(tmpbuf,"\"HEX\"");
					else if(format == 2)
						strcpy(tmpbuf,"\"IRA\"");
					else if(format == 4)
						strcpy(tmpbuf,"\"UCS2\"");

					sAPI_Debug("get cscs sucess format=%d,buf=%s",format,tmpbuf);
                    sprintf(buf,"Get cscs success! cscs=%s",tmpbuf);
                    PrintfResp(buf);
				}
                else
                {
                    sAPI_Debug("Get cscs falied!");
                    PrintfResp("\r\nGet cscs falied!\r\n");
                }
                return ret;
            }
            case 2:
            {
                ret = sAPI_SmssetChset(0);
                if(ret == SC_SMS_SUCESS)
                {
                    sAPI_Debug("SET GSM sucess!!!");
                    PrintfResp("\r\nSET GSM sucess!!!\r\n");
                }
                else
                {
                    sAPI_Debug("SET GSM  falied!");
                    PrintfResp("\r\nSET GSM  falied!\r\n");
                }
                return ret;
            }
            case 3:
            {
                ret = sAPI_SmssetChset(2);
                if(ret == SC_SMS_SUCESS)
                {
                    sAPI_Debug("SET IRA sucess!!!");
                    PrintfResp("\r\nSET IRA sucess!!!\r\n");
                }
                else
                {
                    sAPI_Debug("SET IRA falied!");
                    PrintfResp("\r\nSET IRA falied!\r\n");
                }
                return ret;
            }
            case 4:
            {
                ret = sAPI_SmssetChset(4);
                if(ret == SC_SMS_SUCESS)
                {
                    sAPI_Debug("SET UCS2 sucess!!!");
                    PrintfResp("\r\nSET UCS2 sucess!!!\r\n");
                }
                else
                {
                    sAPI_Debug("SET UCS2 falied!");
                    PrintfResp("\r\nSET UCS2 falied!\r\n");
                }
                return ret;
            }
            default:
                break;
        }
    }
        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
    return ret;

}

SC_SMSReturnCode smsDecodePdudemo(void)
{
	SC_SMSReturnCode ret = SC_SMS_SUCESS;
    char addrbuf[100] = {0};
    int len = 0;
    char txtbuf[500] = {0};
    char sms_data[SC_SMS_MAX_CI_MSG_PDU_SIZE + 1];
    UINT32 opt = 0;
    int resp_buff_len = 500;
    char rsp_buff[resp_buff_len];
    char buf[500] = {0};

    char *note = "\r\n Please input PDU data sms para in order:\r\n";
     char *options_list[] =
    {
        "\t1. pdudata: ",
        "\t2. Confirm to decode: \r\n\t\t1. yes\t\t2. no\r\n"
    };
    PrintfResp(note);
    sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);

    while (1)
    {
    	len = UartRead(buf, 500, FALSE);

         sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
            continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        switch (opt)
        {
            case 0:
            {
                memset(sms_data, 0, SC_SMS_MAX_CI_MSG_PDU_SIZE + 1);
                memcpy(sms_data, input_data, input_data_len);
                opt = 1;
                break;
            }
            case 1:
            {
				if(atoi(input_data) == 1)
				{
					sAPI_Debug("sms_data=%s",sms_data);
					ret = sAPI_SmsDecodePdu(sms_data,addrbuf,&len,txtbuf);
					if(ret == SC_SMS_SUCESS)
					{
						sAPI_Debug("pdu to txt sucess!Addr[%s]len[%d]Txtbuf[%s]", addrbuf,len, txtbuf);
						sprintf(rsp_buff, "\r\nsAPI_SmsDecodePdu:\r\n\tAddr[%s] \r\n\tlen[%d]\r\n\tTxtbuf[%s]\r\n", addrbuf,
						len, txtbuf);
						sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
					}
					else{
						sAPI_Debug("decode error");
						sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nERROR: SMS decode fail!!");
					}
					return ret;
				}
				else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nERROR: SMS decode CANCEL!!");
                    return SC_SMS_FAIL;
                }
            }
			default:
				break;
		}
			sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
			sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);
    }
    return ret;
}


SC_SMSReturnCode smsEncodePdudemo(void)
{
	SC_SMSReturnCode ret = SC_SMS_SUCESS;
    int len = 0;
    char sms_data[SC_SMS_MAX_CI_MSG_PDU_SIZE + 1];
    UINT32 opt = 0;
    int resp_buff_len = 500;
    char rsp_buff[resp_buff_len];
    char buf[500] = {0};
    char addrbuf[50];

    char *note = "\r\n Please input txt data sms para in order:\r\n";
     char *options_list[] =
    {
        "\t1. addr: ",
        "\t2. txtdata: ",
        "\t3. Confirm to encode: \r\n\t\t1. yes\t\t2. no\r\n"
    };
    PrintfResp(note);
    sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);

    while (1)
    {
    	len = UartRead(buf, 500, FALSE);

         sAPI_Debug("--SMSDemo--, operation=[%d] buf = [%s]", opt, buf);
        /*echo input data*/
        if (!inputDataProcessor(buf, len))
        {
            continue;
        }
        sAPI_UartWriteString(SC_UART, (UINT8 *)input_data);
        switch (opt)
        {
            case 0:
            {
                memset(addrbuf, 0, 50);
                memcpy(addrbuf, input_data, input_data_len);
                opt = 1;
                break;
            }
            case 1:
            {
                memset(sms_data, 0, SC_SMS_MAX_CI_MSG_PDU_SIZE + 1);
                memcpy(sms_data, input_data, input_data_len);
                opt = 2;
                break;
            }
            case 2:
            {
				if(atoi(input_data) == 1)
				{
					sAPI_Debug("sms_data=%s",sms_data);
					int pduLen = 0;
					int smsDataLength = 0;
					smsDataLength = strlen(sms_data);
					char pduText[400] = {0};
					ret = sAPI_SmsEncodePdu(addrbuf, sms_data,smsDataLength, pduText, &pduLen);
					
					if(ret == SC_SMS_SUCESS)
					{
						sAPI_Debug("pdu to txt sucess! pduText[%s] pdulen[%d]", pduText,pduLen);
						sprintf(rsp_buff, "\r\nsAPI_SmsEncodePdu:\r\n pduText[%s] \r\n\t \r\n\tpdulen[%d]\r\n", pduText,
						pduLen);
						sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
					}
					else{
						sAPI_Debug("encode error");
						sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nERROR: SMS encode fail!!");
					}
					return ret;
				}
				else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nERROR: SMS encode CANCEL!!");
                    return SC_SMS_FAIL;
                }
            }
			default:
				break;
		}
			sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");
			sAPI_UartWriteString(SC_UART, (UINT8 *)options_list[opt]);
    }
    return ret;
}

SC_SMSReturnCode smsFormatdemo(void)
{
	SC_SMSReturnCode ret = SC_SMS_SUCESS;
	UINT32 opt = 0;
    UINT8 format;
    char rsp_buff[50];

    char *note = "\r\n1. Get format 2. Set format PDU mode 3.Set format TEXT mode\r\n";
    PrintfResp(note);
    opt = UartReadValue();

    while (1)
    {
        switch (opt)
        {
            case 1:
            {
            	ret = sAPI_SmsgetFormat(&format);
            	if(ret == SC_SMS_SUCESS)
            	{
                    sAPI_Debug("%s,Get format sucess! format=%d",__func__,format);
                    sprintf(rsp_buff,"Get format sucess! format=%d",format);
					sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
            	}
            	else
            	{
                    sAPI_Debug("%s,Get format fail!",__func__);
					sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nGet format fail!\r\n");
            	}
            	return ret;
            }
            case 2:
            {
                ret = sAPI_SmsSetFormat(0);
				if(ret == SC_SMS_SUCESS)
				{
					sAPI_Debug("%s,Set pdu format sucess!",__func__);
					sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nSet pdu format sucess!\r\n");
				}
				else
				{
					sAPI_Debug("%s,Set pdu fail!",__func__);
					sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nSet pdu format fail!\r\n");
				}
				return ret;
                
            }
            case 3:
            {
                ret = sAPI_SmsSetFormat(1);
				if(ret == SC_SMS_SUCESS)
				{
					sAPI_Debug("%s,Set text format sucess!",__func__);
					sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nSet text format sucess!\r\n");
				}
				else
				{
					sAPI_Debug("%s,Set text fail!",__func__);
					sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\nSet text format fail!\r\n");
				}
				return ret;
                
            }
            default:
				break;
        }
		PrintfResp(note);
		opt = UartReadValue();
		sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n");	
	}
    return ret;
}


/**
  * @brief  SMS operation demo
  * @param  void
  * @note
  * @retval void
  */
void SMSDemo(void)
{
    SC_SMSReturnCode ret;
    int resp_buff_len = 120;
    SIM_MSG_T msg;
    char buf[100] = {0};
    int len;
    char rsp_buff[resp_buff_len];
    UINT32 opt = 0;

    char *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    char *options_list[] =
    {
        "1. Init sms demo",
        "2. Write a msg to SIM card",
        "3. Read a msg",
        "4. Send a msg",
        "5. Send a stored msg",
        "6. Send multiple long msg",
        "7. Delete all stored msg",
        "8. Delete one stored msg",
        "9. Simple sms test",
        "10. Set the way of new msg report",
        "11. Get the way of new msg report",
        "12. List msg",
        "13. character set format",
        "14. DecodePdu",
        "15. EncodePdu",
        "16. SMS message format",
        "99. DeInit sms demo"
    };

    while (1)
    {
		PrintfResp(note);
		PrintfOptionMenu(options_list, sizeof(options_list) / sizeof(options_list[0]));
        memset(rsp_buff, 0, resp_buff_len);
        len = UartRead(buf, 99, FALSE);

        sAPI_Debug("--SMSDemo--, buf = [%s]", buf);
        if (!inputDataProcessor(buf, len))
           // continue;
        opt = UartReadValue();
        sAPI_Debug("--SMSDemo--opt=%d", opt);

        /*INIT status check*/
        if ((FALSE == gSmsDemohasinited) && (opt > SC_SMS_DEMO_INIT) && (opt < SC_SMS_DEMO_DEINIT))
        {
            sAPI_Debug("sms init is necessary");
            sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: sms init is necessary!\r\n");
            continue;
        }
        else if((FALSE == gSmsDemohasinited) && (opt == SC_SMS_DEMO_DEINIT))
            return;


        switch (opt)
        {
            case SC_SMS_DEMO_INIT:
            {
                ret = SmsDemoInit(SC_SMS_INIT);
                if (SC_SMS_SUCESS == ret)
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSUCCESS: SMS Init success!\r\n");
                }
                else
                {
                    sAPI_Debug("sms init failed");
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: SMS Init Failed!\r\n");
                }
                break;
            }

            case SC_SMS_DEMO_WRITE:
            {
                ret = smsMsgWritedemo();
                break;
            }
            case SC_SMS_DEMO_READ:
            {
                ret = smsMsgReaddemo();
                break;
            }
            case SC_SMS_DEMO_SEND:
            {
                ret = smsMsgSenddemo();
                break;
            }
            case SC_SMS_DEMO_SEND_STORAGE:
            {
                ret = smsMsgSendStorageMsgdemo();
                break;
            }
            case SC_SMS_DEMO_SEND_MULTIPLE:
            {
                ret = smsMsgSendLongMsgdemo();
                break;
            }
            case SC_SMS_DEMO_DELETEALL:
            {
                ret = sAPI_SmsDelAllMsg(g_sms_demo_msgQ);
                sAPI_Debug("%s, %d, msg write, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsDelAllMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsDelAllMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tDelete all msg failed!!\r\n");
                }
                break;
            }
            case SC_SMS_DEMO_DELETEONE:
            {
                ret = smsMsgDelOnedemo();
                break;
            }
            case SC_SMS_DEMO_DEINIT:
            {
                ret = SmsDemoInit(SC_SMS_EXIT);
                if (SC_SMS_SUCESS == ret)
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSUCCESS: SMS Init success!\r\n");
                }
                else
                {
                    sAPI_Debug("sms init failed");
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: SMS Init Failed!\r\n");
                }
                /*demo process end*/
                return;
            }
            case SC_SMS_DEMO_SIMPLE_TESET:
            {
                sAPI_UartWriteString(SC_UART, (UINT8 *)"\t\r\nSend simple msg test now.");



                /****************************************************************************************************/
                /*NOTICE:
                 *1. user need make sure api get valid data,
                 *2. the msgQ must be created before using
                 */
                /****************************************************************************************************/
                /*delete all sms in sim card*/
                sAPI_UartWriteString(SC_UART, (UINT8 *)"\t\r\nSend text sms test.\r\n");
                ret = sAPI_SmsDelAllMsg(g_sms_demo_msgQ);
                sAPI_Debug("%s, %d, msg write, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsDelAllMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsDelAllMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tDelete all msg failed!!\r\n");
                    break;
                }

                /****************************************************************************************************/
                /*send text sms(123asdfd) to (13364028515)*/
                sAPI_UartWriteString(SC_UART, (UINT8 *)"\t\r\n1. Send text sms test.");
                ret = sAPI_SmsSendMsg(1, (UINT8 *)"123asdfd", strlen("123asdfd"), (UINT8 *)"13364028515", g_sms_demo_msgQ);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsSendMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsSendMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: SMS Send msg failed!\r\n");
                    break;
                }

                /****************************************************************************************************/
                /*send pud sms(1234??o?, 0001000D91683163048215F500080C00310032003300344F60597D) to (13364028515)*/
                sAPI_UartWriteString(SC_UART, (UINT8 *)"\t\r\n2. Send pdu sms test.");
                ret = sAPI_SmsSendMsg(0, (UINT8 *)"0001000D91683163048215F500080C00310032003300344F60597D", 26, NULL, g_sms_demo_msgQ);
                sAPI_Debug("%s, %d, msg write, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsSendMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsSendMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: SMS Send msg failed!\r\n");
                    break;
                }

                /****************************************************************************************************/
                /*write text sms to sim card*/
                sAPI_UartWriteString(SC_UART, (UINT8 *)"\t\r\n3. Write text sms test.");
                ret = sAPI_SmsWriteMsg(1, (UINT8 *)"123asd", strlen("123asd"), (UINT8 *)"13364028515", 1, g_sms_demo_msgQ);
                sAPI_Debug("%s, %d, msg write, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsWriteMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsWriteMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: SMS write text msg failed!\r\n");
                    break;
                }

                /****************************************************************************************************/
                /*write pdu sms to sim card*/
                sAPI_UartWriteString(SC_UART, (UINT8 *)"\t\r\n4. Write pdu sms test.");
                /*write sms(1234??o?, 0001000D91683163048215F500080C00310032003300344F60597D)*/
                ret = sAPI_SmsWriteMsg(0, (UINT8 *)"0001000D91683163048215F500080C00310032003300344F60597D",
                                       strlen("0001000D91683163048215F500080C00310032003300344F60597D"), NULL, 1, g_sms_demo_msgQ);
                sAPI_Debug("%s, %d, msg write, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsWriteMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsWriteMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tERROR: SMS write text msg failed!\r\n");
                    break;
                }

                /****************************************************************************************************/
                /*send stored pdu sms */
                sAPI_UartWriteString(SC_UART, (UINT8 *)"\t\r\n5. Send stored pdu sms.");
                /*the pdu sms has been write to index-2, send it to(15730119169)*/
                ret = sAPI_SmsSendStorageMsg(2, (UINT8 *)"15730119169", g_sms_demo_msgQ);
                sAPI_Debug("%s, %d, msg sAPI_SmsSendStrogeMsg, ret=%d!!", __func__, __LINE__, ret);
                if (ret == SC_SMS_SUCESS)
                {
                    memset(&msg, 0, sizeof(msg));
                    sAPI_MsgQRecv(g_sms_demo_msgQ, &msg, SMS_URC_RECIVE_TIME_OUT);
                    sAPI_Debug("[sms], sAPI_SmsSendStrogeMsg, primID[%d] resultCode[%d],rspStr[%s]", msg.arg1, msg.arg2, msg.arg3);
                    sprintf(rsp_buff, "\r\n\r\nsAPI_SmsSendStrogeMsg:\r\n\tprimID[%d] \r\n\tresultCode[%d]\r\n\trspStr[%s]\r\n", msg.arg1,
                            msg.arg2, (char *)msg.arg3);
                    sAPI_Free(msg.arg3);
                    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
                }
                else
                {
                    sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\tSMS send stored msg failed!\r\n");
                    break;
                }
            }

            case SC_SMS_DEMO_NEWMSGIND:
            {
                ret = smsSetNewMsgInddemo();
                break;
            }
            case SC_SMS_DEMO_GETNEWMSGIND:
            {
                ret = smsGetNewMsgInddemo();
                break;
            }
            case SC_SMS_DEMO_LISTMSG:
            {
                ret = smsListMsgdemo();
                break;
            }
            case SC_SMS_DEMO_CHSET:
            {
                ret = smschsetdemo();
                break;
            }
            case SC_SMS_DEMO_DECODEPDU:
            {
                ret = smsDecodePdudemo();
                break;
            }
            case SC_SMS_DEMO_ENCODEPDU:
            {
                ret = smsEncodePdudemo();
                break;
            }
            case SC_SMS_DEMO_FORMAT:
            {
                ret = smsFormatdemo();
                break;
            }
            default :
                break;
        }

        sAPI_UartWriteString(SC_UART, (UINT8 *)"\r\n\r\n");
    }
}
