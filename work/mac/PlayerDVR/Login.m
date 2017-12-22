//
//  Login.m
//  PlayerDVR
//
//  Created by Kelvin on 2/28/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#include "kt_core_config.h"
#import "Login.h"
#import "DvrManage.h"
#include "kt_core_handler.h"
#include "kt_stream_playback.h"
#include "kt_language_str.h"
#include "kt_language_util.h"
#import "MAC_NetDvrApi.h"
#include "../NetDVRSDK/NetDvrApi.h"

UPHandle g_Dvs = NULL;
DvrManage *m_dvrManage = nil;

DvrManage* getDvrManage()
{
	return m_dvrManage;
}

void dumpDvrConfig(DvrConfig *cfg)
{
	printf("Dvr UserName: %s \n", cfg->userName);
	printf("Dvr Password: %s \n", cfg->password);
	printf("Dvr server address: %s \n", cfg->serverAddr);
	printf("Dvr server port: %d \n", cfg->port);
	printf("Dvr info remember me: %d \n", cfg->rememberFlag);
	printf("Dvr info network type: %d \n", cfg->netType);
	printf("Dvr info channel number: %d \n", cfg->channelNum);	
}

@interface Login (LoginPrivateMethods)

- (void)loadLanguage;

@end

@implementation Login
-(void)awakeFromNib {
	NSLog(@"awake from nib");
    DvrConfig *cfg;
	/*1: get user information*/
	kt_core_fwInit();
	kt_dvrMgr_init();
	kt_dvrMgr_loadDvrList();
    MAC_DVS_Init();
	cfg = kt_dvrMgr_getCurDvr();
	if(cfg == NULL)
	{
		NSLog(@"dvr config file empty");
	}
	else
	{
		printf("server ipaddress: %s \n", cfg->serverAddr);
		printf("server port: %d \n", cfg->port);
		printf("server network type: %d \n", cfg->netType);
		printf("server language: %d \n", cfg->language);
	}
	
	if(cfg == NULL)
	{
#if defined(KT_MTY_CLIENT) && (KT_MTY_CLIENT == 1)
		[labelLanguage  setStringValue:@""];
		[language removeFromSuperview];
#endif /*KT_MTY_CLIENT*/
#if defined(KT_OFFLINE_FEATURE) && (KT_OFFLINE_FEATURE == 1)
		
#else
		[offLineBtn removeFromSuperview];
#endif /*KT_OFFLINE_FEATURE*/
		[language selectItemAtIndex:0];
		[networkType selectItemAtIndex:0];
		return;
	}

	
	/*set user information*/
	[password setStringValue:@""];
	NSString *ipaddr = [[NSString alloc] initWithUTF8String:(char*)(cfg->serverAddr)];
	[IPAddress setStringValue:ipaddr];
	[ipaddr release];

	[netPort setIntValue:cfg->port];
	[rememberMe setState:NSOnState];
	if(cfg->netType > 1)
		cfg->netType = 0;
	[networkType selectItemAtIndex:cfg->netType];
	//[networkType selectItemAtIndex:0];
#if defined(KT_MTY_CLIENT) && (KT_MTY_CLIENT == 1)
	[language removeFromSuperview];
#else
	if(cfg->language > 5)
		cfg->language = 0;
	[language selectItemAtIndex:cfg->language];
#endif /*KT_MTY_CLIENT*/
	
#if defined(KT_OFFLINE_FEATURE) && (KT_OFFLINE_FEATURE == 1)
	
#else
	[offLineBtn removeFromSuperview];
#endif /*KT_OFFLINE_FEATURE*/
	
    
	/*init language*/
	[self loadLanguage];
	[self InitLanguage];
	
}

- (void)InitLanguage {
	NSLog(@"Init language");
	DvrConfig *cfg = NULL;
	NSString *str = nil;
	
	cfg = kt_dvrMgr_getCurDvr();
	
	if(loginTitle)
	{
		str = [[NSString alloc] initWithUTF8String:loginTitle];
		[loginWin setTitle:str];
		[str release];
	}
	if(loginServer)
	{
		str = [[NSString alloc] initWithUTF8String:loginServer];
		[labelIPAddress setStringValue:str];
		[str release];
	}
	if(loginPort)
	{
		str = [[NSString alloc] initWithUTF8String:loginPort];
		[labelMediaPort setStringValue:str];
		[str release];
	}
	if(loginPassword)
	{
		str = [[NSString alloc] initWithUTF8String:loginPassword];
		[labelPassword setStringValue:str];
		[str release];
	}
	if(loginNetwork)
	{
		str = [[NSString alloc] initWithUTF8String:loginNetwork];
		[labelNetwork setStringValue:str];
		[str release];
	}
	if(loginSigned)
	{
		str = [[NSString alloc] initWithUTF8String:loginSigned];
		[signedIn setTitle:str];
		[str release];
	}
#if defined(KT_MTY_CLIENT) && (KT_MTY_CLIENT == 1)
	[labelLanguage  setStringValue:@""];
#else
	if(loginLanguage)
	{
		str = [[NSString alloc] initWithUTF8String:loginLanguage];
		[labelLanguage setStringValue:str];
		[str release];
	}
#endif /*KT_MTY_CLIENT*/
	if(loginLan && loginInternet)
	{
		[networkType removeAllItems];
		str = [[NSString alloc] initWithUTF8String:loginInternet];
		[networkType insertItemWithObjectValue:str atIndex:0];
		[str release];
		str = [[NSString alloc] initWithUTF8String:loginLan];
		[networkType insertItemWithObjectValue:str atIndex:1];
		[str release];
		if(cfg)
		{
			[networkType selectItemAtIndex:cfg->netType];
		}
	}
}

