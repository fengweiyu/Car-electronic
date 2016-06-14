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
#include "Includes.h"

T_DatWithSignForPassthrouh g_tDatBufUploadPassthrough = {0};
T_TerminalEncryptAck g_tTerminalEncryptAck = {0};






bool bEncryptPassthroughAckOK = TRUE;			// 数据透传应答的数据透传结果
void setEncryptPassthroughAckResult(bool ackResult) 
{
		OSSchedLock();
		bEncryptPassthroughAckOK = ackResult;
		OSSchedUnlock();
}	
bool getEncryptPassthroughAckResult(void) 
{
		bool bRetVal = FALSE;	
		OSSchedLock();
		bRetVal = bEncryptPassthroughAckOK;
		OSSchedUnlock();
		return bRetVal;
}	
bool bEncryptPassthroughReplyOK = TRUE;			// 数据透传请求是否应答
void setEncryptPassthroughReplyResult(bool replyResult) 
{
		OSSchedLock();
		bEncryptPassthroughReplyOK = replyResult;
		OSSchedUnlock();
}
bool getEncryptPassthroughReplyResult(void) 
{
		bool bRetVal = FALSE;	
		OSSchedLock();
		bRetVal = bEncryptPassthroughReplyOK;
		OSSchedUnlock();	
		return bRetVal;
}
/*****************************************************************************
-Fuction		: EncryptOprDateAckHandle
-Description	: EncryptOprDateAckHandle
-Input			: BOOL bAck
-Output 		: none
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/

void EncryptOprDateAckHandle(BOOL bAck)
{
	if( IS_TRUE_BIT(bAck) ) 
	{
		setEncryptPassthroughAckResult(TRUE);
		setEncryptPassthroughReplyResult(TRUE);
	}
	else 
	{
		setEncryptPassthroughAckResult(FALSE);
		setEncryptPassthroughReplyResult(TRUE);
	}
}

/*****************************************************************************
-Fuction		: Jt808PassthrouhReq
-Description	: Jt808PassthrouhReq
-Input			: u8 u8Type, u8 *pu8Dat, u16 u16Len, 
								void (*pFunExeIsOkHook)(BOOL)
-Output 		: none
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/

BOOL Jt808PassthrouhReq(u8 u8Type, u8 *pu8Dat, u16 u16Len, 
								void (*pFunExeIsOkHook)(BOOL))
{
	BOOL boRetVal; 
	myevent stEveTmp; 
	M_ClrBit(boRetVal); 
	if(IS_NULL_P(pu8Dat) || IS_FALSE_BIT(u16Len)) 
	{; 
	}
	else 
	{
		OSSchedLock(); 		
		if(IS_TRUE_BIT(g_tDatBufUploadPassthrough.boDatAvailable)
		|| IS_TRUE_BIT(g_tDatBufUploadPassthrough.stWaitAckInfo.stAckSt.u8AviabileFlg)) 
		{
			; 
		}
		else 
		{
			stEveTmp.size = u16Len; 
			stEveTmp.buff = mymalloc(stEveTmp.size); 
			if(IS_NULL_P(stEveTmp.buff)) {; 
			}
			else 
			{
				memcpy(stEveTmp.buff, pu8Dat, stEveTmp.size); 
				memcpy(&g_tDatBufUploadPassthrough.stEve, &stEveTmp, sizeof(stEveTmp)); 
				g_tDatBufUploadPassthrough.u8Type = u8Type; 
				g_tDatBufUploadPassthrough.pFunEndHook = pFunExeIsOkHook; 
				M_SetBit(g_tDatBufUploadPassthrough.boDatAvailable); 
				M_SetBit(boRetVal); 
			}
			
		}		
		OSSchedUnlock(); 
	}
	
	return boRetVal; 
}
/*****************************************************************************
-Fuction		: TerminalTxEncryptDataExtract
-Description	: TerminalTxEncryptDataExtract
-Input			: T_TerminalTxEncryptMsgPack *ptTerminalTxEncryptMsgPack
-Output 		: none
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/

u8 TerminalTxEncryptDataExtract(T_TerminalTxEncryptMsgPack *ptTerminalTxEncryptMsgPack)
{
	u8 *tmp; 
	u8 ret;
	ptTerminalTxEncryptMsgPack->data_buff = mymalloc(g_tEncryptMsg.IDLen+ 129);
	tmp = ptTerminalTxEncryptMsgPack->data_buff;
	memcpy(tmp,&g_tEncryptMsg.IDLen,1);
	tmp++;
	memcpy(tmp,g_tEncryptMsg.ID,g_tEncryptMsg.IDLen);
	tmp += g_tEncryptMsg.IDLen;
	memcpy(tmp,g_tEncryptMsg.PubKey,64);
	tmp +=64;
	memcpy(tmp,g_tEncryptMsg.PubTmpKey,64);
	ptTerminalTxEncryptMsgPack->data_len =g_tEncryptMsg.IDLen+ 129;
	if(ptTerminalTxEncryptMsgPack->data_buff ==NULL)
	{
		ret = FALSE;
	}
	else 
	{
		ret = TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: EncryptOperDataPassthrough
-Description	: EncryptOperDataPassthrough
-Input			: none
-Output 		: none
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/

u8  EncryptOperDataPassthrough(void)
{
	static enEncryptTxMsgOper EncryptOper = EncryptOperIdle;
	static u32 u32TimeCnt = 0;
	static T_TerminalTxEncryptMsgPack tTerminalTxEncryptMsgPack;
	u8 ret;
	switch(EncryptOper) 
	{
		case EncryptOperIdle:
			if(IS_TRUE_BIT(TerminalTxEncryptDataExtract(&tTerminalTxEncryptMsgPack))) 
			{
				setEncryptPassthroughReplyResult(FALSE);
				if( IS_TRUE_BIT( Jt808PassthrouhReq( 0xF5,
												tTerminalTxEncryptMsgPack.data_buff,
												tTerminalTxEncryptMsgPack.data_len,
												EncryptOprDateAckHandle ) ) ) 
				{
				
					M_ClrBit(u32TimeCnt);
					EncryptOper = EncryptOperWait;
				}
				else 
				{
					;
				}
				myfree(tTerminalTxEncryptMsgPack.data_buff,sizeof(tTerminalTxEncryptMsgPack.data_buff));
			}
			else 
			{
				;
			}
			break;

		case EncryptOperWait:
			u32TimeCnt++;
			if(u32TimeCnt < EncryptTxMsgTimeOut) 
			{
				if(IS_TRUE_BIT(getEncryptPassthroughReplyResult())) 
				{
					if(IS_TRUE_BIT(getEncryptPassthroughAckResult())) 
					{

						debugString("Encrypt passthrough ack is OK\r\n");
						ret =  TRUE;
					}
					else 
					{
						debugString("Encrypt passthrough ack is fail\r\n");
						ret = FALSE;
					}
					EncryptOper = EncryptOperIdle;
				}
				else 
				{
					;
				}
			}
			else 
			{
				debugString("Encrypt passthrough ack is over time\r\n");
				ret = FALSE;
				EncryptOper = EncryptOperIdle;
				M_ClrBit(u32TimeCnt);
			}
			break;
		default:
			EncryptOper = EncryptOperIdle;
			ret = FALSE;
			break;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: Jt808UploadPassthroughWaitAckProc
-Description	: Jt808UploadPassthroughWaitAckProc
-Input			: none
-Output 		: none
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/

void Jt808UploadPassthroughWaitAckProc(void)
{	
	BOOL boExeEnd; 
	BOOL boExeOkFlg; 
	M_ClrBit(boExeEnd); 
	M_ClrBit(boExeOkFlg);
	if(IS_TRUE_BIT(g_tDatBufUploadPassthrough.stWaitAckInfo.stAckSt.u8AviabileFlg)) 
	{
		if(ProChkGenAckIfOkClr(&g_tDatBufUploadPassthrough.stWaitAckInfo.stAckSt, 
								&stJt808PltGenAck)) 
		{
				M_SetBit(boExeEnd); 
				M_SetBit(boExeOkFlg); 
		}
		else 
		{

			if(IS_TRUE_BIT(g_tDatBufUploadPassthrough.stWaitAckInfo.u8Timer)) 
			{; 
			}
			else 
			{
				M_SetBit(boExeEnd); 
				M_ClrBit(boExeOkFlg); 
			}
		}
		if(IS_FALSE_BIT(boExeEnd)) 
		{; 
		}
		else 
		{
			if(IS_NULL_P(g_tDatBufUploadPassthrough.pFunEndHook)) 
			{; 
			}
			else 
			{
				g_tDatBufUploadPassthrough.pFunEndHook(boExeOkFlg); 
			}
			myfree(g_tDatBufUploadPassthrough.stEve.buff, 
					g_tDatBufUploadPassthrough.stEve.size); 
			memset(&g_tDatBufUploadPassthrough, 0, sizeof(g_tDatBufUploadPassthrough)); 
		}
	}
	else 
	{
		u8 *D_buff; 
		u16 D_len; 
		u8 type; 
		gprs_pro_cmd_t local_com;
		u8 *com_para;			

		D_buff = g_tDatBufUploadPassthrough.stEve.buff; 
		D_len = g_tDatBufUploadPassthrough.stEve.size; 
		type = g_tDatBufUploadPassthrough.u8Type; 
		memset((u8*)&local_com, 0, sizeof(gprs_pro_cmd_t));
		local_com.pro_data = mymalloc(1 + D_len);
		if(local_com.pro_data == NULL) {
			debugString("DataStreamUpload can't malloc!\n");
			debugQueue();
		}
		else {
			com_para = local_com.pro_data;			
			Jt808PktHeaderGet(&local_com, 0x0900); 			
			*com_para++ = type;
			local_com.head.attribute.info_len = 1;
			memcpy(com_para, D_buff, D_len);
			local_com.head.attribute.info_len += D_len;
			ProGetWaitAckOkInfo(&g_tDatBufUploadPassthrough.stWaitAckInfo.stAckSt,
								&local_com); 
			g_tDatBufUploadPassthrough.stWaitAckInfo.u8Timer = Sys_para.Internet.link_para.tcp_k; 
			if(g_tDatBufUploadPassthrough.stWaitAckInfo.u8Timer < PRO_K) {
				g_tDatBufUploadPassthrough.stWaitAckInfo.u8Timer = PRO_K; 
			}
			else 
			{; 
			}
			app_protocol_Package(&local_com,SOCKET_1);
			myfree(local_com.pro_data, 1+D_len);
		}
	}
}

/*****************************************************************************
-Fuction		: TerminalKeyAgreeAck
-Description	: TerminalKeyAgreeAck
-Input			: none
-Output 		: none
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/

void TerminalKeyAgreeAck(void)
{
	gprs_pro_cmd_t local_com;
	u8 tmp_buff[10];
	memset((u8*)&local_com,0,sizeof(gprs_pro_cmd_t));
	Jt808PktHeaderGet(&local_com, 0x0001); 
	tmp_buff[0] = (g_tTerminalEncryptAck.SeqNum>>8)&0x00ff;
	tmp_buff[1] = g_tTerminalEncryptAck.SeqNum&0x00ff;
	tmp_buff[2] = (g_tTerminalEncryptAck.MsgID>>8)&0x00ff;
	tmp_buff[3] = g_tTerminalEncryptAck.MsgID&0x00ff;
	//memcpy(&tmp_buff[0],&g_tTerminalEncryptAck.SeqNum,2);
	//memcpy(&tmp_buff[2],&g_tTerminalEncryptAck.MsgID,2);
	tmp_buff[4] = g_tTerminalEncryptAck.Ack;
	local_com.head.attribute.info_len = 5;
	local_com.pro_data = tmp_buff;
	app_protocol_Package(&local_com,SOCKET_1);
}




