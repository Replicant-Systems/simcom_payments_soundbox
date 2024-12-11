#ifndef __SIMCOM_LOCALFOTA__H__
#define __SIMCOM_LOCALFOTA__H__

#ifdef FEATURE_SIMCOM_NORFLASH_MINI_LFOTA

#if defined(SIMCOM_A7680C_V801) || defined(SIMCOM_A7670C_V801) || defined (SIMCOM_A7605C1_V201)
#define simcom_default_sspport 2
#else
#define simcom_default_sspport 0
#endif

#define LFOTA_OPENED_FILE_MAX (8)
#define LFOTA_MAX_FULL_NAME_LENGTH 256
#define SIMCOM_FOTA_DOWNLOAD_MSGQ_COUNT   1
#define SIMCOM_FOTA_DOWNLOAD_STACK_SIZE 4096*2
#define SIMCOM_FOTA_DOWNLOAD_TASK_PRO   120
#define  SET_FOTA_START_REQ 0

#define fatIO_RDONLY		0x01
#define fatIO_RDWR			0x02
#define fatIO_CREATE		0x10
#define fatIO_TRUNCATE		0x40

typedef enum{
    LOC_FLASH,
    LOC_EXT_FLASH,
    LOC_SD_CARD,
    LOC_EXT_NANDFLASH
} LFOTA_SC_FILE_LOCATION;

typedef enum
{
    SSP_CLOCK_6M  = 0x0,
    SSP_CLOCK_13M = 0x1,
    SSP_CLOCK_26M = 0x2,
    SSP_CLOCK_52M = 0x3,
} SSP_Clock;

typedef struct {
    unsigned long msgID;
    void *pArgs;
} fota_msg_t;

struct SC_LFotaApiParam
{
    char filepath[LFOTA_MAX_FULL_NAME_LENGTH]; 
    unsigned char mode; /*1: norflash*/
};

#endif
#endif