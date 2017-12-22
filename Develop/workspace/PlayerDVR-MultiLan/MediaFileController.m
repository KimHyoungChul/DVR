//
//  MediaFileController.m
//  DVRPlayer
//
//  Created by Kelvin on 3/31/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "MediaFileController.h"
#import "MediaFileInfo.h"
#import "CustomButton.h"

#include "kt_file_manage.h"
#include "kt_stream_playback.h"
#include "kt_dvr_manage.h"
#include "kt_util.h"
#include "kt_core_handler.h"
#include "kt_language_str.h"

#ifdef KT_GET_FILELIST_ASYNC
BOOL fileListTh;
BOOL m_conWait;
NSCondition *_myCondition = nil;
#endif /*KT_GET_FILELIST_ASYNC*/

@implementation MediaFileController

- (void)awakeFromNib {
	NSLog(@"=============>>>>MediaFileController awakeFromNib");
	
	DvrConfig *dvrCfg = NULL;
	char *homePath = getenv("HOME");
	char savePath[128] = {0};
	
	dvrCfg = kt_dvrMgr_getCurDvr();
	
	sprintf(savePath, "%s/DvrRecord/%s/tmp", homePath, dvrCfg->serverAddr);
	
	NSLog(@"server address: %s, port: %d", dvrCfg->serverAddr, dvrCfg->port);
	onlineCtl = playback_online_init((UPSint8*)dvrCfg->serverAddr, dvrCfg->port);
	playback_online_setSavePath(onlineCtl, (UPSint8*)savePath);

	animationTimer = nil;
}

- (void)stopTimer {
	NSLog(@"---------------------MediaFileControl stopTimer");
	
	[animationTimer invalidate];
}

- (void)ontimer {
	int precent;
	
	precent = playback_online_getPrecent(onlineCtl);
	if(precent == 1000)
	{
		/*notify*/
		[[NSNotificationCenter defaultCenter]
		 postNotificationName:@"Update Backup Status" 
		 object:nil];
	}
	
	[playbackCtl setDownloadPrecent:precent];
	
}

- (void)getCalendaAtYear:(int)year Month:(int)month andDay:(int)day {
	playback_online_getCalenda(onlineCtl, year, month, day, 0);
}

- (void)getCalendaDescrip:(char*)descrip {
	playback_online_getDescription(onlineCtl, (UPSint8*)descrip);
}

- (void)openFileInFS {
	[playbackCtl openFileinFS];
}

- (int)initSearchMgrAtYear:(int)year Month:(int)month andDay:(int)day {
	NSLog(@"MediaFileController initSearchMgrAtYear");
	DvrConfig *dvrCfg = NULL;
	
	dvrCfg = kt_dvrMgr_getCurDvr();
	if(dvrCfg)
	{
		kt_core_createDvrHome((char*)dvrCfg->serverAddr);
	}
	
	return playback_online_getSearchMgr(onlineCtl, year, month, day);
}

#ifdef KT_GET_FILELIST_ASYNC
- (int)searchMgrGetNumberAtYear:(int)year Month:(int)month andDay:(int)day {
	NSLog(@"MediaFileController searchMgrGetNumberAtYear");
	
	return playback_online_getOnlineFileNumber(onlineCtl, year, month, day);
}

- (int)searchMgrGetFileByPageAtYear:(int)year Month:(int)month andDay:(int)day byPage:(int)page {
	NSLog(@"MediaFileController searchMgrGetFileByPageAtYear");
	
	return playback_online_getOnlineFilePage(onlineCtl, year, month, day, page);
}
#endif /*KT_GET_FILELIST_ASYNC*/

- (RecFile*)convertToRecFile:(int)channel andType:(int)type {
	NSLog(@"MedaiFileController convertToRecFile");
	return kt_fileMgr_getRecFileOnline(onlineCtl, channel, type);
}

- (RecFile*)getOnlineHistory:(char*)path {
	NSLog(@"MedaiFileController getOnlineHistory");
	return kt_fileMgr_getHistoryFileOnline(onlineCtl, path);
}

- (RecFile*)getHistory:(char*)path {
	int count = 0;
	NSLog(@"MediaFileController getHistory");
	DvrConfig *dvrCfg = NULL;
	
	dvrCfg = kt_dvrMgr_getCurDvr();
	if(dvrCfg)
	{
		kt_core_createDvrHome((char*)dvrCfg->serverAddr);
	}
	
	return kt_fileMgr_getRecFileInFS(path, ".264", 0xff, 1, &count);
}

- (RecFile*)getLocalRec:(char*)path {
	int count = 0;
	NSLog(@"MediaFileController getHistory");
	DvrConfig *dvrCfg = NULL;
	
	dvrCfg = kt_dvrMgr_getCurDvr();
	if(dvrCfg)
	{
		kt_core_createDvrHome((char*)dvrCfg->serverAddr);
	}
	
	return kt_fileMgr_getRecFileInFS(path, ".264", 0xff, 0, &count);
}

- (void)stopDownload {
	NSLog(@"MediaFileController stopDownload");
	playback_online_stopDownload(onlineCtl);
}

