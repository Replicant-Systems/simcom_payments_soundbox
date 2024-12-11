/**
  ******************************************************************************
  * @file    simcom_demo.c
  * @author  SIMCom OpenSDK Team
  * @brief   Source code for all OpenSDK demo task management with UI.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 SIMCom Wireless.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "userspaceConfig.h"

#include "simcom_demo.h"
#include "simcom_os.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "simcom_debug.h"
#include "simcom_common.h"
#ifdef HAS_USB
#include "simcom_usb_vcom.h"
#endif
#ifdef HAS_UART
#include "simcom_uart.h"
#include "uart_api.h"
#endif
#include "simcom_simcard.h"
#include "simcom_tcpip.h"
#include "simcom_gpio.h"
#include "A7680C_V801_GPIO.h"
#include "simcom_network.h"
#include "sc_power.h"
#include "simcom_file.h"
#include "simcom_audio.h"

sTaskRef simcomUIProcesser;
static SC_DEMO_T gDemoList[SC_DEMO_MAX];
UINT8 NetworkConnectInit(void);
extern  void MqttDemo(void);
extern void sAPP_HelloWorldDemo(void);
sTaskRef Audio_taskProcesser;
static UINT8 Audio_taskProcesserStack[1024*40];
unsigned int mqtt_received=0;
unsigned int mqtt_value;
int volume =4,volume_changed=1;
void scDemoListInit(void)
{
#ifdef HAS_DEMO_NETWORK
    gDemoList[SC_DEMO_FOR_NETWORK].description = "NETWORK";
    gDemoList[SC_DEMO_FOR_NETWORK].cb = NetWorkDemo;
#endif
#ifdef HAS_DEMO_SIMCARD
    gDemoList[SC_DEMO_FOR_SIMCARD].description = "SIMCARD";
    gDemoList[SC_DEMO_FOR_SIMCARD].cb = SimcardDemo;
#endif
#ifdef HAS_DEMO_SMS
    gDemoList[SC_DEMO_FOR_SMS].description = "SMS";
    gDemoList[SC_DEMO_FOR_SMS].cb = SMSDemo;
#endif
#ifdef HAS_DEMO_UART
    gDemoList[SC_DEMO_FOR_UART].description = "UART";
    gDemoList[SC_DEMO_FOR_UART].cb = UartDemo;
#endif
#ifdef HAS_DEMO_USB
    gDemoList[SC_DEMO_FOR_USB].description = "USB";
    gDemoList[SC_DEMO_FOR_USB].cb = NULL;
#endif
#ifdef HAS_DEMO_GPIO
    gDemoList[SC_DEMO_FOR_GPIO].description = "GPIO";
    gDemoList[SC_DEMO_FOR_GPIO].cb = GpioDemo;
#endif
#ifdef HAS_DEMO_PMU
    gDemoList[SC_DEMO_FOR_PMU].description = "PMU";
    gDemoList[SC_DEMO_FOR_PMU].cb = PMUDemo;
#endif
#ifdef HAS_DEMO_I2C
    gDemoList[SC_DEMO_FOR_I2C].description = "I2C";
    gDemoList[SC_DEMO_FOR_I2C].cb = I2cDemo;
#endif
#ifdef HAS_DEMO_AUDIO
    gDemoList[SC_DEMO_FOR_AUDIO].description = "AUDIO";
    gDemoList[SC_DEMO_FOR_AUDIO].cb = AudioDemo;
#endif
#ifdef HAS_DEMO_FS
    gDemoList[SC_DEMO_FOR_FILE_SYSTEM].description = "FILE SYSTEM";
    gDemoList[SC_DEMO_FOR_FILE_SYSTEM].cb = FsDemo;
#endif
#ifdef HAS_DEMO_TCPIP
    gDemoList[SC_DEMO_FOR_TCP_IP].description = "TCPIP";
    gDemoList[SC_DEMO_FOR_TCP_IP].cb = TcpipDemo;
#endif
#ifdef HAS_DEMO_HTTPS
    gDemoList[SC_DEMO_FOR_HTTP_HTTPS].description = "HTTPS";
    gDemoList[SC_DEMO_FOR_HTTP_HTTPS].cb = HttpsDemo;
#endif
#ifdef HAS_DEMO_FTPS
    gDemoList[SC_DEMO_FOR_FTP_FTPS].description = "FTPS";
    gDemoList[SC_DEMO_FOR_FTP_FTPS].cb = FtpsDemo;
#endif
#ifdef HAS_DEMO_MQTTS
    gDemoList[SC_DEMO_FOR_MQTT_MQTTS].description = "MQTTS";
    gDemoList[SC_DEMO_FOR_MQTT_MQTTS].cb = MqttDemo;
#endif
#ifdef HAS_DEMO_SSL
    gDemoList[SC_DEMO_FOR_SSL].description = "SSL";
    gDemoList[SC_DEMO_FOR_SSL].cb = SslDemo;
#endif
#ifdef HAS_DEMO_OTA
    gDemoList[SC_DEMO_FOR_OTA].description = "FOTA";
    gDemoList[SC_DEMO_FOR_OTA].cb = FotaDemo;
#endif
#ifdef HAS_DEMO_LBS
    gDemoList[SC_DEMO_FOR_LBS].description = "LBS";
    gDemoList[SC_DEMO_FOR_LBS].cb = LbsDemo;
#endif

#ifdef HAS_DEMO_SJDR
    gDemoList[SC_DEMO_FOR_SJDR].description = "JAMMING DETECT";
    gDemoList[SC_DEMO_FOR_SJDR].cb = JamDectDemo;
#endif

#ifdef HAS_DEMO_NTP
    gDemoList[SC_DEMO_FOR_NTP].description = "NTP";
    gDemoList[SC_DEMO_FOR_NTP].cb = NtpDemo;
#endif
#ifdef HAS_DEMO_HTP
    gDemoList[SC_DEMO_FOR_HTP].description = "HTP";
    gDemoList[SC_DEMO_FOR_HTP].cb = HtpDemo;
#endif
#ifdef HAS_DEMO_INTERNET_SERVICE
    gDemoList[SC_DEMO_FOR_INTERNET_SERVICE].description = "INTERNET_SERVICE";
    gDemoList[SC_DEMO_FOR_INTERNET_SERVICE].cb = NULL;
#endif
#ifdef HAS_DEMO_TTS
    gDemoList[SC_DEMO_FOR_TTS].description = "TTS";
    gDemoList[SC_DEMO_FOR_TTS].cb = TTSDemo;
#endif
#ifdef HAS_DEMO_CALL
    gDemoList[SC_DEMO_FOR_CALL].description = "CALL";
    gDemoList[SC_DEMO_FOR_CALL].cb = CALLDemo;
#endif
#ifdef HAS_DEMO_WIFI
    gDemoList[SC_DEMO_FOR_WIFI].description = "WIFI";
    gDemoList[SC_DEMO_FOR_WIFI].cb = WIFIDemo;
#endif
#ifdef HAS_DEMO_GNSS
    gDemoList[SC_DEMO_FOR_GNSS].description = "GNSS";
    gDemoList[SC_DEMO_FOR_GNSS].cb = GNSSDemo;
#endif
#ifdef HAS_DEMO_LCD
    gDemoList[SC_DEMO_FOR_LCD].description = "LCD";
    gDemoList[SC_DEMO_FOR_LCD].cb = LcdDemo;
#endif
#ifdef HAS_DEMO_RTC
    gDemoList[SC_DEMO_FOR_RTC].description = "RTC";
    gDemoList[SC_DEMO_FOR_RTC].cb = RTCDemo;
#endif
#ifdef HAS_DEMO_FLASH
    gDemoList[SC_DEMO_FOR_FLASH].description = "FLASH";
    gDemoList[SC_DEMO_FOR_FLASH].cb = FlashRWdemo;
#endif
#ifdef HAS_DEMO_FS_OLD
    gDemoList[SC_DEMO_FOR_FILE_SYSTEM_OLD].description = "FILE SYSTEM Compatible API";
    gDemoList[SC_DEMO_FOR_FILE_SYSTEM_OLD].cb = NULL;
#endif
#ifdef HAS_DEMO_SPI
    gDemoList[SC_DEMO_FOR_SPI].description = "SPI";
    gDemoList[SC_DEMO_FOR_SPI].cb = SpiDemo;

    gDemoList[SC_DEMO_FOR_SPI_NOR].description = "SPI_NOR";
    gDemoList[SC_DEMO_FOR_SPI_NOR].cb = SpiNorDemo;

    gDemoList[SC_DEMO_FOR_SPI_NAND].description = "SPI_NAND";
    gDemoList[SC_DEMO_FOR_SPI_NAND].cb = SpiNandDemo;
#endif
#ifdef HAS_DEMO_CAM
    gDemoList[SC_DEMO_FOR_CAM].description = "CAM";
    gDemoList[SC_DEMO_FOR_CAM].cb = CamDemo;
#endif
#ifdef HAS_DEMO_SYS
    gDemoList[SC_DEMO_FOR_SYS].description = "SYS";
    gDemoList[SC_DEMO_FOR_SYS].cb = SysDemo;
#endif
#ifdef HAS_DEMO_BLE
    gDemoList[SC_DEMO_FOR_BLE].description = "BLE";
    gDemoList[SC_DEMO_FOR_BLE].cb = BLEDemo;
#endif
#ifdef HAS_DEMO_BT
    gDemoList[SC_DEMO_FOR_BT].description = "BT";
    gDemoList[SC_DEMO_FOR_BT].cb = BTDemo;
#endif
#ifdef HAS_DEMO_BT_STACK
    gDemoList[SC_DEMO_FOR_BTSTACK].description = "BT/BLE";
    gDemoList[SC_DEMO_FOR_BTSTACK].cb = BTDemo;
#endif

#ifdef HAS_DEMO_APP_DOWNLOAD
    gDemoList[SC_DEMO_FOR_APP_DOWNLOAD].description = "APP_DOWNLOAD";
    gDemoList[SC_DEMO_FOR_APP_DOWNLOAD].cb = AppDownloadDemo;
#endif
#ifdef HAS_DEMO_APP_UPDATE
    gDemoList[SC_DEMO_FOR_APP_UPDATE_FOR_NVM].description = "APP_UPDATE_FOR_NVM";
    gDemoList[SC_DEMO_FOR_APP_UPDATE_FOR_NVM].cb = AppUpdateDemo;
#endif
#ifdef HAS_DEMO_PWM
    gDemoList[SC_DEMO_FOR_PWM].description = "PWM";
    gDemoList[SC_DEMO_FOR_PWM].cb = PwmDemo;
#endif
#ifdef HAS_DEMO_POC
    gDemoList[SC_DEMO_FOR_POC].description = "POC";
    gDemoList[SC_DEMO_FOR_POC].cb = POCDemo;
#endif
#ifdef HAS_DEMO_WTD
    gDemoList[SC_DEMO_FOR_WTD].description = "WTD";
    gDemoList[SC_DEMO_FOR_WTD].cb = WTDDemo;
#endif
#ifdef HAS_DEMO_PING
    gDemoList[SC_DEMO_FOR_PING].description = "PING";
    gDemoList[SC_DEMO_FOR_PING].cb = PingDemo;
#endif
#ifdef HAS_SM2
    gDemoList[SC_DEMO_FOR_SM2].description = "SM2";
    gDemoList[SC_DEMO_FOR_SM2].cb = SM2Demo;
#endif
#ifdef HAS_ZLIB
    gDemoList[SC_DEMO_FOR_ZLIB].description = "ZLIB";
    gDemoList[SC_DEMO_FOR_ZLIB].cb = ZLIBDemo;
#endif
#ifdef HAS_CJSON
    gDemoList[SC_DEMO_FOR_CJSON].description = "CJSON";
    gDemoList[SC_DEMO_FOR_CJSON].cb = CjsonDemo;
#endif
#ifdef HAS_MBEDTLS
    gDemoList[SC_DEMO_FOR_MBEDTLS].description = "MBEDTLS";
    gDemoList[SC_DEMO_FOR_MBEDTLS].cb = MbedTLSDemo;
#endif

#ifdef HAS_DEMO_EXFLASHMOUNTFS
    gDemoList[SC_DEMO_FOR_MBEDTLS].description = "exflashmountfs";
    gDemoList[SC_DEMO_FOR_MBEDTLS].cb = exflashmountfsDemo;
#endif

#ifdef HAS_DEMO_ONEWIRE
    gDemoList[SC_DEMO_FOR_ONEWIRE].description = "ONE_WIRE";
    gDemoList[SC_DEMO_FOR_ONEWIRE].cb = OneWireDemo;
#endif

}

/**
  * @brief  Print string to UART1 or USB AT port.
  * @param  format,data pointer
  * @note   Please define SIMCOM_UI_DEMO_TO_UART1_PORT or SIMCOM_UI_DEMO_TO_USB_AT_PORT in advance.
  * @retval void
  */
