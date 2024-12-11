/**
  ******************************************************************************
  * @file    demo_mqtt.c
  * @author  SIMCom OpenSDK Team
  * @brief   Source file of MQTT operation.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 SIMCom Wireless.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "simcom_api.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "simcom_mqtts_client.h"
#include "simcom_network.h"
#include "simcom_debug.h"
#include "simcom_uart.h"
#include "simcom_common.h"
#include "core_auth.h"
#include "uart_api.h"
extern unsigned int network_sts;
extern unsigned int mqtt_received;
extern unsigned int mqtt_value;
typedef enum{
    SC_MQTTS_DEMO_ALIYUN      = 1,
    SC_MQTTS_DEMO_TENCENT     = 2,
    SC_MQTTS_DEMO_ONENET      = 3,
    SC_MQTTS_DEMO_TWING       = 4,
    SC_MQTTS_DEMO_MAX         = 99
}SC_MQTTS_DEMO_CLOUD_TYPE;

typedef enum{
	SC_MQTTS_DEMO_START       = 1,
	SC_MQTTS_DEMO_CONNECT     = 2,
	SC_MQTTS_DEMO_SUBSCRIBE   = 3,
	SC_MQTTS_DEMO_PUBLISH     = 4,
	SC_MQTTS_DEMO_UNSUBSCRIBE = 5,
	SC_MQTTS_DEMO_DISCONNECT  = 6,
	SC_MQTTS_DEMO_RELEASE     = 7,
	SC_MQTTS_DEMO_STOP        = 8,
}SC_MQTTS_DEMO_YUNPLATFORM_TYPE;

static sTaskRef gMqttRecvTask = NULL;
static UINT8 gMqttRecvTaskStack[SC_DEFAULT_THREAD_STACKSIZE*2];


sMsgQRef urc_mqtt_msgq_1 = NULL;

sMsgQRef uart_mqtt_msgq_1;
sMsgQRef simcomUI_msgq;
extern void PrintfOptionMenu(char* options_list[], int array_size);
extern void PrintfResp(char *format);
INT32 ret;
SCmqttReturnCode ret1;
char host[100] = {0};
int mqtt_connection_live=0,mqtt_reconnection=0;
char *endptr;
char NetResp[100] = {0};

/**
  * @brief  MQTT receiving task processor
  * @param  void
  * @note   
  * @retval void
  */    
 void sTask_MqttRecvProcesser(void* arg)                    //thread of sub topic data process
{
    while(1)
    {
        SIM_MSG_T msgQ_data_recv = {SIM_MSG_INIT, 0, -1, NULL};                 //NULL pointer for msgQ_data_recv.arg3 is necessary!
        SCmqttData *sub_data = NULL;
        //recv the subscribed topic data, from the message queue: urc_mqtt_msgq_1, it is set buy sAPI_MqttAccq        
        sAPI_MsgQRecv(urc_mqtt_msgq_1, &msgQ_data_recv, SC_SUSPEND);                                                                                //                     //
        if((SC_SRV_MQTT != msgQ_data_recv.msg_id) || (0 != msgQ_data_recv.arg1) || (NULL == msgQ_data_recv.arg3))   //wrong msg received                //                     //
            continue; 
        PrintfResp("\r\nsTask_MqttRecvProcesser\r\n");                                                                                                                                   //                     //
        sub_data = (SCmqttData *)(msgQ_data_recv.arg3);
        sub_data->payload_P[strlen(sub_data->payload_P)] = '\0';
        mqtt_value = strtol(sub_data->payload_P, &endptr, 10);
        memset(NetResp, 0, sizeof(NetResp));
        sprintf(NetResp, "\r\nmqtt value : %d\r\n", mqtt_value);
        PrintfResp(NetResp);
        /*these msg pointer must be free after using, don not change the free order*/                                                                   //                     //
        sAPI_Free(sub_data->topic_P);
        sAPI_Free(sub_data->payload_P);
        sAPI_Free(sub_data);  
        mqtt_received=1;
        sAPI_TaskSleep(10);                                                                                                                       //                     //
    }
}