- (void)startDownload:(MediaFileInfo*)media {
	NSLog(@"MediaFileController startDownload");
	DvrConfig *dvrCfg = NULL;
	char cachePath[128] = {0};
	char startTime[64] = {0};
	char *homePath = getenv("HOME");
	char *path = (char*)[media.path UTF8String];
	char name[64] = {0};
	unsigned int size;
	
	dvrCfg = kt_dvrMgr_getCurDvr();
	sprintf(cachePath, "%s/DvrRecord/%s/tmp/", homePath, dvrCfg->serverAddr);
	
	playback_online_getCacheFileList(onlineCtl, (UPSint8*)cachePath);
	
	kt_fileMgr_getFileName(path, name);
	kt_fileMgr_getFileStartTime(name, startTime);
	
	/*check cache file list first*/
	if(playback_online_isFileInCache(onlineCtl, (UPSint8*)name, 1) == 0) /*file in cache*/
	{
		NSLog(@"--------------->download not finish, continue");
		size = playback_online_getFileSizeInCache(onlineCtl, (UPSint8*)name);
		playback_online_startDownload(onlineCtl, (UPSint8*)path, size);
		playback_online_setSaveName(onlineCtl, (UPSint8*)name);
		size = playback_online_getDownloadSize(onlineCtl);
		NSLog(@"download file size: %d", size);
		if(animationTimer == nil)
		{
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/10.0) target:self selector:@selector(ontimer) userInfo:nil repeats:YES];
		}
		[playbackCtl setDownloadFileSize:size];
		[playbackCtl setDownloadProcess:YES];
		
	}
	else
	{
		NSLog(@"---------------->file not in cache, start download");
		playback_online_startDownload(onlineCtl, (UPSint8*)path, 0);
		playback_online_setSaveName(onlineCtl, (UPSint8*)name);
		size = playback_online_getDownloadSize(onlineCtl);
		NSLog(@"download file size: %d", size);
		if(animationTimer == nil)
		{
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/10.0) target:self selector:@selector(ontimer) userInfo:nil repeats:YES];
		}
		[playbackCtl setDownloadFileSize:size];
		[playbackCtl setDownloadProcess:YES];
		
	}
}

- (void)startPlayback:(MediaFileInfo*)media andType:(int)type {
	NSLog(@"MediaFileController startPlayback");
	char *homePath = getenv("HOME");
	char savePath[128] = {0};
	char startTime[64] = {0};
	DvrConfig *dvrCfg = NULL;
	
	NSLog(@"--------------------->>>>>>>channel: %@", media.channel);

	if(type == 0)
	{
		[playbackCtl setDownloadProcess:NO];
		[playbackCtl startMediaFile:media withType:0];
	}
	else if(type == 1)
	{
		NSLog(@"playback online");
		char *path = (char*)[media.path UTF8String];
		unsigned int size;
		char name[64] = {0};
		char cachePath[128] = {0};
		printf("path: %s \n", path);
		NSLog(@"--------------------------->>>>>>>>>>>>>>>>>>>>>%d", [media.name intValue]);
		
		dvrCfg = kt_dvrMgr_getCurDvr();
		sprintf(cachePath, "%s/DvrRecord/%s/tmp/", homePath, dvrCfg->serverAddr);
		playback_online_getCacheFileList(onlineCtl, (UPSint8*)cachePath);
		
#if 0
		if(playback_online_isFileInCache(onlineCtl, (UPSint8*)path, 0) == 0) /*for playback in history list*/
		{
			if(animationTimer == nil)
			{
				animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/60.0) target:self selector:@selector(ontimer) userInfo:nil repeats:YES];
			}
			
			[playbackCtl setDownloadProcess:NO];
			[playbackCtl startMediaFile:media withType:0];
		}
		else /*for playback online*/
#endif /*0*/
		{
			kt_fileMgr_getFileName(path, name);
			kt_fileMgr_getFileStartTime(name, startTime);
			
			/*check cache file list first*/
			if(playback_online_isFileInCache(onlineCtl, (UPSint8*)name, 1) == 0) /*file in cache*/
			{
				NSLog(@"--------------->download not finish, continue");
				size = playback_online_getFileSizeInCache(onlineCtl, (UPSint8*)name);
				playback_online_startDownload(onlineCtl, (UPSint8*)path, size);
				playback_online_setSaveName(onlineCtl, (UPSint8*)name);
				size = playback_online_getDownloadSize(onlineCtl);
				NSLog(@"download file size: %d", size);
				if(animationTimer == nil)
				{
					animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/60.0) target:self selector:@selector(ontimer) userInfo:nil repeats:YES];
				}
				[playbackCtl setDownloadFileSize:size];
				[playbackCtl setDownloadProcess:YES];
				
				dvrCfg = kt_dvrMgr_getCurDvr();
				sprintf(savePath, "%s/DvrRecord/%s/tmp/%s.264", homePath, dvrCfg->serverAddr, name);
				[playbackCtl startMediaFileByPath:savePath withType:1 andChannel:([media.channel intValue]-1) andStartTime:media.name];
			}
			else
			{
				NSLog(@"---------------->file not in cache, start download");
				playback_online_startDownload(onlineCtl, (UPSint8*)path, 0);
				playback_online_setSaveName(onlineCtl, (UPSint8*)name);
				size = playback_online_getDownloadSize(onlineCtl);
				NSLog(@"download file size: %d", size);
				if(animationTimer == nil)
				{
					animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/60.0) target:self selector:@selector(ontimer) userInfo:nil repeats:YES];
				}
				[playbackCtl setDownloadFileSize:size];
				[playbackCtl setDownloadProcess:YES];
				
				dvrCfg = kt_dvrMgr_getCurDvr();
				sprintf(savePath, "%s/DvrRecord/%s/tmp/%s.264", homePath, dvrCfg->serverAddr, name);
				[playbackCtl startMediaFileByPath:savePath withType:1 andChannel:([media.channel intValue]-1) andStartTime:media.name];
			}
		}
	}
}

