//  PlaybackController.m
//  DVRPlayer
//
//  Created by Kelvin on 3/22/11.
//  Copyright 2011 KangTop. All rights reserved.

#import "PlaybackController.h"
#import "CustomButton.h"
#include "kt_core_handler.h"
#include "kt_dvr_manage.h"
#include "kt_language_str.h"

@implementation PlaybackController

- (void)captureStatusHandler:(NSNotification*)notification {
	NSLog(@"=====>captureStatusHandler");
	[playbackStatus setStringValue:@"Capture success"];
}

- (void)updatePlaybackStatus:(NSNotification*)notification {
	//NSLog(@"=====>updatePlaybackStatus");
	[self statusLisstener];
}

- (void)awakeFromNib {
	NSLog(@"awake from nib");
	
	[self statusLisstener];
	
	[self InitLanguage];
	
	[[NSNotificationCenter defaultCenter] addObserver:self 
				selector:@selector(captureStatusHandler:) name:@"Update Capture Status" object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self 
				selector:@selector(updatePlaybackStatus:) name:@"Update Playback Status" object:nil];
}

- (void)InitLanguage {
	NSString *str = nil;
	
	if(playbackTitle)
	{
		str = [[NSString alloc] initWithUTF8String:playbackTitle];
		[ctlWin setTitle:str];
		[str release];
	}
	if(playbackOnline){
		str = [[NSString alloc] initWithUTF8String:playbackOnline];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:0];
		[item setLabel:str];		
		[str release];		
	}
	if(playbackHistroy){
		str = [[NSString alloc] initWithUTF8String:playbackHistroy];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:1];
		[item setLabel:str];		
		[str release];		
	}
	if(playbackLocal){
		str = [[NSString alloc] initWithUTF8String:playbackLocal];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:2];
		[item setLabel:str];		
		[str release];		
	}	
	if(playbackStartTimeStr){
		str = [[NSString alloc] initWithUTF8String:playbackStartTimeStr];
		[labelStartTime setStringValue:str];		
		[str release];		
	}
	
}

