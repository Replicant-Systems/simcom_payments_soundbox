
#ifndef __SIMCOM_ONEWIRE_H__
#define __SIMCOM_ONEWIRE_H__

void sAPI_OneWireInit(void);
int sAPI_OneWireSetupReset(void);
int sAPI_OneWireReadByte(unsigned int *data);
int sAPI_OneWireWriteByte(unsigned int data);
int sAPI_OneWireRead(unsigned int *data, int len);
int sAPI_OneWireWrite(unsigned int *data, int len);
#endif