- (void)dealloc {
	NSLog(@"------------------MediaFileController dealloc");
#ifdef KT_GET_FILELIST_ASYNC
	[_myCondition lock];
	if(fileListTh == YES)
	{
		/*stop thread first*/
	//	fileListTh = NO;
		m_conWait = YES;
		[_myCondition wait];
	}
	[_myCondition unlock];
#endif /*KT_GET_FILELIST_ASYNC*/
	playback_online_fini(onlineCtl);
#ifdef KT_GET_FILELIST_ASYNC
	[_myCondition release];
	_myCondition = nil;
#endif /*KT_GET_FILELIST_ASYNC*/
	//[self stopTimer];
	[super dealloc];
}

@end

@implementation MediaFileOnline

- (void)doubleClicked:(id)sender {
	NSLog(@"MediaFileOnline double Clicked");
	NSInteger row;
	NSString *str = nil;
	
	row = [tableView clickedRow];
	NSLog(@"row: %d", row);
	if(row < 0)
		return;

	MediaFileInfo *media = [list objectAtIndex:row];
	NSLog(@"StartTime: %@", media.name);
	NSLog(@"file path: %@", media.path);
	
	[mainCtl startPlayback:media andType:1];
	
	downloadStatus = YES;
	if(downloadStatus == NO)
	{
		if(playbackBackup)
		{
			str = [[NSString alloc] initWithUTF8String:playbackBackup];
			[deleteBtn setTitle:str];
			[str release];
		}
	}
	else
	{
		if(playbackStop)
		{
			str = [[NSString alloc] initWithUTF8String:playbackStop];
			[deleteBtn setTitle:str];
			[str release];
		}
	}
}

- (void)updateBackupStatus:(NSNotification*)notification {
	NSString *str = nil;
	if(downloadStatus == YES)
	{
		NSLog(@"MediaFileOnline updateBackupStatus");
		if(playbackBackup)
		{
			str = [[NSString alloc] initWithUTF8String:playbackBackup];
			[deleteBtn setTitle:str];
			[str release];
		}
		downloadStatus = NO;
	}
}

- (void)awakeFromNib {
	NSLog(@"===========>MediaFileOnline awakeFromNib");
	list = [[NSMutableArray alloc] init];
	
	[tableView setTarget:self];
	[tableView setDoubleAction:@selector(doubleClicked:)];
	
	[datePicker setDateValue:[NSDate date]];
	//NSFont *font = [NSFont userFontOfSize:32.0];
	//NSFont *font = [NSFont fontWithName:@"Helvetica" size:32.0];
	//[datePickerCell setFont:font];
	downloadStatus = NO;
	datePickerDown = NO;
	
#ifdef KT_GET_FILELIST_ASYNC
	if(_myCondition == nil)
	{
		_myCondition = [[NSCondition alloc] init];
	}
	
	[_myCondition lock];
	fileListTh = NO;
	m_conWait = NO;
	[_myCondition unlock];
#endif /*KT_GET_FILELIST_ASYNC*/
	
	[self refreshRecDescrip];
	[self InitLanguage];
	
	[Channel selectItemAtIndex:16];
	[RecType selectItemAtIndex:2];
	
	[[NSNotificationCenter defaultCenter] addObserver:self 
				selector:@selector(updateBackupStatus:) name:@"Update Backup Status" object:nil];
#ifdef KT_GET_FILELIST_ASYNC
	[[NSNotificationCenter defaultCenter] addObserver:self 
					selector:@selector(updateListSource:) name:@"Update ListSource" object:nil];
#endif /*KT_GET_FILELIST_ASYNC*/
}

