/*** 
 * @Author: yangkui.zhang
 * @Date: 2022-12-20 16:28:20
 * @LastEditTime: 2023-08-15 16:02:54
 * @LastEditors: yangkui.zhang
 * @Description: 
 * @FilePath: \ASR1606\simcom\userspace\sc_lib\inc\GPIO\A7670C_MANS_LANS_GPIO.h
 * @no error,no warning
 */
#ifndef __A7670C_MANS_LANS_GPIO_H__
#define __A7670C_MANS_LANS_GPIO_H__

/* A7670C_MASNS_LANS GPIO Resource Sheet  */
typedef enum {
    SC_MODULE_GPIO_NOT_ASSIGNED = -1,
#ifdef FEATURE_SIMCOM_NORI
    SC_MODULE_GPIO_00 = 80,  /*  pin4, pad name: RI  */
#endif
    SC_MODULE_GPIO_01 = 32,  /*  pin5, pad name: GPIO_01 */
    SC_MODULE_GPIO_02 = 9,   /*  pin6, pad name: GPIO_02/PWM4  */
    SC_MODULE_GPIO_03 = 74,  /*  pin7, pad name: GPIO_03/UART4_CTS */
    SC_MODULE_GPIO_04 = 73,  /*  pin8, pad name: GPIO_04/UART4_RTS  */
    SC_MODULE_GPIO_05 = 120, /*  pin19, pad name: GPIO_05  */
#ifndef FEATURE_SIMCOM_GPS
    SC_MODULE_GPIO_06 = 6,   /*  pin20, pad name: GPIO_06/MK_IN_3  */
#endif
    SC_MODULE_GPIO_07 = 7,   /*  pin21, pad name: GPIO_07/MK_OUT_3  */
    SC_MODULE_GPIO_08 = 77,  /*  pin48, pad name: GPIO_08  */
    SC_MODULE_GPIO_09 = 28,  /*  pin52, pad name: GPIO_09  */
    SC_MODULE_GPIO_10 = 37,  /*  pin66, pad name: GPIO_10  */

    SC_MODULE_GPIO_11 = 11,  /*  pin67, pad name: GPIO_11/MK_OUT_5  */
    SC_MODULE_GPIO_12 = 10,  /*  pin68, pad name: GPIO_12/MK_IN_5/PWM3  */
    SC_MODULE_GPIO_13 = 13,  /*  pin35, pad name: GPIO_13/MK_OUT_6/I2C3_SDA  */
    SC_MODULE_GPIO_14 = 12,  /*  pin36, pad name: GPIO_14/MK_IN_6/I2C3_SCL  */
    SC_MODULE_GPIO_15 = 5,   /*  pin44, pad name: GPIO_15/MK_OUT_2  */
    SC_MODULE_GPIO_16 = 4,   /*  pin47, pad name: GPIO_16/MK_IN_2  */
    SC_MODULE_GPIO_17 = 124, /*  pin26, pad name: GPIO_17  */
    SC_MODULE_GPIO_18 = 118, /*  pin53, pad name: GPIO_18/PWM2  */

    SC_MODULE_GPIO_34 = 16,  /*  pin11, pad name: GPIO_34/SSP0_CLK/PCM_CLK  */
    SC_MODULE_GPIO_35 = 17,  /*  pin12, pad name: GPIO_35/SSP0_CS/PCM_SYNC  */
#ifndef CUS_A7670C_LANS_WHXA
    SC_MODULE_GPIO_36 = 19, /*  pin13, pad name: GPIO_36/SSP0_TXD/PCM_DIN  */
    SC_MODULE_GPIO_37 = 18, /*  pin14, pad name: GPIO_37/SSP0_RXD/PCM_DOUT  */
#else
    SC_MODULE_GPIO_37 = 19, /*  pin13, pad name: GPIO_36/SSP0_TXD/PCM_DIN  */
    SC_MODULE_GPIO_36 = 18, /*  pin14, pad name: GPIO_37/SSP0_RXD/PCM_DOUT  */
#endif
    SC_MODULE_GPIO_38 = 50,  /*  pin37, pad name: GPIO_39/CI2C_SDA  */
    SC_MODULE_GPIO_39 = 49,  /*  pin38, pad name: GPIO_40/CI2C_CLK  */
#ifdef FEATURE_SIMCOM_NODTR
    SC_MODULE_GPIO_40 = 79,  /*  pin3, pad name: DTR  */
#endif

#ifdef CUS_A7670C_LANS_HWQS
    SC_MODULE_GPIO_41 = 45,  /*  pin108, pad name: GPIO_USIM2_UCLK */
    SC_MODULE_GPIO_42 = 46,   /*  pin109, pad name: GPIO_USIM2_UIO  */
    SC_MODULE_GPIO_43 = 47,  /*  pin111, pad name: GPIO_USIM2_URSTn */
    SC_MODULE_GPIO_44 = 119,  /*  pin112, pad name: GPIO_119  */
#endif

#ifdef CUS_ADSUN
    SC_MODULE_GPIO_45 = 51,  /*  pin115, pad name: MMC_D3 */
    SC_MODULE_GPIO_46 = 52,  /*  pin114, pad name: MMC_D2 */
    SC_MODULE_GPIO_47 = 53,  /*  pin113, pad name: MMC_D1 */
    SC_MODULE_GPIO_48 = 54,  /*  pin128, pad name: MMC_D0 */
    SC_MODULE_GPIO_49 = 27,  /*  pin129, pad name: MMC_CLK */
    SC_MODULE_GPIO_50 = 31,  /*  pin130, pad name: MMC_CMD */
#endif

    SC_MODULE_GPIO_MAX = 128,
} SC_Module_GPIONumbers;

#endif