void PrintfResp(char *format)
{
#ifdef SIMCOM_UI_DEMO_TO_UART1_PORT
    #ifdef HAS_UART
        #if (defined SIMCOM_A7680C_V5_01) || (defined SIMCOM_A7670C_V701) || (defined SIMCOM_A7670C_V702) || defined (SIMCOM_A7680C_V506) || defined (SIMCOM_A7680C_V603) || defined (SIMCOM_A7605C1_V201) || defined(SIMCOM_A7673G_V201) || defined (SIMCOM_A7680C_V801)
            sAPI_UartWrite(SC_UART4, (UINT8 *)format, strlen(format));
        #else
            sAPI_UartWrite(SC_UART, (UINT8 *)format, strlen(format));
        #endif
    #endif
#else
    #ifdef HAS_USB
        sAPI_UsbVcomWrite((UINT8 *)format, strlen(format));
    #endif
#endif
}

void PrintfRespData(char *buff, UINT32 length)
{
#ifdef SIMCOM_UI_DEMO_TO_UART1_PORT
    #ifdef HAS_UART
        #if (defined SIMCOM_A7680C_V5_01) || (defined SIMCOM_A7670C_V701) || (defined SIMCOM_A7670C_V702) || defined (SIMCOM_A7680C_V506) || defined (SIMCOM_A7680C_V603) || defined (SIMCOM_A7605C1_V201) || defined (SIMCOM_A7673G_V201) || defined (SIMCOM_A7680C_V801)
            sAPI_UartWrite(SC_UART4, (UINT8 *)buff, length);
        #else
            sAPI_UartWrite(SC_UART, (UINT8 *)buff, length);
        #endif
    #endif
#else
    #ifdef HAS_USB
        sAPI_UsbVcomWrite((UINT8 *)buff, length);
    #endif
#endif
}

