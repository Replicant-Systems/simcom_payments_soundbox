/**
  ******************************************************************************
  * @file    demo_call.c
  * @author  SIMCom OpenSDK Team
  * @brief   Source file of voice call function.
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
#include "simcom_call.h"
#include "simcom_common.h"
#include "simcom_debug.h"
#include "simcom_uart.h"
#include "uart_api.h"
#include "simcom_os.h"


#define FALSE 0
#define TRUE 1
#define CALL_URC_RECIVE_TIME_OUT 20000

//message definition
sMsgQRef g_call_demo_msgQ;
extern void PrintfOptionMenu(char *options_list[], int array_size);
extern void PrintfResp(char *format);

#if (defined SIMCOM_A7680C_V5_01) || (defined SIMCOM_A7670C_V701) || (defined SIMCOM_A7670C_V702) || defined (SIMCOM_A7680C_V506) || defined (SIMCOM_A7680C_V603)
#define SC_UART    SC_UART4
#else
#define SC_UART    SC_UART
#endif


typedef enum
{
    SC_CALL_DEMO_DIAL            = 1,
    SC_CALL_DEMO_ANSWER          = 2,
    SC_CALL_DEMO_END             = 3,
    SC_CALL_DEMO_STATE           = 4,
    SC_CALL_DEMO_AUTO_ANSWER     = 5,
    SC_CALL_DEMO_MAX             = 99
} SC_CALL_DEMO_TYPE;

/**
  * @brief  Voice call dial up demo
  * @param  void
  * @note   Need input the number to be dialed, same effect as ATDxxx;
  * @retval ret
  */
SC_CALLReturnCode callDialdemo()
{
    SC_CALLReturnCode ret = SC_CALL_SUCESS;
    char rsp_buff[100];
    SIM_MSG_T msg;
    __attribute__((__unused__)) SC_STATUS status;
    UINT8 dialstring[SC_CALL_MAX_NUMBER_LENGTH];
    char *note = "\r\nPlease input dial number: \r\n";
    PrintfResp(note);
    UartReadLine((char *)dialstring, SC_CALL_MAX_NUMBER_LENGTH);
    sAPI_Debug("--callDialdemo--, dialstring = [%s]", dialstring);

    ret = sAPI_CallDialMsg(dialstring, g_call_demo_msgQ);
    if (ret == SC_CALL_SUCESS)
    {
        memset(&msg, 0, sizeof(msg));
        status = sAPI_MsgQRecv(g_call_demo_msgQ, &msg, CALL_URC_RECIVE_TIME_OUT);
        sprintf(rsp_buff, "\r\nsAPI_CallDialMsg:\r\n\tresultCode[%d]\r\n", msg.arg2);
        sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
    }

    return ret;
}

/**
  * @brief  Voice call answer demo
  * @param  void
  * @note   same effect as ATA
  * @retval ret
  */
SC_CALLReturnCode callAnswerdemo()
{
    SC_CALLReturnCode ret = SC_CALL_SUCESS;
    char rsp_buff[100];
    SIM_MSG_T msg;
    __attribute__((__unused__)) SC_STATUS status;

    ret = sAPI_CallAnswerMsg(g_call_demo_msgQ);
    if (ret == SC_CALL_SUCESS)
    {
        memset(&msg, 0, sizeof(msg));
        status = sAPI_MsgQRecv(g_call_demo_msgQ, &msg, CALL_URC_RECIVE_TIME_OUT);
        sprintf(rsp_buff, "\r\nsAPI_CallAnswerMsg:\r\n\tresultCode[%d]\r\n", msg.arg2);
        sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
    }

    return ret;
}

/**
  * @brief  Voice call hung up demo
  * @param  void
  * @note   same effect as AT+CHUP
  * @retval ret
  */
SC_CALLReturnCode callEnddemo()
{
    SC_CALLReturnCode ret = SC_CALL_SUCESS;
    char rsp_buff[100];
    SIM_MSG_T msg;
    __attribute__((__unused__)) SC_STATUS status;

    ret = sAPI_CallEndMsg(g_call_demo_msgQ);
    if (ret == SC_CALL_SUCESS)
    {
        memset(&msg, 0, sizeof(msg));
        status = sAPI_MsgQRecv(g_call_demo_msgQ, &msg, CALL_URC_RECIVE_TIME_OUT);
        sprintf(rsp_buff, "\r\nsAPI_CallEndMsg:\r\n\tresultCode[%d]\r\n", msg.arg2);
        sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
    }

    return ret;
}

/**
  * @brief  Voice call state acquired demo
  * @param  void
  * @note
  * @retval ret
  */
SC_CALLReturnCode callStatedemo()
{
    SC_CALLReturnCode ret = SC_CALL_SUCESS;
    char rsp_buff[100];
    UINT8 callstate = 9;

    callstate = sAPI_CallStateMsg();

    sprintf(rsp_buff, "\r\nsAPI_CallStateMsg:\r\n\tcallstate[%d]\r\n", callstate);
    sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);

    return ret;
}

/**
  * @brief  Voice call auto answer demo
  * @param  void
  * @note
  * @retval ret
  */