- (void)InitLanguage {
	NSString *str = nil;
	
	if(playbackRefresh)
	{
		str = [[NSString alloc] initWithUTF8String:playbackRefresh];
		[refreshBtn setTitle:str];
		[str release];
	}
	if(downloadStatus == NO)
	{
		if(playbackBackup)
		{
			str = [[NSString alloc] initWithUTF8String:playbackBackup];
			[deleteBtn setTitle:str];
			[str release];
		}
	}
	else
	{
		if(playbackStop)
		{
			str = [[NSString alloc] initWithUTF8String:playbackStop];
			[deleteBtn setTitle:str];
			[str release];
		}
	}

	if(playbackOpenFS)
	{
		str = [[NSString alloc] initWithUTF8String:playbackOpenFS];
		[openBtn setTitle:str];
		[str release];
	}
	if(playbackSearch)
	{
		str = [[NSString alloc] initWithUTF8String:playbackSearch];
		[searchBtn setTitle:str];
		[str release];
	}
	if(golbalAllChannel)
	{
		str = [[NSString alloc] initWithUTF8String:golbalAllChannel];
		[Channel removeItemAtIndex:16];
		[Channel addItemWithObjectValue:str];
		[str release];
	}
	if(playbackRecDay)
	{
		str = [[NSString alloc] initWithUTF8String:playbackRecDay];
		[labelRecDay setStringValue:str];
		[str release];
	}
	
	NSArray *array = [tableView tableColumns];
	NSTableColumn *item = nil;
	NSCell *cell = nil;
	if(golbalStartTime)
	{
		str = [[NSString alloc] initWithUTF8String:golbalStartTime];
		item = [array objectAtIndex:0];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
	if(golbalChannel)
	{
		str = [[NSString alloc] initWithUTF8String:golbalChannel];
		item = [array objectAtIndex:1];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
	if(golbalSize)
	{
		str = [[NSString alloc] initWithUTF8String:golbalSize];
		item = [array objectAtIndex:2];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
	
	if(golbalCommonRec && golbalAlarmRec && golbalAllTypeRec)
	{
		[RecType removeAllItems];
		str = [[NSString alloc] initWithUTF8String:golbalCommonRec];
		[RecType insertItemWithObjectValue:str atIndex:0];
		[str release];
		str = [[NSString alloc] initWithUTF8String:golbalAlarmRec];
		[RecType insertItemWithObjectValue:str atIndex:1];
		[str release];
		str = [[NSString alloc] initWithUTF8String:golbalAllTypeRec];
		[RecType insertItemWithObjectValue:str atIndex:2];
		[str release];
	}
}

#ifdef KT_GET_FILELIST_ASYNC
- (void)getRemoteFileList:(id)obj {
	NSLog(@"------------------->>>>>>>>>>>>>>>>>11MediaFileOnline getRemoteFileList");
	int count = 0;
	int year = 0, month = 0, day = 0;
	int i = 0;
//	int flag = 0;
	int counter = 0;
	
	/*add auto release pool*/
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	[self getSelectYear:&year Month:&month andDay:&day];
	year -= 2000;
	count = [mainCtl searchMgrGetNumberAtYear:year Month:month andDay:day];
	NSLog(@"file number: %d", count);
	if(count == -1)
	{
		goto LOOP_EXIT;
	}
	counter = count/200;
	if(count%200)
		counter += 1;
	count = counter;
	NSLog(@"count: %d", count);
	
	//	[_myCondition lock];
	//	fileListTh = YES;
	//	[_myCondition unlock];
	fileListTh = YES;
	
	while(fileListTh == YES)
	{
		if(count > 0)
		{
			NSLog(@"MediaFileOnline getRemoteFileList count: %d", count);
			count--;
			[mainCtl searchMgrGetFileByPageAtYear:year Month:month andDay:day byPage:i];
			i++;
//			[_myCondition lock];
			//if(fileListTh == NO)
//			{
//				if(m_conWait == YES)
//				{
//					[_myCondition signal];
//					m_conWait = NO;
//				}
//				[_myCondition unlock];
//				flag = 1;
//				break;
//			}
//			[_myCondition unlock];
			if(m_conWait == YES)
			{
				[_myCondition lock];
				[_myCondition signal];
				m_conWait = NO;
				[_myCondition unlock];
				break;
			}
			[[NSNotificationCenter defaultCenter]
			 postNotificationName:@"Update ListSource" 
			 object:nil];
			//sleep(1);
		}
		else
		{
			break;
		}
	}
	NSLog(@"------------------->>>>>>>>>>>>>>>>>22MediaFileOnline getRemoteFileList");
	
LOOP_EXIT:
	[_myCondition lock];
	fileListTh = NO;
	if(m_conWait == YES)
	{
		[_myCondition signal];
		m_conWait = NO;
	}
	[_myCondition unlock];
	
	[pool release];
	
	return;
}
#endif /*KT_GET_FILELIST_ASYNC*/

- (IBAction)datePickerAction:(id)sender {
	NSLog(@"datePickerAction");
	NSDate *theDate = [datePicker dateValue];
	NSLog(@"description: %@", [theDate description]);
	int year, month, day;
	
#ifndef KT_GET_FILELIST_ASYNC
	datePickerDown = YES;
#endif /*KT_GET_FILELIST_ASYNC*/
	
	if(datePickerDown == NO)
	{
		datePickerDown = YES;
	}
	else
	{
#ifdef KT_GET_FILELIST_ASYNC
	[_myCondition lock];
	if(fileListTh == NO)// && m_conWait == NO)
	{
		NSLog(@"11++++++++++++++++++++++++++++++++++");
		NSLog(@"11++++++++++++++++++++++++++++++++++");
		NSLog(@"11++++++++++++++++++++++++++++++++++");
		fileListTh = YES;
		[NSThread detachNewThreadSelector:@selector(getRemoteFileList:) toTarget:self withObject:nil];
	}
	else if(fileListTh == YES)
	{
		NSLog(@"22++++++++++++++++++++++++++++++++++");
		NSLog(@"22++++++++++++++++++++++++++++++++++");
		NSLog(@"22++++++++++++++++++++++++++++++++++");
//		fileListTh = NO;
		m_conWait = YES;
		[_myCondition wait];
//		fileListTh = YES;
//		m_conWait = NO;
		[NSThread detachNewThreadSelector:@selector(getRemoteFileList:) toTarget:self withObject:nil];
		fileListTh = YES;
	}
	else {
		NSLog(@"-----------------------------------");
		NSLog(@"-----------------------------------");
		NSLog(@"-----------------------------------");
	}

	[_myCondition unlock];
#else
	{
		[self getSelectYear:&year Month:&month andDay:&day];
		year -= 2000;
		[self getOnlineFileListAtYear:year Month:month andDay:day];
	}
#endif /*KT_GET_FILELIST_ASYNC*/
		datePickerDown = NO;
	}
}

- (IBAction)monthPickerAction:(id)sender {
	NSLog(@"Month Picker Action");
}

- (IBAction)doSearch:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		NSLog(@"do search");

		NSLog(@"update file list");
		int year, month, day;
		[self getSelectYear:&year Month:&month andDay:&day];
		year -= 2000;
		[self getOnlineFileListAtYear:year Month:month andDay:day];
	}
}

- (void)refreshRecDescrip {
	NSLog(@"refresh record description");
	int year, month, day;
	char description[256] = {0};

	[self getSelectYear:&year Month:&month andDay:&day];
	year = year - 2000;
	[mainCtl getCalendaAtYear:year Month:month andDay:day];
	[mainCtl getCalendaDescrip:description];
	
	NSString *descrip = nil;
	if(strlen(description) > 0)
	{
		descrip = [[NSString alloc] initWithUTF8String:description];
	}
	else if(strlen(description) == 0)
	{
		if(noRecordAtMonth)
		{
			descrip = [[NSString alloc] initWithUTF8String:noRecordAtMonth];
		}
	}
	[descripStr setStringValue:descrip];
	[descrip release];
}

- (IBAction)dateRefresh:(id)sender {
	int state;
	int year, month, day;
	char description[256] = {0};
	NSDate *theDate = [datePicker dateValue];	
	
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		/**/
		NSLog(@"do refresh");
		NSLog(@"description: %@", [theDate description]);
		[self getSelectYear:&year Month:&month andDay:&day];
		year = year - 2000;
		[mainCtl getCalendaAtYear:year Month:month andDay:day];
		[mainCtl getCalendaDescrip:description];
		
		NSString *descrip = nil;
		if(strlen(description) > 0)
		{
			descrip = [[NSString alloc] initWithUTF8String:description];
		}
		else if(strlen(description) == 0)
		{
			if(noRecordAtMonth)
			{
				descrip = [[NSString alloc] initWithUTF8String:noRecordAtMonth];
			}
		}
		[descripStr setStringValue:descrip];
		[descrip release];
	}
}

