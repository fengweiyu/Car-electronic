/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	GPS  186H Communication function module
* Description		: 	GPS  186H Data operation center
* Created			: 	2016.05.30.
* Author			: 	Yu Weifeng
* Function List 		: 	app_EncryptGPS_Task
* Last Modified 	: 	
* History			: 	
******************************************************************************/

#ifndef _APP_ENCRYPTGPS_H_
#define _APP_ENCRYPTGPS_H_

#include"app_EncryptGPS_report.h"
#define EncryptGPS
#define DEBUG_ENCRYPTGPS
typedef struct Encrypt{
	u8 IsKeyAgreeNotice;
	u8 GettedPasswordStatus;
	u8 CmdLen;
	u8 IDLen;
	u8 ID[20];
	u8 KeyLen;
	u8 PubKey[64];
	u8 PubTmpKey[64];
	u8 WritePasswordStatus;
	u8 WritePlatIdRespond;
	u8 WritePlatKeyRespond;
	u8 PasswordStatus;
	u16 usCipherTextLen;
	u8   ucCipherText[192];
	u16 TXMsgType;
}T_Encrypt;
extern T_Encrypt g_tEncryptMsg;
void app_EncryptGPS_Task(void *pdata);
void  EncryptGPSRxQWrite(PT_EncryptRxBuf pstGpsRxBuf);
void EncryptGpsOprSys(void);
#endif