//  memset(NetResp, 0, sizeof(NetResp));
//         sprintf(NetResp, "\r\nclient_index :%d:\r\n", sub_data->client_index);
//         PrintfResp(NetResp);
//         memset(NetResp, 0, sizeof(NetResp));
//         sprintf(NetResp, "\r\ntopic_len :%d:\r\n", sub_data->topic_len);
//         PrintfResp(NetResp);
//         memset(NetResp, 0, sizeof(NetResp));
//         sprintf(NetResp, "\r\ntopic_P :%s:\r\n", sub_data->topic_P);
//         PrintfResp(NetResp);
//         memset(NetResp, 0, sizeof(NetResp));
//         sprintf(NetResp, "\r\npayload_len :%d:\r\n", sub_data->payload_len);
//         PrintfResp(NetResp);
//         memset(NetResp, 0, sizeof(NetResp));
//         sprintf(NetResp, "\r\npayload_P :%s:\r\n", sub_data->payload_P);
//         PrintfResp(NetResp);



// please use correct cacert,clientcert,clientkey;
//The following content is wrong, just an example
char cacert[] =
{
"-----BEGIN CERTIFICATE-----\r\n\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n\
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n\
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n\
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n\
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n\
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n\
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n\
rqXRfboQnoZsG4q5WTP468SQvvG5\r\n\
-----END CERTIFICATE-----"
};

char clientcert[] =
{
"-----BEGIN CERTIFICATE-----\r\n\
MIIDWjCCAkKgAwIBAgIVAJ2qHyQSOL/ZFZ2YSf5R0DwnDSouMA0GCSqGSIb3DQEB\r\n\
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\r\n\
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDA4MTQwNjEx\r\n\
MDRaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\r\n\
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDC63De3+PcJ4tSM/iN\r\n\
yR3Oexd+ya5UHFmc8UaPoB4mH+qU22ZXZFPVnAIiXRuyAsNUzy9X+9MqrXZpuB5E\r\n\
ClJu4gIzDCt/gXrb4kNSUzOJyUjppCZllxl5LXLOhKfAFBqP/+g85KuHmlo9k7rF\r\n\
htyZyHv2dq+/1kI8WqlrbznhnDaToyNYNuGVszM7kGeY5Uzg1B4URzexZLeo6WaT\r\n\
2A5eyYMDvKDeCvx/RAS0cVFCa3s6oGU1Tr0GzOELuU6pLJYP+G+lDxcaINEgL5px\r\n\
N98AYBKKglLCW5FvM+x4ObeqT1Ww9+GpqNNT9D7zqnn7nsXtFCPAB+1nLYy1+MYC\r\n\
O39BAgMBAAGjYDBeMB8GA1UdIwQYMBaAFGAilJ7OJqa8f1FwI4FQ/ULC9n+5MB0G\r\n\
A1UdDgQWBBQe3eoeLnMbFmalWx4TdgrRYbNjKDAMBgNVHRMBAf8EAjAAMA4GA1Ud\r\n\
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAWeYIpjaOf/2E84cbazAeBxNc\r\n\
GEnUuY6MMF1xG0C87/yuOn5BLx1hoiDWMcgm2eNymyxNNJV3YeuN0CbF14T3PJdr\r\n\
FQhhx2VBFsfaHekiLzFB9svPqoJFmUA9y66uCc1+Pqk+Di3esqvN0RB4NbGpaI58\r\n\
DxO7r4k22ptx6Dhh8cbOTUUT7KIk1SHINDJbmg6Mus2phi1t3VIgMw1eh+OobIyx\r\n\
QfIqPSIIuOjpaZs7C0cm5BUYAN+GKyKL+asu0stY7ZeeNP+PPg+rp7kHwhLYr3Eg\r\n\
kTjypV9QSWp8YSTc80/M/iht08YzA3kOqW0bkH6ek3jRTsURUnNacOfnwakR9w==\r\n\
-----END CERTIFICATE-----"
};

