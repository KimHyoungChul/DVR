/*
 *  srv_vs_vms.c
 *  PlayerCore
 *
 *  Created by Kelvin on 2/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "srv_com_handler.h"
#include "srv_vs_vmstcp.h"
#include "srv_vs_vmsdvr.h"
#include "srv_vs_vms.h"

#include "up_ui_context.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <pthread.h>

static UPSint m_sequence = 0;
static systemparameter_t *m_system = NULL;

#define NET_SEND_TIMEOUT_INTEGAL    2000
#define NET_RECV_TIMEOUT_INTEGAL    20000

UPResult net_sock_send(UPSocketHandle socket, const UPSint8* dataBuf, UPSint dataLen, UPSint timeout)
{
	UPSint offset, waitTimes, ret = 0;
	
	if(socket == UP_INVALID_HANDLE || dataBuf == UP_NULL || dataLen <= 0) return UPFailure;
	
	//send out all the data
	offset    = 0;
	waitTimes = 0;
	while(waitTimes < 3 && offset < dataLen)
	{
		//check the availability
		if(UP_SOCKET_ERROR_DROPLINE == (ret = up_sock_available(socket, UP_SOCKET_OPERATION_TYPE_SEND, timeout/3)))
		{
			UPLOG(net_sock_send,("not available"))
			return UPFailure;
		}
		
		//ok, send out then
		if(ret > (dataLen - offset) || ret <= 0)
		{
			ret = (dataLen - offset);
		}
		UPLOG(net_sock_send, ("ret: %d", ret));
		
		if(0 >= (ret = up_sock_send(socket, dataBuf, ret)))
		{
			//error happened
			UPLOG(net_sock_send, ("send error happened"));
			printf("offset: %d \n", offset);
			return UPFailure;
		}
		UPLOG(net_sock_send, ("offset: %d, ret: %d", offset, ret));		
		
		offset += ret;
	}
	
	if(waitTimes == 3)
	{
		//timeout then
		return UPFailure;
	}
	
	return UPSuccess;
}

UPSint8* net_sock_recvByLength(UPSocketHandle socket, UPSint length, UPSint timeout)
{
	UPSint   offset, waitTimes, ret;
    UPSint8* recvData;
	
	if(socket == UP_INVALID_HANDLE) return UP_NULL;
	
	//get the availibility firstly
	recvData     = (UPSint8*)memset(ucm_malloc(UP_NULL, length), 0, length);
	ret = offset = waitTimes = 0;
	
	while(waitTimes < 20)
	{
		//check the availability
		if(UP_SOCKET_ERROR_OTHER == (ret = up_sock_available(socket, UP_SOCKET_OPERATION_TYPE_RECEIVE, timeout/20)))
		{
			if(offset == length && recvData)
			{
				return recvData;
			}
		}
		
		if(ret == UP_SOCKET_ERROR_TIMEOUT)
		{			
			waitTimes++;
			continue;
		}
		
		if(offset == length && recvData)
		{
			return recvData;
		}
		
		UPLOG(net_sock_recv, ("ret: %d", ret));
		
		//receive the header firstly
		if(ret + offset < length)
		{
			if(ret && 0 > (ret = up_sock_receive(socket, recvData + offset, ret)))
			{
				if(offset == length && recvData)
				{
					return recvData;
				}
				UPLOG(net_sock_recvByLength, ("socket recv error"));
				ucm_free(UP_NULL, recvData);
				return UP_NULL;
			}
		}
		else
		{
			if(ret && 0 > (ret = up_sock_receive(socket, recvData + offset, (length - offset))))
			{
				if(offset == length && recvData)
				{
					return recvData;
				}
				UPLOG(net_sock_recvByLength, ("socket recv error"));
				ucm_free(UP_NULL, recvData);
				return UP_NULL;
			}
		}
		
		if(ret == 0)
		{
			if(offset == length && recvData)
			{
				UPLOG(net_sock_recv, ("socket recv finish"));
				return recvData;
			}
			else
			{
				UPLOG(net_sock_recvByLength, ("socket recv error"));
				if(recvData)
				{
					ucm_free(UP_NULL, recvData);
					recvData = UP_NULL;
				}
			}
			
		}
		
	    waitTimes++;
		offset += ret;
		if(offset == length)
		{
			UPLOG(net_sock_recvByLength, ("socket recv finish"));
			return recvData;
		}
		UPLOG(net_sock_recv, ("ret: %d", ret));
		
		//timeout = timeout/2;
	}
	
	if(offset == length && recvData)
	{
		return recvData;
	}
	
	ucm_free(UP_NULL, recvData);
	return UP_NULL;
	
}

UPSint vs_vms_login(const UPSint8* addr, UPSint32 port, const UPUint8* userName, const UPUint8* password)
{
	UPSint ret = 0;
	UPResult result = UPFailure;
	UPSocketHandle socket = UP_NULL;
	Net_Protocol_Header *netHeader = UP_NULL;
	vms_resp_head *resHeader;
	char *rBuf = NULL;
	char sBuf[500];
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	
	socket = srv_com_connect((UPSint8*)addr, port);
	if(socket == UP_NULL)
	{
		printf("connect peer error \n");
		return -1;
	}
	UPLOG(vs_vms_login, ("connect success"));
	
	netHeader = (Net_Protocol_Header*)sBuf;
	netHeader->MsgType = htonl(VMS_USRPSW);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(USRPSW_vms));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	if(userName)
	{
		memcpy(sBuf + sizeof(Net_Protocol_Header), userName, strlen((const char*)userName));
	}
	if(password)
	{
		memcpy(sBuf + sizeof(Net_Protocol_Header) + VS_USRNAME_LEN, password, strlen((const char*)password));
	}
	
	result = net_sock_send(socket, (UPSint8*)sBuf, 500, NET_SEND_TIMEOUT_INTEGAL);
	if(result == UPFailure)
	{
		printf("send data error \n");
		up_sock_close(socket);
		return -1;
	}
	UPLOG(vs_vms_login, ("send request success"));
	
	rBuf = (char*)net_sock_recvByLength(socket, sizeof(vms_resp_head), NET_RECV_TIMEOUT_INTEGAL);
	if(rBuf == UP_NULL)
	{
		printf("receive data error \n");
		up_sock_close(socket);
		return -1;
	}
	UPLOG(vs_vms_login, ("recv response success"));
	
	resHeader = (vms_resp_head *)rBuf;
	ret = resHeader->errorcode;
	
	up_sock_close(socket);
	if(rBuf)
	{
		ucm_free(UP_NULL, rBuf);
		rBuf = UP_NULL;
	}
	if(ret == LOGIN_FAILED)
	{
		UPLOG(vs_vms_login, ("login failed"));
	}
	else if(ret == NORMAL_USER)
	{
		UPLOG(vs_vms_login, ("login as normal user"));
	}
	else if(ret == MANAGER)
	{
		UPLOG(vs_vms_login, ("login as super user"));
	}
	
	return ret;
}

UPSint vs_vms_getChnNumber(UPSint8* addr, UPSint32 port)
{
	UPResult result = UPFailure;	
	UPSint chNum;
	UPSocketHandle socket = UP_NULL;
	Net_Protocol_Header *netHeader = UP_NULL;
	ChSignalState_vms chState;
	char sBuf[500];
	char *rBuf;
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	memset((void*)&chState, 0x0, sizeof(ChSignalState_vms));
	chState.ch_id = 1;
	
	if(addr == UP_NULL)
	{
		UPLOG(vs_vms_getChnNumber, ("invailiable IP address"));
		return -1;
	}
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_getChnNumber, ("connect server failer"));
		return -1;
	}
	UPLOG(vs_vms_getChnNumber, ("connect success"));
	
	netHeader = (Net_Protocol_Header*)sBuf;
	netHeader->MsgType = htonl(VMS_GET_CH_SIGNAL);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(ChSignalState_vms));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	memcpy(sBuf + sizeof(Net_Protocol_Header), &chState, sizeof(ChSignalState_vms));
	
	result = net_sock_send(socket, (UPSint8*)sBuf, 500, NET_SEND_TIMEOUT_INTEGAL);
	if(result == UPFailure)
	{
		UPLOG(vs_vms_getChnNumber, ("send data error"));
		up_sock_close(socket);
		return -1;
	}
	UPLOG(vs_vms_getChnNumber, ("send success"));
	
	rBuf = (char*)net_sock_recvByLength(socket, sizeof(Net_Protocol_Header) + sizeof(ChSignalState_vms), NET_RECV_TIMEOUT_INTEGAL);
	if(rBuf == UP_NULL)
	{
		up_sock_close(socket);
		return -1;
	}
	
	UPLOG(vs_vms_getChnNumber, ("receive success"));
	
	up_sock_close(socket);
	
	memcpy((void*)&chState, rBuf + sizeof(Net_Protocol_Header), sizeof(ChSignalState_vms));
	UPLOG(vs_vms_getChnNumber, ("chid: %d, state: %x", chState.ch_id, chState.State));
	
	if(rBuf)
	{
		ucm_free(UP_NULL, rBuf);
		rBuf = UP_NULL;
	}
	QueryChannelCount(chState, chNum);
	
	return chNum;
}

static void dump_systemParam(systemparameter_t *pSystem)
{
	printf("network setting: \n");
	printf("IP address: %d.%d.%d.%d \n", pSystem->setup.NetworkSetting.IPAddr[0], 
		   pSystem->setup.NetworkSetting.IPAddr[1], pSystem->setup.NetworkSetting.IPAddr[2], pSystem->setup.NetworkSetting.IPAddr[3]);
	printf("Net mask: %d.%d.%d.%d \n", pSystem->setup.NetworkSetting.NetMask[0], pSystem->setup.NetworkSetting.NetMask[1], 
		   pSystem->setup.NetworkSetting.NetMask[2], pSystem->setup.NetworkSetting.NetMask[3]);
	printf("Gate way: %d.%d.%d.%d \n", pSystem->setup.NetworkSetting.GateWay[0], pSystem->setup.NetworkSetting.GateWay[1], 
		   pSystem->setup.NetworkSetting.GateWay[2], pSystem->setup.NetworkSetting.GateWay[3]);
	printf("media port: %d \n", pSystem->setup.NetworkSetting.Port);
	printf("web port: %d \n", pSystem->setup.NetworkSetting.WebPort);
	printf("dns: %d.%d.%d.%d \n", pSystem->setup.NetworkSetting.dns[0], pSystem->setup.NetworkSetting.dns[1], 
		   pSystem->setup.NetworkSetting.dns[2], pSystem->setup.NetworkSetting.dns[3]);
	
	printf("DDNS setting: \n");
	printf("Host name: %s \n", pSystem->setup.DDNSSetting.HostName);
	printf("Srv address: %s \n", pSystem->setup.DDNSSetting.ServerAddr);
	printf("User name: %s \n", pSystem->setup.DDNSSetting.UserName);
	printf("User pwd: %s \n", pSystem->setup.DDNSSetting.UserPW);
	
	printf("IEClient setting: \n");
	printf("User name: %s \n", pSystem->setup.ieclient.UserPwd);
	printf("Admin pwd: %s \n", pSystem->setup.ieclient.AdminPwd);
	
	printf("email setting: \n");
	printf("smtp: %s \n", pSystem->setup.email.SMTP);
	printf("from: %s \n", pSystem->setup.email.SendEmail);
	printf("from pwd: %s \n", pSystem->setup.email.SendEmailPW);
	printf("recv: %s \n", pSystem->setup.email.RecvEmail);
	printf("port: %d \n", pSystem->setup.email.Port);
}

UPSint vs_vms_setParam(void *pSystem, UPSint length, UPSint8* addr, UPSint32 port)
{
	UPSint ret;
	UPSint len;
	Net_Protocol_Header *netHeader;
	vms_resp_head *pRespHead;
	UPSocketHandle socket = UP_NULL;
	configfile_info_vms *pConfigFileInfo;
	unsigned char sBuf[500];
	unsigned char rBuf[10240];
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	memset((void*)rBuf, 0x0, sizeof(rBuf));
	
	UPLOG(vs_vms_setParam, ("start"));
	
	if(pSystem == UP_NULL)
	{
		UPLOG(vs_vms_setParam, ("system parameters is NULL"));
		return -1;
	}
	if(addr == UP_NULL || port == 0)
	{
		UPLOG(vs_vms_setParam, ("invailable addr"));
		return -1;
	}
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_setParam, ("connect peer error"));
		return -1;
	}
	
	netHeader = (Net_Protocol_Header*)sBuf;
	netHeader->MsgType = htonl(VMS_PARAM_SET);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	pConfigFileInfo = (configfile_info_vms*)(sBuf + sizeof(Net_Protocol_Header));
	pConfigFileInfo->filesize = length;
	
	ret = up_sock_send(socket, sBuf, sizeof(sBuf));
	if(ret < 0)
	{
		UPLOG(vs_vms_setParam, ("send data error"));
		up_sock_close(socket);
		return -1;
	}
	
	len = up_sock_receive(socket, rBuf, sizeof(vms_resp_head));
	if(len <= 0)
	{
		UPLOG(vs_vms_setParam, ("recv data error"));
		up_sock_close(socket);
		return -1;
	}
	pRespHead = (vms_resp_head*)rBuf;
	if(pRespHead->errorcode == 0)
	{		
		up_sock_send(socket, (const UPSint8*)pSystem, length);
	}
	else
	{
		UPLOG(vs_vms_setParam, ("fail"));
		up_sock_close(socket);
		return pRespHead->errorcode;
	}
	
	len = up_sock_receive(socket, rBuf, sizeof(vms_resp_head));
	if(len <= 0)
	{
		UPLOG(vs_vms_setParam, ("recv data error"));
		up_sock_close(socket);
		return -1;
	}
	pRespHead = (vms_resp_head*)rBuf;
	if(pRespHead->errorcode != 0)
	{
		UPLOG(vs_vms_setParam, ("errorcode: %d", pRespHead->errorcode));
		up_sock_close(socket);
		return pRespHead->errorcode;
	}

	up_sock_close(socket);
	return 0;
	
}

UPSint vs_vms_setSystemParam(systemparameter_t *pSystem, UPSint8* addr, UPSint32 port)
{
	UPSint8 cfgBuf[10240];
	memset((void*)cfgBuf, 0x0, sizeof(cfgBuf));
	
	switch(pSystem->nSize)
	{
		case 5576:
			UPLOG(vs_vms_setSystemParam, ("3515 latest"));
			memcpy((void*)cfgBuf, (void*)pSystem, sizeof(systemparameter_t) - 8);
			break;
		case 5448:
			UPLOG(vs_vms_setSystemParam, ("3515 sencond version"));
			memcpy((void*)cfgBuf, (void*)pSystem, 184);
			memcpy((void*)cfgBuf + 184, (void*)pSystem + 312, 5264);
			break;
		case 5384:
			UPLOG(vs_vms_setSystemParam, ("3515 oldest version"));
			memcpy((void*)cfgBuf, (void*)pSystem, 132);
			memcpy((void*)cfgBuf + 132, (void*)pSystem + 156, 28);
			memcpy((void*)cfgBuf + 160, (void*)pSystem + 312, 4716);
			memcpy((void*)cfgBuf + 4876, (void*)pSystem + 5068, 508);
			break;
		case 5312:
			UPLOG(vs_vms_setSystemParam, ("3511/3512 version"));
			memcpy((void*)cfgBuf, (void*)pSystem, 132);
			memcpy((void*)cfgBuf + 132, (void*)pSystem + 156, 28);
			memcpy((void*)cfgBuf + 160, (void*)pSystem + 312, 4716);
			memcpy((void*)cfgBuf + 4876, (void*)pSystem + 5068, 348);
			memcpy((void*)cfgBuf + 5224, (void*)pSystem + 5488, 88);
			break;
		default:
			UPLOG(vs_vms_setSystemParam, ("default version"));
			memcpy((void*)cfgBuf, (void*)pSystem, sizeof(systemparameter_t) - 8);
			break;
	}
	
	return vs_vms_setParam((void*)cfgBuf, pSystem->nSize, addr, port);
}

UPSint vs_vms_getParam(systemparameter_t *pSystem, UPSint8* addr, UPSint32 port)
{
	UPSint ret;
	UPSint len;
	UPSint lenTotal;
	Net_Protocol_Header *netHeader;
	vms_resp_head *pRespHead;
	char sBuf[500];
	char rBuf[10240];
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	memset((void*)rBuf, 0x0, sizeof(rBuf));
	
	UPSocketHandle socket = NULL;
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		printf("connect peer error \n");
		return -1;
	}
	
	netHeader = (Net_Protocol_Header*)sBuf;
	netHeader->MsgType = htonl(VMS_PARAM_GET);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	ret = up_sock_send(socket, sBuf, sizeof(sBuf));
	if(ret < 0)
	{
		printf("send data error \n");
		up_sock_close(socket);
		return -1;
	}
	
	lenTotal = 0;
	len = -1;
	while(len != 0)
	{
		len = up_sock_receive(socket, rBuf + lenTotal, 10240 - lenTotal);
		printf("len: %d \n", len);
		lenTotal += len;
	}
	
	UPLOG(vs_vms_getParam, ("lenTotal: %d", lenTotal));
	pRespHead = (vms_resp_head*)rBuf;
	if(pRespHead->errorcode == 0)
	{
		if(sizeof(systemparameter_t) != 5384)
		{
			printf("not available Version \n");
		}
		if(lenTotal - pRespHead->packagelen == 5384) /*3515*/
		{
			printf("the DVR is 3515 \n");
			memcpy((void*)pSystem, rBuf+pRespHead->packagelen, 132);
			memcpy((void*)pSystem + 156, rBuf+pRespHead->packagelen + 132, 28);
			memcpy((void*)pSystem + 312, rBuf+pRespHead->packagelen + 160, 4716);
			memcpy((void*)pSystem + 5068, rBuf+pRespHead->packagelen + 4876, 508);
			pSystem->nSize = 5384;
		}
		else if(lenTotal - pRespHead->packagelen == 5312) /*3511, 3512*/
		{
			printf("the DVR is 3511/3512 \n");
			memcpy((void*)pSystem, rBuf+pRespHead->packagelen, 132);
			memcpy((void*)pSystem + 156, rBuf+pRespHead->packagelen + 132, 28);
			memcpy((void*)pSystem + 312, rBuf+pRespHead->packagelen + 160, 4716);
			memcpy((void*)pSystem + 5068, rBuf+pRespHead->packagelen + 4876, 348);
			memcpy((void*)pSystem + 5448, rBuf+pRespHead->packagelen + 5224, 88);
			pSystem->nSize = 5312;
		}
		else if(lenTotal - pRespHead->packagelen == 5448)
		{
			UPLOG(vs_vms_getParam, ("new version of software, packet length is: %d ", 5448));
			memcpy((void*)pSystem, rBuf+pRespHead->packagelen, 184);
			memcpy((void*)pSystem + 312, rBuf+pRespHead->packagelen + 184, 5264);
			pSystem->nSize = 5448;
		}
		else if(lenTotal - pRespHead->packagelen == 5576)
		{
			UPLOG(vs_vms_getParam, ("new version of software, packet length is: %d ", 5576));
			UPLOG(vs_vms_getParam, ("systemparameter_t length: %d", sizeof(systemparameter_t)));
			memcpy((void*)pSystem, rBuf+pRespHead->packagelen, sizeof(systemparameter_t) - 8);
			pSystem->nSize = 5576;
		}
	}
	else
	{
		printf("error occur... \n");
		up_sock_close(socket);
		return -1;
	}

	up_sock_close(socket);
	return 0;
}

