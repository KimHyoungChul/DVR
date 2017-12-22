//
//  OffLineFileController.m
//  DVRPlayer
//
//  Created by Kelvin on 6/29/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "OffLineFileController.h"
#import "MediaFileInfo.h"
#import "CustomButton.h"

#include "kt_file_manage.h"
#include "kt_stream_playback.h"
#include "kt_dvr_manage.h"
#include "kt_util.h"
#include "kt_core_handler.h"
#include "kt_language_str.h"

@implementation OffLineFileController

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
	
	[playbackCtl startMediaFile:media withType:0];
}

- (void)awakeFromNib {
	NSLog(@"=========>>>>>MediaFileHistory awakeFromNib");
	
	list = [[NSMutableArray alloc] init];
	
	[tableView setTarget:self];
	[tableView setDoubleAction:@selector(doubleClicked:)];
	
	[self InitLanguage];
	
	//[Channel selectItemAtIndex:16];
	//[self getLocalRec];
}

- (void)InitLanguage {
	NSLog(@"=====>>>>>OffLineFileController InitLanguage");
	
}

- (IBAction)doOpenFS:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		/**/
		NSLog(@"do open fs");
		[playbackCtl openFileinFS];
	}
}

- (IBAction)doDelFile:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		//NSLog(@"do del file");
		{
			NSLog(@"do delete");
			int row = [tableView selectedRow];
			NSLog(@"row: %d", row);
			MediaFileInfo *media = [list objectAtIndex:row];
			NSLog(@"StartTime: %@", media.name);
			NSLog(@"file path: %@", media.path);
			char *filePath = (char*)[media.path UTF8String];
			kt_util_remove((const char*)filePath);
			[list removeObjectAtIndex:row];
			[tableView reloadData];
		}
	}
}

- (IBAction)doClearFileList:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
		NSLog(@"do clear file List");
		[list removeAllObjects];
		[tableView reloadData];
	}
}

- (IBAction)doAddFolderOrFile:(id)sender {
	int state;
	state = [(CustomButton*)sender getBtnStatus];
	if(state == 2)
	{
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
	}
}

- (BOOL)isFileInList:(const char*)path {
	//NSLog(@"MediaFileController isFileInList");
	int count = [list count];
	int i = 0;
	BOOL ret = NO;
	MediaFileInfo *media;
	const char *str = NULL;
	
	for(i = 0; i < count; i++)
	{
		media = [list objectAtIndex:i];
		NSLog(@"%@", media.path);
		str = [media.path UTF8String];
		if(strcmp(path, str) == 0)
		{
			ret = YES;
			break;
		}
	}
	
	return ret;
}

- (void)addRecFileToList:(RecFile*)recFile {
	NSString *tmp = nil;
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
	
	kt_fileMgr_listFini(recFile);
	[tableView reloadData];
}

- (int)addSingleFileToList:(char *)path {
	NSLog(@"add single file to list");
	if([self isFileInList:path] == YES)
	{
		NSLog(@"file already in fileList");
	}
	else
	{
		/*add to list*/
		RecFile *recFile = kt_fileMgr_convertLocalFile((const char*)path, ".h264", 0);
		if(recFile)
		{
			[self addRecFileToList:recFile];

		}
	}
	
	return 0;
}

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
		/*check special dir*/
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
			/*add to list*/
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

- (RecFile*)getLocalRec2:(char*)path {
	int count = 0;
	NSLog(@"MediaFileController getLocal");
	//DvrConfig *dvrCfg = NULL;
	
	//dvrCfg = kt_dvrMgr_getCurDvr();
	//if(dvrCfg)
	//{
	//	kt_core_createDvrHome((char*)dvrCfg->serverAddr);
	//}
	
	return kt_fileMgr_getRecFileInFS(path, ".h264", 0xff, 0, &count);
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
	//DvrConfig *dvrCfg = kt_dvrMgr_getCurDvr();
	
	sprintf(recPath, "%s/DvrRecord/%s/record/", homePath, "192.168.6.9");
	
	fileList = [self getLocalRec2:recPath];
	kt_fileMgr_dump(fileList);
	
	[list removeAllObjects];
	
	//idx = [Channel indexOfSelectedItem];
	idx = 16;
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
		//NSLog(@"xxxxxx");
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