- (void)openFileinFS {
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    char *homePath = getenv("HOME");
    char recPath[128] = {0};
    sprintf(recPath, "%s/DvrRecord/", homePath);
    NSString *str = [[NSString alloc] initWithUTF8String:recPath];
	[openPanel setCanChooseFiles:YES];
	[openPanel setCanChooseDirectories:NO];
	//[openPanel setDirectory:[NSURL fileURLWithPath:@"/Users/kelvin/DvrRecord"]];
    [openPanel setDirectory:str];
	
	[openPanel beginSheetModalForWindow:ctlWin completionHandler:^(NSInteger result) {
		if(result == NSFileHandlingPanelOKButton) {
			NSURL *url = [openPanel URL];
			NSString *path = [url path];
			const char *utf8Path = [path UTF8String];
			printf("utf8 path: %s \n", utf8Path);
			[playbackSrf destroyPlayback];
			[playbackSrf createPlayback:0];
			[playbackSrf setPlaybackFile:utf8Path andChannel:0];
			[playbackSrf startPlayback];
			[self statusLisstener];
		}
	}];/*
    NSLog(@"do add folder or file");
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    char *homePath = getenv("HOME");
    char recPath[128] = {0};
    sprintf(recPath, "%s/DvrRecord/", homePath);
    NSString *str = [[NSString alloc] initWithUTF8String:recPath];
    NSLog(@"DVR Record Path: %@", str);
    
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanChooseDirectories:YES];
    NSURL *recURL = [[NSURL alloc] initFileURLWithPath:str];
    [openPanel setDirectory:str];
    NSLog(@"222DVR Record Path: %@", str);
    
    [openPanel beginSheetModalForWindow:ctlWin completionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelOKButton) {
            NSURL *url = [openPanel URL];
            NSString *path = [url path];
            const char *utf8Path = [path UTF8String];
            printf("utf8 path: %s \n", utf8Path);
            if(kt_util_isDir((char*)utf8Path) == 1)
            {
                NSLog(@"handle add dir");
                [self addFolderFilesToList:(char*)utf8Path];
            }
            else
            {
                [self addSingleFileToList:(char*)utf8Path];
            }
        }
    }];
    [str release];
    [recURL release];
*/
}
/*
- (int)addFolderFilesToList:(char*)path {
	//int count = 0;
	DIR *pDir = NULL;
	struct dirent *pdirEnt;
	RecFile *head = NULL;
	RecFile *tmp = NULL;
	char filePath[256] = {0};
	
	if(path == NULL)
		return -1;
	
	if((pDir = opendir(path)) == NULL)
	{
		printf("open %s dir error \n", path);
		return -1;
	}
	
	while((pdirEnt = readdir(pDir)) != NULL)
	{
		/*check special dir
		if(strcmp(pdirEnt->d_name, ".") == 0 || strcmp(pdirEnt->d_name, "..") == 0)
		{
			printf("\".\" or \"..\" \n");
			continue;
		}
		if(pdirEnt->d_type == DT_DIR)
		{
			printf("this is a dir");
			sprintf(filePath, "%s/%s", path, pdirEnt->d_name);
			[self addFolderFilesToList:filePath];
			continue;
		}
		sprintf(filePath, "%s/%s", path, pdirEnt->d_name);
		printf("filePath: %s\n", filePath);
		
		if([self isFileInList:filePath] == YES)
		{
			NSLog(@"file already in fileList");
		}
		else
		{
			/*add to list*
			if(head == NULL)
			{
				head = kt_fileMgr_convertLocalFile((const char*)filePath, ".h264", 0);
				if(head != NULL)
				{
					tmp = head;
				}
			}
			else if(tmp != NULL)
			{
				tmp->next = kt_fileMgr_convertLocalFile((const char*)filePath, ".h264", 0);
				if(tmp->next != NULL)
				{
					tmp = tmp->next;
				}
			}
            
		}
		
	}
	
	if(head != NULL)
	{
		[self addRecFileToList:head];
	}
	
	return 0;
}

- (void)addRecFileToList:(RecFile*)recFile {
	NSString *tmp = nil;
	while(recFile)
	{
		/*add to list*
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
	
	kt_fileMgr_listFini(recFile);
	[tableView reloadData];
}
*/

- (IBAction)startPlayback:(id)sender {
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2)
	{
		[playbackSrf normalPlayback];
	}
	[self statusLisstener];
}

- (IBAction)pausePlayback:(id)sender {
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2) /*clicked*/
	{
		[playbackSrf pausePlayback];
	}
	[self statusLisstener];
}

- (IBAction)stopPlayback:(id)sender {
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2)
	{
		[playbackSrf stopPlayback];
	}
	[self statusLisstener];
}

- (IBAction)fastForwardPlayback:(id)sender {
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2) /*clicked*/
	{
		[playbackSrf fastForwardPlayback];
	}
	[self statusLisstener];
}

- (IBAction)slowForwardPlayback:(id)sender {
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2)
	{
		/*handler*/
		[playbackSrf slowForwardPlayback];
	}
	[self statusLisstener];
}

- (IBAction)stepPlayback:(id)sender {
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2)
	{
		[playbackSrf stepPlayback];
	}
	[self statusLisstener];
}

- (IBAction)imgCapture:(id)sender {
	int status;
	DvrConfig *dvrCfg = NULL;
	
	dvrCfg = kt_dvrMgr_getCurDvr();
	if(dvrCfg)
	{
		kt_core_createDvrHome((char*)dvrCfg->serverAddr);
	}
	
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2)
	{
		[playbackSrf setNeedCapture];
	}
}

- (IBAction)dragProcessBar:(id)sender {
	NSLog(@"Playback Controller drayProcessBar");
	
}