UPSint vs_vms_getSystemParam(systemparameter_t *pSystem, UPSint8* addr, UPSint32 port)
{
	UPSint ret;
	if(m_system == NULL)
	{
	m_system = (systemparameter_t*)ucm_malloc(UP_NULL, sizeof(systemparameter_t));
	if(m_system == NULL)
	{
		printf("memory pool not enought memory \n");
		return -1;
	}
	memset((void*)m_system, 0x0, sizeof(systemparameter_t));
	}
	
	ret = vs_vms_getParam(m_system, addr, port);
	if(ret == 0)
	{
		dump_systemParam(m_system);
	}
	
	ucm_free(UP_NULL, m_system);
	m_system = NULL;
	
	return ret;
}

UPSint vs_vms_getCalenda(UPSint8* addr, UPSint32 port, UPUint ch, UPUint8 year, UPUint8 month, UPUint8 day, UPUint8 type, UPUint *calendar)
{
	UPSint ret;
	UPResult result;
	UPSint8 sBuf[500];
	UPSint8 *recvBuf;
	UPSocketHandle socket;
	calenda_setting_vms calendaSet;
	Net_Protocol_Header *netHeader;
	vms_resp_head *respHead;
	
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	netHeader = (Net_Protocol_Header*)sBuf;
	
	calendaSet.year = year;
	calendaSet.month = month;
	calendaSet.day = day;
	calendaSet.type = type;
	
	netHeader->MsgType = htonl(VMS_GET_CALENDA);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(calenda_setting_vms));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	memcpy((void*)sBuf + sizeof(Net_Protocol_Header), &calendaSet, sizeof(calenda_setting_vms));
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_getCalenda, ("connect failed"));
		return -1;
	}
	
	result = net_sock_send(socket, sBuf, sizeof(sBuf), NET_SEND_TIMEOUT_INTEGAL);
	if(result == UPFailure)
	{
		UPLOG(vs_vms_getCalenda, ("send data error"));
		up_sock_close(socket);
		return -1;
	}	
	
	respHead = (vms_resp_head*)net_sock_recvByLength(socket, sizeof(vms_resp_head), NET_RECV_TIMEOUT_INTEGAL);
	if(respHead == UP_NULL)
	{
		UPLOG(vs_vms_getCalenda, ("recv data error"));
		up_sock_close(socket);
		return -1;		
	}
	
	if(respHead->errorcode != 0)
	{
		UPLOG(vs_vms_getCalenda, ("errorcode: %d", respHead->errorcode));
		up_sock_close(socket);
		ucm_free(UP_NULL, respHead);
		return -1;
	}
	
	recvBuf = net_sock_recvByLength(socket, sizeof(UPUint), NET_RECV_TIMEOUT_INTEGAL);
	if(recvBuf == UP_NULL)
	{
		UPLOG(vs_vms_getCalenda, ("recv data error"));
		up_sock_close(socket);
		ucm_free(UP_NULL, respHead);
		return -1;		
	}
	
	memcpy((void*)calendar, recvBuf, sizeof(UPUint));
	
	up_sock_close(socket);
	ret = 0;
	if(respHead)
	{
		ucm_free(UP_NULL, respHead);
	}
	if(recvBuf)
	{
		ucm_free(UP_NULL, recvBuf);
	}
	
	return ret;
}