- (IBAction)doDelete:(id)sender {
	int state;
	NSInteger row;
	NSString *str = nil;
//	char *filePath = NULL;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		row = [tableView selectedRow];
		NSLog(@"row: %d", row);
		if(row < 0)
		{
			NSLog(@"no file selected");
			return;
		}
		MediaFileInfo *media = [list objectAtIndex:row];
		if(downloadStatus == NO)
		{
			[mainCtl startDownload:media];
			downloadStatus = YES;
		}
		else
		{
			downloadStatus = NO;
			[mainCtl stopDownload];
		}
		
		if(downloadStatus == NO)
		{
			if(playbackBackup)
			{
				str = [[NSString alloc] initWithUTF8String:playbackBackup];
				[deleteBtn setTitle:str];
				[str release];
			}
		}
		else
		{
			if(playbackStop)
			{
				str = [[NSString alloc] initWithUTF8String:playbackStop];
				[deleteBtn setTitle:str];
				[str release];
			}
		}

		/**/
//		{
//			NSLog(@"do delete");
//			row = [tableView selectedRow];
//			NSLog(@"row: %d", row);
//			MediaFileInfo *media = [list objectAtIndex:row];
//			NSLog(@"StartTime: %@", media.name);
//			NSLog(@"file path: %@", media.path);
//			filePath = (char*)[media.path UTF8String];
//			kt_util_remove((const char*)filePath);
//			[list removeObjectAtIndex:row];
//			[tableView reloadData];
//		}
	}
}

- (IBAction)doOpenFS:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		/**/
		NSLog(@"do open fs");
		[mainCtl openFileInFS];
	}
}

- (void)getSelectYear:(int*)year Month:(int*)month andDay:(int*)day {
	int i = 0, j = 0;
	int sepCount = 0;
	char tmp[16] = {0};
	NSDate *theDate = [datePicker dateValue];
	NSString *date = [theDate description];
	char *dateStr = (char*)[date UTF8String];
	printf("selected day: %s \n", dateStr);
	
	for(i = 0; i < strlen(dateStr); i++)
	{
		if(dateStr[i] != '-' && dateStr[i] != ' ')
		{
			tmp[j] = dateStr[i];
			j++;
		}
		else
		{
			if(dateStr[i] != ' ')
			{
				j = 0;
				if(sepCount == 0)
				{
					*year = atoi((const char*)tmp);
					memset(tmp, 0x0, sizeof(tmp));
				}
				else if(sepCount == 1)
				{
					*month = atoi((const char*)tmp);
					memset(tmp, 0x0, sizeof(tmp));
				}
				else if(sepCount == 2)
				{
					*day = atoi((const char*)tmp);
					memset(tmp, 0x0, sizeof(tmp));
				}
				sepCount++;
			}
			else
			{
				j = 0;
				if(sepCount == 0)
				{
					*year = atoi((const char*)tmp);
					memset(tmp, 0x0, sizeof(tmp));
				}
				else if(sepCount == 1)
				{
					*month = atoi((const char*)tmp);
					memset(tmp, 0x0, sizeof(tmp));
				}
				else if(sepCount == 2)
				{
					*day = atoi((const char*)tmp);
					memset(tmp, 0x0, sizeof(tmp));
				}
				sepCount++;				
			}
			
		}
	}
	printf("year: %d, month: %d, day: %d \n", *year, *month, *day);
}

