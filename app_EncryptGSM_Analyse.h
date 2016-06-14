/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	Encrypt Platform Communication function module
* Description		: 	Encrypt Platform Data operation center
* Created			: 	2016.06.5.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/


#ifndef _APP_ENCRYPTGSMAnalyse_H_
#define _APP_ENCRYPTGSMAnalyse_H_



#define KeyAgreementInform          (0xf7)
#define KeyAgreementParam (0xf6)
#define SetTerminalParam     (0xF300)

typedef struct PlatformTxEncryptMsg
{
	u8 RevFlg;
	u16 MsgType;
	u8 IDLen;
	u8 *ID;
	u8 PubKey[64];
	u8 PubTmpKey[64];
	u8 SetParamLen;
	u8 SetParamVal;
}T_PlatformTxEncryptMsg;



extern T_PlatformTxEncryptMsg g_tPlatformTxEncryptMsg;


void EncryptGSMAnalyse(void);


#endif