void PrintfRespHexData(char *buff, UINT32 length)
{
    int i = 0;
    char *buff_hex = NULL;
    char tem[3] = {0};

    buff_hex = malloc(length * 2);
    if (NULL == buff_hex)
    {
        return;
    }
    memset(buff_hex, 0, length * 2);

    for (i = 0; i < length; i++)
    {
        snprintf(tem, 3, "%.2x", buff[i]);
        memcpy(&buff_hex[i * 2], tem, 2);
    }

#ifdef SIMCOM_UI_DEMO_TO_UART1_PORT
    #ifdef HAS_UART
        #if (defined SIMCOM_A7680C_V5_01) || (defined SIMCOM_A7670C_V701) || (defined SIMCOM_A7670C_V702) || defined (SIMCOM_A7680C_V506) || defined (SIMCOM_A7680C_V603) || defined (SIMCOM_A7605C1_V201) || defined (SIMCOM_A7680C_V801)
            sAPI_UartWrite(SC_UART4, (UINT8 *)buff_hex, length * 2);
        #else
            sAPI_UartWrite(SC_UART, (UINT8 *)buff_hex, length * 2);
        #endif
    #endif
#else
    #ifdef HAS_USB
        sAPI_UsbVcomWrite((UINT8 *)buff_hex, length * 2);
    #endif
#endif
}

