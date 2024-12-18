#ifndef __SC_I2C_H__
#define __SC_I2C_H__


typedef enum
{
    SC_I2C_INVALID_PORT = -1,
    SC_I2C_RC_OK = 0,
    SC_I2C_RC_NOT_OK,
    SC_I2C_RC_INVALID_DATA_SIZE,
    SC_I2C_RC_INVALID_DATA_PTR,
    SC_I2C_RC_TOO_MANY_REGISTERS,
    SC_I2C_RC_TIMEOUT_ERROR,                               // 5
    SC_I2C_RC_CHIP_BUSY,                                   // 6
    SC_I2C_RC_INVALID_GENERAL_CALL_SLAVE_ADDRESS,          // 7
	SC_I2C_RC_UNREGISTER_ERR,                              // 8
	SC_I2C_RC_MESSAGE_QUEUE_IS_FULL,                       // 9
    SC_I2C_ISR_UNEXPECTED_INTERRUPT,                       // 0xA
	SC_I2C_ISR_BUS_ERROR,                                  // 0xB
	SC_I2C_ISR_BUS_BUSY,                                   // 0xC
	SC_I2C_ISR_EARLY_BUS_BUSY,                             // 0xD
	SC_I2C_ISR_CALL_BACK_FUNCTION_ERR,                     // 0xE
	SC_I2C_ISR_ARBITRATION_LOSS,                           // 0xF
	SC_I2C_PARAMS_ERROR,
	SC_I2C_RC_ILLEGAL_USE_OF_API
}SC_I2C_ReturnCode;

typedef enum
{
    SC_I2C_CHANNEL0, //the A7680C module(PIN68 and PIN69) and the A7670C module(PIN37 and PIN38)
    SC_I2C_CHANNEL1, //the A7630C module(PIN56 and PIN57) and the A7670C module(PIN67 and PIN68)
    SC_I2C_CHANNEL2, //the A7670C module(PIN35 and PIN36)
    SC_I2C_CHANNEL3,
}SC_I2C_CHANNEL;

typedef enum
{
    SC_I2C_STANDARD_MODE = 0,	/*100Kbps*/
    SC_I2C_FAST_MODE     = 1,	/*400Kbps*/
    SC_I2C_HS_MODE       = 2,	/*3.4 Mbps slave/3.3 Mbps master,standard mode when not doing a high speed transfer*/
    SC_I2C_HS_MODE_FAST  = 3,	/*3.4 Mbps slave/3.3 Mbps master,fast mode when not doing a high speed transfer*/
}SC_I2C_CLOCK;

typedef struct
{
    SC_I2C_CHANNEL i2c_channel;
    SC_I2C_CLOCK i2c_clock;
}SC_I2C_DEV;

SC_I2C_ReturnCode sAPI_NoRegAddr_I2CRead(int i2cChannel,UINT8 slaveAddress,UINT8 *receiveDataBuffer,UINT16 datasize);
SC_I2C_ReturnCode sAPI_NoRegAddr_I2CWrite(int i2cChannel,UINT8 slaveAddress,UINT8 *data,UINT16 datasize);
SC_I2C_ReturnCode sAPI_16BitRegAddr_I2CRead(int i2cChannel,UINT8 slaveAddress,UINT16 regAddress,UINT8 *receiveDataBuffer,UINT16 datasize);
SC_I2C_ReturnCode sAPI_16BitRegAddr_I2CWrite(int i2cChannel,UINT8 slaveAddress,UINT16 regAddress,UINT8 *data,UINT16 datasize);

SC_I2C_ReturnCode sAPI_I2CRead(int i2cChannel,UINT8 slaveAddress,UINT8 regAddress,UINT8 *receiveDataBuffer,UINT16 datasize);
SC_I2C_ReturnCode sAPI_I2CReadEx(int i2cChannel,UINT8 slaveAddress,UINT8 regAddress,UINT8 *receiveDataBuffer,UINT16 datasize);
SC_I2C_ReturnCode sAPI_I2CWrite(int i2cChannel,UINT8 slaveAddress,UINT8 regAddress,UINT8 *data,UINT16 datasize);
SC_I2C_ReturnCode sAPI_I2CWriteEx(int i2cChannel,UINT8 slaveAddress,UINT8 regAddress,UINT8 *data,UINT16 datasize);
SC_I2C_ReturnCode sAPI_I2CConfigInit(SC_I2C_DEV *handle);
SC_I2C_ReturnCode sAPI_I2CDeInit(SC_I2C_DEV *handle);

#endif
