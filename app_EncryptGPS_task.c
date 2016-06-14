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
#include "Includes.h"

T_Encrypt g_tEncryptMsg;

extern T_EncryptGPSFlag tEncryptGPSFlag;
#ifdef EncryptGPS
extern OS_EVENT *pstOsQForEncryptGpsRx; 
#endif
u8 EncryptOperDataPassthrough(void);


/*****************************************************************************
-Fuction 		: app_EncryptGPS_Task
-Description 	: app_EncryptGPS_Task
-Input 		  	: none
-Output 		: none
-Return 		: none
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
******************************************************************************/
void app_EncryptGPS_Task(void *pdata)
{
	static T_Encrypt tEncrypt;
	myevent *pstEve; 
	GpsRxCmdBufSt sstGpsRxCmdDat; 
	pstEve = NULL; 
	while(1)	
	{
		u8 u8Err; 
		
		if(NULL == pstEve) 
		{
			; //因后续流程中断多,在此统一释放内存
		}
		else 
		{
			myfree((u8 *)pstEve, pstEve->size); 
			pstEve = NULL; 
		}
		EncryptGpsOprSys();
		EncryptGPSSetTXProc();
		EncryptGpsTxProc();
		EncryptGSMAnalyse();
		pstEve = (myevent *)OSQPend(pstOsQForEncryptGpsRx, OS_TICKS_PER_SEC / 2, &u8Err); 
		if(OS_ERR_NONE != u8Err)
		{
			memset(&sstGpsRxCmdDat, 0, sizeof(sstGpsRxCmdDat));  
		}
		else 
		{
			sstGpsRxCmdDat.u16Len = pstEve->size - ST_MYEVENT_SIZE; 
			sstGpsRxCmdDat.pu8Dat = pstEve->buff;

			
			if(IS_TRUE_BIT(EncryptGPSAnalyse(&tEncrypt, &sstGpsRxCmdDat))) //把密文中所有需要的放到一个结构体里
			{
				OSSchedLock();
				memcpy(&g_tEncryptMsg, &tEncrypt,sizeof(g_tEncryptMsg)); 
				OSSchedUnlock();	
			}
			myfree(sstGpsRxCmdDat.pu8Dat,sstGpsRxCmdDat.u16Len);

		}		
	}
}
/*****************************************************************************
-Fuction 		: EncryptGPSRxQWrite
-Description 	: EncryptGPSRxQWrite
-Input 		  	: GpsRxBufSt *pstGpsRxBuf
-Output 		: none
-Return 		: none
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
******************************************************************************/

void  EncryptGPSRxQWrite(PT_EncryptRxBuf pstGpsRxBuf)
{
		myevent *pstEveTmp; 
		u16 u16Len = ST_MYEVENT_SIZE + pstGpsRxBuf->u16Len; 
		pstEveTmp = (myevent *)mymalloc(u16Len); 
		if(NULL == pstEveTmp) {
			debugString("EncryptGps rx malloc err1\r\n"); 
		}
		else	{
			memset(pstEveTmp, 0, u16Len); 
			pstEveTmp->buff = (u8 *)pstEveTmp + ST_MYEVENT_SIZE;
			pstEveTmp->size = u16Len; 		
			memcpy(pstEveTmp->buff, pstGpsRxBuf->u8Con, pstGpsRxBuf->u16Len); 
			if(OS_ERR_NONE == OSQPost(pstOsQForEncryptGpsRx, pstEveTmp)) 
			{
				; 
			}
			else	
			{
				myfree((u8 *)pstEveTmp, u16Len); 
				debugString("Gps rx write err\r\n"); 
			}
		}					
		
}