UPSint vs_vms_searchGetNum(UPSint8* addr, UPSint32 port, UPUint ch, UPUint8 year, 
						   UPUint8 month, UPUint8 day, UPSint type, UPUint *fileNum)
{
	UPSint ret;
	UPSint8 sBuf[500];
	UPSocketHandle socket;
	Net_Protocol_Header *netHeader;
	vms_resp_head *respHead;
	search_setting searchSet;
	search_result_num *searchRet;
	UPLOG(vs_vms_searchGetNum, ("------------------get file number"));
	
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	netHeader = (Net_Protocol_Header*)sBuf;
	
	/*init search setting*/
	searchSet.channel = 0xffff;
	searchSet.type = 3;
	searchSet.start_day = day;
	searchSet.start_year = year;
	searchSet.start_month = month;
	searchSet.start_hour = 0;
	searchSet.start_min = 0;
	searchSet.start_sec = 0;
	searchSet.end_hour = 23;
	searchSet.end_min = 59;
	searchSet.end_sec = 59;
	
	/*init vms protocol*/
	netHeader->MsgType = htonl(VMS_SEARCH_FILE_NUM);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(search_setting));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	memcpy(sBuf + sizeof(Net_Protocol_Header), &searchSet, sizeof(search_setting));
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_searchGetNum, ("connect failed"));
		*fileNum = 0;
		return -1;
	}
	UPLOG(vs_vms_searchGetNum, ("connect success"));
	ret = up_sock_send(socket, sBuf, sizeof(sBuf));
	if(ret < 0)
	{
		UPLOG(vs_vms_searchGetNum, ("send data error"));
		up_sock_close(socket);
		*fileNum = 0;
		return -1;
	}
	UPLOG(vs_vms_searchGetNum, ("send success"));
	
	respHead = (vms_resp_head*)net_sock_recvByLength(socket, sizeof(vms_resp_head), NET_RECV_TIMEOUT_INTEGAL);
	if(respHead == UP_NULL)
	{
		UPLOG(vs_vms_searchGetNum, ("recv data error"));
		up_sock_close(socket);
		return -1;		
	}
	
	if(respHead->errorcode != 0)
	{
		UPLOG(vs_vms_searchGetNum, ("server response error"));
		up_sock_close(socket);
		ucm_free(UP_NULL, respHead);
		return -1;
	}
	UPLOG(vs_vms_searchGetNum, ("get file number"));
	searchRet = (search_result_num*)net_sock_recvByLength(socket, sizeof(search_result_num), NET_RECV_TIMEOUT_INTEGAL);
	if(searchRet == UP_NULL)
	{
		UPLOG(vs_vms_searchGetNum, ("recv search result error"));
		up_sock_close(socket);
		ucm_free(UP_NULL, respHead);
		return -1;
	}
	
	UPLOG(vs_vms_searchGetNum, ("------xxxxxx------file number: %d", searchRet->fileNum));
	*fileNum = searchRet->fileNum;
	
	ucm_free(UP_NULL, respHead);
	ucm_free(UP_NULL, searchRet);
	up_sock_close(socket);
	
	return 0;
}