- (IBAction)signedIn:(id)sender {
	NSLog(@"signedIn clicked");
	NSString *nsStrIp,*nsStrPort,*nsStrName,*nsStrPsw;
	int chNum, loginRet;
	char *tmpIPAddr, *tmpUsrName, *tmpPwd;
	int tmpPort;
	
	int state = [rememberMe state];
	if(state == NSOnState)
	{
		NSLog(@"rememberMe on");
	}
	else if(state == NSOffState)
	{
		NSLog(@"rememberMe off");
	}
	
	int idx = [networkType indexOfSelectedItem];
	if(idx == 0)
	{
		NSLog(@"select internet");
	}
	else if(idx == 1)
	{
		NSLog(@"select LAN");
	}
	
	/*do login*/
	nsStrIp = [IPAddress stringValue];
    nsStrPort = [netPort stringValue];
    nsStrName = [userName stringValue];
    nsStrPsw = [password stringValue];
    
	tmpIPAddr = (char*)[nsStrIp UTF8String];
	tmpUsrName = (char*)[nsStrName UTF8String];
	tmpPwd = (char*)[nsStrPsw UTF8String];	
	tmpPort = [nsStrPort intValue];
    
	[notify setStringValue:@"Login ongoing"];
	//loginRet = upui_fw_srvLogin((UPSint8*)tmpIPAddr, tmpPort, (UPUint8*)tmpUsrName, (UPUint8*)tmpPwd);
    
    g_Dvs = MAC_DVS_Login((UPSint8*)tmpIPAddr, tmpPort, (UPUint8*)"admin", (UPUint8*)tmpPwd);
    
/*	if(loginRet == 0x0a)
	{
		[notify setStringValue:@"Login as super user"];
		NSLog(@"login as manager");
	}
	else if(loginRet == 0x09)
	{
		[notify setStringValue:@"Login as normal user"];
		NSLog(@"login as normal user");
	}
	else if(loginRet == 0x04)
	{
		[notify setStringValue:@"Login failed"];
		NSLog(@"login failed");
	}
	
	if(loginRet < 0)
	{
		if(loginFailInfo)
		{
			NSString *str = [[NSString alloc] initWithUTF8String:loginFailInfo];
			[notify setStringValue:str];
			[str release];
		}
		else
		{
			[notify setStringValue:@"Login failed"];
		}
		return ;
	}
*/	
	NSLog(@"result: %x ", g_Dvs);
	
	/*login ok, switch view*/
	//if(loginRet == 0x0a || loginRet == 0x09 || loginRet == 0x0b) /*super password result 0x0b*/
    if(g_Dvs != 0)
	{		
		DvrConfig *cfg = NULL;
		char *tmpStr = NULL;
		cfg = (DvrConfig*)malloc(sizeof(DvrConfig));
		memset((void*)cfg, 0x0, sizeof(DvrConfig));
		cfg->next = NULL;
		
		NSString *tmp;
		/*maybe overflow*/
		tmpStr = NULL;
		cfg->userName[0] = '\0';
		printf("Dvr UserName: %s \n", cfg->userName);
		
		tmp = [password stringValue];
		tmpStr = (char*)[tmp UTF8String];
		memcpy(cfg->password, tmpStr, strlen(tmpStr));
		cfg->password[strlen(tmpStr) + 1] = '\0';
		printf("Dvr Password: %s \n", cfg->password);
		
		tmp = [IPAddress stringValue];
		tmpStr = (char*)[tmp UTF8String];
		memcpy(cfg->serverAddr, tmpStr, strlen(tmpStr));
		cfg->serverAddr[strlen(tmpStr) + 1] = '\0';
		printf("Dvr server address: %s \n", cfg->serverAddr);
		
		cfg->port = [netPort intValue];
		printf("Dvr server port: %d \n", cfg->port);
		
		cfg->rememberFlag = [rememberMe state];
		printf("Dvr info remember me: %d \n", cfg->rememberFlag);
		
		cfg->netType = [networkType indexOfSelectedItem];
		printf("Dvr info network type: %d \n", cfg->netType);
		if(cfg->netType < 0)
		{
			cfg->netType = 0;
		}
		
#if defined(KT_MTY_CLIENT) && (KT_MTY_CLIENT == 1)
		
#else
		cfg->language = [language indexOfSelectedItem];
		printf("Dvr info language: %d \n", cfg->language);
#endif /*KT_MTY_CLIENT*/
		
		//chNum = upui_fw_srvGetChnNumber((UPSint8 *)cfg->serverAddr, cfg->port);
		chNum = DVS_GetChnNumber(g_Dvs);
		
		if(chNum <= 0)
		{
			NSLog(@"get channel number error");
			if(loginFailInfo)
			{
				NSString *str = [[NSString alloc] initWithUTF8String:loginFailInfo];
				[notify setStringValue:str];
				[str release];
			}
			else
			{
				[notify setStringValue:@"Login failed"];
			}
			free(cfg);
			return;
		}
		cfg->channelNum = chNum;
		printf("Dvr info channel number: %d \n", cfg->channelNum);
		
	//	if(loginRet == 0x0a || loginRet == 0x0b)
	//	{
	//		NSLog(@"++++++++++++++supper user");
			cfg->userType = 0;	/*admin*/
	//	}
	//	else if(loginRet == 0x09)
	//	{
	//		NSLog(@"++++++++++++++normal user");
	//		cfg->userType = 1;	/*normal*/
	//	}
		
		/*add dvr to dvr manage module*/
		
		kt_dvrMgr_updateCurDvr(cfg);
		kt_dvrMgr_commit();
		
		/*create home dir*/
		kt_core_createDvrHome((char*)(cfg->serverAddr));
		free(cfg);
		
		/*switch window*/
		if(!add_window)
		{
			add_window = [[NSWindowController alloc] initWithWindowNibName:@"VideoPreview"];
		}
		NSWindow *wnd = [add_window window];
	
		if(![wnd isVisible])
		{
			NSLog(@"show second window");
			[wnd makeKeyAndOrderFront:sender];
			[loginWin close];
		}
		else
		{
			NSLog(@"show first window");
			[add_window showWindow:sender];
		}
	}

}