SC_CALLReturnCode callAutoAnswerdemo()
{
    SC_CALLReturnCode ret = SC_CALL_SUCESS;
    char rsp_buff[100] = { 0 };
    SIM_MSG_T msg;
    INT32 seconds = 0;

    char *note = "\r\nPlease input delay seconds:(0-255) \r\n";
    PrintfResp(note);
    seconds = UartReadValue();
    sAPI_Debug("seconds [%d]", seconds);

    ret = sAPI_CallAutoAnswer(seconds, g_call_demo_msgQ);
    if (ret == SC_CALL_SUCESS)
    {
        memset(&msg, 0, sizeof(msg));
        sAPI_MsgQRecv(g_call_demo_msgQ, &msg, CALL_URC_RECIVE_TIME_OUT);
        sprintf(rsp_buff, "\r\nsAPI_CallAutoAnswer:\r\n\tresultCode[%d]\r\n", msg.arg2);
        sAPI_UartWriteString(SC_UART, (UINT8 *)rsp_buff);
    }

    return ret;
}

/**
  * @brief  Voice call operation demo
  * @param  void
  * @note
  * @retval void
  */
void CALLDemo(void)
{
    SC_CALLReturnCode ret;
    int resp_buff_len = 120;
    __attribute__((__unused__)) SC_STATUS status;
    //SIM_MSG_T msg;
    char rsp_buff[resp_buff_len];
    UINT32 opt = 0;

    char *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    char *options_list[] =
    {
        "1. Dial",
        "2. Answer",
        "3. End",
        "4. State",
        "5. Auto Answer",
        "99. Back"
    };

    status = sAPI_MsgQCreate(&g_call_demo_msgQ, "g_call_demo_msgQ", sizeof(SIM_MSG_T), 4, SC_FIFO);
    if (status != SC_SUCCESS)
    {
        sAPI_Debug("ERROR: message queue creat err!\n");
    }

    while (1)
    {
        memset(rsp_buff, 0, resp_buff_len);
        PrintfResp(note);
        PrintfOptionMenu(options_list, sizeof(options_list) / sizeof(options_list[0]));

        opt = UartReadValue();
        printf("opt = [%d]", (int) opt);

        switch (opt)
        {
            case SC_CALL_DEMO_DIAL:
            {
                ret = callDialdemo();
                if (ret == SC_CALL_FAIL)
                {
                    sAPI_Debug("%s: call dial error!", __func__);
                }
                break;
            }
            case SC_CALL_DEMO_ANSWER:
            {
                ret = callAnswerdemo();
                if (ret == SC_CALL_FAIL)
                {
                    sAPI_Debug("%s: call answer error!", __func__);
                }
                break;
            }
            case SC_CALL_DEMO_END:
            {
                ret = callEnddemo();
                if (ret == SC_CALL_FAIL)
                {
                    sAPI_Debug("%s: call end error!", __func__);
                }
                break;
            }
            case SC_CALL_DEMO_STATE:
            {
                ret = callStatedemo();
                if (ret == SC_CALL_FAIL)
                {
                    sAPI_Debug("%s: call get state error!", __func__);
                }
                break;
            }
            case SC_CALL_DEMO_AUTO_ANSWER:
            {
                ret = callAutoAnswerdemo();
                if (ret == SC_CALL_FAIL)
                {
                    sAPI_Debug("%s: call anto answer error!", __func__);
                }
                break;
            }
            case SC_CALL_DEMO_MAX:
            {
                sAPI_Debug("[CALL] Return to the previous menu!");
                PrintfResp("\r\nReturn to the previous menu!\r\n");
                sAPI_MsgQDelete(g_call_demo_msgQ);
                return;
            }
            default :
                break;
        }
    }
}

#ifdef CSD_SUPPORT
#define RECCSDDAT_TASK_STACK_SIZE (1024 * 2)
#define RECCSDDAT_TASK_PRIORITY (100)
static sMsgQRef g_apRecCsdDatMsgQ = NULL;
static sTaskRef gRecCsdDatTask = NULL;

void sTask_RecCsdDatProcesser(void *ptr)
{
	SC_RecCsdDat *p_Csdinfo=NULL;
    SIM_MSG_T msg={0,0,0,NULL};
	while(1)
	{
        if (sAPI_MsgQRecv(g_apRecCsdDatMsgQ, &msg, SC_SUSPEND) != SC_SUCCESS)
        {
            continue;
        }
		p_Csdinfo = (SC_RecCsdDat *)msg.arg3;
		{
			switch(p_Csdinfo->dattype)
			{
				case SC_REC_CSDDAT:
				{
					sAPI_Debug("RecCsD: len [%d]", p_Csdinfo->len);
					int i =0;
					for(i=0;i<p_Csdinfo->len;i++)
					{
						sAPI_Debug("RecCsD: [%02X]", p_Csdinfo->data[i]);
					}
					sAPI_Free(p_Csdinfo);
					break;
				}

		        default:
		            sAPI_Debug("default process!!");
		            break;
			}
		}
	}
}
void RecCsdDatdemo(void)
{
    SC_STATUS osStatus = 1; // fail
    osStatus = sAPI_MsgQCreate(&g_apRecCsdDatMsgQ, "RecCsdDatMsgQ", sizeof(SIM_MSG_T), 2, SC_FIFO);
    if (0 != osStatus)
    {
        return;
    }
    if (osStatus == 0)
    {
		if (NULL != gRecCsdDatTask)
		{
			return;
		}
		if (sAPI_TaskCreate(&gRecCsdDatTask,
							NULL,
							RECCSDDAT_TASK_STACK_SIZE,
							RECCSDDAT_TASK_PRIORITY,
							(char *)"Rec Csd Data task",
							sTask_RecCsdDatProcesser,
							(void *)0) != SC_SUCCESS)
		{
			gRecCsdDatTask = NULL;
		}
		else
			sAPI_apMsgQInit(g_apRecCsdDatMsgQ);
    }
}
#endif