- (void)clearFileList {
	[list removeAllObjects];
	[tableView reloadData];
}

#ifdef KT_GET_FILELIST_ASYNC
- (void)updateListSource:(NSNotification*)notification {
	NSLog(@"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxMediaFileOnline updateListSource");
	int channel, type;
	RecFile *fileList = NULL;
	RecFile *recFile = NULL;
	NSString *tmp = nil;
	
	channel = [Channel indexOfSelectedItem];
	type = [RecType indexOfSelectedItem];
	if(type == 2) /*all type*/
	{
		type = 3;
	}
	printf("channel: %d, type: %d \n", channel, type);
	
	if(channel == 16)
	{
		channel = 0xffff;
	}
	else
	{
		channel = (int)(0x01 << channel);
	}
	printf("channel: %x, type: %d \n", channel, type);
	
	fileList = [mainCtl convertToRecFile:channel andType:type];
	kt_fileMgr_dump(fileList);
	
	//NSLog(@"111");
	[list removeAllObjects];
	//NSLog(@"222");
	recFile = fileList;
	while(recFile)
	{
		/*add to list*/
		MediaFileInfo *media = [[MediaFileInfo alloc] init];
		tmp = [[NSString alloc] initWithUTF8String:recFile->startTime];
		media.name = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithUTF8String:recFile->size];
		media.size = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithFormat:@"%02d", recFile->channel];
		media.channel = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithUTF8String:recFile->path];
		media.path = tmp;
		[tmp release];
		[list addObject:media];
		[media release];
		
		recFile = recFile->next;
	}
	
	kt_fileMgr_listFini(fileList);
	
	//NSLog(@"333");
	[tableView reloadData];
	//NSLog(@"444");
}
#endif /*KT_GET_FILELIST_ASYNC*/

- (void)getOnlineFileListAtYear:(int)year Month:(int)month andDay:(int)day {
	int ret;
	int channel, type;
	RecFile *fileList = NULL;
	RecFile *recFile = NULL;
	NSString *tmp = nil;
	
	ret = [mainCtl initSearchMgrAtYear:year Month:month andDay:day];
	if(ret < 0)
	{
		NSLog(@"get search mgr error");
		[self clearFileList];
		return;
	}
	
	NSLog(@"convert to file ");
	channel = [Channel indexOfSelectedItem];
	type = [RecType indexOfSelectedItem];
	if(type == 2) /*all type*/
	{
		type = 3;
	}
	printf("channel: %d, type: %d \n", channel, type);
	
	if(channel == 16)
	{
		channel = 0xff;
	}
	else
	{
		channel = (int)(0x01 << channel);
	}
	printf("channel: %x, type: %d \n", channel, type);
	
	fileList = [mainCtl convertToRecFile:channel andType:type];
	kt_fileMgr_dump(fileList);
	
	[list removeAllObjects];
	recFile = fileList;
	while(recFile)
	{
		/*add to list*/
		MediaFileInfo *media = [[MediaFileInfo alloc] init];
		tmp = [[NSString alloc] initWithUTF8String:recFile->startTime];
		media.name = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithUTF8String:recFile->size];
		media.size = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithFormat:@"%02d", recFile->channel];
		media.channel = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithUTF8String:recFile->path];
		media.path = tmp;
		[tmp release];
		[list addObject:media];
		[media release];
		
		recFile = recFile->next;
	}
	
	kt_fileMgr_listFini(fileList);
	
	[tableView reloadData];
}

- (void)dealloc {
	NSLog(@"---------->>MediaFileOnline dealloc");
	[list release];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[super dealloc];
}

#pragma mark -
#pragma mark TableView DataSource

- (NSInteger)numberOfRowsInTableView:(NSTableView*)aTableView {
	return [list count];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*)aTableColumn row:(NSInteger)rowIndex {
	NSString *tmp;
	MediaFileInfo *media = [list objectAtIndex:rowIndex];
	tmp = [aTableColumn identifier];
	if([tmp isEqual:@"StartTime"])
	{
		tmp = media.name;
	}
	else if([tmp isEqual:@"Channel"])
	{
		tmp = media.channel;
	}
	else if([tmp isEqual:@"Size"])
	{
		tmp = media.size;
	}
	
	return tmp;
}

@end

@implementation MediaFileHistory

- (void)doubleClicked:(id)sender {
	NSLog(@"MediaFileHistory double Clicked");
	NSInteger row;
	
	row = [tableView clickedRow];
	NSLog(@"row: %d", row);
	if(row < 0)
		return;
	
	MediaFileInfo *media = [list objectAtIndex:row];
	NSLog(@"StartTime: %@", media.name);
	NSLog(@"file path: %@", media.path);
	
	[mainCtl startPlayback:media andType:0];
}

