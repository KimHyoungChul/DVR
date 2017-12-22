
#ifdef TIXML_USE_STL
	#include <iostream>
	#include <sstream>
	using namespace std;
#else
	#include <stdio.h>
#endif

#if defined( WIN32 ) && defined( TUNE )
	#include <crtdbg.h>
	_CrtMemState startMemState;
	_CrtMemState endMemState;
#endif

#include "tinyxml.h"

#include "kt_language_util.h"
#include "kt_language_str.h"


TiXmlDocument *myDocument = NULL;

int kt_language_load(const char *path)
{
	if(myDocument)
	{
		kt_language_close();
	}
	myDocument = new TiXmlDocument(path);
	if(myDocument == NULL)
	{
		printf("create TiXmlDocument error \n");
		return -1;
	}
	myDocument->LoadFile();

	return 0;
}

int kt_language_close()
{
	printf("kt_language_close START...\n");
	
	if(myDocument)
	{
		delete myDocument;
		myDocument = NULL;
	}
	
	kt_language_resFini();
	
	printf("kt_language_close END...\n");

	return 0;
}

static int kt_language_resFiniLogin()
{
	if(loginTitle)
	{
		free(loginTitle);
		loginTitle = NULL;
	}
	if(loginServer)
	{
		free(loginServer);
		loginServer = NULL;
	}
	if(loginPort)
	{
		free(loginPort);
		loginPort = NULL;
	}
	if(loginPassword)
	{
		free(loginPassword);
		loginPassword = NULL;
	}
	if(loginNetwork)
	{
		free(loginNetwork);
		loginNetwork = NULL;
	}
	if(loginSigned)
	{
		free(loginSigned);
		loginSigned = NULL;
	}	

	return 0;
}

