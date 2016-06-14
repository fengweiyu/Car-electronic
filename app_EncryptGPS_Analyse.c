/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	GPS  186H Communication function module
* Description		: 	GPS  186H Data operation center
* Created			: 	2016.05.30.
* Author			: 	Yu Weifeng
* Function List 		: 	EncryptGPSAnalyse
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "Includes.h"


T_EncryptGPSHandle g_tEncryptGPSHandleMenu[]=
{
	{"encrypt_location", CiphertextAnalyse},
	{"write_password _respond", WritePasswordRespond},
	{"get_password_status_respond", GetPasswordStatusRespond},
	{"get_id_respond", GetIDRespond},	
	{"get_public_key_respond", GetPublicKeyRespond},
	{"put_id_respond", WritePlatformIDRespond},
	{"put_platform_public_key_respond", WritePlatformKeyRespond}
};
const u8 ENCRYPTGPS_MAX_ANLY_NUM = (sizeof(g_tEncryptGPSHandleMenu) / sizeof(T_EncryptGPSHandle)); 

/*****************************************************************************
-Fuction 		: EncryptGPSDatapackVerify
-Description 	: EncryptGPSDatapackVerify
-Input 		  	: GpsRxCmdBufSt *pstGpsRxCmdDat
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
******************************************************************************/
static u8 EncryptGPSDatapackVerify(GpsRxCmdBufSt *pstGpsRxCmdDat)
{
	BOOL boRetVal; 
	u8  u8ChkSum = 0;
	u8  gpsck = 0;
	u8  u8i = 1;
	M_ClrBit(boRetVal); 
	if(NULL != pstGpsRxCmdDat)
	{		
		;
	}
	else
	{ 		
		return boRetVal; 
	}	
	if(pstGpsRxCmdDat->u16Len > MAX_ENCRYPTGPS_BUF_LEN)
	{	
		M_ClrBit(boRetVal); 
	}
	else
	{
		if((pstGpsRxCmdDat->pu8Dat[0] != '$')
		|| (pstGpsRxCmdDat->pu8Dat[pstGpsRxCmdDat->u16Len - 1] != '\n')
		|| (pstGpsRxCmdDat->pu8Dat[pstGpsRxCmdDat->u16Len - 2] != '\r')
		|| (pstGpsRxCmdDat->pu8Dat[pstGpsRxCmdDat->u16Len - 5] != '*'))
		{			
			debugString("GPS_BUFF  err!\n");			
			M_ClrBit(boRetVal); 
		}
		else
		{
			M_ClrBit(u8ChkSum); 
			for(u8i = 1; u8i < (pstGpsRxCmdDat->u16Len - 6 + 1); u8i++)
			{
				u8ChkSum ^= *(pstGpsRxCmdDat->pu8Dat + u8i); 
			}
			u8i++; //skip '*'			
			if(-1 == AsciiToByte(&pstGpsRxCmdDat->pu8Dat[u8i], &gpsck))
			{
			    	M_ClrBit(boRetVal); 
			}
			else
			{
				if(gpsck != u8ChkSum)
				{
				    	M_ClrBit(boRetVal); 
				}
				else
				{
				    	M_SetBit(boRetVal); 
				}
			}
			M_SetBit(boRetVal); 
		}
	}	
	return boRetVal; 
}
/*****************************************************************************
-Fuction 		: GetPasswordStatusRespond
-Description 	: GetPasswordStatusRespond
-Input 		  	: T_Encrypt *ptEncrypt, u8 *buff
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
*****************************************************************************/
u8 GetPasswordStatusRespond(T_Encrypt *ptEncrypt, u8 *buff)
{
	u8 *pTemBuff;
	u16 RespondLen;
	if(NULL == buff) 
	{
		return FALSE;
	}
	pTemBuff = buff +1;
	pTemBuff += ptEncrypt->CmdLen+1;
	//加密命令字长度
	RespondLen=atoi((const char *)pTemBuff);
	pTemBuff = (u8*)strchr((const s8*)pTemBuff, ',');
	pTemBuff ++;
	memcpy(&ptEncrypt->PasswordStatus,pTemBuff,RespondLen);
	ptEncrypt->GettedPasswordStatus = TRUE;
	return TRUE;
}
/*****************************************************************************
-Fuction 		: WritePasswordRespond
-Description 	: WritePasswordRespond
-Input 		  	: T_Encrypt *ptEncrypt, u8 *buff
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
*****************************************************************************/
u8 WritePasswordRespond(T_Encrypt *ptEncrypt, u8 *buff)
{
	u8 *pTemBuff;
	u16 RespondLen;
	if(NULL == buff) 
	{
		return FALSE;
	}
	pTemBuff = buff +1;
	pTemBuff += ptEncrypt->CmdLen+1;
	//加密命令字长度
	RespondLen=atoi((const char *)pTemBuff);
	pTemBuff = (u8*)strchr((const s8*)pTemBuff, ',');
	pTemBuff ++;
	memcpy(&ptEncrypt->WritePasswordStatus,pTemBuff,RespondLen);
	return TRUE;
}

