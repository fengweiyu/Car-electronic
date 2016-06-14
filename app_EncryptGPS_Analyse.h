/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	GPS  186H Communication function module
* Description		: 	GPS  186H Data operation center
* Created			: 	2016.05.30.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/

#ifndef _APP_ENCRYPTGPSAnalyse_H_
#define _APP_ENCRYPTGPSAnalyse_H_

#include "dev_gps_Database.h"
#include "app_gps_Intf.h"




typedef struct EncryptGPSHandle
{
	s8 *head;
	u8 (*EncryptGPSHandle)(T_Encrypt *ptEncrypt, u8 *buff);
}T_EncryptGPSHandle;

u8 EncryptGPSAnalyse(T_Encrypt *ptEncrypt, GpsRxCmdBufSt *pstGpsRxCmdDat);
u8 CiphertextAnalyse (T_Encrypt *ptEncrypt, u8 *buff);
u8 GetIDRespond(T_Encrypt *ptEncrypt, u8 *buff);
u8 WritePasswordRespond(T_Encrypt *ptEncrypt, u8 *buff);
u8 GetPasswordStatusRespond(T_Encrypt *ptEncrypt, u8 *buff);
u8 GetPublicKeyRespond(T_Encrypt *ptEncrypt, u8 *buff);
u8 WritePlatformIDRespond(T_Encrypt *ptEncrypt, u8 *buff);
u8 WritePlatformKeyRespond(T_Encrypt *ptEncrypt, u8 *buff);




#endif