/**
  * @brief  Print operation menu.
  * @param  options_list,operation list
  * @param  array_size,size of options_list
  * @note   Please define SIMCOM_UI_DEMO_TO_UART1_PORT or SIMCOM_UI_DEMO_TO_USB_AT_PORT in advance.
  * @retval void
  */
void PrintfOptionMenu(char *options_list[], int array_size)
{
    UINT32 i = 0;
    sAPI_Debug("array_size = [%d]", array_size);
    char menu[80] = {0};
    PrintfResp("\r\n************************************************************\r\n");
    for (i = 0; i < (array_size / 2); i++)
    {
        memset(menu, 0, 80);
        snprintf(menu, 80, "%-30s%-30s", options_list[2 * i], options_list[2 * i + 1]);
        PrintfResp(menu);
        PrintfResp("\r\n");
    }

    if (array_size % 2 != 0)
    {
        memset(menu, 0, 80);
        snprintf(menu, 80, "%s", options_list[array_size - 1]);
        PrintfResp(menu);
        PrintfResp("\r\n");
    }
    PrintfResp("************************************************************\r\n");

}

void PrintfMainMenu(void)
{
    int i = 0;
    int counter = 0;
    char menu[40] = {0};

    PrintfResp("\r\n************************************************************\r\n");
    for (i = 0; i < SC_DEMO_MAX; i++)
    {
        if (gDemoList[i].cb != NULL)
        {
            memset(menu, 0, 40);
            snprintf(menu, 40, "%2d. %-30s", i, gDemoList[i].description);
            PrintfResp(menu);
            counter++;
        }

        if (counter >= 2)
        {
            PrintfResp("\r\n");
            counter = 0;
        }
    }
    PrintfResp("************************************************************\r\n");
}
UINT8 NetworkConnectInit(void)
{
    UINT8 csq;
    UINT8 ret;
    int creg, cgreg;
    int cnmp;
    SCcpsiParm Scpsi = { 0 };
    char NetResp[300] = {0};
    char cops[200] = {0};
    SCAPNact SCact[8] = {0};
    SCcgpaddrParm cgpaddrParm;
    SCApnParm SCapn[8] = {0};
    PrintfResp("\r\nNetworkConnectInit\r\n");
    sAPI_GpioSetValue(SC_MODULE_GPIO_09, 0);
    while(1)
    while(1)
    {
        sAPI_NetworkInit();
        ret = sAPI_NetworkGetCnmp(&cnmp);
        if (ret == SC_NET_SUCCESS)
        {
            sAPI_Debug("Get cnmp success. cnmp=%d!", cnmp);
            memset(NetResp, 0, sizeof(NetResp));
            sprintf(NetResp, "\r\nGet cnmp success. cnmp:%d!\r\n", cnmp);
            PrintfResp(NetResp);
        }
        else
        {
            sAPI_Debug("Get cnmp falied!");
            PrintfResp("\r\nGet cnmp falied!\r\n");
        }
        int z=10;
        while (z--)
        {
            if(sAPI_NetworkGetCsq(&csq) == SC_NET_SUCCESS)
            {
                sAPI_Debug("Get csq success. csq=%d!", csq);
                memset(NetResp, 0, sizeof(NetResp));
                sprintf(NetResp, "\r\nGet csq success. csq:%d!\r\n", csq);
                PrintfResp(NetResp);
            }
            else
            {
                sAPI_Debug("Get csq falied!");
                PrintfResp("\r\nGet csq falied!\r\n");
            }
            if((csq>10)&&(csq<32))
            {
                sAPI_GpioSetValue(SC_MODULE_GPIO_09, 1);
                break;
            }
            else sAPI_DelayUs(2000000);
        }
        ret = sAPI_NetworkGetCreg(&creg);
        if (ret == SC_NET_SUCCESS)
        {
            sAPI_Debug("Get creg success. creg=%d!", creg);
            memset(NetResp, 0, sizeof(NetResp));
            sprintf(NetResp, "\r\nGet creg success. creg=%d!\r\n", creg);
            PrintfResp(NetResp);
        }
        else
        {
            sAPI_Debug("Get creg falied!");
            PrintfResp("\r\nGet creg falied!\r\n");
        }
        ret = sAPI_NetworkGetCgreg(&cgreg);
        if (ret == SC_NET_SUCCESS)
        {
            sAPI_Debug("Get cgreg success. cgreg=%d!", cgreg);
            memset(NetResp, 0, sizeof(NetResp));
            sprintf(NetResp, "\r\nGet cgreg success. cgreg=%d!\r\n", cgreg);
            PrintfResp(NetResp);
        }
        else
        {
            sAPI_Debug("Get cgreg falied!");
            PrintfResp("\r\nGet cgreg falied!\r\n");
        }
        memset(&Scpsi, 0, sizeof(Scpsi));
        ret = sAPI_NetworkGetCpsi(&Scpsi);
        if (ret == SC_NET_SUCCESS)
        {
            sAPI_Debug("Get cpsi success. NEmode=%s,MM=%s,LAC=%d,CELL=%d,Gband=%s,Lband=%s,TAC=%d,RSRP=%d,RXLEV=%d,TA=%d!",
                        Scpsi.networkmode, Scpsi.Mnc_Mcc, Scpsi.LAC, Scpsi.CellID, Scpsi.GSMBandStr, Scpsi.LTEBandStr, Scpsi.TAC, Scpsi.Rsrp,
                        Scpsi.RXLEV, Scpsi.TA);
            memset(NetResp, 0, sizeof(NetResp));
            sprintf(NetResp,
                    "\r\nGet cpsi success. NEmode=%s,MM=%s,LAC=%d,CELL=%d,Gband=%s,Lband=%s,TAC=%d,RSRP=%d,RXLEV=%d,TA=%d,SINR=%d!\r\n",
                    Scpsi.networkmode, Scpsi.Mnc_Mcc, Scpsi.LAC, Scpsi.CellID, Scpsi.GSMBandStr, Scpsi.LTEBandStr, Scpsi.TAC, Scpsi.Rsrp,
                    Scpsi.RXLEV, Scpsi.TA, Scpsi.SINR);
            PrintfResp(NetResp);
        }
        else
        {
            sAPI_Debug("Get cpsi falied!");
            PrintfResp("\r\nGet cpsi falied!\r\n");
        }
        memset(cops, 0, sizeof(cops));
        ret = sAPI_NetworkGetCops(cops);
        if (ret == SC_NET_SUCCESS)
        {
            sAPI_Debug("Get cops success. cops=%s!", cops);
            memset(NetResp, 0, sizeof(NetResp));
            sprintf(NetResp, "\r\nGet cops success. cops=%s!\r\n", cops);
            PrintfResp(NetResp);
        }
        else
        {
            sAPI_Debug("Get cops falied!");
            PrintfResp("\r\nGet cops falied!\r\n");
        }
        memset(SCact, 0, sizeof(SCact));
        ret = sAPI_NetworkGetCgact(SCact);
        if (ret == SC_NET_SUCCESS)
        {
            for (int i = 0; i < 8; i++) /*Read it eight times*/
            {
                sAPI_Debug("Get cgact success. cid=%d,actstate=%d!", SCact[i].cid, SCact[i].isActived);
                if (SCact[i].isdefine)
                {
                    memset(NetResp, 0, sizeof(NetResp));
                    sprintf(NetResp, "\r\n cid=%d, actstate=%d\r\n", SCact[i].cid, SCact[i].isActived);
                    PrintfResp(NetResp);
                }
            }
        }
        else
        {
            sAPI_Debug("Get cgact falied!");
            PrintfResp("\r\nGet cgact falied!\r\n");
        }
        ret = sAPI_NetworkSetCgact(1, 1);
        if (ret == SC_NET_SUCCESS)
        {
            sAPI_Debug("Set cgactt success.!");
            PrintfResp("\r\nSet cgact success.!\r\n");
        }
        else
        {
            sAPI_Debug("Set cgact falied!");
            PrintfResp("\r\nSet cgact falied!\r\n");
            break;
        }
        sAPI_GpioSetValue(SC_MODULE_GPIO_09, 0);
        ret = sAPI_NetworkGetCgdcont(SCapn);
        if (ret == SC_NET_SUCCESS)
        {
            for (int i = 0; i < 8; i++) /*Read it eight times*/
            {
                sAPI_Debug("Get Cgdcont success. cid=%d,iptype=%d,apnstr=%s!", SCapn[i].cid, SCapn[i].iptype, SCapn[i].ApnStr);
                if (SCapn[i].iptype) /*Iptype 0 means undefined*/
                {
                    memset(NetResp, 0, sizeof(NetResp));
                    sprintf(NetResp, "\r\n cid=%d,iptype=%d,apnstr=%s \r\n", SCapn[i].cid, SCapn[i].iptype, SCapn[i].ApnStr);
                    PrintfResp(NetResp);
                }
            }
        }
        else
        {
            sAPI_Debug("Get cgdcont falied!");
            PrintfResp("\r\nGet cgdcont falied!\r\n");
            break;
        }
        sAPI_DelayUs(1000000);
        sAPI_NetworkSetCgdcont(1, "IP", "airtelgprs.com");
        if (ret == SC_NET_SUCCESS)
        {
            sAPI_Debug("Set cgdcont success.!");
            PrintfResp("\r\nSet cgdcont success.!\r\n");
        }
        else
        {
            sAPI_Debug("Set cgdcont falied!");
            PrintfResp("\r\nSet cgdcont falied!\r\n");
            break;
        }

        sAPI_DelayUs(2000000*1);
        memset(&cgpaddrParm, 0, sizeof(cgpaddrParm));
        ret = sAPI_NetworkGetCgpaddr(1, &cgpaddrParm);
        if (ret == SC_NET_SUCCESS)
        {
            sAPI_Debug("Get Ipaddr success. cid=%d,type=%d,ipv4=%s,ipv6=%s!", cgpaddrParm.cid, cgpaddrParm.iptype,
                        cgpaddrParm.ipv4addr, cgpaddrParm.ipv6addr);
            memset(NetResp, 0, sizeof(NetResp));
            sprintf(NetResp, "\r\nGet Ipaddr success. cid=%d,type=%d,ipv4=%s,ipv6=%s!", cgpaddrParm.cid, cgpaddrParm.iptype,
                    cgpaddrParm.ipv4addr, cgpaddrParm.ipv6addr);
            PrintfResp(NetResp);
            sAPI_GpioSetValue(SC_MODULE_GPIO_09, 1);
            return ret;
        }
        else
        {
            sAPI_Debug("Get Ipaddr falied!");
            PrintfResp("\r\nGet Ipaddr falied!\r\n");
            break;
        }
    }
    return ret;
}
void GPIO_IntHandlerRPT   (void)
{
    sAPI_Debug("\r\n GPIO_IntHandlerRPT led1 \r\n");
    PrintfResp("\r\nGPIO_IntHandlerRPT led1\r\n");
    mqtt_received=1;
}
void GPIO_IntHandlerVplus (void)
{
    sAPI_Debug("\r\n GPIO_IntHandlerVplus led2 \r\n");
    PrintfResp("\r\nGPIO_IntHandlerVplus led2\r\n");
    if(volume<7)
    {
        volume++;
        volume_changed=1;
    }
}
void GPIO_IntHandlerVminus(void)
{
    sAPI_Debug("\r\n GPIO_IntHandlerVminus led3 \r\n");
    PrintfResp("\r\nGPIO_IntHandlerVminus led3\r\n");
    if(volume>0)
    {
        volume--;
        volume_changed=1;
    }
}
void GpioInit()
{
    int ret;
    SC_GPIOConfiguration led;
	SC_GPIOReturnCode retValue;

    //For LED Output
    led.initLv = 0;
    led.isr =  NULL;
    led.pinDir = SC_GPIO_OUT_PIN;
    led.pinEd = SC_GPIO_NO_EDGE;
    led.wu = NULL; //GPIO_WakeupHandler;
    led.pinPull = SC_GPIO_PULLUP_ENABLE;

    retValue = sAPI_GpioConfig(SC_MODULE_GPIO_12, led); //led1
    if(retValue != SC_GPIORC_OK)
    {   sAPI_Debug("\r\n Config led1 (Output) failed \r\n");
        PrintfResp("\r\nConfig led1 (Output) failed !\r\n");
    }
    sAPI_GpioSetValue(SC_MODULE_GPIO_12, 0);

    retValue = sAPI_GpioConfig(SC_MODULE_GPIO_08, led);  //netmode
    if(retValue != SC_GPIORC_OK)
    {  sAPI_Debug("\r\n Config netmode (Output) failed \r\n");
        PrintfResp("\r\nConfig netmode (Output) failed !\r\n");
    }
    sAPI_GpioSetValue(SC_MODULE_GPIO_08, 0);

    retValue = sAPI_GpioConfig(SC_MODULE_GPIO_09, led);  //netstatus
    if(retValue != SC_GPIORC_OK)
    {  sAPI_Debug("\r\n Config netstatus (Output) failed \r\n");
        PrintfResp("\r\nConfig netstatus (Output) failed !\r\n");
    }
    sAPI_GpioSetValue(SC_MODULE_GPIO_09, 0);

    retValue = sAPI_GpioConfig(11, led);  //led2
    if(retValue != SC_GPIORC_OK)
    {  sAPI_Debug("\r\n Config led2 (Output) failed \r\n");
        PrintfResp("\r\nConfig led2 (Output) failed !\r\n");
    }
    sAPI_GpioSetValue(11, 0);

    retValue = sAPI_GpioConfig(10, led);  //led3
    if(retValue != SC_GPIORC_OK)
    {  sAPI_Debug("\r\n Config led3 (Output) failed \r\n");
        PrintfResp("\r\nConfig led3 (Output) failed !\r\n");
    }
    sAPI_GpioSetValue(10, 0);


    ret = sAPI_GpioSetDirection(4, 0); //RPT
    if (ret != SC_GPIORC_OK)
    {
        printf("sAPI_setGpioDirection RPT:    failed.");
        PrintfResp("\r\nsAPI_setGpioDirection RPT:    failed. !\r\n");
    }
    ret = sAPI_GpioConfigInterrupt(4, SC_GPIO_RISE_EDGE, GPIO_IntHandlerRPT);
    if (ret != SC_GPIORC_OK)
    {
        printf("sAPI_GpioConfigInterrupt RPT:    failed.");
        PrintfResp("\r\nsAPI_GpioConfigInterrupt RPT:    failed !\r\n");
    }
    led.initLv = 0;
    led.isr =  NULL;
    led.pinDir = SC_GPIO_IN_PIN;
    led.pinEd = SC_GPIO_FALL_EDGE;
    led.wu = NULL;
    led.pinPull = SC_GPIO_PULLDN_ENABLE;
    retValue = sAPI_GpioConfig(53, led);  //netmode
    if(retValue != SC_GPIORC_OK)
    {  sAPI_Debug("\r\n Config led2 (input) failed \r\n");
        PrintfResp("\r\nConfig led2 (input) failed !\r\n");
    }
    ret = sAPI_GpioSetDirection(53, 0); //Vplus
    if (ret != SC_GPIORC_OK)
    {
        printf("sAPI_setGpioDirection Vplus:    failed.");
        PrintfResp("\r\nsAPI_setGpioDirection Vplus:    failed. !\r\n");
    }
    ret = sAPI_GpioConfigInterrupt(53, SC_GPIO_FALL_EDGE, GPIO_IntHandlerVplus);
    if (ret != SC_GPIORC_OK)
    {
        printf("sAPI_GpioConfigInterrupt Vplus:    failed.");
        PrintfResp("\r\nsAPI_GpioConfigInterrupt Vplus:    failed !\r\n");
    }
    ret = sAPI_GpioSetDirection(54, 0); //Vminus
    if (ret != SC_GPIORC_OK)
    {
        printf("sAPI_setGpioDirection Vminus:    failed.");
        PrintfResp("\r\nsAPI_setGpioDirection Vminus:    failed. !\r\n");
    }
    ret = sAPI_GpioConfigInterrupt(54, SC_GPIO_RISE_EDGE, GPIO_IntHandlerVminus);
    if (ret != SC_GPIORC_OK)
    {
        printf("sAPI_GpioConfigInterrupt Vminus:    failed.");
        PrintfResp("\r\nsAPI_GpioConfigInterrupt Vminus:    failed !\r\n");
    }
    PrintfResp("\r\n gpio init successful!\r\n");
}