char clientkey[] =
{
"-----BEGIN RSA PRIVATE KEY-----\r\n\
MIIEpAIBAAKCAQEAwutw3t/j3CeLUjP4jckdznsXfsmuVBxZnPFGj6AeJh/qlNtm\r\n\
V2RT1ZwCIl0bsgLDVM8vV/vTKq12abgeRApSbuICMwwrf4F62+JDUlMziclI6aQm\r\n\
ZZcZeS1yzoSnwBQaj//oPOSrh5paPZO6xYbcmch79navv9ZCPFqpa2854Zw2k6Mj\r\n\
WDbhlbMzO5BnmOVM4NQeFEc3sWS3qOlmk9gOXsmDA7yg3gr8f0QEtHFRQmt7OqBl\r\n\
NU69BszhC7lOqSyWD/hvpQ8XGiDRIC+acTffAGASioJSwluRbzPseDm3qk9VsPfh\r\n\
qajTU/Q+86p5+57F7RQjwAftZy2MtfjGAjt/QQIDAQABAoIBAQC8rIPD6Lo6Jzjv\r\n\
qAjGzc8BSBr1CimplwuTI+UmL1ofkzNRRZIps56GnQ1fz/p38765gBcWtlav79o3\r\n\
+RWqzMcAN9rjKLuOH3oaD0kVDh5MGD9wkQjtuzXYGwsPJk7uWUdg7dgR4VK73sfM\r\n\
5QucGa+9V9fOORJjwfS5URS/PpmrbAMvos3BQ0kVfECaG3i348hdNIj+NVDKWsUz\r\n\
SRu78J31VhlhFSrmUMQ7IRLcn+P+LOmQkiYuB9nyoEXRKtA5I0fqhWE3AoduBRX1\r\n\
s587nulbHBuKm7RSH3aGvsdb4d1ILAHSLC1iuiJ2PMp5C3UtOfs8qwQUie9s2U0A\r\n\
sJSa3/JFAoGBAP6FtQoswUbKi7gRsUNZDyuEDLMmUPXoU1D1jKieZrgyTn40SZhp\r\n\
xE8EXmrPe/FKUx/eVg1BceEz3rzr2tzcTl8oZenjAbnYxxFPq2JuF9MZZzJ2LJZP\r\n\
dw2Oej2jZ0dHQWzUMaP/EqAAyr8kAfukFJGhWK9zdLvTycMh0nCI08mbAoGBAMQN\r\n\
JbCV4iiARi/W6eBxhP3VfDchgpqE8kweWq8QQ+jOz/SBlhG98gr+wGbmI0rWx0fb\r\n\
6QMpd1VrcWfzjnzTUJF2RT1dFcS5KJCPRrfRzX8UUCwyu7vR6MtQO6djH/0RfudV\r\n\
DGVS1ztXhgGV3r7yki47XP0A45z9UJAZ40yLvoZTAoGAQzPakU0v2xkvuvIw869Y\r\n\
Kg07rTG4vrURCpdLBFqOlKPzxa7Yn72Nb6+fnmQaPaN6PzfdULKu9t9TOF6t8xLY\r\n\
PpfUEOU3V3K9XPzEK8EV2U8jyFcbfrSMoq+z5wJ2CoPt6mY8lSiMKmMLJVzEiUTR\r\n\
iYIogGrjr5RD+Zm8jqoSAXECgYB43fIikAVCGvMqg0DlEpd/NqLCCnLOeLA4pW4b\r\n\
B9gWK4qhn1K51XTnum88AR45AXkSoE1jmajsjLT59XofEthVFalST5UgF1ce6HYc\r\n\
1cfvVMKovQ5omN0Am5Jre/04tBSc4h4mSZBQEYGYcKFG752XyxllEatCGhoU/SDZ\r\n\
DD6F6QKBgQC5ocb+8fjRUJ3h2eoXLZu6gvSPiVYsMAT0tPdLn47EW4WecLWwwOQ7\r\n\
d7Ju21SCvFapZujVQoyRmHa8leKNggZYEhL2Wy3Tu9i9iJ6eII7YrfoN4eYtZA+e\r\n\
PWzpr6flu61Ds5WLhlCW7tYvmt6rN9WoCl8OYcVfg64BHfmttpWRUg==\r\n\
-----END RSA PRIVATE KEY-----"
};

static void mqtt_write_file(char *file_name, char *data)
{
    SCFILE *file_hdl = NULL;
    char pTemBuffer [300] = {0};
    int actul_write_len = 0, buff_data_len = 0, ret = 0;

    file_hdl = sAPI_fopen(file_name, "wb");
    if(file_hdl == NULL)
    {
        sAPI_Debug("sAPI_FsFileOpen err");
        sprintf(pTemBuffer, "\r\nFILE SYSTEM Write file failed because open file failed\r\n");
        goto err;
    }

    buff_data_len = strlen(data);
    actul_write_len = sAPI_fwrite(data, buff_data_len,1,file_hdl);
    sAPI_Debug("sAPI_fwrite (%d,%d)\r\n", buff_data_len, actul_write_len);
    if(actul_write_len != buff_data_len)
    {
        sAPI_Debug("sAPI_fwrite err write length: %d\r\n", actul_write_len);
        sprintf(pTemBuffer, "\r\nFILE SYSTEM Write file failed\r\n");
        goto err;
    }
    
    ret = sAPI_fclose(file_hdl);
    if(ret != 0)
    {
        sAPI_Debug("sAPI_fclose err");
        sprintf(pTemBuffer, "\r\nFILE SYSTEM Write file failed because close file failed\r\nn");
        goto err;
    }else{
        file_hdl = NULL;
    }
    sprintf(pTemBuffer, "\r\nFILE SYSTEM Write file successful\r\n\r\nFilename is %s \r\n",file_name);

err:
    PrintfResp(pTemBuffer);
}