UPSint vs_vms_searchGetPage(UPSint8* addr, UPSint32 port, UPUint ch, UPUint8 year, 
							UPUint8 month, UPUint8 day, UPSint type, UPUint page, UPSint8 **fileList, UPUint *fileNum)
{
	UPSint ret;
	UPSint number;
	UPSint8 sBuf[500];
	UPSint8 *tmp = UP_NULL;
	UPSint len = 0, totalLen = 0;
	UPSocketHandle socket;
	Net_Protocol_Header *netHeader;
	vms_resp_head *respHead;
	search_setting searchSet;
	Search_Page searchPage;
	UPLOG(vs_vms_searchGetPage, ("------------------get file page"));
	
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	netHeader = (Net_Protocol_Header*)sBuf;
	
	/*init search setting*/
	searchSet.channel = 0xffff;
	searchSet.type = 3;
	searchSet.start_day = day;
	searchSet.start_year = year;
	searchSet.start_month = month;
	searchSet.start_hour = 0;
	searchSet.start_min = 0;
	searchSet.start_sec = 0;
	searchSet.end_hour = 23;
	searchSet.end_min = 59;
	searchSet.end_sec = 59;
	
	searchPage.page = page;
	
	/*init vms protocol*/
	netHeader->MsgType = htonl(VMS_SEARCH_FILE_GETPAGE);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(search_setting) + sizeof(Search_Page));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	memcpy(sBuf + sizeof(Net_Protocol_Header), &searchSet, sizeof(search_setting));
	memcpy(sBuf + sizeof(Net_Protocol_Header) + sizeof(search_setting), &searchPage, sizeof(Search_Page));
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_searchGetPage, ("connect failed"));
		return -1;
	}
	UPLOG(vs_vms_searchGetPage, ("connect success"));
	ret = up_sock_send(socket, sBuf, sizeof(sBuf));
	if(ret < 0)
	{
		UPLOG(vs_vms_searchGetPage, ("send data error"));
		up_sock_close(socket);
		return -1;
	}
	UPLOG(vs_vms_searchGetPage, ("send success"));
	
	/*recv response here*/
	respHead = (vms_resp_head*)net_sock_recvByLength(socket, sizeof(vms_resp_head), NET_RECV_TIMEOUT_INTEGAL);
	if(respHead == UP_NULL)
	{
		UPLOG(vs_vms_searchGetPage, ("recv data error"));
		up_sock_close(socket);
		return -1;		
	}
	
	if(respHead->errorcode != 0)
	{
		UPLOG(vs_vms_searchGetPage, ("server response error"));
		up_sock_close(socket);
		ucm_free(UP_NULL, respHead);
		return -1;
	}
	
	number = (respHead->packagelen - sizeof(vms_resp_head))/sizeof(search_file_setting);
	UPLOG(vs_vms_searchGetPage, ("number: %d", number));
	
	*fileList = (UPSint8*)ucm_malloc(UP_NULL, number*sizeof(search_file_setting));
	if(fileList == UP_NULL)
	{
		UPLOG(vs_vms_searchGetPage, ("too many file, not enought memory to save the list"));
		up_sock_close(socket);
		*fileNum = 0;
		*fileList = UP_NULL;
		return -1;
	}
	
	tmp = *fileList;
	totalLen = respHead->packagelen - sizeof(vms_resp_head);
	UPLOG(vs_vms_searchGetPage, ("totalLen: %d", totalLen));
	while(totalLen > 0)
	{
		len = up_sock_receive(socket, tmp, totalLen);
		//UPLOG(vs_vms_searchGetPage, ("get file list, len: %d", len));
		if(len < 0)
		{
			UPLOG(vs_vms_searchGetPage, ("file list recv error"));
			up_sock_close(socket);
			ucm_free(UP_NULL, *fileList);
			return -1;
		}
		totalLen -= len;
		tmp += len;
	}
	
	*fileNum = number;
	ucm_free(UP_NULL, respHead);
	
	up_sock_close(socket);
	
	return 0;
}

UPSint vs_vms_searchFile(UPSint8* addr, UPSint32 port, UPUint ch, UPUint8 year, 
			UPUint8 month, UPUint8 day, UPSint type, UPSint8 **fileList, UPUint *fileNum)
{
	UPSint ret;
	UPSint len = 0, totalLen = 0;
	UPSint number = 0;
	UPSint8 sBuf[500];
	UPSint8 *tmp = UP_NULL;
	UPSocketHandle socket;	
	Net_Protocol_Header *netHeader;
	vms_resp_head *respHead;
	search_setting searchSet;
	
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	netHeader = (Net_Protocol_Header*)sBuf;
	
	/*init search setting*/
	searchSet.channel = 0xff;
	searchSet.type = 3;
	searchSet.start_day = day;
	searchSet.start_year = year;
	searchSet.start_month = month;
	searchSet.start_hour = 0;
	searchSet.start_min = 0;
	searchSet.start_sec = 0;
	searchSet.end_hour = 23;
	searchSet.end_min = 59;
	searchSet.end_sec = 59;
	
	/*init vms protocol*/
	netHeader->MsgType = htonl(VMS_SEARCH_FILE);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(search_setting));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	memcpy(sBuf + sizeof(Net_Protocol_Header), &searchSet, sizeof(search_setting));
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_searchFile, ("connect failed"));
		*fileNum = 0;
		*fileList = UP_NULL;
		return -1;
	}
	UPLOG(vs_vms_searchFile, ("connect success"));
	ret = up_sock_send(socket, sBuf, sizeof(sBuf));
	if(ret < 0)
	{
		UPLOG(vs_vms_searchFile, ("send data error"));
		up_sock_close(socket);
		*fileNum = 0;
		*fileList = UP_NULL;
		return -1;
	}
	UPLOG(vs_vms_searchFile, ("send success"));
	
	respHead = (vms_resp_head*)net_sock_recvByLength(socket, sizeof(vms_resp_head), NET_RECV_TIMEOUT_INTEGAL);
	if(respHead == UP_NULL)
	{
		UPLOG(vs_vms_getCalenda, ("recv data error"));
		up_sock_close(socket);
		return -1;		
	}
	
	if(respHead->errorcode != 0)
	{
		UPLOG(vs_vms_searchFile, ("server response error"));
		up_sock_close(socket);
		ucm_free(UP_NULL, respHead);
		*fileNum = 0;
		*fileList = UP_NULL;
		return -1;
	}
	
	number = (respHead->packagelen - sizeof(vms_resp_head))/sizeof(search_file_setting);
	*fileList = (UPSint8*)ucm_malloc(UP_NULL, number*sizeof(search_file_setting));
	if(fileList == UP_NULL)
	{
		UPLOG(vs_vms_searchFile, ("too many file, not enought memory to save the list"));
		up_sock_close(socket);
		*fileNum = 0;
		*fileList = UP_NULL;
		return -1;
	}
	
	tmp = *fileList;
	totalLen = respHead->packagelen - sizeof(vms_resp_head);
	while(totalLen > 0)
	{
		len = up_sock_receive(socket, tmp, totalLen);
		UPLOG(vs_vms_searchFile, ("get file list, len: %d", len));
		if(len < 0)
		{
			UPLOG(vs_vms_searchFile, ("file list recv error"));
			up_sock_close(socket);
			ucm_free(UP_NULL, *fileList);
			return -1;
		}
		totalLen -= len;
		tmp += len;
	}
	
	*fileNum = number;
	ucm_free(UP_NULL, respHead);
	
	return 0;
}