- (void)awakeFromNib {
	NSLog(@"=========>>>>>MediaFileHistory awakeFromNib");
	
	list = [[NSMutableArray alloc] init];
	
	[tableView setTarget:self];
	[tableView setDoubleAction:@selector(doubleClicked:)];
	
	[self InitLanguage];
	
	[Channel selectItemAtIndex:16];
	[self getOnlineHistory];
	
}

- (void)InitLanguage {
	NSString *str = nil;
	
	if(playbackDelete)
	{
		str = [[NSString alloc] initWithUTF8String:playbackDelete];
		[deleteBtn setTitle:str];
		[str release];
	}
	if(playbackOpenFS)
	{
		str = [[NSString alloc] initWithUTF8String:playbackOpenFS];
		[openBtn setTitle:str];
		[str release];
	}
	if(playbackSearch)
	{
		str = [[NSString alloc] initWithUTF8String:playbackSearch];
		[searchBtn setTitle:str];
		[str release];
	}
	if(golbalAllChannel)
	{
		str = [[NSString alloc] initWithUTF8String:golbalAllChannel];
		[Channel removeItemAtIndex:16];
		[Channel addItemWithObjectValue:str];
		[str release];
	}
	
	NSArray *array = [tableView tableColumns];
	NSTableColumn *item = nil;
	NSCell *cell = nil;
	if(golbalStartTime)
	{
		str = [[NSString alloc] initWithUTF8String:golbalStartTime];
		item = [array objectAtIndex:0];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
	if(golbalChannel)
	{
		str = [[NSString alloc] initWithUTF8String:golbalChannel];
		item = [array objectAtIndex:1];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
	if(golbalSize)
	{
		str = [[NSString alloc] initWithUTF8String:golbalSize];
		item = [array objectAtIndex:2];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
}

- (IBAction)doDelete:(id)sender {
	int state;
	NSInteger row;
	char *filePath = NULL;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		/**/
		{
			NSLog(@"do delete");
			row = [tableView selectedRow];
			NSLog(@"row: %d", row);
			MediaFileInfo *media = [list objectAtIndex:row];
			NSLog(@"StartTime: %@", media.name);
			NSLog(@"file path: %@", media.path);
			filePath = (char*)[media.path UTF8String];
			kt_util_remove((const char*)filePath);
			[list removeObjectAtIndex:row];
			[tableView reloadData];
		}
	}
}

- (IBAction)doOpenFS:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		/**/
		NSLog(@"do open fs");
		[mainCtl openFileInFS];
	}
}

- (IBAction)doSearch:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		NSLog(@"do search");
		
		[self getOnlineHistory];
	}
}

- (void)getOnlineHistory {
	NSLog(@"get Local History FileList");
	//int count;
	int idx;
	RecFile *fileList;
	RecFile *recFile = NULL;
	NSString *tmp = nil;
	char *homePath = getenv("HOME");
	char recPath[128] = {0};
	DvrConfig *dvrCfg = kt_dvrMgr_getCurDvr();
	
	sprintf(recPath, "%s/DvrRecord/%s/tmp/", homePath, dvrCfg->serverAddr);
	printf("recPath: %s \n", recPath);
	
	fileList = [mainCtl getHistory:recPath];
	kt_fileMgr_dump(fileList);
	
	[list removeAllObjects];
	
	idx = [Channel indexOfSelectedItem];
	NSLog(@"channel index: %d", idx);
	
	recFile = fileList;
	while(recFile)
	{
		/*add to list*/
		if(idx != 16 && (recFile->channel - 1 != idx))
		{
			//continue;
			recFile = recFile->next;
			continue;
		}
		MediaFileInfo *media = [[MediaFileInfo alloc] init];
		tmp = [[NSString alloc] initWithUTF8String:recFile->startTime];
		media.name = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithUTF8String:recFile->size];
		media.size = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithFormat:@"%02d", recFile->channel];
		media.channel = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithUTF8String:recFile->path];
		media.path = tmp;
		[tmp release];
		[list addObject:media];
		[media release];
		
		recFile = recFile->next;
	}
	kt_fileMgr_listFini(fileList);
	
	[tableView reloadData];	
	
}

- (void)dealloc {
	NSLog(@"---------->>MediaFileHistory dealloc");
	[list release];
	
	[super dealloc];
}

#pragma mark -
#pragma mark TableView DataSource

- (NSInteger)numberOfRowsInTableView:(NSTableView*)aTableView {
	return [list count];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*)aTableColumn row:(NSInteger)rowIndex {
	NSString *tmp;
	MediaFileInfo *media = [list objectAtIndex:rowIndex];
	tmp = [aTableColumn identifier];
	if([tmp isEqual:@"StartTime"])
	{
		tmp = media.name;
	}
	else if([tmp isEqual:@"Channel"])
	{
		tmp = media.channel;
	}
	else if([tmp isEqual:@"Size"])
	{
		tmp = media.size;
	}
	
	return tmp;
}

@end

@implementation MediaFileLocal

- (void)doubleClicked:(id)sender {
	NSLog(@"MediaFileHistory double Clicked");
	NSInteger row;
	
	row = [tableView clickedRow];
	NSLog(@"row: %d", row);
	if(row < 0)
		return;
	
	MediaFileInfo *media = [list objectAtIndex:row];
	NSLog(@"StartTime: %@", media.name);
	NSLog(@"file path: %@", media.path);
	
	[mainCtl startPlayback:media andType:0];
}