static int kt_language_resFiniPriview()
{
	if(previewTitle)
	{
		free(previewTitle);
		previewTitle = NULL;
	}
	if(previewVideoControl)
	{
		free(previewVideoControl);
		previewVideoControl = NULL;
	}
	if(previewPTZControl)
	{
		free(previewPTZControl);
		previewPTZControl = NULL;
	}
	if(previewPTZCtlZoom)
	{
		free(previewPTZCtlZoom);
		previewPTZCtlZoom = NULL;
	}
	if(previewPTZCtlFocus)
	{
		free(previewPTZCtlFocus);
		previewPTZCtlFocus = NULL;
	}
	if(previewPTZCtlIris)
	{
		free(previewPTZCtlIris);
		previewPTZCtlIris = NULL;
	}
	if(previewSetup)
	{
		free(previewSetup);
		previewSetup = NULL;
	}
	if(previewPlayback)
	{
		free(previewPlayback);
		previewPlayback = NULL;
	}
	if(previewLogout)
	{
		free(previewLogout);
		previewLogout = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniPlayback()
{
	if(playbackTitle)
	{
		free(playbackTitle);
		playbackTitle = NULL;
	}
	if(playbackRefresh)
	{
		free(playbackRefresh);
		playbackRefresh = NULL;
	}
	if(playbackSearch)
	{
		free(playbackSearch);
		playbackSearch = NULL;
	}
	if(playbackDelete)
	{
		free(playbackDelete);
		playbackDelete = NULL;
	}
	if(playbackOpenFS)
	{
		free(playbackOpenFS);
		playbackOpenFS = NULL;
	}
	if(playbackOnline)
	{
		free(playbackOnline);
		playbackOnline = NULL;
	}
	if(playbackHistroy)
	{
		free(playbackHistroy);
		playbackHistroy = NULL;
	}
	if(playbackLocal)
	{
		free(playbackLocal);
		playbackLocal = NULL;
	}
	if(playbackStartTimeStr)
	{
		free(playbackStartTimeStr);
		playbackStartTimeStr = NULL;
	}
	if(playbackStop)
	{
		free(playbackStop);
		playbackStop = NULL;
	}
	if(playbackNormal)
	{
		free(playbackNormal);
		playbackNormal = NULL;
	}
	if(playbackPause)
	{
		free(playbackPause);
		playbackPause = NULL;
	}
	if(playbackFinish)
	{
		free(playbackFinish);
		playbackFinish = NULL;
	}
	if(playbackStep)
	{
		free(playbackStep);
		playbackStep = NULL;
	}
	if(playbackNoFile)
	{
		free(playbackNoFile);
		playbackNoFile = NULL;
	}
	if(playbackRecDay)
	{
		free(playbackRecDay);
		playbackRecDay = NULL;
	}
	if(playbackBackup)
	{
		free(playbackBackup);
		playbackBackup = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniSettingMain()
{
	if(settingMainTitle)
	{
		free(settingMainTitle);
		settingMainTitle = NULL;
	}
	if(settingMainApply)
	{
		free(settingMainApply);
		settingMainApply = NULL;
	}
	if(settingMainRecord)
	{
		free(settingMainRecord);
		settingMainRecord = NULL;
	}
	if(settingMainAlarm)
	{
		free(settingMainAlarm);
		settingMainAlarm = NULL;
	}
	if(settingMainEmail)
	{
		free(settingMainEmail);
		settingMainEmail = NULL;
	}
	if(settingMainPTZ)
	{
		free(settingMainPTZ);
		settingMainPTZ = NULL;
	}
	if(settingMainNetwork)
	{
		free(settingMainNetwork);
		settingMainNetwork = NULL;
	}
	if(settingMainManage)
	{
		free(settingMainManage);
		settingMainManage = NULL;
	}
	if(settingMainInfo)
	{
		free(settingMainInfo);
		settingMainInfo = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniSettingRecord()
{
	if(settingRecordMode)
	{
		free(settingRecordMode);
		settingRecordMode = NULL;
	}
	if(settingRecordAudio)
	{
		free(settingRecordAudio);
		settingRecordAudio = NULL;
	}
	if(settingRecordQuality)
	{
		free(settingRecordQuality);
		settingRecordQuality = NULL;
	}
	if(settingRecordPacketLen)
	{
		free(settingRecordPacketLen);
		settingRecordPacketLen = NULL;
	}
	if(settingRecordResolution)
	{
		free(settingRecordResolution);
		settingRecordResolution = NULL;
	}
	if(settingRecordChannel)
	{
		free(settingRecordChannel);
		settingRecordChannel = NULL;
	}
	if(settingRecordScheSetup)
	{
		free(settingRecordScheSetup);
		settingRecordScheSetup = NULL;
	}
	if(settingRecordScheMotion)
	{
		free(settingRecordScheMotion);
		settingRecordScheMotion = NULL;
	}
	if(settingRecordScheRecord)
	{
		free(settingRecordScheRecord);
		settingRecordScheRecord = NULL;
	}
	if(settingRecordScheNoRec)
	{
		free(settingRecordScheNoRec);
		settingRecordScheNoRec = NULL;
	}
	if(settingRecordModeSche)
	{
		free(settingRecordModeSche);
		settingRecordModeSche = NULL;
	}
	if(settingRecordModePowerOn)
	{
		free(settingRecordModePowerOn);
		settingRecordModePowerOn = NULL;
	}
	if(settingRecordQualityNor)
	{
		free(settingRecordQualityNor);
		settingRecordQualityNor = NULL;
	}
	if(settingRecordQualityMid)
	{
		free(settingRecordQualityMid);
		settingRecordQualityMid = NULL;
	}
	if(settingRecordQualityBest)
	{
		free(settingRecordQualityBest);
		settingRecordQualityBest = NULL;
	}
	if(settingRecordPacketLen15)
	{
		free(settingRecordPacketLen15);
		settingRecordPacketLen15 = NULL;
	}
	if(settingRecordPacketLen30)
	{
		free(settingRecordPacketLen30);
		settingRecordPacketLen30 = NULL;
	}
	if(settingRecordPacketLen45)
	{
		free(settingRecordPacketLen45);
		settingRecordPacketLen45 = NULL;
	}
	if(settingRecordPacketLen60)
	{
		free(settingRecordPacketLen60);
		settingRecordPacketLen60 = NULL;
	}
	if(settingRecordResolD1)
	{
		free(settingRecordResolD1);
		settingRecordResolD1 = NULL;
	}
	if(settingRecordResolHD1)
	{
		free(settingRecordResolHD1);
		settingRecordResolHD1 = NULL;
	}
	if(settingRecordResolCIF)
	{
		free(settingRecordResolCIF);
		settingRecordResolCIF = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniSettingAlarm()
{
	if(settingAlarmChannel)
	{
		free(settingAlarmChannel);
		settingAlarmChannel = NULL;
	}
	if(settingAlarmIOSwitch)
	{
		free(settingAlarmIOSwitch);
		settingAlarmIOSwitch = NULL;
	}
	if(settingAlarmMotion)
	{
		free(settingAlarmMotion);
		settingAlarmMotion = NULL;
	}
	if(settingAlarmSensitivity)
	{
		free(settingAlarmSensitivity);
		settingAlarmSensitivity = NULL;
	}
	if(settingAlarmVideoLose)
	{
		free(settingAlarmVideoLose);
		settingAlarmVideoLose = NULL;
	}
	if(settingAlarmHDDLose)
	{
		free(settingAlarmHDDLose);
		settingAlarmHDDLose = NULL;
	}
	if(settingAlarmHDDSpace)
	{
		free(settingAlarmHDDSpace);
		settingAlarmHDDSpace = NULL;
	}
	if(settingAlarmOutput)
	{
		free(settingAlarmOutput);
		settingAlarmOutput = NULL;
	}
	if(settingAlarmBuzzer)
	{
		free(settingAlarmBuzzer);
		settingAlarmBuzzer = NULL;
	}
	if(settingAlarmPostRec)
	{
		free(settingAlarmPostRec);
		settingAlarmPostRec = NULL;
	}
	if(settingAlarmRegionSetup)
	{
		free(settingAlarmRegionSetup);
		settingAlarmRegionSetup = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniSettingEmail()
{
	if(settingEmailStatus)
	{
		free(settingEmailStatus);
		settingEmailStatus = NULL;
	}
	if(settingEmailSSL)
	{
		free(settingEmailSSL);
		settingEmailSSL = NULL;
	}
	if(settingEmailSmtpPort)
	{
		free(settingEmailSmtpPort);
		settingEmailSmtpPort = NULL;
	}
	if(settingEmailSmtpServer)
	{
		free(settingEmailSmtpServer);
		settingEmailSmtpServer = NULL;
	}
	if(settingEmailSendEmail)
	{
		free(settingEmailSendEmail);
		settingEmailSendEmail = NULL;
	}
	if(settingEmailSendPw)
	{
		free(settingEmailSendPw);
		settingEmailSendPw = NULL;
	}
	if(settingEmailRecvEmail)
	{
		free(settingEmailRecvEmail);
		settingEmailRecvEmail = NULL;
	}

	return 0;
}

static int kt_language_resFiniSettingPTZ()
{
	if(settingPTZProtocol)
	{
		free(settingPTZProtocol);
		settingPTZProtocol = NULL;
	}
	if(settingPTZAddress)
	{
		free(settingPTZAddress);
		settingPTZAddress = NULL;
	}
	if(settingPTZBaudrate)
	{
		free(settingPTZBaudrate);
		settingPTZBaudrate = NULL;
	}
	if(settingPTZDataBit)
	{
		free(settingPTZDataBit);
		settingPTZDataBit = NULL;
	}
	if(settingPTZStopBit)
	{
		free(settingPTZStopBit);
		settingPTZStopBit = NULL;
	}
	if(settingPTZVerity)
	{
		free(settingPTZVerity);
		settingPTZVerity = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniSettingNetwork()
{
	if(settingNetworkType)
	{
		free(settingNetworkType);
		settingNetworkType = NULL;
	}
	if(settingNetworkTypeStatic)
	{
		free(settingNetworkTypeStatic);
		settingNetworkTypeStatic = NULL;
	}
	if(settingNetworkIPAdd)
	{
		free(settingNetworkIPAdd);
		settingNetworkIPAdd = NULL;
	}
	if(settingNetworkSubnetM)
	{
		free(settingNetworkSubnetM);
		settingNetworkSubnetM = NULL;
	}
	if(settingNetworkGateway)
	{
		free(settingNetworkGateway);
		settingNetworkGateway = NULL;
	}
	if(settingNetworkDNS)
	{
		free(settingNetworkDNS);
		settingNetworkDNS = NULL;
	}
	if(settingNetworkInternetBt)
	{
		free(settingNetworkInternetBt);
		settingNetworkInternetBt = NULL;
	}
	if(settingNetworkMediaPort)
	{
		free(settingNetworkMediaPort);
		settingNetworkMediaPort = NULL;
	}
	if(settingNetworkWebPort)
	{
		free(settingNetworkWebPort);
		settingNetworkWebPort = NULL;
	}
	if(settingNetworkMobilePort)
	{
		free(settingNetworkMobilePort);
		settingNetworkMobilePort = NULL;
	}
	if(settingNetworkUserName)
	{
		free(settingNetworkUserName);
		settingNetworkUserName = NULL;
	}
	if(settingNetworkPassword)
	{
		free(settingNetworkPassword);
		settingNetworkPassword = NULL;
	}
	if(settingNetworkDDNSService)
	{
		free(settingNetworkDDNSService);
		settingNetworkDDNSService = NULL;
	}
	if(settingNetworkDDNSHostName)
	{
		free(settingNetworkDDNSHostName);
		settingNetworkDDNSHostName = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniSettingManage()
{
	if(settingManageDVRPassword)
	{
		free(settingManageDVRPassword);
		settingManageDVRPassword = NULL;
	}
	if(settingManageIEPassword)
	{
		free(settingManageIEPassword);
		settingManageIEPassword = NULL;
	}
	if(settingManageUserPassword)
	{
		free(settingManageUserPassword);
		settingManageUserPassword = NULL;
	}
	if(settingManageAdminPassword)
	{
		free(settingManageAdminPassword);
		settingManageAdminPassword = NULL;
	}
	if(settingManageReEnter)
	{
		free(settingManageReEnter);
		settingManageReEnter = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniSettingInfo()
{
	if(settingInfoSerialNum)
	{
		free(settingInfoSerialNum);
		settingInfoSerialNum = NULL;
	}
	if(settingInfoMACAddress)
	{
		free(settingInfoMACAddress);
		settingInfoMACAddress = NULL;
	}
	if(settingInfoVersion)
	{
		free(settingInfoVersion);
		settingInfoVersion = NULL;
	}
	if(settingInfoHDDState)
	{
		free(settingInfoHDDState);
		settingInfoHDDState = NULL;
	}
	if(settingInfoHDD)
	{
		free(settingInfoHDD);
		settingInfoHDD = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniInformation()
{
	if(loginSuccessInfo)
	{
		free(loginSuccessInfo);
		loginSuccessInfo = NULL;
	}
	if(loginFailInfo)
	{
		free(loginFailInfo);
		loginFailInfo = NULL;
	}
	if(setParamSuccessInfo)
	{
		free(setParamSuccessInfo);
		setParamSuccessInfo = NULL;
	}
	if(setParamFailInfo)
	{
		free(setParamFailInfo);
		setParamFailInfo = NULL;
	}
	
	return 0;
}

static int kt_language_resFiniGolbal()
{
	if(golbalAllChannel)
	{
		free(golbalAllChannel);
		golbalAllChannel = NULL;
	}
	if(golbalStartTime)
	{
		free(golbalStartTime);
		golbalStartTime = NULL;
	}
	if(golbalChannel)
	{
		free(golbalChannel);
		golbalChannel = NULL;
	}
	if(golbalSize)
	{
		free(golbalSize);
		golbalSize = NULL;
	}
	if(golbalCommonRec)
	{
		free(golbalCommonRec);
		golbalCommonRec = NULL;
	}
	if(golbalAlarmRec)
	{
		free(golbalAlarmRec);
		golbalAlarmRec = NULL;
	}
	if(golbalAllTypeRec)
	{
		free(golbalAllTypeRec);
		golbalAllTypeRec = NULL;
	}
	if(golbalOpen)
	{
		free(golbalOpen);
		golbalOpen = NULL;
	}
	if(golbalClose)
	{
		free(golbalClose);
		golbalClose = NULL;
	}

	return 0;
}

int kt_language_resFini()
{
	/*login page*/
	kt_language_resFiniLogin();
	/*preview page*/
	kt_language_resFiniPriview();
	/*playback page*/
	kt_language_resFiniPlayback();
	/*setting main page*/
	kt_language_resFiniSettingMain();
	/*setting record page*/
	kt_language_resFiniSettingRecord();
	/*setting alarm page*/
	kt_language_resFiniSettingAlarm();
	/*setting email page*/
	kt_language_resFiniSettingEmail();
	/*setting ptz page*/
	kt_language_resFiniSettingPTZ();
	/*setting network page*/
	kt_language_resFiniSettingNetwork();
	/*setting manage page*/
	kt_language_resFiniSettingManage();
	/*setting info page*/
	kt_language_resFiniSettingInfo();
	/*notify information*/
	kt_language_resFiniInformation();
	/*golbal string*/
	kt_language_resFiniGolbal();
	
	return 0;
}

static char *m_remalloc(char *src, int len)
{
	char *dst;
	if(src)
	{
		free(src);
		src = NULL;
	}
	
	dst = (char*)malloc(len + 8);
	if(dst)
	{
		memset(dst, 0x0, len);
	}
	
	return dst;
}

int kt_language_loginDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "TITLE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			loginTitle = m_remalloc(loginTitle, strlen(string));
			strcpy(loginTitle, string);

		}
		else if(strcmp(string, "IPADDRESS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("IP Address: %s \n", string);
			loginServer = m_remalloc(loginServer, strlen(string));
			strcpy(loginServer, string);
		}
		else if(strcmp(string, "MEDIAPORT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Media Port: %s \n", string);
			loginPort = m_remalloc(loginPort, strlen(string));
			strcpy(loginPort, string);
		}
		else if(strcmp(string, "PASSWORD") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Password: %s\n", string);
			loginPassword = m_remalloc(loginPassword, strlen(string));
			strcpy(loginPassword, string);
		}
		else if(strcmp(string, "NETWORK") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Network: %s \n", string);
			loginNetwork = m_remalloc(loginNetwork, strlen(string));
			strcpy(loginNetwork, string);
		}
		else if(strcmp(string, "IDC_BTN_LOGIN") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Button Login: %s \n", string);
			loginSigned = m_remalloc(loginSigned, strlen(string));
			strcpy(loginSigned, string);
		}
		else if(strcmp(string, "LANGUAGE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Language: %s \n", string);
			loginLanguage = m_remalloc(loginLanguage, strlen(string));
			strcpy(loginLanguage, string);
		}
		else if(strcmp(string, "LAN") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Lan: %s \n", string);
			loginLan = m_remalloc(loginLan, strlen(string));
			strcpy(loginLan, string);
		}
		else if(strcmp(string, "INTERNET") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Internet: %s \n", string);
			loginInternet = m_remalloc(loginInternet, strlen(string));
			strcpy(loginInternet, string);
		}
		else if(strcmp(string, "SEARCH") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Internet: %s \n", string);
			loginSigned = m_remalloc(loginSigned, strlen(string));
			strcpy(loginSigned, string);
		}		
		
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_previewDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	TiXmlElement *child2 = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "TITLE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			previewTitle = m_remalloc(previewTitle, strlen(string));
			strcpy(previewTitle, string);
		}
		else if(strcmp(string, "PLAYCONTROL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Preview Control: %s \n", string);
			previewVideoControl = m_remalloc(previewVideoControl, strlen(string));
			strcpy(previewVideoControl, string);
		}
		else if(strcmp(string, "PTZCONTROL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Ptz Control: %s \n", string);
			previewPTZControl = m_remalloc(previewPTZControl, strlen(string));
			strcpy(previewPTZControl, string);
			
			child2 = (TiXmlElement*)child->FirstChildElement();
			while(child2)
			{
				string = (char*)child2->Value();
				if(strcmp(string, "ZOOM") == 0)
				{
					string = (char*)child2->Attribute("_name_");
					printf("Ptz Control Zoom: %s \n", string);
					previewPTZCtlZoom = m_remalloc(previewPTZCtlZoom, strlen(string));
					strcpy(previewPTZCtlZoom, string);
				}
				else if(strcmp(string, "FOCUS") == 0)
				{
					string = (char*)child2->Attribute("_name_");
					printf("Ptz Control Focus: %s \n", string);
					previewPTZCtlFocus = m_remalloc(previewPTZCtlFocus, strlen(string));
					strcpy(previewPTZCtlFocus, string);
				}
				else if(strcmp(string, "IRIS") == 0)
				{
					string = (char*)child2->Attribute("_name_");
					printf("Ptz Control Iris: %s \n", string);
					previewPTZCtlIris = m_remalloc(previewPTZCtlIris, strlen(string));
					strcpy(previewPTZCtlIris, string);
				}
				child2 = child2->NextSiblingElement();
			}
		}
		else if(strcmp(string, "IDC_BTN_PLAYBACK") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Btn Playback: %s \n", string);
			previewPlayback = m_remalloc(previewPlayback, strlen(string));
			strcpy(previewPlayback, string);
		}
		else if(strcmp(string, "IDC_BTN_SETUP") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Btn setup: %s \n", string);
			previewSetup = m_remalloc(previewSetup, strlen(string));
			strcpy(previewSetup, string);
		}
		else if(strcmp(string, "IDC_BTN_LOGIN_OUT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Btn Logout: %s \n", string);
			previewLogout = m_remalloc(previewLogout, strlen(string));
			strcpy(previewLogout, string);
		}
		
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_playbackDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "TITLE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			playbackTitle = m_remalloc(playbackTitle, strlen(string));
			strcpy(playbackTitle, string);
		}
		else if(strcmp(string, "IDC_BTN_SEARCH") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Btn Search: %s\n", string);
			playbackSearch = m_remalloc(playbackSearch, strlen(string));
			strcpy(playbackSearch, string);
		}
		else if(strcmp(string, "IDC_BTN_REFRESH") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Btn Refresh: %s\n", string);
			playbackRefresh = m_remalloc(playbackRefresh, strlen(string));
			strcpy(playbackRefresh, string);
		}
		else if(strcmp(string, "IDC_BTN_DEL_LIST") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Btn Refresh: %s\n", string);
			playbackDelete = m_remalloc(playbackDelete, strlen(string));
			strcpy(playbackDelete, string);
		}
		else if(strcmp(string, "IDM_BTN_OPENFS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Btn Refresh: %s\n", string);
			playbackOpenFS = m_remalloc(playbackOpenFS, strlen(string));
			strcpy(playbackOpenFS, string);
		}
		else if(strcmp(string, "IDM_TAB_ONLINE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Online: %s\n", string);
			playbackOnline = m_remalloc(playbackOnline, strlen(string));
			strcpy(playbackOnline, string);
		}
		else if(strcmp(string, "IDM_TAB_HISTORY") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab History: %s\n", string);
			playbackHistroy = m_remalloc(playbackHistroy, strlen(string));
			strcpy(playbackHistroy, string);
		}
		else if(strcmp(string, "IDM_TAB_LOCAL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackLocal = m_remalloc(playbackLocal, strlen(string));
			strcpy(playbackLocal, string);
		}
		else if(strcmp(string, "IDM_TAB_SEARCH") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackSearch = m_remalloc(playbackSearch, strlen(string));
			strcpy(playbackSearch, string);
		}
		else if(strcmp(string, "IDC_LABEL_STARTTIME") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackStartTimeStr = m_remalloc(playbackStartTimeStr, strlen(string));
			strcpy(playbackStartTimeStr, string);
		}
		else if(strcmp(string, "IDC_LABEL_STOP") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackStop = m_remalloc(playbackStop, strlen(string));
			strcpy(playbackStop, string);
		}
		else if(strcmp(string, "IDC_LABEL_NORMAL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackNormal = m_remalloc(playbackNormal, strlen(string));
			strcpy(playbackNormal, string);
		}
		else if(strcmp(string, "IDC_LABEL_PAUSE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackPause = m_remalloc(playbackPause, strlen(string));
			strcpy(playbackPause, string);
		}
		else if(strcmp(string, "IDC_LABEL_FINISH") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackFinish = m_remalloc(playbackFinish, strlen(string));
			strcpy(playbackFinish, string);
		}
		else if(strcmp(string, "IDC_LABEL_STEP") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackStep = m_remalloc(playbackStep, strlen(string));
			strcpy(playbackStep, string);
		}
		else if(strcmp(string, "IDC_LABEL_NOSELECT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackNoFile = m_remalloc(playbackNoFile, strlen(string));
			strcpy(playbackNoFile, string);
		}
		else if(strcmp(string, "IDC_LABEL_RECDAY") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackRecDay = m_remalloc(playbackRecDay, strlen(string));
			strcpy(playbackRecDay, string);
		}
		else if(strcmp(string, "IDC_LABEL_BACKUP") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Tab Local: %s\n", string);
			playbackBackup = m_remalloc(playbackBackup, strlen(string));
			strcpy(playbackBackup, string);
		}
		
		child = child->NextSiblingElement();		
	}

	return 0;
}

int kt_language_settingRecordDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;

	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
	    string = (char*)child->Value();
		if(strcmp(string, "IDC_STAT_CHANNELL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordChannel= m_remalloc(settingRecordChannel, strlen(string));
			strcpy(settingRecordChannel, string);
		}		
		else if(strcmp(string, "IDC_STAT_RESOLUTION") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordResolution = m_remalloc(settingRecordResolution, strlen(string));
			strcpy(settingRecordResolution, string);
		}
		else if(strcmp(string, "IDC_STAT_PACKETLEN") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordPacketLen = m_remalloc(settingRecordPacketLen, strlen(string));
			strcpy(settingRecordPacketLen, string);
		}
		else if(strcmp(string, "IDC_STAT_QUALITY") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordQuality = m_remalloc(settingRecordQuality, strlen(string));
			strcpy(settingRecordQuality, string);
		}
		else if(strcmp(string, "IDC_STAT_AUDIO") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordAudio = m_remalloc(settingRecordAudio, strlen(string));
			strcpy(settingRecordAudio, string);
		}
		else if(strcmp(string, "IDC_STAT_MODE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordMode = m_remalloc(settingRecordMode, strlen(string));
			strcpy(settingRecordMode, string);
		}
		else if(strcmp(string, "IDC_LABEL_SCHESETUP") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordScheSetup = m_remalloc(settingRecordScheSetup, strlen(string));
			strcpy(settingRecordScheSetup, string);
		}
		else if(strcmp(string, "IDC_RADIO_ALARMREC") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordScheMotion = m_remalloc(settingRecordScheMotion, strlen(string));
			strcpy(settingRecordScheMotion, string);
		}
		else if(strcmp(string, "IDC_RADIO_NORMALREC") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordScheRecord = m_remalloc(settingRecordScheRecord, strlen(string));
			strcpy(settingRecordScheRecord, string);
		}
		else if(strcmp(string, "IDC_RADIO_NOREC") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordScheNoRec = m_remalloc(settingRecordScheNoRec, strlen(string));
			strcpy(settingRecordScheNoRec, string);
		}
		else if(strcmp(string, "IDC_RADIO_RECMODE_TIMED") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordModeSche = m_remalloc(settingRecordModeSche, strlen(string));
			strcpy(settingRecordModeSche, string);
		}
		else if(strcmp(string, "IDC_RADIO_RECMODE_POWERUP") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordModePowerOn = m_remalloc(settingRecordModePowerOn, strlen(string));
			strcpy(settingRecordModePowerOn, string);
		}
		else if(strcmp(string, "IDC_RADIO_QUALITY_NORMAL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordQualityNor = m_remalloc(settingRecordQualityNor, strlen(string));
			strcpy(settingRecordQualityNor, string);
		}
		else if(strcmp(string, "IDC_RADIO_QUALITY_FINE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordQualityMid = m_remalloc(settingRecordQualityMid, strlen(string));
			strcpy(settingRecordQualityMid, string);
		}
		else if(strcmp(string, "IDC_RADIO_QUALITY_BEST") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordQualityBest = m_remalloc(settingRecordQualityBest, strlen(string));
			strcpy(settingRecordQualityBest, string);
		}
		else if(strcmp(string, "IDC_RADIO_15MIN") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordPacketLen15 = m_remalloc(settingRecordPacketLen15, strlen(string));
			strcpy(settingRecordPacketLen15, string);
		}
		else if(strcmp(string, "IDC_RADIO_30MIN") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordPacketLen30 = m_remalloc(settingRecordPacketLen30, strlen(string));
			strcpy(settingRecordPacketLen30, string);
		}
		else if(strcmp(string, "IDC_RADIO_45MIN") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordPacketLen45 = m_remalloc(settingRecordPacketLen45, strlen(string));
			strcpy(settingRecordPacketLen45, string);
		}
		else if(strcmp(string, "IDC_RADIO_60MIN") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordPacketLen60 = m_remalloc(settingRecordPacketLen60, strlen(string));
			strcpy(settingRecordPacketLen60, string);
		}
		else if(strcmp(string, "IDC_RADIO_RESD1") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordResolD1 = m_remalloc(settingRecordResolD1, strlen(string));
			strcpy(settingRecordResolD1, string);
		}
		else if(strcmp(string, "IDC_RADIO_RESHD1") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordResolHD1 = m_remalloc(settingRecordResolHD1, strlen(string));
			strcpy(settingRecordResolHD1, string);
		}
		else if(strcmp(string, "IDC_RADIO_RESCIF") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingRecordResolCIF = m_remalloc(settingRecordResolCIF, strlen(string));
			strcpy(settingRecordResolCIF, string);
		}
		child = child->NextSiblingElement();		
	}
	
	return 0;
}

int kt_language_settingAlarmDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;

	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "IDC_LABEL_CHANNELL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("IOAlarm name: %s\n", string);
			settingAlarmChannel= m_remalloc(settingAlarmChannel, strlen(string));
			strcpy(settingAlarmChannel, string);
		}
		else if(strcmp(string, "IDC_LABEL_IOALARM") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("IOAlarm name: %s\n", string);
			settingAlarmIOSwitch= m_remalloc(settingAlarmIOSwitch, strlen(string));
			strcpy(settingAlarmIOSwitch, string);
		}
		else if(strcmp(string, "IDC_LABEL_MOTIONDETECT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("MotionDetect name: %s\n", string);
			settingAlarmMotion= m_remalloc(settingAlarmMotion, strlen(string));
			strcpy(settingAlarmMotion, string);
		}
		else if(strcmp(string, "IDC_LABEL_MOTION_SENS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Sensitivity name: %s\n", string);
			settingAlarmSensitivity= m_remalloc(settingAlarmSensitivity, strlen(string));
			strcpy(settingAlarmSensitivity, string);
		}
		else if(strcmp(string, "IDC_LABEL_VIDEOLOSS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("VideoLoss name: %s\n", string);
			settingAlarmVideoLose= m_remalloc(settingAlarmVideoLose, strlen(string));
			strcpy(settingAlarmVideoLose, string);
		}
		else if(strcmp(string, "IDC_LABEL_HDDSPACE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("HDDSpace name: %s\n", string);
			settingAlarmHDDSpace= m_remalloc(settingAlarmHDDSpace, strlen(string));
			strcpy(settingAlarmHDDSpace, string);
		}
		else if(strcmp(string, "IDC_LABEL_HDDLOSS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("HDDLose name: %s\n", string);
			settingAlarmHDDLose= m_remalloc(settingAlarmHDDLose, strlen(string));
			strcpy(settingAlarmHDDLose, string);
		}
		else if(strcmp(string, "IDC_LABEL_OUTPUT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("HDDLose name: %s\n", string);
			settingAlarmOutput= m_remalloc(settingAlarmOutput, strlen(string));
			strcpy(settingAlarmOutput, string);
		}
		else if(strcmp(string, "IDC_LABEL_BUZZER") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("HDDLose name: %s\n", string);
			settingAlarmBuzzer= m_remalloc(settingAlarmBuzzer, strlen(string));
			strcpy(settingAlarmBuzzer, string);
		}
		else if(strcmp(string, "IDC_LABEL_POSTREC") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("HDDLose name: %s\n", string);
			settingAlarmPostRec= m_remalloc(settingAlarmPostRec, strlen(string));
			strcpy(settingAlarmPostRec, string);
		}
		else if(strcmp(string, "IDC_LABEL_MDAERA") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("HDDLose name: %s\n", string);
			settingAlarmRegionSetup= m_remalloc(settingAlarmRegionSetup, strlen(string));
			strcpy(settingAlarmRegionSetup, string);
		}
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_settingEmailDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "IDC_LABEL_STATUS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingEmailStatus= m_remalloc(settingEmailStatus, strlen(string));
			strcpy(settingEmailStatus, string);
		}
		else if(strcmp(string, "IDC_LABEL_SSL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("SSL name: %s\n", string);
			settingEmailSSL= m_remalloc(settingEmailSSL, strlen(string));
			strcpy(settingEmailSSL, string);
		}
		else if(strcmp(string, "IDC_LABEL_SMTPPORT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Smtp port name: %s\n", string);
			settingEmailSmtpPort= m_remalloc(settingEmailSmtpPort, strlen(string));
			strcpy(settingEmailSmtpPort, string);
		}
		else if(strcmp(string, "IDC_LABEL_SMTPSERVER") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Smtp server name: %s\n", string);
			settingEmailSmtpServer= m_remalloc(settingEmailSmtpServer, strlen(string));
			strcpy(settingEmailSmtpServer, string);
		}
		else if(strcmp(string, "IDC_LABEL_SENDMAIL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Send email name: %s\n", string);
			settingEmailSendEmail= m_remalloc(settingEmailSendEmail, strlen(string));
			strcpy(settingEmailSendEmail, string);
		}
		else if(strcmp(string, "IDC_LABEL_SENDPASSWORD") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Send password name: %s\n", string);
			settingEmailSendPw= m_remalloc(settingEmailSendPw, strlen(string));
			strcpy(settingEmailSendPw, string);
		}
		else if(strcmp(string, "IDC_LABEL_RECVMAIL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Recv email name: %s\n", string);
			settingEmailRecvEmail= m_remalloc(settingEmailRecvEmail, strlen(string));
			strcpy(settingEmailRecvEmail, string);
		}
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_settingPTZDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "IDC_LABEL_PROTOCOL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingPTZProtocol= m_remalloc(settingPTZProtocol, strlen(string));
			strcpy(settingPTZProtocol, string);
		}
		else if(strcmp(string, "IDC_LABEL_ADDRESS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingPTZAddress= m_remalloc(settingPTZAddress, strlen(string));
			strcpy(settingPTZAddress, string);
		}
		else if(strcmp(string, "IDC_LABEL_BAUDRATE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingPTZBaudrate= m_remalloc(settingPTZBaudrate, strlen(string));
			strcpy(settingPTZBaudrate, string);
		}
		else if(strcmp(string, "IDC_LABEL_DATABIT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingPTZDataBit= m_remalloc(settingPTZDataBit, strlen(string));
			strcpy(settingPTZDataBit, string);
		}
		else if(strcmp(string, "IDC_LABEL_STOPBIT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingPTZStopBit= m_remalloc(settingPTZStopBit, strlen(string));
			strcpy(settingPTZStopBit, string);
		}
		else if(strcmp(string, "IDC_LABEL_VERIFY") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingPTZVerity= m_remalloc(settingPTZVerity, strlen(string));
			strcpy(settingPTZVerity, string);
		}
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_settingNetworkDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "IDC_LABEL_TYPE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkType= m_remalloc(settingNetworkType, strlen(string));
			strcpy(settingNetworkType, string);
		}
		else if(strcmp(string, "TYPE_LABEL_STATIC") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkTypeStatic = m_remalloc(settingNetworkTypeStatic, strlen(string));
			strcpy(settingNetworkTypeStatic, string);
		}
		else if(strcmp(string, "IDC_LABEL_IPADDR") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkIPAdd= m_remalloc(settingNetworkIPAdd, strlen(string));
			strcpy(settingNetworkIPAdd, string);
		}
		else if(strcmp(string, "IDC_LABEL_NETMASK") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkSubnetM= m_remalloc(settingNetworkSubnetM, strlen(string));
			strcpy(settingNetworkSubnetM, string);
		}
		else if(strcmp(string, "IDC_LABEL_GATEWAY") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkGateway= m_remalloc(settingNetworkGateway, strlen(string));
			strcpy(settingNetworkGateway, string);
		}
		else if(strcmp(string, "IDC_LABEL_DNS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkDNS= m_remalloc(settingNetworkDNS, strlen(string));
			strcpy(settingNetworkDNS, string);
		}
		else if(strcmp(string, "IDC_LABEL_BITRATE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkInternetBt= m_remalloc(settingNetworkInternetBt, strlen(string));
			strcpy(settingNetworkInternetBt, string);
		}
		else if(strcmp(string, "IDC_LABEL_MEDIAPORT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkMediaPort= m_remalloc(settingNetworkMediaPort, strlen(string));
			strcpy(settingNetworkMediaPort, string);
		}
		else if(strcmp(string, "IDC_LABEL_WEBPORT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkWebPort= m_remalloc(settingNetworkWebPort, strlen(string));
			strcpy(settingNetworkWebPort, string);
		}
		else if(strcmp(string, "IDC_LABEL_MOBILEPORT") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkMobilePort= m_remalloc(settingNetworkMobilePort, strlen(string));
			strcpy(settingNetworkMobilePort, string);
		}
		else if(strcmp(string, "IDC_LABEL_USERNAME") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkUserName= m_remalloc(settingNetworkUserName, strlen(string));
			strcpy(settingNetworkUserName, string);
		}
		else if(strcmp(string, "IDC_LABEL_PASSWORD") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkPassword= m_remalloc(settingNetworkPassword, strlen(string));
			strcpy(settingNetworkPassword, string);
		}
		else if(strcmp(string, "IDC_LABEL_DNSSSERVICE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkDDNSService= m_remalloc(settingNetworkDDNSService, strlen(string));
			strcpy(settingNetworkDDNSService, string);
		}
		else if(strcmp(string, "IDC_LABEL_DNSSHOSTNAME") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingNetworkDDNSHostName= m_remalloc(settingNetworkDDNSHostName, strlen(string));
			strcpy(settingNetworkDDNSHostName, string);
		}
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_settingManageDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "IDC_LABEL_DVRPWDENABLE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingManageDVRPassword= m_remalloc(settingManageDVRPassword, strlen(string));
			strcpy(settingManageDVRPassword, string);
		}
		else if(strcmp(string, "IDC_LABEL_IEPWDENABLE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingManageIEPassword= m_remalloc(settingManageIEPassword, strlen(string));
			strcpy(settingManageIEPassword, string);
		}
		else if(strcmp(string, "IDC_LABEL_USERPWD") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingManageUserPassword= m_remalloc(settingManageUserPassword, strlen(string));
			strcpy(settingManageUserPassword, string);
		}
		else if(strcmp(string, "IDC_LABEL_ADMINPWD") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingManageAdminPassword= m_remalloc(settingManageAdminPassword, strlen(string));
			strcpy(settingManageAdminPassword, string);
		}
		else if(strcmp(string, "IDC_LABEL_USRREENTER") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingManageReEnter= m_remalloc(settingManageReEnter, strlen(string));
			strcpy(settingManageReEnter, string);
		}
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_settingInfoDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "IDC_STAT_MACADDRESS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingInfoMACAddress= m_remalloc(settingInfoMACAddress, strlen(string));
			strcpy(settingInfoMACAddress, string);
		}
		else if(strcmp(string, "IDC_STAT_SERIALNUM") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingInfoSerialNum= m_remalloc(settingInfoSerialNum, strlen(string));
			strcpy(settingInfoSerialNum, string);
		}
		else if(strcmp(string, "IDC_STAT_DVRSOFTVERSION") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingInfoVersion= m_remalloc(settingInfoVersion, strlen(string));
			strcpy(settingInfoVersion, string);
		}
		else if(strcmp(string, "IDC_STAT_HDDSTATUS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingInfoHDDState= m_remalloc(settingInfoHDDState, strlen(string));
			strcpy(settingInfoHDDState, string);
		}
		else if(strcmp(string, "IDC_STAT_HDD") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Status name: %s\n", string);
			settingInfoHDD= m_remalloc(settingInfoHDD, strlen(string));
			strcpy(settingInfoHDD, string);
		}
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_settingDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "TITLE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Title name: %s\n", string);
			settingMainTitle = m_remalloc(settingMainTitle, strlen(string));
			strcpy(settingMainTitle, string);
		}
		else if(strcmp(string, "IDM_TAB_RECORD") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Record name: %s\n", string);
			settingMainRecord = m_remalloc(settingMainRecord, strlen(string));
			strcpy(settingMainRecord, string);
		}
		else if(strcmp(string, "IDM_TAB_ALARM") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Alarm name: %s\n", string);
			settingMainAlarm = m_remalloc(settingMainAlarm, strlen(string));
			strcpy(settingMainAlarm, string);
		}
		else if(strcmp(string, "IDM_TAB_EMAIL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Email name: %s\n", string);
			settingMainEmail = m_remalloc(settingMainEmail, strlen(string));
			strcpy(settingMainEmail, string);
		}
		else if(strcmp(string, "IDM_TAB_PTZ") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("PTZ name: %s\n", string);
			settingMainPTZ = m_remalloc(settingMainPTZ, strlen(string));
			strcpy(settingMainPTZ, string);
		}
		else if(strcmp(string, "IDM_TAB_NETWORK") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Network name: %s\n", string);
			settingMainNetwork = m_remalloc(settingMainNetwork, strlen(string));
			strcpy(settingMainNetwork, string);
		}
		else if(strcmp(string, "IDM_TAB_MANAGE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Manage name: %s\n", string);
			settingMainManage = m_remalloc(settingMainManage, strlen(string));
			strcpy(settingMainManage, string);
		}
		else if(strcmp(string, "IDM_TAB_INFO") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			settingMainInfo = m_remalloc(settingMainInfo, strlen(string));
			strcpy(settingMainInfo, string);
		}
		else if(strcmp(string, "IDM_TAB_APPLY") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			settingMainApply = m_remalloc(settingMainApply, strlen(string));
			strcpy(settingMainApply, string);
		}
		else if(strcmp(string, "IDC_SUBVIEW_RECORD") == 0)
		{
			kt_language_settingRecordDump(*child);
		}
		else if(strcmp(string, "IDC_SUBVIEW_ALARM") == 0)
		{ 
			kt_language_settingAlarmDump(*child);
		}
		else if(strcmp(string, "IDC_SUBVIEW_EMAIL") == 0)
		{
			kt_language_settingEmailDump(*child);
		}
		else if(strcmp(string, "IDC_SUBVIEW_PTZ") == 0)
		{
			kt_language_settingPTZDump(*child);
		}
		else if(strcmp(string, "IDC_SUBVIEW_NETWORK") == 0)
		{
			kt_language_settingNetworkDump(*child);
		}
		else if(strcmp(string, "IDC_SUBVIEW_MANAGE") == 0)
		{
			kt_language_settingManageDump(*child);
		}
		else if(strcmp(string, "IDC_SUBVIEW_HOSTINFO") == 0)
		{
			kt_language_settingInfoDump(*child);
		}
		
		
		child = child->NextSiblingElement();		
	}
	
	return 0;
		
}

int kt_language_informationDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "IDC_STRING_LOGINSUCCESS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			loginSuccessInfo = m_remalloc(loginSuccessInfo, strlen(string));
			strcpy(loginSuccessInfo, string);
		}
		else if(strcmp(string, "IDC_STRING_LOGINFAIL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			loginFailInfo = m_remalloc(loginFailInfo, strlen(string));
			strcpy(loginFailInfo, string);
		}
		else if(strcmp(string, "IDC_STRING_PARAMSETUPSUCCESS") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			setParamSuccessInfo = m_remalloc(setParamSuccessInfo, strlen(string));
			strcpy(setParamSuccessInfo, string);
		}
		else if(strcmp(string, "IDC_STRING_PARAMSETUPFAIL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			setParamFailInfo = m_remalloc(setParamFailInfo, strlen(string));
			strcpy(setParamFailInfo, string);
		}
		else if(strcmp(string, "IDC_STRING_NORECTHISMONTH") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			noRecordAtMonth = m_remalloc(noRecordAtMonth, strlen(string));
			strcpy(noRecordAtMonth, string);
		}
		child = child->NextSiblingElement();
	}
	
	return 0;
}

int kt_language_golbalDump(const TiXmlElement &element)
{
	TiXmlElement *child = NULL;
	char *string = NULL;
	
	child = (TiXmlElement*)element.FirstChildElement();
	while(child)
	{
		string = (char*)child->Value();
		if(strcmp(string, "IDC_ALLCHANNEL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalAllChannel = m_remalloc(golbalAllChannel, strlen(string));
			strcpy(golbalAllChannel, string);
		}
		else if(strcmp(string, "IDC_STARTTIME") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalStartTime = m_remalloc(golbalStartTime, strlen(string));
			strcpy(golbalStartTime, string);
		}
		else if(strcmp(string, "IDC_CHANNEL") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalChannel = m_remalloc(golbalChannel, strlen(string));
			strcpy(golbalChannel, string);
		}
		else if(strcmp(string, "IDC_SIZE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalSize = m_remalloc(golbalSize, strlen(string));
			strcpy(golbalSize, string);
		}
		else if(strcmp(string, "IDC_RECCOMMON") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalCommonRec = m_remalloc(golbalCommonRec, strlen(string));
			strcpy(golbalCommonRec, string);
		}
		else if(strcmp(string, "IDC_RECALARM") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalAlarmRec = m_remalloc(golbalAlarmRec, strlen(string));
			strcpy(golbalAlarmRec, string);
		}
		else if(strcmp(string, "IDC_RECALLTYPE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalAllTypeRec = m_remalloc(golbalAllTypeRec, strlen(string));
			strcpy(golbalAllTypeRec, string);
		}
		else if(strcmp(string, "IDC_OPEN") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalOpen = m_remalloc(golbalOpen, strlen(string));
			strcpy(golbalOpen, string);
		}
		else if(strcmp(string, "IDC_CLOSE") == 0)
		{
			string = (char*)child->Attribute("_name_");
			printf("Info name: %s\n", string);
			golbalClose = m_remalloc(golbalClose, strlen(string));
			strcpy(golbalClose, string);
		}
		child = child->NextSiblingElement();
	}

	return 0;
}

int kt_language_page(const TiXmlElement &element)
{
	char *string = NULL;

	string = (char*)element.Value();
	printf("view Page: %s \n", string);
	
	if(strcmp(string, "IDC_LOGIN_PAGE") == 0)
	{
		kt_language_loginDump(element);
	}
	else if(strcmp(string, "IDC_MAIN_PAGE") == 0)
	{
		kt_language_previewDump(element);
	}
	else if(strcmp(string, "IDC_PLAYBACK_PAGE") == 0)
	{
		kt_language_playbackDump(element);
	}
    else if(strcmp(string, "IDC_SETUP_PAGE") == 0)
	{
		kt_language_settingDump(element);
	}
	else if(strcmp(string, "IDC_INFORMATION") == 0)
	{
		kt_language_informationDump(element);
	}
	else if(strcmp(string, "IDC_GOLBAL") == 0)
	{
		kt_language_golbalDump(element);
	}
	
	return 0;
}

int kt_language_parser()
{
	if(myDocument == NULL)
	{
		return -1;
	}

	TiXmlElement *RootElement = myDocument->RootElement();
	const char *string = RootElement->Value();
	printf("----->Root Name: %s \n", string);

	char *string2 = NULL;
	TiXmlElement *element = RootElement->FirstChildElement();
	//kt_language_page(*element);
	while(element != NULL)
	{
		string2 = (char*)element->Value();
		//printf("----->Element: %s \n", string2);
		string2 = (char*)element->Attribute("_name_");
		//printf("----->Element Name: %s \n", string2);
		kt_language_page(*element);
		element = element->NextSiblingElement();
	}

	return 0;
}