static void app_MqttLostConnCb(int index, int cause)
{
    sAPI_GpioSetValue(69, 0);
    char notice[100] = {0};
    snprintf(notice, sizeof(notice) - 1, "\r\nMQTT LostConn Notice:index_%d,cause_%d\r\n", index, cause);
    PrintfResp(notice);
    memset(notice, 0, sizeof(notice));
    mqtt_connection_live=0;
}

/**
  * @brief  Tencet cloud connection demo
  * @param  void
  * @note   
  * @retval void
  */  
 void TencentDemo(void)
 {
    while(1)//network_sts)
    {
        sAPI_GpioSetValue(69, 0);
        sAPI_TaskSleep(2*200);
        sAPI_GpioSetValue(69, 1);
        ret = sAPI_MqttStart(-1);
        if(SC_MQTT_RESULT_SUCCESS == ret)
            {
                sAPI_Debug("Init SUCCESS");
                PrintfResp("\r\nMQTT Init Successful!\r\n");
            }
        else
            {
                sAPI_Debug("Init FAIL,ERRCODE = [%d]",ret);
                PrintfResp("\r\nMQTT Init Fail!\r\n");
                break;
            }

        ret= sAPI_MqttAccq(0, NULL, 0, "Replicant_payments", 1, urc_mqtt_msgq_1);
        if(SC_MQTT_RESULT_SUCCESS == ret)
            {
                sAPI_Debug("Init SUCCESS");
                sAPI_Debug("\r\nMQTT accquire Successful!\r\n");
            }
        else
            {
                sAPI_Debug("Init FAIL,ERRCODE = [%d]",ret);
                PrintfResp("\r\nMQTT accquire Fail!\r\n");
                break;
            }

        int ret_ssl = -1;
        ret_ssl = sAPI_SslSetContextIdMsg("authmode",0,"2");
        if(SC_MQTT_RESULT_SUCCESS == ret_ssl)
            {
                sAPI_Debug("authmode=%d", ret_ssl);
                PrintfResp("\r\nMQTT authmode Successful!\r\n");
            }
        ret_ssl = sAPI_SslSetContextIdMsg("cacert", 0, "cacert.pem");
        if(SC_MQTT_RESULT_SUCCESS == ret_ssl)
            {
                sAPI_Debug("cacert=%d", ret_ssl);
                PrintfResp("\r\nMQTT cacert Successful!\r\n");
            }
        ret_ssl = sAPI_SslSetContextIdMsg("clientcert", 0, "clientcert.pem");
        if(SC_MQTT_RESULT_SUCCESS == ret_ssl)
            {
                sAPI_Debug("clientcert=%d", ret_ssl);
                PrintfResp("\r\nMQTT clientcert Successful!\r\n");
            }
        ret_ssl =sAPI_SslSetContextIdMsg("clientkey", 0, "clientkey.pem");
        if(SC_MQTT_RESULT_SUCCESS == ret_ssl)
            {
                sAPI_Debug("clientkey=%d", ret_ssl);
                PrintfResp("\r\nMQTT clientkey Successful!\r\n");
            }
        ret_ssl = sAPI_SslSetContextIdMsg("enableSNI", 0, "1");
            {
                sAPI_Debug("enableSNI=%d", ret_ssl);
            }
        
        PrintfResp("\r\n MQTT connect! \r\n");
        //sAPI_TaskSleep(2*200);//sleep 2s
        sAPI_MqttCfg(0, NULL, 0, 0, 0);
        sAPI_MqttConnLostCb(app_MqttLostConnCb);
        ret= sAPI_MqttConnect(0, NULL, 0, host, 10, 0, NULL, NULL);
        if(SC_MQTT_RESULT_SUCCESS == ret)
        {
            sAPI_Debug("connect SUCCESS");
            PrintfResp("\r\nMQTT connect Successful!\r\n");
            sAPI_TaskSleep(1*200);//sleep 5s
            
            ret1= sAPI_MqttSub(0, "arn:aws:iot:ap-south-1:288761763182:thing/Replicant_payments/+", strlen("arn:aws:iot:ap-south-1:288761763182:thing/Replicant_payments/+"),0,0);
            if (ret1 == SC_MQTT_RESULT_SUCCESS) {
                sAPI_Debug("Subscription successful");
                PrintfResp("\r\nSubscription successful!\r\n");
                mqtt_connection_live=1;
                sAPI_GpioSetValue(53, 1);
                while(mqtt_connection_live)
                {
                    sAPI_TaskSleep(200);
                    sAPI_GpioSetValue(53, 0);
                    sAPI_TaskSleep(200);
                    sAPI_GpioSetValue(53, 1);
                }
                sAPI_GpioSetValue(53, 0);
                
            } 
            else 
                sAPI_Debug("Subscription failed with error code: %d", ret1);

            ret= sAPI_MqttDisConnect(0, NULL, 0, 10);
            sAPI_GpioSetValue(69, 0);
            if(SC_MQTT_RESULT_SUCCESS == ret)
            {
                sAPI_Debug("disconnect SUCCESS");
                PrintfResp("\r\nMQTT disconnect Successful!\r\n");
            }
            else
            {
                sAPI_Debug("disconnect FAIL,ERRCODE = [%d]",ret);
                PrintfResp("\r\nMQTT disconnect Fail!\r\n");
            }
        }
        else
        {
            sAPI_Debug("connect FAIL,ERRCODE = [%d]",ret);
            PrintfResp("\r\nMQTT connect Fail!\r\n");
        }

        ret= sAPI_MqttRel(0);
        if(SC_MQTT_RESULT_SUCCESS == ret)
        {
            sAPI_Debug("sAPI_MqttRel SUCCESS");
            PrintfResp("\r\nMQTT sAPI_MqttRel Successful!\r\n");
        }
        else
        {
            sAPI_Debug("sAPI_MqttRel FAIL,ERRCODE = [%d]",ret);
            PrintfResp("\r\nMQTT sAPI_MqttRel Fail!\r\n");
        }

        ret= sAPI_MqttStop();
        if(SC_MQTT_RESULT_SUCCESS == ret)
        {
            sAPI_Debug("sAPI_MqttStop SUCCESS");
            PrintfResp("\r\nMQTT sAPI_MqttStop Successful!\r\n");
        }
        else
        {
            sAPI_Debug("sAPI_MqttStop FAIL,ERRCODE = [%d]",ret);
            PrintfResp("\r\nMQTT sAPI_MqttStop Fail!\r\n");
        }
        break;
    }
  }