const char *units[] = {"", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
const char *teens[] = {"ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"};
const char *tens[] = {"", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};
char output_file[14] = "C:/temp.mp3";
#define BUFFER_SIZE 4096
#define MAX_WORDS 50
#define MAX_WORD_LENGTH 30

// Function to convert number to words
void number_to_words(int num, char *result) {
    if (num < 0 || num > 99999) {
        strcpy(result, "Number out of range");
        return;
    }

    result[0] = '\0';  // Initialize the result string

    if (num >= 1000) {
        int thousands = num / 1000;
        num %= 1000;
        if (thousands >= 10 && thousands < 20) {
            strcat(result, teens[thousands - 10]);
        } else {
            strcat(result, tens[thousands / 10]);
            if (thousands % 10 != 0) {
                // strcat(result, "-");
				strcat(result, " ");
                strcat(result, units[thousands % 10]);
            }
        }
        strcat(result, " thousand");
        if (num > 0) strcat(result, " ");
    }

    if (num >= 100) {
        int hundreds = num / 100;
        num %= 100;
        strcat(result, units[hundreds]);
        strcat(result, " hundred");
        if (num > 0) strcat(result, " and ");
    }

    if (num >= 20) {
        strcat(result, tens[num / 10]);
        if (num % 10 > 0) {
            // strcat(result, "-");
			strcat(result, " ");
            strcat(result, units[num % 10]);
        }
    } else if (num >= 10) {
        strcat(result, teens[num - 10]);
    } else if (num > 0) {
        strcat(result, units[num]);
    } else if (strlen(result) == 0) {
        strcpy(result, "zero");
    }
}
void test_play_file(int mqttvalue)
{
    char NetResp[250] = {0};
    char result[200];  // Buffer to hold the result
    char *words[MAX_WORDS];
    int word_count = 0;

    // Convert number to words
    number_to_words(mqttvalue, result);
    sprintf(NetResp, "The number in words is: %s \r\n", result);
    PrintfResp(NetResp);

    // Trim leading/trailing spaces
    char *trimmed_result = strtok(result, "\r\n"); // Handle newlines if present
    if (trimmed_result != NULL) {
        // Remove leading spaces
        while (*trimmed_result == ' ') trimmed_result++;
        // Remove trailing spaces
        char *end = trimmed_result + strlen(trimmed_result) - 1;
        while (end > trimmed_result && *end == ' ') end--;
        *(end + 1) = '\0'; // Null-terminate the trimmed string
        // Tokenize
        char *token = strtok(trimmed_result, " ");
        while (token != NULL) {
            words[word_count] = malloc(strlen(token) + 1);
            if (words[word_count] == NULL) {
                //fprintf(stderr, "Memory allocation failed\n");
                PrintfResp("Memory allocation failed");
                // Free previously allocated words before returning
                for (int i = 0; i < word_count; i++) {
                    free(words[i]);
                }
                return;
            }
            strcpy(words[word_count], token);
            word_count++;
            token = strtok(NULL, " ");
        }
    }

    SCFILE *ot_fp = sAPI_fopen(output_file, "wb");
    if (!ot_fp) {
        //perror("Failed to open output file");
        PrintfResp("Failed to open ot_fp file");
        //return;
    }
    sAPI_fclose(ot_fp);
    SCFILE *output_fp = sAPI_fopen(output_file, "wb");
    if (!output_fp) {
        //perror("Failed to open output file");
        PrintfResp("Failed to open output file");
        //return;
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;

    for (int i = 0; i < word_count; i++) 
    {
        char filename[40]; // Adjust size as needed
		if(i==0)
		{
			SCFILE *input_fp = sAPI_fopen("D:/audio/rupees.mp3", "rb");
			if (!input_fp) {
				//fprintf(stderr, "Failed to open input file: %s\n", filename);
                PrintfResp("Failed to open input file");
				continue; // Skip to the next file if this one fails
			}

				// Copy data from input file to output file
			while ((bytes_read = sAPI_fread(buffer, 1, BUFFER_SIZE, input_fp)) > 0) {
				sAPI_fwrite(buffer, 1, bytes_read, output_fp);
			}

			sAPI_fclose(input_fp); // Close the input file
		}
        if(mqtt_value)
        {
            snprintf(filename, sizeof(filename), "D:/audio/%s.mp3", words[i]);
            // Open the current MP3 file
            SCFILE *input_fp = sAPI_fopen(filename, "rb");
            if (!input_fp) {
                //fprintf(stderr, "Failed to open input file: %s\n", filename);
                PrintfResp("Failed to open input file");
                continue; // Skip to the next file if this one fails
            }
            // Copy data from input file to output file
            while ((bytes_read = sAPI_fread(buffer, 1, BUFFER_SIZE, input_fp)) > 0) {
                sAPI_fwrite(buffer, 1, bytes_read, output_fp);
            }
            sAPI_fclose(input_fp); // Close the input file
        }
    }
    SCFILE *input_fp = sAPI_fopen("D:/audio/received.mp3", "rb");
        // Copy data from input file to output file
    while ((bytes_read = sAPI_fread(buffer, 1, BUFFER_SIZE, input_fp)) > 0) {
        sAPI_fwrite(buffer, 1, bytes_read, output_fp);
    }
    sAPI_fclose(input_fp); // Close the input file
    sAPI_fclose(output_fp); // Close the output file
}

void sTask_Audio_taskProcesser(void* argv)
{
    char NetResp[100] = {0};
    unsigned int adc=0;
    sAPI_AudioSetVolume(volume);
    while(1)  
    {  
        if(volume_changed)
        {
            sAPI_AudioSetVolume(volume);
            volume_changed=0;
        }
        adc=sAPI_ReadAdc(2);
        sprintf(NetResp, "ADC %dmv\r\n",sAPI_ReadAdc(2));
        PrintfResp(NetResp);
        if(adc>930)
        {
            sAPI_GpioSetValue(SC_MODULE_GPIO_12, 1);//led1
            sAPI_GpioSetValue(11, 1);//led2
            sAPI_GpioSetValue(10, 1);//led3
        }
        else if(adc>880)
        {
            sAPI_GpioSetValue(SC_MODULE_GPIO_12, 0);
            sAPI_GpioSetValue(11, 1);
            sAPI_GpioSetValue(10, 1);  
        }
        else if(adc>830)
        {
            sAPI_GpioSetValue(SC_MODULE_GPIO_12, 0);
            sAPI_GpioSetValue(11, 0);
            sAPI_GpioSetValue(10, 1); 
        }
        else
        {
            sAPI_GpioSetValue(SC_MODULE_GPIO_12, 0);
            sAPI_GpioSetValue(11, 0);
            sAPI_GpioSetValue(10, 0); 
        }
        
        if(mqtt_received==1)
        {
            sprintf(NetResp, "\r\nmqtt value : %d\r\n", mqtt_value);
            PrintfResp(NetResp);
            //mqtt_value=123;
            test_play_file(mqtt_value);
            PrintfResp("sAPI_AudioPlay");
            sAPI_AudioPlay ("C:/temp.mp3", 1, 1);
            sAPI_TaskSleep(500);
            mqtt_received=0;
        }
        sAPI_TaskSleep(200);
    }
}
void Audio_task(void)
{
    SC_STATUS status = SC_SUCCESS;
    status = sAPI_TaskCreate(&Audio_taskProcesser, Audio_taskProcesserStack, 1024*40, 100, "Audio_taskProcesser",sTask_Audio_taskProcesser,(void *)0);
    if(SC_SUCCESS != status)
    {
        sAPI_Debug("Task create fail,status = [%d]",status);
    }
}

/**
  * @brief  SIMCom UI demo processer.
  * @param  arg
  * @note   Please select demo according to CLI.
  * @retval void
  */
void sTask_SimcomUIProcesser(void *arg)
{
    //UINT32 opt = 0;
    //char *note = " Please select an option to test from the items listed below.\n";
    GpioInit();
    NetworkConnectInit();
    Audio_task();
    MqttDemo();
 
    // while (1)
    // {
    //     PrintfResp(note);
    //     PrintfMainMenu();

    //     opt = UartReadValue();
    //     sAPI_Debug("opt = [%d]", opt);

    //     if (opt >= 0 && opt < SC_DEMO_MAX && gDemoList[opt].cb != NULL)
    //     {
    //         sAPI_Debug("call %s demo!!", gDemoList[opt].description);
    //         gDemoList[opt].cb();
    //     }
    //     else
    //     {
    //         sAPI_Debug("opt: %d, not support!", opt);
    //     }
    // }
}

/**
  * @brief  Create SIMCom UI demo task.
  * @param  void
  * @note   UI demo based on message(queue) with blocking method.
  * @retval void
  */
void sAPP_SimcomUIDemo(void)
{
    SC_STATUS status;

    scDemoListInit();

    void *simcomUIProcesserStack = (void *)malloc(40 * 1024);
    if (!simcomUIProcesserStack)
    {
        printf("malloc simcomUIProcesserStack fail!");
        return;
    }
    status = sAPI_TaskCreate(&simcomUIProcesser,
                             simcomUIProcesserStack,
                             1024 * 40,
                             100,
                             "simcomUIProcesser",
                             sTask_SimcomUIProcesser,
                             (void *)0);
    if (SC_SUCCESS != status)
    {
        sAPI_Debug("task create fail");
    }
}