UPSint vs_vms_ptzControl(UPSint8* addr, UPSint32 port, UPUint ch, PTZControlAction action, UPUint param)
{
	UPSint ret;
	PTZAction ptzAct;
	UPSocketHandle socket;
	UPSint8 sBuf[500];
	UPSint8 *recvBuf = UP_NULL;
	Net_Protocol_Header *netHeader;
	vms_resp_head *respHead;
	
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	netHeader = (Net_Protocol_Header*)sBuf;
	
	netHeader->MsgType = htonl(VMS_PTZ);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(PTZAction));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	memset((void*)&ptzAct, 0x0, sizeof(PTZAction));
	
	ptzAct.channel = 1 << ch;
	ptzAct.para = param;
	
	switch(action)
	{
		case PTZ_ACTION_UP:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_UP"));
			ptzAct.cmd = PTZCONTROL_UP;
			break;
		case PTZ_ACTION_DOWN:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_DOWN"));
			ptzAct.cmd = PTZCONTROL_DOWN;
			break;
		case PTZ_ACTION_LEFT:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_LEFT"));
			ptzAct.cmd = PTZCONTROL_LEFT;
			break;
		case PTZ_ACTION_RIGHT:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_RIGHT"));
			ptzAct.cmd = PTZCONTROL_RIGHT;
			break;
		case PTZ_ACTION_ZOOMIN:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_ZOOMIN"));
			ptzAct.cmd = PTZCONTROL_ZOOM_IN;
			break;
		case PTZ_ACTION_ZOOMOUT:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_ZOOMOUT"));
			ptzAct.cmd = PTZCONTROL_ZOOM_OUT;
			break;
		case PTZ_ACTION_FOCUSFAR:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_FOCUSFAR"));
			ptzAct.cmd = PTZCONTROL_FOCUS_FAR;
			break;
		case PTZ_ACTION_FOCUSNEAR:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_FOCUSNEAR"));
			ptzAct.cmd = PTZCONTROL_FOCUS_NEAR;
			break;
		case PTZ_ACTION_IRISOPEN:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_IRISOPEN"));
			ptzAct.cmd = PTZCONTROL_IRIS_OPEN;
			break;
		case PTZ_ACTION_IRISCLOSE:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_IRISCLOSE"));
			ptzAct.cmd = PTZCONTROL_IRIS_CLOSE;
			break;
		case PTZ_ACTION_AUTOPAN:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_AUTOPAN"));
			ptzAct.cmd = PTZCONTROL_AUTOPAN;
			break;
		case PTZ_ACTION_CRUISE_LOAD:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_CRUISE_LOAD"));
			ptzAct.cmd = PTZCONTROL_LOAD;
			break;
		case PTZ_ACTION_CRUISE_RECALL:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_CRUISE_RECALL"));
			ptzAct.cmd = PTZCONTROL_RECALL;
			break;
		case PTZ_ACTION_CRUISE_START:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_CRUISE_START"));
			ptzAct.cmd = PTZCONTROL_CRUISE;
			break;
		case PTZ_ACTION_CRUISE_CLEARPRESET:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_CRUISE_CLEARPRESET"));
			ptzAct.cmd = PTZCONTROL_CLEARPRESET;
			break;
		case PTZ_ACTION_CRUISE_PRESET:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_CRUISE_PRESET"));
			ptzAct.cmd = PTZCONTROL_PRESET;
			break;
		case PTZ_ACTION_CRUISE_SAVE:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_CRUISE_SAVE"));
			ptzAct.cmd = PTZCONTROL_SAVE;
			break;
		case PTZ_ACTION_CRUISE_STOP:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_CRUISE_STOP"));
			ptzAct.cmd = PTZACTION_CRUISE_STOP;
			break;
		case PTZ_ACTION_STOP:
			UPLOG(vs_vms_ptzControl, ("PTZ_ACTION_STOP"));
			ptzAct.cmd = PTZCONTROL_STOP;
			break;
		default:
			UPLOG(vs_vms_ptzControl, ("default"));
			ptzAct.cmd = PTZCONTROL_STOP;
			break;
	}
	
	memcpy(sBuf + sizeof(Net_Protocol_Header), &ptzAct, sizeof(PTZAction));
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_ptzControl, ("connect failed"));
		return -1;
	}
	
	ret = up_sock_send(socket, sBuf, 500);
	
	//memset((void*)respHead, 0x0, sizeof(vms_resp_head));
	respHead = (vms_resp_head*)net_sock_recvByLength(socket, sizeof(vms_resp_head), NET_RECV_TIMEOUT_INTEGAL);
	if(respHead == UP_NULL)
	{
		UPLOG(vs_vms_getCalenda, ("recv data error"));
		up_sock_close(socket);
		return -1;		
	}
	
	if(respHead->errorcode != 0)
	{
		UPLOG(vs_vms_searchFile, ("server response error"));
		up_sock_close(socket);
		ucm_free(UP_NULL, respHead);
		return -1;
	}
	
	if(respHead->packagelen - sizeof(vms_resp_head) == 4)
	{
		UPLOG(vs_vms_ptzControl, ("get ptz control response"));
		recvBuf = net_sock_recvByLength(socket, sizeof(UPUint), NET_RECV_TIMEOUT_INTEGAL);
		if(recvBuf == UP_NULL)
		{
			UPLOG(vs_vms_getCalenda, ("recv data error"));
			up_sock_close(socket);
			ucm_free(UP_NULL, respHead);
			return -1;		
		}
	}
	
	ret = 0;
	memcpy((void*)&ret, recvBuf, sizeof(UPSint));
	UPLOG(vs_vms_ptzControl, ("preset point number: %d", ret));
//	if(ret < 0)
//	{
//		ret = 0;
//	}
	
	if(respHead)
	{
		ucm_free(UP_NULL, respHead);
		respHead = UP_NULL;
	}
	if(recvBuf)
	{
		ucm_free(UP_NULL, recvBuf);
		recvBuf = UP_NULL;
	}
	up_sock_close(socket);
	socket = UP_NULL;
	
	return ret;
}