- (void)awakeFromNib {
	NSLog(@"=========>>>>>MediaFileHistory awakeFromNib");
	
	list = [[NSMutableArray alloc] init];
	
	[tableView setTarget:self];
	[tableView setDoubleAction:@selector(doubleClicked:)];
	
	[self InitLanguage];
	
	[Channel selectItemAtIndex:16];
	[self getLocalRec];
}

- (void)InitLanguage {
	NSString *str = nil;
	
	if(playbackDelete)
	{
		str = [[NSString alloc] initWithUTF8String:playbackDelete];
		[deleteBtn setTitle:str];
		[str release];
	}
	if(playbackOpenFS)
	{
		str = [[NSString alloc] initWithUTF8String:playbackOpenFS];
		[openBtn setTitle:str];
		[str release];
	}
	if(playbackSearch)
	{
		str = [[NSString alloc] initWithUTF8String:playbackSearch];
		[searchBtn setTitle:str];
		[str release];
	}
	if(golbalAllChannel)
	{
		str = [[NSString alloc] initWithUTF8String:golbalAllChannel];
		[Channel removeItemAtIndex:16];
		[Channel addItemWithObjectValue:str];
		[str release];
	}
	
	NSArray *array = [tableView tableColumns];
	NSTableColumn *item = nil;
	NSCell *cell = nil;
	if(golbalStartTime)
	{
		str = [[NSString alloc] initWithUTF8String:golbalStartTime];
		item = [array objectAtIndex:0];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
	if(golbalChannel)
	{
		str = [[NSString alloc] initWithUTF8String:golbalChannel];
		item = [array objectAtIndex:1];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
	if(golbalSize)
	{
		str = [[NSString alloc] initWithUTF8String:golbalSize];
		item = [array objectAtIndex:2];
		cell = [item headerCell];
		[cell setStringValue:str];
		[str release];
	}
}

- (IBAction)doDelete:(id)sender {
	int state;
	NSInteger row;
	char *filePath = NULL;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		/**/
		{
			NSLog(@"do delete");
			row = [tableView selectedRow];
			NSLog(@"row: %d", row);
			MediaFileInfo *media = [list objectAtIndex:row];
			NSLog(@"StartTime: %@", media.name);
			NSLog(@"file path: %@", media.path);
			filePath = (char*)[media.path UTF8String];
			kt_util_remove((const char*)filePath);
			[list removeObjectAtIndex:row];
			[tableView reloadData];
		}
	}
}

- (IBAction)doOpenFS:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		/**/
		NSLog(@"do open fs");
		[mainCtl openFileInFS];
	}
}

- (IBAction)doSearch:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		NSLog(@"do search");
		
		//[self getOnlineHistory];
		[self getLocalRec];
	}
}

- (void)getLocalRec {
	NSLog(@"get Local FileList");
	//int count;
	int idx;
	RecFile *fileList;
	RecFile *recFile = NULL;
	NSString *tmp = nil;
	char *homePath = getenv("HOME");
	char recPath[128] = {0};
	DvrConfig *dvrCfg = kt_dvrMgr_getCurDvr();
	
	sprintf(recPath, "%s/DvrRecord/%s/record/", homePath, dvrCfg->serverAddr);
	
	fileList = [mainCtl getLocalRec:recPath];
	kt_fileMgr_dump(fileList);
	
	[list removeAllObjects];
	
	idx = [Channel indexOfSelectedItem];
	NSLog(@"channel index: %d", idx);
	
	recFile = fileList;
	while(recFile)
	{
		/*add to list*/
		if(idx != 16 && (recFile->channel - 1 != idx))
		{
			//continue;
			recFile = recFile->next;
			continue;
		}
		MediaFileInfo *media = [[MediaFileInfo alloc] init];
		tmp = [[NSString alloc] initWithUTF8String:recFile->startTime];
		media.name = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithUTF8String:recFile->size];
		media.size = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithFormat:@"%02d", recFile->channel];
		media.channel = tmp;
		[tmp release];
		tmp = [[NSString alloc] initWithUTF8String:recFile->path];
		media.path = tmp;
		[tmp release];
		[list addObject:media];
		[media release];
		
		recFile = recFile->next;
	}
	
	kt_fileMgr_listFini(fileList);
	
	[tableView reloadData];	
	
}

- (void)dealloc {
	NSLog(@"------------>>MediaFileLocal dealloc");
	[list release];
	
	[super dealloc];
}

#pragma mark -
#pragma mark TableView DataSource

- (NSInteger)numberOfRowsInTableView:(NSTableView*)aTableView {
	return [list count];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*)aTableColumn row:(NSInteger)rowIndex {
	NSString *tmp;
	MediaFileInfo *media = [list objectAtIndex:rowIndex];
	tmp = [aTableColumn identifier];
	if([tmp isEqual:@"StartTime"])
	{
		tmp = media.name;
	}
	else if([tmp isEqual:@"Channel"])
	{
		tmp = media.channel;
	}
	else if([tmp isEqual:@"Size"])
	{
		tmp = media.size;
	}
	
	return tmp;
}

@end



@implementation MediaControlDelegate

- (BOOL)windowShouldClose:(id)window {
	NSLog(@"MediaControlDelegate window should close");
		
	return YES;
}

@end
