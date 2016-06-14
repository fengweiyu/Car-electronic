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
#include "Includes.h"

T_EncryptGPSFlag tEncryptGPSFlag = {0};
T_EncryptGPSMsgType g_EncryptGPSMsgType = {0};
static T_EncryptRxBuf  g_tEncryptRxDevBuf = {0};
extern T_EncryptGPSHandle g_tEncryptGPSHandleMenu[]; 
extern u8 ENCRYPTGPS_MAX_ANLY_NUM; 

static u8 Hex2ASCII(u8 hex)
{
	const s8 Byte2Hex[] = "0123456789ABCDEF";
	return Byte2Hex[hex];
}

/*****************************************************************************
-Fuction		: EncryptGPSSetTXProc
-Description	: EncryptGPSSetTXProc
通过设置设置标记和结构体中的类型来确定发什么
-Input			: none
-Output 		: none
-Return 		: none
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/
void EncryptGPSSetTXProc(void)
{

	if(IS_FALSE_BIT(tEncryptGPSFlag.EncryptSetFlg))
	{
		; 
	}
	else
	{
		M_ClrBit(tEncryptGPSFlag.EncryptSetFlg); 			
		switch(g_tEncryptMsg.TXMsgType)
		{
			case GET_ID:
			{
				g_EncryptGPSMsgType = EncryptGPSGetID; 
				break; 
			}	
			case GET_PASSWORD_STATUS:
			{		
				g_EncryptGPSMsgType = EncryptGPSGetPasswordStatus; 
				break; 
			}
			case WIRTE_PASSWORD:
			{		
				g_EncryptGPSMsgType = EncryptGPSWritePassword; 
				break; 
			}
			case GET_PUBLIC_KEY:
			{		
				g_EncryptGPSMsgType = EncryptGPSGetPublicKey; 
				break; 
			}
			case WIRTE_PLATFORM_ID:
			{		
				g_EncryptGPSMsgType = EncryptGPSWritePlatformId; 
				break; 
			}
			case WIRTE_PLATFORM_KEY:
			{		
				g_EncryptGPSMsgType = EncryptGPSWritePlatformKey; 
				break; 
			}
			default :	
			{	
				break;
			}
		}
			M_SetBit(tEncryptGPSFlag.EncryptGpsTxReq); 
	}
}
/*****************************************************************************
-Fuction		: EncryptGpsTxMsgPack
-Description	: EncryptGpsTxMsgPack
-Input			: T_EncryptGPSMsgType EncryptGPSMsgType
-Output 		: PT_EncryptGPSTXMsg ptEncryptGPSTXMsg
-Return 		: none
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/
u8 EncryptGpsTxMsgPack(T_EncryptGPSMsgType EncryptGPSMsgType,PT_EncryptGPSTXMsg ptEncryptGPSTXMsg)
{
	s8  tmp[3];
	u8 u8Idx; 
	s8  XorSum;
	T_EncryptGPSMsgType tmpEncryptGPSMsgType;
	tmpEncryptGPSMsgType = EncryptGPSMsgType;
	switch(tmpEncryptGPSMsgType)
	{
		case EncryptGPSGetID:
		{
			ptEncryptGPSTXMsg->MsgLen = 20;//临时的长度
			ptEncryptGPSTXMsg->MsgBuf = mymalloc(ptEncryptGPSTXMsg->MsgLen);
			memset(ptEncryptGPSTXMsg->MsgBuf, 0, ptEncryptGPSTXMsg->MsgLen );
			XorSum = GetXorSum("get_id,0,", 9);
			memset(tmp,0,sizeof(tmp));
			M_ClrBit(u8Idx); 
			tmp[u8Idx++] = Hex2ASCII((XorSum >> 4) & 0x0f); 
			tmp[u8Idx++] = Hex2ASCII((XorSum ) & 0x0f); 
			ptEncryptGPSTXMsg->MsgLen = sprintf((s8*)ptEncryptGPSTXMsg->MsgBuf ,"$get_id,0,*%s\r\n",tmp);			
			break;
		}
		case EncryptGPSGetPasswordStatus:
		{	
			ptEncryptGPSTXMsg->MsgLen = 35;
			ptEncryptGPSTXMsg->MsgBuf = mymalloc(ptEncryptGPSTXMsg->MsgLen);
			memset(ptEncryptGPSTXMsg->MsgBuf, 0, ptEncryptGPSTXMsg->MsgLen );
			XorSum = GetXorSum("get_password_status,0,", 22);
			memset(tmp,0,sizeof(tmp));
			M_ClrBit(u8Idx); 
			tmp[u8Idx++] = Hex2ASCII((XorSum >> 4) & 0x0f); 
			tmp[u8Idx++] = Hex2ASCII((XorSum ) & 0x0f); 
			ptEncryptGPSTXMsg->MsgLen = sprintf((s8*)ptEncryptGPSTXMsg->MsgBuf ,"$get_password_status,0,*%s\r\n",tmp);
			break; 
		}
		case EncryptGPSWritePassword:
		{	
			ptEncryptGPSTXMsg->MsgLen = 22;	
			ptEncryptGPSTXMsg->MsgBuf = mymalloc(ptEncryptGPSTXMsg->MsgLen);
			memset(ptEncryptGPSTXMsg->MsgBuf, 0, ptEncryptGPSTXMsg->MsgLen );
			XorSum = GetXorSum("write_password,0,", 17);
			memset(tmp,0,sizeof(tmp));
			M_ClrBit(u8Idx); 
			tmp[u8Idx++] = Hex2ASCII((XorSum >> 4) & 0x0f); 
			tmp[u8Idx++] = Hex2ASCII((XorSum ) & 0x0f); 
			ptEncryptGPSTXMsg->MsgLen = sprintf((s8*)ptEncryptGPSTXMsg->MsgBuf ,"$write_password,0,*%s\r\n",tmp);
			break;
		}
		case EncryptGPSGetPublicKey:
		{
			ptEncryptGPSTXMsg->MsgLen = 30;//临时的长度
			ptEncryptGPSTXMsg->MsgBuf = mymalloc(ptEncryptGPSTXMsg->MsgLen);
			memset(ptEncryptGPSTXMsg->MsgBuf, 0, ptEncryptGPSTXMsg->MsgLen );
			XorSum = GetXorSum("get_public_key,0,", 17);
			memset(tmp,0,sizeof(tmp));
			M_ClrBit(u8Idx); 
			tmp[u8Idx++] = Hex2ASCII((XorSum >> 4) & 0x0f); 
			tmp[u8Idx++] = Hex2ASCII((XorSum ) & 0x0f); 
			ptEncryptGPSTXMsg->MsgLen = sprintf((s8*)ptEncryptGPSTXMsg->MsgBuf ,"$get_public_key,0,*%s\r\n",tmp);			
			break;
		}
		case EncryptGPSWritePlatformId:
		{
			ptEncryptGPSTXMsg->MsgLen = 70;//临时的长度
			ptEncryptGPSTXMsg->MsgBuf = mymalloc(ptEncryptGPSTXMsg->MsgLen);
			memset(ptEncryptGPSTXMsg->MsgBuf, 0, ptEncryptGPSTXMsg->MsgLen );
			ptEncryptGPSTXMsg->MsgLen = sprintf((s8*)ptEncryptGPSTXMsg->MsgBuf ,"$put_id,%d,",g_tPlatformTxEncryptMsg.IDLen);
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),g_tPlatformTxEncryptMsg.ID,g_tPlatformTxEncryptMsg.IDLen);
			ptEncryptGPSTXMsg->MsgLen +=g_tPlatformTxEncryptMsg.IDLen;
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),"*",1);	
			XorSum = GetXorSum((ptEncryptGPSTXMsg->MsgBuf+1), (ptEncryptGPSTXMsg->MsgLen-1));
			memset(tmp,0,sizeof(tmp));
			M_ClrBit(u8Idx); 
			tmp[u8Idx++] = Hex2ASCII((XorSum >> 4) & 0x0f); 
			tmp[u8Idx++] = Hex2ASCII((XorSum ) & 0x0f); 
			ptEncryptGPSTXMsg->MsgLen += 1;
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),tmp,2);
			ptEncryptGPSTXMsg->MsgLen += 2;
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),"\r\n",2);
			ptEncryptGPSTXMsg->MsgLen += 2;
			//ptEncryptGPSTXMsg->MsgLen = sprintf((s8*)ptEncryptGPSTXMsg->MsgBuf ,"$put_id,%d,%x*%s\r\n",g_tPlatformTxEncryptMsg.IDLen,g_tPlatformTxEncryptMsg.ID,tmp);
			//myfree(g_tPlatformTxEncryptMsg.ID,g_tPlatformTxEncryptMsg.IDLen);
			debugPrintf(ptEncryptGPSTXMsg->MsgBuf,ptEncryptGPSTXMsg->MsgLen);
			//memset(g_tPlatformTxEncryptMsg.ID,0,20);
			break;
		}
		case EncryptGPSWritePlatformKey:
		{
			ptEncryptGPSTXMsg->MsgLen = 200;//临时的长度
			ptEncryptGPSTXMsg->MsgBuf = mymalloc(ptEncryptGPSTXMsg->MsgLen);
			memset(ptEncryptGPSTXMsg->MsgBuf, 0, ptEncryptGPSTXMsg->MsgLen );
			memcpy(ptEncryptGPSTXMsg->MsgBuf,"$put_platform_public_key,128,",29);
			ptEncryptGPSTXMsg->MsgLen = 29;
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),g_tPlatformTxEncryptMsg.PubTmpKey,64);
			ptEncryptGPSTXMsg->MsgLen += 64;
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),g_tPlatformTxEncryptMsg.PubKey,64);
			ptEncryptGPSTXMsg->MsgLen += 64;
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),"*",1);
			XorSum = GetXorSum((ptEncryptGPSTXMsg->MsgBuf+1), (ptEncryptGPSTXMsg->MsgLen-1));
			memset(tmp,0,sizeof(tmp));
			M_ClrBit(u8Idx); 
			tmp[u8Idx++] = Hex2ASCII((XorSum >> 4) & 0x0f); 
			tmp[u8Idx++] = Hex2ASCII((XorSum ) & 0x0f); 
			ptEncryptGPSTXMsg->MsgLen += 1;
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),tmp,2);
			ptEncryptGPSTXMsg->MsgLen += 2;
			memcpy((ptEncryptGPSTXMsg->MsgBuf+ptEncryptGPSTXMsg->MsgLen),"\r\n",2);
			ptEncryptGPSTXMsg->MsgLen += 2;
			
			//ptEncryptGPSTXMsg->MsgLen = sprintf((s8*)ptEncryptGPSTXMsg->MsgBuf ,"$put_platform_public_key,128,%s%s*%s\r\n",g_tPlatformTxEncryptMsg.PubTmpKey,g_tPlatformTxEncryptMsg.PubKey,tmp);	
			memset(g_tPlatformTxEncryptMsg.PubKey,0,64);
			memset(g_tPlatformTxEncryptMsg.PubTmpKey,0,64);
			break;
		}
		
		default:
		{
			break;
		} 
	}
	if(ptEncryptGPSTXMsg->MsgBuf == NULL)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


/*****************************************************************************
-Fuction		: EncryptGpsTxProc
-Description	: EncryptGpsTxProc
-Input			: none
-Output 		: none
-Return 		: none
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/

void EncryptGpsTxProc(void)
{
	static _DbaTxInfoSt tEncryptGPSTxInfo; 
	T_EncryptGPSTXMsg tEncryptGPSTXMsg;
	u8 ret; 
	static enum
	{
		GPS_TX_ST_IDLE, 
		GPS_TX_ST_PREPARE, 
		GPS_TX_ST_TX_REQ, 
		GPS_TX_ST_WAIT_END, 
		GPS_TX_ST_END, 
	}senGpsTxSt; 
	switch(senGpsTxSt)
	{
		case GPS_TX_ST_IDLE: 
		{	
			if(IS_FALSE_BIT(tEncryptGPSFlag.EncryptGpsTxReq))
			{
				; 
			}
			else
			{
				M_ClrBit(tEncryptGPSFlag.EncryptGpsTxReq); 
				M_SetBit(tEncryptGPSFlag.EncryptGpsTxBusyFlg); 
				u8GpsTxTimer = 5; //?
				senGpsTxSt = GPS_TX_ST_PREPARE; 
			}
			break;
		}
		case GPS_TX_ST_PREPARE:
		{
			tEncryptGPSTxInfo.enCmdId = DBA_CMD_EXP_FOR_GPS; 
			memset(&tEncryptGPSTXMsg,0,sizeof(tEncryptGPSTXMsg));
			ret =EncryptGpsTxMsgPack(g_EncryptGPSMsgType,&tEncryptGPSTXMsg); 
			if(ret == FALSE)
			{
				debugString("EncryptGpsTxMsgPack err \n");
				senGpsTxSt = GPS_TX_ST_IDLE;
				break;//超时或者改状态
			}
			else
			{
				tEncryptGPSTxInfo.u16Len = tEncryptGPSTXMsg.MsgLen; 
				tEncryptGPSTxInfo.pu8Dat = mymalloc(tEncryptGPSTxInfo.u16Len);
				if(NULL != tEncryptGPSTxInfo.pu8Dat)
				{
				memcpy(tEncryptGPSTxInfo.pu8Dat,tEncryptGPSTXMsg.MsgBuf,tEncryptGPSTxInfo.u16Len); 
				myfree(tEncryptGPSTXMsg.MsgBuf, tEncryptGPSTXMsg.MsgLen);
				u8GpsTxTimer = 5; 
				senGpsTxSt = GPS_TX_ST_TX_REQ; 		
				}
				else if(IS_FALSE_BIT(u8GpsTxTimer))
				{
				senGpsTxSt = GPS_TX_ST_END; 
				debugString("gps tx malloc err\r\n"); 
				}
				else
				{
				; 
				}
			}
			break; 			
		}
		case GPS_TX_ST_TX_REQ:
		{
			if(IS_TRUE_BIT(DbaTxReq(tEncryptGPSTxInfo)))
			//DbaTxReq里面把要发送的内容放到串口中断服务程序中使用的buf里
			{
				senGpsTxSt = GPS_TX_ST_END; 
			}
			else if(IS_FALSE_BIT(u8GpsTxTimer))
			{
				myfree(tEncryptGPSTxInfo.pu8Dat, tEncryptGPSTxInfo.u16Len); 
				debugString("gps tx req error\r\n"); 
				senGpsTxSt = GPS_TX_ST_END; 
			}
			else
			{
				; 
			}
			break;
		}
		case GPS_TX_ST_END: 
		{	
			M_ClrBit(tEncryptGPSFlag.EncryptGpsTxBusyFlg); 
			senGpsTxSt = GPS_TX_ST_IDLE; 
			break; 
		}
		default:
		{	M_ClrBit(tEncryptGPSFlag.EncryptGpsTxReq); 
			M_ClrBit(tEncryptGPSFlag.EncryptGpsTxBusyFlg); 
			senGpsTxSt = GPS_TX_ST_IDLE; 
			break;
		}
	}
}
/*****************************************************************************
-Fuction		: Is186HExpCmd
-Description	: Is186HExpCmd
-Input			: u8 *pubuff, u8 CmdLen
-Output 		: none
-Return 		: none
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/
BOOL Is186HExpCmd(u8 *pubuff, u8 CmdLen)
{
	u8 index;
	u16 u16Len; 
	BOOL ret = FALSE;
	for(index = 0; index < ENCRYPTGPS_MAX_ANLY_NUM; index++)
	{
		u16Len = strlen(g_tEncryptGPSHandleMenu[index].head); 
		if(u16Len != CmdLen) {
			ret = FALSE;
		}
		else {
			if(memcmp(g_tEncryptGPSHandleMenu[index].head, pubuff, CmdLen) == 0)
			{
				ret = TRUE;
				break;
			}		
			else
			{
				ret = FALSE;
			}
		}
	}
	return ret;
}
	
/*****************************************************************************
-Fuction		: EncryptGPSDataRecv
-Description	: EncryptGPSDataRecv
-Input			: u8 gps_data
-Output 		: none
-Return 		: none
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6		 Yu Weifeng 	 Created
******************************************************************************/
void EncryptGPSDataRecv(u8 gps_data)
{
	PT_EncryptRxBuf ptEncryptRxBuf; 
	static u8 temLen[4];
	static u16 ProtocolLen;
	static u16 tem ;
	static u16 DataLen ;
	static u8 CmdLen;
	static enum 
	{
		EncryptGPSDatRxIdle,		
		EncryptGPSDatRxIdleCmdRxing,
		EncryptGPSDatRxIdleLen,
		EncryptGPSDatRxIdleRxing,
		EncryptGPSDatRxIdleSum,
		EncryptGPSDatRxIdleVerify1,
		EncryptGPSDatRxIdleVerify2,
		EncryptGPSDatRxIdleEnd1,
		EncryptGPSDatRxIdleEnd2,
	}EncryptGPSDatRx; 	

	ptEncryptRxBuf= &g_tEncryptRxDevBuf; 
	
	switch(EncryptGPSDatRx)
	{
		case EncryptGPSDatRxIdle: 
			if('$' != gps_data)	
			{
				;
			}
			else	
			{
				memset(ptEncryptRxBuf, 0, sizeof(T_EncryptRxBuf)); 
				ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;
				CmdLen = 0;
				EncryptGPSDatRx = EncryptGPSDatRxIdleCmdRxing; 
			}
			break; 	
			
		case EncryptGPSDatRxIdleCmdRxing:

			ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;
			if(ptEncryptRxBuf->u16Len < (MAX_ENCRYPTGPS_BUF_LEN - 1))	
			{
				
				if(',' != gps_data)	
				{
					CmdLen++;
				}
				else	
				{
					tem = 0;
					ProtocolLen = 0;
					memset(temLen,0,sizeof(temLen));
					if(Is186HExpCmd(&ptEncryptRxBuf->u8Con[1], CmdLen)) {
						EncryptGPSDatRx = EncryptGPSDatRxIdleLen; 
					}
					else {
						EncryptGPSDatRx = EncryptGPSDatRxIdle; 
					}
				}
			}
			else	
			{ 
				debugString("Encryptgps data len Err1!\n");
				EncryptGPSDatRx = EncryptGPSDatRxIdle;  
			}
			break; 
			
		case EncryptGPSDatRxIdleLen:
			//debugPrintf(ptEncryptRxBuf->u8Con,ptEncryptRxBuf->u16Len);
			ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;
			temLen[tem] = gps_data;
			tem++;
			if(ptEncryptRxBuf->u16Len < (MAX_ENCRYPTGPS_BUF_LEN - 1))	
			{
				
				if(',' != gps_data)	
				{
					;
				}
				else	
				{
					temLen[sizeof(temLen) - 1] =  '\0';
					ProtocolLen=atoi((const char *)temLen);
					DataLen = 1+CmdLen+1+tem+ProtocolLen+GPSDataEnd;
					EncryptGPSDatRx = EncryptGPSDatRxIdleRxing; 
				}
			}
			else
			{ 
				debugString("Encryptgps data len Err2!\n");
				EncryptGPSDatRx = EncryptGPSDatRxIdle;  
			}
			break; 
						
		case EncryptGPSDatRxIdleRxing:
			//debugPrintf(ptEncryptRxBuf->u8Con,ptEncryptRxBuf->u16Len);
			ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;
			
			if(IS_TRUE_BIT(ProtocolLen)) 
			{
				M_NOT_ZERO_DEC(ProtocolLen);;
			}
			else 
			{
				;
			}
			
			if(IS_TRUE_BIT(ProtocolLen)) 
			{
				;
			}
			else 
			{
				EncryptGPSDatRx = EncryptGPSDatRxIdleSum; 
			}	
			break;

		case EncryptGPSDatRxIdleSum:
			//debugPrintf(ptEncryptRxBuf->u8Con,ptEncryptRxBuf->u16Len);
			ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;			
			if(ptEncryptRxBuf->u16Len < (DataLen ))	
			{
				if('*' != gps_data)	
				{
					debugString("Encryptgps Encrypt content is empty!\n");
					EncryptGPSDatRx = EncryptGPSDatRxIdle;
				}
				else	
				{
					EncryptGPSDatRx = EncryptGPSDatRxIdleVerify1; 
				}
			}
			else	
			{ 
				debugString("Encryptgps data len Err!\n");
				EncryptGPSDatRx = EncryptGPSDatRxIdle;
			}
			break; 
			
		case EncryptGPSDatRxIdleVerify1:
			//debugPrintf(ptEncryptRxBuf->u8Con,ptEncryptRxBuf->u16Len);
			ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;
			if(ptEncryptRxBuf->u16Len < (DataLen))	
			{
				if((0x0D != gps_data) && (0x0A != gps_data))	
				{ 
					EncryptGPSDatRx = EncryptGPSDatRxIdleVerify2; 
				}
				else	
				{
					debugString("Encryptgps data last err!\n");
					EncryptGPSDatRx = EncryptGPSDatRxIdle;
				}
			}
			else	
			{ 
				debugString("Encryptgps data len Err4!\n");
				EncryptGPSDatRx = EncryptGPSDatRxIdle;
			}
			break; 

		case EncryptGPSDatRxIdleVerify2:
			//debugPrintf(ptEncryptRxBuf->u8Con,ptEncryptRxBuf->u16Len);
			ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;
			if(ptEncryptRxBuf->u16Len < (DataLen)) 
			{
				if((0x0D != gps_data) && (0x0A != gps_data))	
				{ 
					EncryptGPSDatRx = EncryptGPSDatRxIdleEnd1; 
				}
				else	
				{
					debugString("Encryptgps data last err!\n");
					EncryptGPSDatRx = EncryptGPSDatRxIdle;
				}
			}
			else	
			{ 
				debugString("Encryptgps data len Err5!\n");
				EncryptGPSDatRx = EncryptGPSDatRxIdle;
			}
			break; 

		case EncryptGPSDatRxIdleEnd1:
			//debugPrintf(ptEncryptRxBuf->u8Con,ptEncryptRxBuf->u16Len);
			ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;
			if(ptEncryptRxBuf->u16Len < DataLen)
			{
				if(0x0D != gps_data) 
				{
					debugString("Encryptgps data last cr err!\n");
					EncryptGPSDatRx = EncryptGPSDatRxIdle;
				}
				else	
				{
					EncryptGPSDatRx = EncryptGPSDatRxIdleEnd2; 
				}
			}
			else	
			{ 
				debugString("Encryptgps data len Err6!\n");
				EncryptGPSDatRx = EncryptGPSDatRxIdle;
			}
			break; 
		
		case EncryptGPSDatRxIdleEnd2:
			debugPrintf(ptEncryptRxBuf->u8Con,ptEncryptRxBuf->u16Len);
			ptEncryptRxBuf->u8Con[ptEncryptRxBuf->u16Len++] = gps_data;
			if(ptEncryptRxBuf->u16Len < (DataLen + 1))	
			{
				if(0x0A != gps_data)
				{
					debugString("Encryptgps data last lf err!\n");
				}
				else	
				{

					
					EncryptGPSRxQWrite(ptEncryptRxBuf);

				}
				EncryptGPSDatRx = EncryptGPSDatRxIdle;
			}
			else	
			{ 
				debugString("Encryptgps data len Err7!\n");
				EncryptGPSDatRx = EncryptGPSDatRxIdle;
			}
			break; 
			
		default:
			EncryptGPSDatRx = EncryptGPSDatRxIdle;
			break; 
	}
}