- (void)statusLisstener {
	UIStreamControl statusCtl;
	NSString *str = nil;
	statusCtl = [playbackSrf getPlaybackStatus];
	switch(statusCtl)
	{
		case UI_STREAM_NOFILESEL:
			if(playbackNoFile)
			{
				str = [[NSString alloc] initWithUTF8String:playbackNoFile];
				[playbackStatus setStringValue:str];
				[str release];
			}
			else
			{
				[playbackStatus setStringValue:@"No file selected"];
			}
			break;
		case UI_STREAM_NORMAL:
			if(playbackNormal)
			{
				str = [[NSString alloc] initWithUTF8String:playbackNormal];
				[playbackStatus setStringValue:str];
				[str release];
			}
			else
			{
				[playbackStatus setStringValue:@"Normal"];
			}
			break;
		case UI_STREAM_STOP:
			if(playbackStop)
			{
				str = [[NSString alloc] initWithUTF8String:playbackStop];
				[playbackStatus setStringValue:str];
				[str release];
			}
			else
			{
				[playbackStatus setStringValue:@"Stop"];
			}
			break;
		case UI_STREAM_FINISH:
			if(playbackFinish)
			{
				str = [[NSString alloc] initWithUTF8String:playbackFinish];
				[playbackStatus setStringValue:str];
				[str release];
			}
			else
			{
				[playbackStatus setStringValue:@"Finish"];
			}
			break;
		case UI_STREAM_PAUSE:
			if(playbackPause)
			{
				str = [[NSString alloc] initWithUTF8String:playbackPause];
				[playbackStatus setStringValue:str];
				[str release];
			}
			else
			{
				[playbackStatus setStringValue:@"Pause"];
			}
			break;
		case UI_STREAM_FF_2X:
			[playbackStatus setStringValue:@"2X"];
			break;
		case UI_STREAM_FF_4X:
			[playbackStatus setStringValue:@"4X"];
			break;
		case UI_STREAM_FF_8X:
			[playbackStatus setStringValue:@"8X"];
			break;
		case UI_STREAM_SF_2X:
			[playbackStatus setStringValue:@"-2X"];
			break;
		case UI_STREAM_SF_4X:
			[playbackStatus setStringValue:@"-4X"];
			break;
		case UI_STREAM_SF_8X:
			[playbackStatus setStringValue:@"-8X"];
			break;
		case UI_STREAM_STEP:
			if(playbackStep)
			{
				str = [[NSString alloc] initWithUTF8String:playbackStep];
				[playbackStatus setStringValue:str];
				[str release];
			}
			else
			{
				[playbackStatus setStringValue:@"Step"];
			}
			break;
		default:
			[playbackStatus setStringValue:@" "];
			break;
	}
}

- (void)startMediaFile:(MediaFileInfo *)mediaFile withType:(int)type {
	if(mediaFile == nil)
		return;
	NSString *path = mediaFile.path;
	const char *utf8Path = [path UTF8String];
	int ch = [mediaFile.channel intValue];
	[playbackSrf destroyPlayback];
	if(type == 0)
	{
		[playbackSrf createPlayback:0];
	}
	else if(type == 1)
	{
		[playbackSrf createPlayback:1];
	}
	[playbackSrf setPlaybackFile:utf8Path andChannel:ch];
	[playbackSrf startPlayback];
	[playbackStartTime setStringValue:mediaFile.name];
	[self statusLisstener];
}

- (void)startMediaFileByPath:(char*)path withType:(int)type andChannel:(int)channel andStartTime:(NSString*)start {
	[playbackSrf destroyPlayback];
	if(type == 0)
	{
		[playbackSrf createPlayback:0];
	}
	else if(type == 1)
	{
		[playbackSrf createPlayback:1];
	}
	[playbackSrf setPlaybackFile:path andChannel:channel];
	[playbackSrf startPlayback];
	[playbackStartTime setStringValue:start];
	[self statusLisstener];
}

- (void)setDownloadProcess:(BOOL)flag {
	[playbackSrf setDownloadProcess:flag];
}

- (void)setDownloadPrecent:(int)precent {
	[playbackSrf setDownloadPrecent:precent];
}

- (void)setDownloadFileSize:(int)size {
	[playbackSrf setDownloadFileSize:size];
}

- (void)CheckPlayState{
    [playbackSrf CheckPlayState];
}

- (void)dealloc {
	NSLog(@"Playback Controller dealloc");
	[playbackSrf destroyPlayback];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}
@end


