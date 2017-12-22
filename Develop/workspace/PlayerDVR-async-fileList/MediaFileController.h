//
//  MediaFileController.h
//  DVRPlayer
//
//  Created by Kelvin on 3/31/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PlaybackController.h"
#include "kt_stream_playback.h"

@class MediaFileController;

@interface MediaFileOnline : NSObject {
	NSMutableArray *list;
	IBOutlet NSTableView *tableView;
	IBOutlet NSDatePicker *datePicker;
	IBOutlet NSDatePickerCell *datePickerCell;
	IBOutlet NSButton *refreshBtn;
	IBOutlet NSButton *searchBtn;
	IBOutlet NSButton *deleteBtn;
	IBOutlet NSButton *openBtn;
	IBOutlet NSTextField *descripStr;
	IBOutlet NSTextField *labelRecDay;
	IBOutlet NSComboBox *Channel;
	IBOutlet NSComboBox *RecType;
	
	BOOL downloadStatus;
	//BOOL fileListTh;
	//NSCondition *_myCondition;
	
	IBOutlet MediaFileController *mainCtl;
}
- (void)InitLanguage;
- (void)updateBackupStatus:(NSNotification*)notification;
- (void)updateListSource:(NSNotification*)notification;

- (void)getRemoteFileList:(id)obj;

- (IBAction)datePickerAction:(id)sender;
- (IBAction)monthPickerAction:(id)sender;
- (IBAction)doSearch:(id)sender;
- (IBAction)dateRefresh:(id)sender;
- (IBAction)doDelete:(id)sender;
- (IBAction)doOpenFS:(id)sender;

- (void)getSelectYear:(int*)year Month:(int*)month andDay:(int*)day;
- (void)clearFileList;
- (void)getOnlineFileListAtYear:(int)year Month:(int)month andDay:(int)day;
- (void)refreshRecDescrip;

@end

@interface MediaFileHistory : NSObject {
	NSMutableArray *list;
	IBOutlet NSTableView *tableView;
	IBOutlet MediaFileController *mainCtl;
	IBOutlet NSComboBox *Channel;
	
	IBOutlet NSButton *searchBtn;
	IBOutlet NSButton *deleteBtn;
	IBOutlet NSButton *openBtn;
}
- (void)InitLanguage;

- (IBAction)doDelete:(id)sender;
- (IBAction)doOpenFS:(id)sender;
- (IBAction)doSearch:(id)sender;

- (void)getOnlineHistory;

@end

@interface MediaFileLocal : NSObject {
	NSMutableArray *list;
	IBOutlet NSTableView *tableView;
	IBOutlet MediaFileController *mainCtl;
	IBOutlet NSComboBox *Channel;
	
	IBOutlet NSButton *searchBtn;
	IBOutlet NSButton *deleteBtn;
	IBOutlet NSButton *openBtn;
}
- (void)InitLanguage;

- (IBAction)doDelete:(id)sender;
- (IBAction)doOpenFS:(id)sender;
- (IBAction)doSearch:(id)sender;

- (void)getLocalRec;

@end


@interface MediaFileController : NSObject {
	NSTimer *animationTimer;
	NSTimeInterval animationInterval;
	
	Playback_Online *onlineCtl;
	
	IBOutlet PlaybackController *playbackCtl;
	//IBOutlet MediaFileOnline *onlineViewCtl;
	//IBOutlet MediaFileHistory *historyViewCtl;
	
	//IBOutlet NSTabView *mainView;
}

/*for online playback*/
- (void)stopTimer;
- (void)ontimer;
- (void)getCalendaAtYear:(int)year Month:(int)month andDay:(int)day;
- (void)getCalendaDescrip:(char*)descrip;
- (void)openFileInFS;
- (int)initSearchMgrAtYear:(int)year Month:(int)month andDay:(int)day;
- (int)searchMgrGetNumberAtYear:(int)year Month:(int)month andDay:(int)day;
- (int)searchMgrGetFileByPageAtYear:(int)year Month:(int)month andDay:(int)day byPage:(int)page;
- (RecFile*)convertToRecFile:(int)channel andType:(int)type;

/*for online playback history*/
- (RecFile*)getOnlineHistory:(char*)path;
- (RecFile*)getHistory:(char*)path;
- (RecFile*)getLocalRec:(char*)path;

/*start playback*/
- (void)startPlayback:(MediaFileInfo*)media andType:(int)type;
/*start download*/
- (void)startDownload:(MediaFileInfo*)media;

@end

@interface MediaControlDelegate : NSObject {
	
}

- (BOOL)windowShouldClose:(id)window;

@end