static UPSint m_channelNum = 0;
UPSocketHandle vs_vms_videoPreview(SRV_Instance *srvInstance)
{
	UPResult result;
	char sBuf[500];
	Net_Protocol_Header *netHeader = UP_NULL;
	AVMessageHead_vms *pAVMsgHead = UP_NULL;
	NET_Instance *instance = srvInstance->netHandle;
	char *rBuf = UP_NULL;
	vms_resp_head *respBuf;
	UPSocketHandle socket = UP_NULL;
	
	UPLOG(vs_vms_videoPreview, ("(ch%02d)addr: %s, port: %d \n", instance->videoChannel + 1, instance->addr, instance->port));
	socket = srv_com_connect(instance->addr, instance->port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_videoPreview, ("(ch%02d)connect peer error", instance->videoChannel + 1));
		srvInstance->netStatus = SRV_NETWORK_DROPLINE;
		return socket;
	}
	
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	netHeader = (Net_Protocol_Header*)sBuf;
	netHeader->MsgType = htonl(VMS_PREVIEW);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(AVMessageHead_vms));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	
	UPLOG(vs_vms_videoPreview, ("(ch%02d)Username: %s, Possword: %s", instance->videoChannel + 1, instance->username, instance->password));
	pAVMsgHead = (AVMessageHead_vms*)(sBuf + sizeof(Net_Protocol_Header));
	memset((void*)pAVMsgHead, 0x0, sizeof(AVMessageHead_vms));
	pAVMsgHead->channel = htonl(1<<instance->videoChannel);
	pAVMsgHead->CmdID = htonl(CMD_START_REALSTREAM_REQ);
	pAVMsgHead->Length = htonl(0x10);
	strcpy(pAVMsgHead->password, (char*)instance->password);
	strcpy(pAVMsgHead->usrname, (char*)instance->username);
	if(instance->networkType == 1)
	{
		pAVMsgHead->stream_select = TRANS_MAINSTREAM;
	}
	else if(instance->networkType == 0)
	{
		pAVMsgHead->stream_select = TRANS_SUBSTREAM;
	}
	else
	{
		pAVMsgHead->stream_select = TRANS_SUBSTREAM;
	}
	
	pAVMsgHead->trans_mode = TRANS_VIDEO_FRAME;
	m_channelNum++;
	
	result = net_sock_send(socket, (UPSint8*)sBuf, sizeof(sBuf), NET_SEND_TIMEOUT_INTEGAL);
	if(result == UPFailure)
	{
		UPLOG(vs_vms_videoPreview, ("(ch%2d)send data error", instance->videoChannel + 1));
		up_sock_close(socket);
		srvInstance->netStatus = SRV_NETWORK_DROPLINE;
		return UP_NULL;
	}
	
	rBuf = (char*)net_sock_recvByLength(socket, sizeof(vms_resp_head), NET_RECV_TIMEOUT_INTEGAL);
	if(rBuf == UP_NULL)
	{
		UPLOG(vs_vms_videoPreview, ("(ch%02d)receive data error", instance->videoChannel + 1));
		up_sock_close(socket);
		srvInstance->netStatus = SRV_NETWORK_DROPLINE;
		return UP_NULL;
	}
	
	respBuf = (vms_resp_head *)rBuf;
	
	if(respBuf->errorcode != 0)
	{
		UPLOG(vs_vms_videoPreview, ("(ch%2d)server return errorcode, errorcode: %d", instance->videoChannel + 1, respBuf->errorcode));
		up_sock_close(socket);
		if(rBuf)
		{
			ucm_free(UP_NULL, rBuf);
		}
		srvInstance->netStatus = SRV_NETWORK_DROPLINE;
		return UP_NULL;
	}
	
	if(rBuf)
	{
		ucm_free(UP_NULL, rBuf);
	}
	
	return socket;
}

UPSint vs_vms_openAudio(NET_Instance* instance)
{
	if(instance == UP_NULL)
	{
		UPLOG(vs_vms_openAudio, ("not available instance"));
		return -1;
	}
	
	return 0;
}

UPSint vs_vms_isRecvOneFrame(NET_Instance* instance)
{
	UPSint packLen;
	char header[24];
	
	memset(header, 0x0, sizeof(header));
	
	if(memcmp(instance->recvQBuf + 2, "dcH264", 6) == 0)
	{
		memcpy(header, instance->recvQBuf, 24);
		packLen = *((int*)(header + 8)) + 24;
	}
	
	return 0;
}

static UPSint m_vms_setupRecord(SRV_Instance *instance)
{
	struct tm *p;
	time_t timep;
	char *homePath;
	char recPath[64];
	char recFile[128];
	
	memset((void*)recPath, 0x0, sizeof(recPath));
	memset((void*)recFile, 0x0, sizeof(recFile));
	
	homePath = getenv("HOME");
	sprintf(recPath, "%s/DvrRecord/%s/record", homePath, instance->netHandle->addr);
	
	if(up_fs_fileExist(recPath) == 0)
	{
		/*create record path*/
		mkdir((const char*)recPath, 0777);
	}
	
	time(&timep);
	p = gmtime(&timep);
	
	sprintf(recFile, "%s/ch%02d-%d%02d%02d-%02d%02d%02d.264", recPath, instance->netHandle->videoChannel + 1, 
			(1900+p->tm_year), (1+p->tm_mon), p->tm_mday, (p->tm_hour+8), p->tm_min, p->tm_sec);
	if(instance->recFp == UP_NULL)
	{
		instance->recFp = fopen(recFile, "w+");
	}
	
	return 0;
}

UPSint vs_vms_RecvQHandle(SRV_Instance* instance)
{
	UPSint packLen;
	UPSint offset = 0;
	UPSint i = 0;
	char header[24];
	unsigned long long pts = 0;
	PRC_data_entry_str pEntry = UP_NULL;
	
	if(instance == UP_NULL || instance->netHandle == UP_NULL)
	{
		return -1;
	}
		
	while(instance->netHandle->recvQBufLen - offset > 24)
	{
		memset((void*)header, 0x0, sizeof(header));
		if(memcmp(instance->netHandle->recvQBuf + 2 + offset, "dcH264", 6) == 0)
		{
			memcpy(header, instance->netHandle->recvQBuf + offset, 24);
			packLen = (*((int*)(header + 8))+7)/8*8 + 24 + (*((unsigned short*)(header + 12)) + 7) / 8 * 8;
			pts = *((unsigned long long*)(header + 16));
			if(instance->netHandle->recvQBufLen - offset >= packLen)
			{
				while((pEntry = (PRC_data_entry_str)srv_com_poolPop(instance)) == UP_NULL && 
					  instance->videoStreamStatus == SRV_Thread_status_running)
				{
					pthread_mutex_lock((pthread_mutex_t*)instance->videoStreamMutex);
					instance->videoStreamStatus = SRV_Thread_status_idle;
					up_event_wait(instance->videoStreamEvent, instance->videoStreamMutex, UP_EVENT_WAIT_INFINITE);
					pthread_mutex_unlock((pthread_mutex_t*)instance->videoStreamMutex);
				}
				
				if(pEntry != UP_NULL)
				{
					pEntry->dataLen = packLen - 24;
					memcpy((void*)pEntry->data, instance->netHandle->recvQBuf + 24 + offset, pEntry->dataLen);
					pEntry->pts = pts;
					
					srv_com_listPush(instance, pEntry);
					upui_ctx_notify(instance->partner);
				}
				
				/*check record flag*/
				if(instance->recFlag)
				{
					if(instance->recFp == UP_NULL)
					{
						UPLOG(vs_vms_RecvQHandle, ("record file not open"));
						if(memcmp(header + 1, "0dcH264", 7) == 0) /*I Frame*/
						{
							m_vms_setupRecord(instance);
						}
					}
					if(instance->recFp)
					{
						fwrite(instance->netHandle->recvQBuf + offset, 1, packLen, instance->recFp);
					}
				}
				else {
					if(instance->recFp)
					{
						fclose(instance->recFp);
						instance->recFp = UP_NULL;
					}
				}

				/**/
				instance->netHandle->recvQBufLen -= packLen;
				instance->netHandle->recvQBufLen -= offset;
				i = 0;
				memcpy(instance->netHandle->recvQBuf, instance->netHandle->recvQBuf + packLen + offset, instance->netHandle->recvQBufLen);
				offset = 0;
			}
			else
			{
				break;
			}

		}
		else if(memcmp(instance->netHandle->recvQBuf + offset, "3wb", 3) == 0)
		{
			/*audio handler*/
			if(instance->netHandle->recvQBufLen - offset >= 7 + 0xA8)
			{
				/*for test*/
#ifdef AUDIO_RAW_SAVE
				FILE *fp = NULL;
				fp = fopen("/Users/kelvin/source2.pcm", "a");
				if(fp)
				{
					fwrite(instance->netHandle->recvQBuf + offset, 1, 7+0xA8, fp);
					fclose(fp);
				}
#endif /*AUDIO_RAW_SAVE*/
				/*push audio to audio list*/
				instance->netHandle->recvQBufLen -= (0xA8+7);
				instance->netHandle->recvQBufLen -= offset;
				memcpy(instance->netHandle->recvQBuf, instance->netHandle->recvQBuf + 7 + 0xA8 + offset, instance->netHandle->recvQBufLen);
				
				offset = 0;
			}
			else
			{
				break;
			}
		}
		else
		{
			offset++;
		}

	}
	
	if(offset > 0)
	{
		memcpy(instance->netHandle->recvQBuf, instance->netHandle->recvQBuf + offset, instance->netHandle->recvQBufLen - offset);
		instance->netHandle->recvQBufLen -= offset;
	}
	
	return 0;
}

