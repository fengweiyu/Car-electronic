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
#ifndef _APP_ENCRYPTGPSReport_H_
#define _APP_ENCRYPTGPSReport_H_

#define GET_ID      			      			0
#define GET_PASSWORD_STATUS 			1
#define WIRTE_PASSWORD             			2
#define GET_PUBLIC_KEY 		      			3
#define WIRTE_PLATFORM_ID 		     	4
#define WIRTE_PLATFORM_KEY 		     	5


#define MAX_ENCRYPTGPS_BUF_LEN  (300)	
#define GPSDataEnd 			       5
typedef struct EncryptGPSFlag
{
	u8 EncryptRxCmdFlg ; 
	u8 EncryptSetFlg ; 
	u8 EncryptGpsOkFlg ; 
	u8 EncryptGpsOkFlgBk ;
	u8 EncryptGpsTxReq ; 
	u8 EncryptGpsTxBusyFlg ; 
} T_EncryptGPSFlag; 
typedef enum EncryptGPSMsgType
{
	EncryptGPSGetID, 
	EncryptGPSGetPasswordStatus, 
	EncryptGPSWritePassword,  
	EncryptGPSGetPublicKey,
	EncryptGPSWritePlatformId,
	EncryptGPSWritePlatformKey,
}T_EncryptGPSMsgType; 
typedef struct EncryptGPSTXMsg
{
	u16 MsgLen;
	u8 *MsgBuf;
}T_EncryptGPSTXMsg,*PT_EncryptGPSTXMsg;
typedef struct EncryptRxBuf
{

	u16 u16Len; 
	u8 u8Con[MAX_ENCRYPTGPS_BUF_LEN]; 
}T_EncryptRxBuf,*PT_EncryptRxBuf; 

void EncryptGPSSetTXProc(void);
void EncryptGpsTxProc(void);
void EncryptGPSDataRecv(u8 gps_data);



#endif

