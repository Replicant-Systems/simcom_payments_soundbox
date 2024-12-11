#ifndef _SIMCOM_AUTH_API_H_
#define _SIMCOM_AUTH_API_H_

/*
 * Name:        sAPI_auth
 *
 * Description: Connect AUTH disk S:/
 *
 * Parameters:
 *  [IN] *password
 *  [IN] *timeout   auth succ timer
 *
 * Returns:
 * if password is 0
 *     return magic
 * else
 *     0      success
 *     other  fail
 *
 * Notes:
 * magic:The random number generated by the IMEI after CRC calculation is seeded
 * password:the result of two HASH calculations and one CRC settlement by <magic>
 */
int sAPI_auth(unsigned int password,int timeout);
#endif