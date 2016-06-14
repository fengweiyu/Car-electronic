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

#ifndef _APP_ENCRYPTGSMReport_H_
#define _APP_ENCRYPTGSMReport_H_

#include "Includes.h"

#define EncryptTxMsgTimeOut	(120)	// 单位大约为0.5s，设定为1min

typedef enum {
	EncryptOperIdle,
	EncryptOperWait,
}enEncryptTxMsgOper;

typedef struct {
	u8 command_id;
	u16 data_len;
	u8 *data_buff;
}T_TerminalTxEncryptMsgPack;
typedef struct TerminalEncryptAck
{
	u8 TerminalEncryptAckReq;
	u16 SeqNum;
	u16 MsgID;
	u8 Ack;
}T_TerminalEncryptAck;

typedef struct {
	BOOL boDatAvailable; 
	u8 u8Type; 
	myevent stEve; 
	Jt808GenWaitAckParaSt stWaitAckInfo; 
	void (*pFunEndHook)(BOOL); //传入执行成功结果
}T_DatWithSignForPassthrouh; 

extern T_DatWithSignForPassthrouh g_tDatBufUploadPassthrough;
extern T_TerminalEncryptAck g_tTerminalEncryptAck;


void TerminalKeyAgreeAck(void);
u8 u8AddedItemExpEncryptText(u8 *pu8Dst);





#endif