/*****************************************************************************
-Fuction 		: EncryptGpsOprSys
-Description 	: EncryptGpsOprSys
-Input 		  	: none
-Output 		: none
-Return 		: none
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
******************************************************************************/
#pragma optimize=none
void EncryptGpsOprSys(void)
{
	static u8 ret; 
	static u8 count;
	static u32 u32TimerBk; 
	u32 u32CurTime;
	static enum
	{
		GpsOprIdle,
		GpsOprGetPasswordstatus,
		GPSOprGetid, 
		GPSOprGetPublicKey, 
		GPSOprSendEncrypt, 
		GPSOprRevEncrypt, 
		GPSOprWritePlatformId,
		GPSOprReadPlatIdRespond,
		GPSOprWritePlatformKey,
		GPSOprReadPlatKeyRespond,
		GPSOprKeyAgreeSuccess,
		GPSOprKeyAgreeFail,
		GPSOprGetEncryptText,
	}EncryptGpsOprStatus; 
	
	switch(EncryptGpsOprStatus)
	{
		case GpsOprIdle: 
		{	
			if((0==g_tEncryptMsg.usCipherTextLen) ||(TRUE ==g_tEncryptMsg.IsKeyAgreeNotice) )
			{
				
				EncryptGpsOprStatus = GpsOprGetPasswordstatus;		
	
			}									
		
		}
		break;
		
		case GpsOprGetPasswordstatus:
		{				
			g_tEncryptMsg.TXMsgType = GET_PASSWORD_STATUS;
			M_SetBit(tEncryptGPSFlag.EncryptSetFlg); 
			EncryptGpsOprStatus = GPSOprGetid;
			u32TimerBk = 0;
			u32TimerBk = OSTimeGet();
		}
		break;
		case GPSOprGetid:
		{
			
			if(TRUE ==g_tEncryptMsg.GettedPasswordStatus)
			{
				if((0 ==g_tEncryptMsg.PasswordStatus)||(TRUE ==g_tEncryptMsg.IsKeyAgreeNotice)) 
				{
					g_tEncryptMsg.TXMsgType = GET_ID;
					M_SetBit(tEncryptGPSFlag.EncryptSetFlg); 
					EncryptGpsOprStatus = GPSOprGetPublicKey;
					u32TimerBk = 0;
					u32TimerBk = OSTimeGet();
				}
				else
				{
					g_tEncryptMsg.TXMsgType = GET_ID;
					M_SetBit(tEncryptGPSFlag.EncryptSetFlg); 
					EncryptGpsOprStatus = GPSOprGetPublicKey;
					u32TimerBk = 0;
					u32TimerBk = OSTimeGet();
					//EncryptGpsOprStatus = GpsOprIdle;
					//u32TimerBk = 0;
				}
			}
			else
			{
				u32CurTime = 0;
				u32CurTime = OSTimeGet();
				if((u32CurTime-u32TimerBk)>(OS_TICKS_PER_SEC*3))
				{
					debugString("GetPasswordstatus fail\r\n");
					EncryptGpsOprStatus = GpsOprIdle;
					u32TimerBk = 0;
				}
				else
				{
					;
				}


			}			
		}
		break; 
		
		case GPSOprGetPublicKey:
		{
			
			if(0 != g_tEncryptMsg.IDLen)
			{
				g_tEncryptMsg.TXMsgType = GET_PUBLIC_KEY ;
				M_SetBit(tEncryptGPSFlag.EncryptSetFlg); 
				EncryptGpsOprStatus = GPSOprSendEncrypt;	
				u32TimerBk = 0;
				u32TimerBk = OSTimeGet();
			}
			else
			{
				u32CurTime = 0;
				u32CurTime = OSTimeGet();
				if((u32CurTime-u32TimerBk)>(OS_TICKS_PER_SEC*3))
				{
					debugString("GET_ID fail\r\n");
					EncryptGpsOprStatus = GpsOprIdle;
					u32TimerBk = 0;
				}
				else
				{
					;
				}				
			}
		}
		break;
		
		case GPSOprSendEncrypt: 
		{	

			ret = 0;
			if(0 !=g_tEncryptMsg.KeyLen)
			{
				M_ClrBit(tEncryptGPSFlag.EncryptSetFlg); 
				u32TimerBk = 0;
				u32TimerBk = OSTimeGet();
				ret=EncryptOperDataPassthrough();
				EncryptGpsOprStatus = GPSOprRevEncrypt;	
			}
			else
			{
				u32CurTime = 0;
				u32CurTime = OSTimeGet();
				if((u32CurTime-u32TimerBk)>(OS_TICKS_PER_SEC*5))
				{
					debugString("GET_PUBLIC_KEY fail\r\n");
					EncryptGpsOprStatus = GpsOprIdle;
					u32TimerBk = 0;
				}
				else
				{
					;
				}				
			}	
		}
		break; 
		
		case GPSOprRevEncrypt: 
		{	
			
			if( ret == TRUE)
			{						
				//memset(gps_intf_info.tEncrypt.ID,0,gps_intf_info.tEncrypt.IDLen);
				//memset(gps_intf_info.tEncrypt.PubKey,0,64);
				//memset(gps_intf_info.tEncrypt.PubTmpKey,0,64);
				EncryptGpsOprStatus = GPSOprWritePlatformId;	
				u32TimerBk = 0;
				u32TimerBk = OSTimeGet();
			}
			else
			{
				u32CurTime = 0;
				u32CurTime = OSTimeGet();
				if((u32CurTime-u32TimerBk)>(OS_TICKS_PER_SEC*50))
				{
					count++;
					if(count ==3 )
					{
						debugString("SendEncrypt fail\r\n");
						EncryptGpsOprStatus = GpsOprIdle;
						u32TimerBk = 0;
						count = 0;
					}
					else
					{
						EncryptGpsOprStatus = GPSOprSendEncrypt;

					}
				}
				else
				{
					;
				}				
			}					
		}
		break; 
		
		case GPSOprWritePlatformId: 
		{	

			if(g_tPlatformTxEncryptMsg.IDLen !=0 )
			{
				//debugPrintf(g_tPlatformTxEncryptMsg.ID,g_tPlatformTxEncryptMsg.IDLen);
				//debugPrintf(g_tPlatformTxEncryptMsg.PubKey,64);
				//debugPrintf(g_tPlatformTxEncryptMsg.PubTmpKey,64);
				g_tEncryptMsg.TXMsgType = WIRTE_PLATFORM_ID ;			
				M_SetBit(tEncryptGPSFlag.EncryptSetFlg);				
				EncryptGpsOprStatus = GPSOprReadPlatIdRespond;
				u32TimerBk = 0;
				u32TimerBk = OSTimeGet();
			}
			else
			{
				u32CurTime = 0;
				u32CurTime = OSTimeGet();
				if((u32CurTime-u32TimerBk)>(OS_TICKS_PER_SEC*30))
				{
					debugString("RevEncrypt fail \r\n");
					EncryptGpsOprStatus = GpsOprIdle;
					u32TimerBk = 0;
				}
				else
				{
					;
				}				
			}						 
		}
		break;
		
		case GPSOprReadPlatIdRespond: 
		{	
			if(g_tEncryptMsg.WritePlatIdRespond != 0)
			{
				myfree(g_tPlatformTxEncryptMsg.ID,g_tPlatformTxEncryptMsg.IDLen);
				EncryptGpsOprStatus = GPSOprWritePlatformKey;	
			}
			else
			{
				u32CurTime = 0;
				u32CurTime = OSTimeGet();
				if((u32CurTime-u32TimerBk)>(OS_TICKS_PER_SEC*5))
				{
					count++;
					if(count ==3 )
					{
						myfree(g_tPlatformTxEncryptMsg.ID,g_tPlatformTxEncryptMsg.IDLen);
						debugString("WritePlatformId fail\r\n");
						EncryptGpsOprStatus = GpsOprIdle;
						u32TimerBk = 0;
						count = 0;
					}
					else
					{
						EncryptGpsOprStatus = GPSOprWritePlatformId;

					}
				}
				else
				{
					;
				}				
			}
		}
		break;
		
		case GPSOprWritePlatformKey: 
		{	
				g_tEncryptMsg.TXMsgType = WIRTE_PLATFORM_KEY ;			
				M_SetBit(tEncryptGPSFlag.EncryptSetFlg); 
				EncryptGpsOprStatus = GPSOprReadPlatKeyRespond;
				u32TimerBk = 0;
				u32TimerBk = OSTimeGet();		
		}
		break; 
		
		case GPSOprReadPlatKeyRespond: 
		{	
			if(g_tEncryptMsg.WritePlatKeyRespond != 0)
			{
				EncryptGpsOprStatus = GPSOprKeyAgreeSuccess;
				
			}
			else
			{
				u32CurTime = 0;
				u32CurTime = OSTimeGet();
				if((u32CurTime-u32TimerBk)>(OS_TICKS_PER_SEC*5))
				{
					count++;
					if(count ==3 )
					{
						debugString("WritePlatformKey  fail\r\n");
						EncryptGpsOprStatus = GPSOprKeyAgreeFail;
						u32TimerBk = 0;
						count = 0;
					}
					else
					{
						EncryptGpsOprStatus = GPSOprWritePlatformKey;

					}
				}
				else
				{
					;
				}
			}
		}
		break;
		
		case GPSOprKeyAgreeSuccess:
		{
			g_tTerminalEncryptAck.Ack = 0;
			g_tTerminalEncryptAck.TerminalEncryptAckReq = TRUE;
			debugString("KeyAgreeSuccess\r\n");
			EncryptGpsOprStatus =  GPSOprGetEncryptText;
			u32TimerBk = 0;
			u32TimerBk = OSTimeGet();
		}	
		break;
		case GPSOprKeyAgreeFail:
		{
			g_tTerminalEncryptAck.Ack = 1;
			g_tTerminalEncryptAck.TerminalEncryptAckReq = TRUE;
			EncryptGpsOprStatus =  GpsOprIdle;
			
		}	
		break;	
		case GPSOprGetEncryptText: 
		{	
			if(g_tEncryptMsg.usCipherTextLen !=0)
			{
				EncryptGpsOprStatus = GpsOprIdle;			
			}							
			else
			{	
				u32CurTime = 0;
				u32CurTime = OSTimeGet();
				if((u32CurTime-u32TimerBk)>(OS_TICKS_PER_SEC*60))
				{
					debugString("GetEncryptText  fail\r\n");
					EncryptGpsOprStatus = GpsOprIdle;
				}
				else
				{
					;
				}
			}
		}
		break; 
		
		default:
		{	
			 
			EncryptGpsOprStatus = GpsOprIdle;
			break;
		}
	}
}