/*****************************************************************************
-Fuction 		: GetIDRespond
-Description 	: GetIDRespond
-Input 		  	: T_Encrypt *ptEncrypt, u8 *buff
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
*****************************************************************************/
u8 GetIDRespond(T_Encrypt *ptEncrypt, u8 *buff)
{
	u8 *pTemBuff;
	u8 *pIDBuff;
	u8 IDLen;
	u8 i;
	if(NULL == buff) 
	{
		return FALSE;
	}
	pTemBuff = buff +1;
	pTemBuff += ptEncrypt->CmdLen+1;
	//加密命令字长度
	IDLen=atoi((const char *)pTemBuff);
	pTemBuff = (u8*)strchr((const s8*)pTemBuff, ',');
	pIDBuff = ++pTemBuff ;
	ptEncrypt->IDLen = IDLen;
	//memcpy(ptEncrypt->ID,pTemBuff,IDLen);
	for(i=0;i<IDLen;i++)
	{
		if(*pIDBuff ==0xFF)
		{
			ptEncrypt->IDLen = i;
			break;		
			
		}
		else
		{
			pIDBuff ++;
		}

	}
	memcpy(ptEncrypt->ID,pTemBuff,ptEncrypt->IDLen);
	return TRUE;
}
/*****************************************************************************
-Fuction 		: GetIDRespond
-Description 	: GetIDRespond
-Input 		  	: T_Encrypt *ptEncrypt, u8 *buff
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
*****************************************************************************/
u8 GetPublicKeyRespond(T_Encrypt *ptEncrypt, u8 *buff)
{
	u8 *pTemBuff;
	u8 KeyLen;
	if(NULL == buff) 
	{
		return FALSE;
	}
	pTemBuff = buff +1;
	pTemBuff += ptEncrypt->CmdLen+1;
	//加密命令字长度
	KeyLen=atoi((const char *)pTemBuff);
	pTemBuff = (u8*)strchr((const s8*)pTemBuff, ',');
	pTemBuff ++;
	ptEncrypt->KeyLen = KeyLen;
	memcpy(ptEncrypt->PubKey,pTemBuff,KeyLen);
	memcpy(ptEncrypt->PubTmpKey,pTemBuff,KeyLen);
	return TRUE;
}
/*****************************************************************************
-Fuction 		: WritePasswordRespond
-Description 	: WritePasswordRespond
-Input 		  	: T_Encrypt *ptEncrypt, u8 *buff
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
*****************************************************************************/
u8 WritePlatformIDRespond(T_Encrypt *ptEncrypt, u8 *buff)
{
	u8 *pTemBuff;
	u16 RespondLen;
	if(NULL == buff) 
	{
		return FALSE;
	}
	pTemBuff = buff +1;
	pTemBuff += ptEncrypt->CmdLen+1;
	//加密命令字长度
	RespondLen=atoi((const char *)pTemBuff);
	pTemBuff = (u8*)strchr((const s8*)pTemBuff, ',');
	pTemBuff ++;
	memcpy(&ptEncrypt->WritePlatIdRespond,pTemBuff,RespondLen);
	return TRUE;
}
/*****************************************************************************
-Fuction 		: WritePasswordRespond
-Description 	: WritePasswordRespond
-Input 		  	: T_Encrypt *ptEncrypt, u8 *buff
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
*****************************************************************************/
u8 WritePlatformKeyRespond(T_Encrypt *ptEncrypt, u8 *buff)
{
	u8 *pTemBuff;
	u16 RespondLen;
	if(NULL == buff) 
	{
		return FALSE;
	}
	pTemBuff = buff +1;
	pTemBuff += ptEncrypt->CmdLen+1;
	//加密命令字长度
	RespondLen=atoi((const char *)pTemBuff);
	pTemBuff = (u8*)strchr((const s8*)pTemBuff, ',');
	pTemBuff ++;
	memcpy(&ptEncrypt->WritePlatKeyRespond,pTemBuff,RespondLen);
	return TRUE;
}
/*****************************************************************************
-Fuction 		: CiphertextAnalyse
-Description 	: CiphertextAnalyse
-Input 		  	: gps_local_t *local_info, u8 *buff
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
******************************************************************************/
u8 CiphertextAnalyse (T_Encrypt *ptEncrypt, u8 *buff)
{
	u8 *com_para;
	u16 CiphertextLen;
	if(NULL == buff) 
	{
		return FALSE;
	}
	com_para = buff +1;
	com_para += ptEncrypt->CmdLen+1;
	//加密命令字长度
	CiphertextLen=atoi((const char *)com_para);
	com_para = (u8*)strchr((const s8*)com_para, ',');
	com_para ++;		
	//指向协议内容
	ptEncrypt->usCipherTextLen= CiphertextLen;
	if(ptEncrypt->usCipherTextLen == 0)
	{
		return FALSE;
	}
	memset(ptEncrypt->ucCipherText,0,sizeof(ptEncrypt->ucCipherText));
	memcpy(ptEncrypt->ucCipherText,com_para,CiphertextLen);
	debugPrintfHex2Ascii(ptEncrypt->ucCipherText,ptEncrypt->usCipherTextLen);
	return TRUE;
}
/*****************************************************************************
-Fuction 		: EncryptGPSAnalyse
-Description 	: EncryptGPSAnalyse
-Input 		  	: gps_local_t *local_info, GpsRxCmdBufSt *pstGpsRxCmdDat
-Output 		: none
-Return 		: True/False
* Modify Date     Version        Author	          Modification
* -----------------------------------------------
* 2016/05/31	   V1.0.6	     Yu Weifeng	     Created
******************************************************************************/
u8 EncryptGPSAnalyse(T_Encrypt *ptEncrypt, GpsRxCmdBufSt *pstGpsRxCmdDat)
{
	u8 index;
	u8 *start;
	u8 *end;
	if(IS_FALSE_BIT(EncryptGPSDatapackVerify(pstGpsRxCmdDat)))
	{
		debugString("EncryptGps pack verify err!\n");
		return(FALSE);
	}
	else
	{
		//No action
	}
	start= pstGpsRxCmdDat->pu8Dat;
	end = (u8*)strchr((const s8*)pstGpsRxCmdDat->pu8Dat, ',');
	ptEncrypt->CmdLen = end-start -1;
	for(index = 0; index < ENCRYPTGPS_MAX_ANLY_NUM; index++)
	{
		if(memcmp(g_tEncryptGPSHandleMenu[index].head, &pstGpsRxCmdDat->pu8Dat[1], ptEncrypt->CmdLen) == 0)
		{
			return(g_tEncryptGPSHandleMenu[index].EncryptGPSHandle(ptEncrypt, pstGpsRxCmdDat->pu8Dat));
		}		
		else
		{
			//No action
		}
	}
	return(FALSE);
}