/**
  * @brief  Cloud demo connection with different cloud in China
  * @param  void
  * @note   
  * @retval void
  */  
 void MqttDemo(void)
 {
    mqtt_write_file("c:/cacert.pem", cacert);
    mqtt_write_file("c:/clientcert.pem", clientcert);
    mqtt_write_file("c:/clientkey.pem", clientkey);
    
    sAPI_FsSwitchDir("c:/cacert.pem", 1);
    sAPI_FsSwitchDir("c:/clientcert.pem", 1);
    sAPI_FsSwitchDir("c:/clientkey.pem", 1);

    memset(host, 0, sizeof(host));
    snprintf(host, sizeof(host),"%s", "tcp://a3tvmb09gnt0ik-ats.iot.ap-south-1.amazonaws.com:8883");//domain or ip

    if(NULL == urc_mqtt_msgq_1)
     {
         if(sAPI_MsgQCreate(&urc_mqtt_msgq_1, "urc_mqtt_msgq_1", (sizeof(SIM_MSG_T)), 4, SC_FIFO) != SC_SUCCESS)
         {
             sAPI_Debug("message queue creat err!");
         }
     }

	if (NULL == gMqttRecvTask)
     {
         if(sAPI_TaskCreate(&gMqttRecvTask, gMqttRecvTaskStack, SC_DEFAULT_THREAD_STACKSIZE*2, 100, (char*)"mqttProcesser", sTask_MqttRecvProcesser,(void *)0)) //init the demo task
		 {
			gMqttRecvTask = NULL;
			sAPI_Debug("Task Create error!\n");
            PrintfResp("\r\nTask Create error\r\n");
		 }
     }
    while(1)
        TencentDemo();
 }
