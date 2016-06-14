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


T_PlatformTxEncryptMsg g_tPlatformTxEncryptMsg = {0};

/*****************************************************************************
-Fuction 		: EncryptGSMAnalyse
-Description 	: EncryptGSMAnalyse
-Input 		  	: none
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
******************************************************************************/
void EncryptGSMAnalyse(void)
{
	switch(g_tPlatformTxEncryptMsg.MsgType)
	{
		case KeyAgreementParam:
		{
			if(g_tPlatformTxEncryptMsg.RevFlg == TRUE)
			{
				g_tPlatformTxEncryptMsg.RevFlg = FALSE;
			}
			else
			{
				; 
			}
		}
		break;
		
		case KeyAgreementInform:
		{
			if(g_tPlatformTxEncryptMsg.RevFlg == TRUE)
			{
				g_tEncryptMsg.IsKeyAgreeNotice = TRUE;
				g_tPlatformTxEncryptMsg.RevFlg = FALSE;

			}
			else
			{
				g_tEncryptMsg.IsKeyAgreeNotice = FALSE;
			}
		}
		break;
		case SetTerminalParam:
		{
			if(g_tPlatformTxEncryptMsg.RevFlg == TRUE)
			{
				if(g_tPlatformTxEncryptMsg.SetParamVal == 0x02)
				{
					Sys_para.IsEncrypt = FALSE;
				}
				else
				{
					Sys_para.IsEncrypt = TRUE;
				}
				g_tPlatformTxEncryptMsg.RevFlg = FALSE;

			}
			else
			{
				
				Sys_para.IsEncrypt = TRUE; 
			}
		}
		break;
	
		default:
		{
		break;
		}
		

	}
}