void vs_vms_main(void* data)
{
	UPSint ret = 0;
	UPSint length = 0;
	char *videoFileHeader = UP_NULL;
	UPSocketHandle socket;
	UPSint counter = 0;
	PREVIEW_RESPONSE_MSG *preResMsg = UP_NULL;
	
	SRV_Instance *instance = (SRV_Instance*)data;
		
	if(instance == UP_NULL)
	{
		UPLOG(vs_vms_main, ("NO SRV_Instance setting"));
		return;
	}
	
	socket = vs_vms_videoPreview(instance);
	if(socket == UP_NULL)
	{
		instance->videoStreamStatus = SRV_Thread_status_exited;
		UPLOG(vs_vms_main, ("(ch%02d)request video preview error", instance->netHandle->videoChannel + 1));
		return;
	}
	
	preResMsg = (PREVIEW_RESPONSE_MSG*)net_sock_recvByLength(socket, sizeof(PREVIEW_RESPONSE_MSG), NET_RECV_TIMEOUT_INTEGAL);
	if(preResMsg == UP_NULL)
	{
		UPLOG(vs_vms_main, ("(ch%02d)receive error ", instance->netHandle->videoChannel + 1));
		up_sock_close(socket);
		instance->videoStreamStatus = SRV_Thread_status_exited;
		instance->netStatus = SRV_NETWORK_DROPLINE;
		return;
	}
	
	videoFileHeader = (char*)net_sock_recvByLength(socket, 512, NET_RECV_TIMEOUT_INTEGAL);
	if(videoFileHeader == UP_NULL)
	{
		UPLOG(vs_vms_main, ("(ch%02d)receive error", instance->netHandle->videoChannel + 1));
		up_sock_close(socket);
		instance->videoStreamStatus = SRV_Thread_status_exited;
		instance->netStatus = SRV_NETWORK_DROPLINE;
		if(preResMsg)
		{
			ucm_free(UP_NULL, preResMsg);
			preResMsg = UP_NULL;
		}
		return;
	}
	instance->netStatus = SRV_NETWORK_NORMAL;
	
	UPLOG(vs_vms_main, ("(ch%02d)video preview finish", instance->netHandle->videoChannel + 1));
	
	while(instance->videoStreamStatus == SRV_Thread_status_running ||
		  instance->videoStreamStatus == SRV_Thread_status_idle)
	{
		length = up_sock_available(socket, UP_SOCKET_OPERATION_TYPE_RECEIVE, 100);
		if(length == UP_SOCKET_ERROR_DROPLINE)
		{
			UPLOG(vs_vms_main, ("(ch%02d)network dropline", instance->netHandle->videoChannel + 1));
			instance->videoStreamStatus = SRV_Thread_status_exitReq;
			/*drop line reconnect*/
			instance->netStatus = SRV_NETWORK_DROPLINE;
			continue;
		}
		else if(length == UP_SOCKET_ERROR_OTHER)
		{
			UPLOG(vs_vms_main, ("(ch%02d)network error occur", instance->netHandle->videoChannel + 1));
			instance->videoStreamStatus = SRV_Thread_status_exitReq;
			/*drop line reconnect*/
			instance->netStatus = SRV_NETWORK_DROPLINE;
			continue;
		}
		else if(length == UP_SOCKET_ERROR_TIMEOUT)
		{
			/*timeout occur, wait for a while*/
			usleep(50);
			counter++;
			if(counter == 200)
			{
				instance->netStatus = SRV_NETWORK_TIMEOUT;
				UPLOG(vs_vms_main, ("(ch%02d)network timeout", instance->netHandle->videoChannel + 1));
				counter = 0;
			}
			continue;
		}
		else if(length < 0)
		{
			instance->videoStreamStatus = SRV_Thread_status_exitReq;
			/*timeout reconnect*/
			instance->netStatus = SRV_NETWORK_DROPLINE;
			/*timeout handler*/
			UPLOG(vs_vms_main, ("(ch%02d)please reconnect", instance->netHandle->videoChannel + 1));
			continue;
		}
		counter = 0;
		
		ret = up_sock_receive(socket, instance->netHandle->recvQBuf + instance->netHandle->recvQBufLen, length);
		if(ret > 0)
		{
			instance->netHandle->recvQBufLen += ret;
		}
		else if(ret == 0)
		{
			/*drop line*/
			UPLOG(vs_vms_main, ("(ch%02d)drop line", instance->netHandle->videoChannel));
		}

		
		vs_vms_RecvQHandle(instance);
	}
	
	up_sock_close(socket);
	if(preResMsg)
	{
		ucm_free(UP_NULL, preResMsg);
	}
	
	if(videoFileHeader)
	{
		ucm_free(UP_NULL, videoFileHeader);
	}
	
	instance->videoStreamStatus = SRV_Thread_status_exited;
}

UPSint vs_vms_downloadStop(UPSint8 *addr, UPSint port)
{
	int len;
	UPSint ret = 0;
	char sBuf[500] = {0};
	UPSocketHandle socket = UP_NULL;
	file_msg_vms fileDownloadMsg;
	Net_Protocol_Header *netHeader = UP_NULL;

	memset((void*)&fileDownloadMsg, 0x0, sizeof(file_msg_vms));
	fileDownloadMsg.CmdID = htonl(CMD_STOP_DOWNLOAD_REQ);
	fileDownloadMsg.flag = htonl(0);
	fileDownloadMsg.offset = htonl(0);
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_downloadStop, ("Download init socket error"));
		return -1;
	}
	
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	netHeader = (Net_Protocol_Header*)sBuf;
	netHeader->MsgType = htonl(VMS_FILE_DOWNLOAD);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(file_msg_vms));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	memcpy((char*)sBuf + sizeof(Net_Protocol_Header), (char*)&fileDownloadMsg, sizeof(file_msg_vms));
	ret = up_sock_send(socket, sBuf, sizeof(sBuf));
	if(ret < 0)
	{
		UPLOG(vs_vms_downloadStop, ("send data error"));
		up_sock_close(socket);
		return -1;
	}
	
	vms_resp_head respHead;
	memset((void*)&respHead, 0x0, sizeof(vms_resp_head));
	len = up_sock_receive(socket, (UPSint8*)&respHead, sizeof(vms_resp_head));
	if(len <= 0)
	{
		UPLOG(vs_vms_downloadStop, ("recv data error"));
		up_sock_close(socket);
		return -1;
	}
	if(respHead.errorcode != 0)
	{
		UPLOG(vs_vms_downloadStop, ("server return errorcode"));
		up_sock_close(socket);
		return -1;
	}
	
	UPLOG(vs_vms_downloadStop, ("server stop success"));
	up_sock_close(socket);
	
	return 0;
}