- (IBAction)offLineClicked:(id)sender {
	NSLog(@"offLineClicked");
    NSString *nsStrIp,*nsStrPort,*nsStrName,*nsStrPsw;
    int chNum, loginRet;
    char *tmpIPAddr, *tmpUsrName, *tmpPwd;
    int tmpPort;
    
    int state = [rememberMe state];
    if(state == NSOnState)
    {
        NSLog(@"rememberMe on");
    }
    else if(state == NSOffState)
    {
        NSLog(@"rememberMe off");
    }
    
    int idx = [networkType indexOfSelectedItem];
    if(idx == 0)
    {
        NSLog(@"select internet");
    }
    else if(idx == 1)
    {
        NSLog(@"select LAN");
    }
    
    /*do login*/
    nsStrIp = [IPAddress stringValue];
    nsStrPort = [netPort stringValue];
    nsStrName = [userName stringValue];
    nsStrPsw = [password stringValue];
    
    tmpIPAddr = (char*)[nsStrIp UTF8String];
    tmpUsrName = (char*)[nsStrName UTF8String];
    tmpPwd = (char*)[nsStrPsw UTF8String];	
    tmpPort = [nsStrPort intValue];
    
    [notify setStringValue:@"Login ongoing"];
    //loginRet = upui_fw_srvLogin((UPSint8*)tmpIPAddr, tmpPort, (UPUint8*)tmpUsrName, (UPUint8*)tmpPwd);
    
    g_Dvs = MAC_DVS_Login((UPSint8*)tmpIPAddr, tmpPort, (UPUint8*)"admin", (UPUint8*)tmpPwd);
    if(!g_Dvs)
        return;
        
	add_window = [[NSWindowController alloc] initWithWindowNibName:@"OffLinePlay"];
	NSWindow *wnd = [add_window window];
	if(![wnd isVisible])
	{		
        NSLog(@"show offline play window");  
        [wnd makeKeyAndOrderFront:sender];
		[loginWin close];
	}
}


- (IBAction)changeLanguage:(id)sender {
	NSLog(@"===========>changeLanguage");
#if defined(KT_MTY_CLIENT) && (KT_MTY_CLIENT == 1)
	
#else
	[self loadLanguage];
	[self InitLanguage];
#endif /*KT_MTY_CLIENT*/
	
}

- (void)dealloc {
	NSLog(@"===========>Login dealloc");
	[super dealloc];
}

- (void)loadLanguage {
	NSString* fullPath = nil;
	unsigned int lang = 0;
	
#if defined(KT_MTY_CLIENT) && (KT_MTY_CLIENT == 1)
	fullPath = [[NSBundle mainBundle] pathForResource:@"English" ofType:@"xml"];
#else
	lang = [language indexOfSelectedItem];
		
	if(lang == 1)
	{
		fullPath = [[NSBundle mainBundle] pathForResource:@"Chinese Simplified" ofType:@"xml"];
	}
	else if(lang == 0)
	{
		fullPath = [[NSBundle mainBundle] pathForResource:@"English" ofType:@"xml"];
	}
	else {
		fullPath = [[NSBundle mainBundle] pathForResource:@"English" ofType:@"xml"];
	}
#endif /*KT_MTY_CLIENT*/

	//NSLog(@"%@", fullPath);
	char *path = (char*)[fullPath UTF8String];
	kt_language_load(path);
	kt_language_parser();
}
@end