UPSocketHandle vs_vms_downloadStart(UPSint8 *addr, UPSint port, UPSint8 *name, UPUint64 offset)
{
	int len;
	UPSint ret = 0;
	char sBuf[500] = {0};
	UPSocketHandle socket = UP_NULL;
	file_msg_vms fileDownloadMsg;
	Net_Protocol_Header *netHeader = UP_NULL;
		
	memset((void*)&fileDownloadMsg, 0x0, sizeof(file_msg_vms));
	fileDownloadMsg.CmdID = htonl(CMD_START_DOWNLOAD_REQ);
	fileDownloadMsg.flag = htonl(0);
	fileDownloadMsg.offset = htonl(offset);
	
	memcpy(fileDownloadMsg.file_name, name, strlen((char*)name));
	
	socket = srv_com_connect(addr, port);
	if(socket == UP_NULL)
	{
		UPLOG(vs_vms_downloadStop, ("Download init socket error"));
		return UP_NULL;
	}
	
	memset((void*)sBuf, 0x0, sizeof(sBuf));
	netHeader = (Net_Protocol_Header*)sBuf;
	netHeader->MsgType = htonl(VMS_FILE_DOWNLOAD);
	netHeader->PacketLen = htonl(sizeof(Net_Protocol_Header) + sizeof(file_msg_vms));
	netHeader->ProtoVersion = htonl(PROTOCOLVERSION);
	netHeader->SessionNo = htonl(0);
	netHeader->TransactionNo = m_sequence;
	m_sequence++;
	
	memcpy((char*)sBuf + sizeof(Net_Protocol_Header), (char*)&fileDownloadMsg, sizeof(file_msg_vms));
	ret = up_sock_send(socket, sBuf, sizeof(sBuf));
	if(ret < 0)
	{
		UPLOG(vs_vms_downloadStop, ("send data error"));
		up_sock_close(socket);
		return UP_NULL;
	}
	
	vms_resp_head respHead;
	memset((void*)&respHead, 0x0, sizeof(vms_resp_head));
	len = up_sock_receive(socket, (UPSint8*)&respHead, sizeof(vms_resp_head));
	if(len <= 0)
	{
		UPLOG(vs_vms_downloadStop, ("recv data error"));
		up_sock_close(socket);
		return UP_NULL;
	}
	if(respHead.errorcode != 0)
	{
		UPLOG(vs_vms_downloadStop, ("server return errorcode"));
		up_sock_close(socket);
		return UP_NULL;
	}
	
	return socket;
}

void vs_vms_downloadMain(void* data)
{
	UPSint ret = 0;
	UPSint len = 0;
	UPUint totalLen = 0;
	UPSint length = 0;
	UPUint counter = 0;
	char saveFile[128] = {0};
	UPLOG(vs_vms_downloadMain, ("download start"));
	SRV_Download *instance = (SRV_Download*)data;
	if(instance == UP_NULL)
	{
		UPLOG(vs_vms_downloadMain, ("Download instance not available"));
		instance->downloadStatus = SRV_DOWNLOAD_ERROR;
		instance->thStatus = SRV_Thread_status_exited;
		return ;
	}
		
	while((instance->pSock = vs_vms_downloadStart(instance->addr, instance->port, instance->filePath, instance->startOffset)) == UP_NULL)
	{
		UPLOG(vs_vms_downloadMain, ("start download ongoing"));
		usleep(100);
		counter++;
		if(counter == 20)
		{
			break;
		}
	}
	if(instance->pSock == UP_NULL)
	{
		UPLOG(vs_vms_downloadMain, ("Download main download start error"));
		instance->downloadStatus = SRV_DOWNLOAD_ERROR;
		instance->thStatus = SRV_Thread_status_exited;
		return;
	}
	
	len = up_sock_receive(instance->pSock, (UPSint8*)&totalLen, sizeof(totalLen));
	if(len <= 0)
	{
		UPLOG(vs_vms_downloadMain, ("recv totalLen error"));
		up_sock_close(instance->pSock);
		instance->pSock = UP_NULL;
		instance->downloadStatus = SRV_DOWNLOAD_ERROR;
		instance->thStatus = SRV_Thread_status_exited;
		return;
	}
	
	instance->fileSize = totalLen;
	UPLOG(vs_vms_downloadMain, ("Total file size: %d", instance->fileSize));
	
	counter = 0;
	instance->downloadStatus = SRV_DOWNLOAD_ONGOING;
	/*handler*/
	while(instance->thStatus == SRV_Thread_status_running || instance->thStatus == SRV_Thread_status_idle)
	{
		if(instance->startOffset + instance->recvSize >= instance->fileSize)
		{
			UPLOG(vs_vms_downloadMain, ("Search download finish"));
			break;
		}
		length = up_sock_available(instance->pSock, UP_SOCKET_OPERATION_TYPE_RECEIVE, 100);
		if(length == UP_SOCKET_ERROR_DROPLINE)
		{
			UPLOG(vs_vms_downloadMain, ("Search download network dropline"));
			if(instance->startOffset + instance->recvSize != instance->fileSize)
			{
				instance->downloadStatus = SRV_DOWNLOAD_ERROR;
			}
			instance->thStatus = SRV_Thread_status_exitReq;
			/*drop line reconnect*/
			continue;
		}
		else if(length == UP_SOCKET_ERROR_OTHER)
		{
			UPLOG(vs_vms_downloadMain, ("Search download error occur"));
			if(instance->startOffset + instance->recvSize != instance->fileSize)
			{
				instance->downloadStatus = SRV_DOWNLOAD_ERROR;
			}
			instance->thStatus = SRV_Thread_status_exitReq;
			/*drop line reconnect*/
			continue;
		}
		else if(length == UP_SOCKET_ERROR_TIMEOUT)
		{
			/*timeout occur, wait for a while*/
			usleep(50);
			counter++;
			if(counter == 200)
			{
				if(instance->startOffset + instance->recvSize != instance->fileSize)
				{
					instance->downloadStatus = SRV_DOWNLOAD_ERROR;
				}
				instance->thStatus = SRV_Thread_status_exitReq;
				UPLOG(vs_vms_downloadMain, ("Search download network timeout"));
				counter = 0;
			}
			continue;
		}
		else if(length < 0)
		{
			if(instance->startOffset + instance->recvSize != instance->fileSize)
			{
				instance->downloadStatus = SRV_DOWNLOAD_ERROR;
			}
			instance->thStatus = SRV_Thread_status_exitReq;
			UPLOG(vs_vms_downloadMain, ("Search download please reconnect"));
			continue;
		}
		counter = 0;

		ret = up_sock_receive(instance->pSock, instance->recvQBuf + instance->recvQBufLen, length);
		if(ret > 0)
		{
			instance->recvQBufLen += ret;
			instance->recvSize += ret;
		}
		else if(ret == 0)
		{
			/*drop line*/
			UPLOG(vs_vms_downloadMain, ("Search download drop line"));
		}
		
		totalLen = 0;
		while(instance->recvQBufLen > 0)
		{
			if(instance->saveFp == UP_NULL)
			{
				sprintf(saveFile, "%s/%s.264", instance->savePath, instance->saveName);
				instance->saveFp = fopen(saveFile, "a");
				if(instance->saveFp == UP_NULL)
				{
					UPLOG(vs_vms_downloadMain, ("fopen file error"));
					instance->downloadStatus = SRV_DOWNLOAD_ERROR;
					instance->thStatus = SRV_Thread_status_exitReq;
					break;
				}
			}
			if(instance->recvQBufLen > 2048)
			{
				len = fwrite(instance->recvQBuf + totalLen, 1, 2048, instance->saveFp);
				totalLen += len;
				instance->recvQBufLen -= len;
			}
			else
			{
				len = fwrite(instance->recvQBuf + totalLen, 1, instance->recvQBufLen, instance->saveFp);
				totalLen += len;
				instance->recvQBufLen -= len;
			}

		}
		
	}
	
	if(instance->saveFp)
	{
		fclose(instance->saveFp);
		instance->saveFp = UP_NULL;
	}
	
	if(instance->pSock)
	{
		up_sock_close(instance->pSock);
		instance->pSock = UP_NULL;
	}
	
	usleep(100);
	
	UPLOG(vs_vms_downloadMain, ("Search download download end"));
	pthread_mutex_lock((pthread_mutex_t*)instance->thMutex);
	instance->thStatus = SRV_Thread_status_exited;
	pthread_mutex_unlock((pthread_mutex_t*)instance->thMutex);
	if(instance->recvSize + instance->startOffset >= instance->fileSize)
	{
		UPLOG(vs_vms_downloadMain, ("Search download finish"));
		instance->downloadStatus = SRV_DOWNLOAD_FINISH;
	}
	else
	{
		UPLOG(vs_vms_downloadMain, ("Search download not finish, error occur"));
	}
	
	return;
}