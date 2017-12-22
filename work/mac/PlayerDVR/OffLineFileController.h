//
//  OffLineFileController.h
//  DVRPlayer
//
//  Created by Kelvin on 6/29/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "OffLinePlayController.h"

@interface OffLineFileController : NSObject {
	IBOutlet OffLinePlayController *playbackCtl;
	NSMutableArray *list;
	IBOutlet NSWindow *ctlWin;
	IBOutlet NSTableView *tableView;
	IBOutlet NSButton *addBtn;
	IBOutlet NSButton *delBtn;
	IBOutlet NSButton *clearBtn;
	IBOutlet NSButton *openBtn;
}
- (void)InitLanguage;
- (void)getLocalRec;
- (BOOL)isFileInList:(const char*)path;
- (void)addRecFileToList:(RecFile*)recFile;
- (int)addSingleFileToList:(char *)path;
- (int)addFolderFilesToList:(char*)path;

- (IBAction)doOpenFS:(id)sender;
- (IBAction)doDelFile:(id)sender;
- (IBAction)doClearFileList:(id)sender;
- (IBAction)doAddFolderOrFile:(id)sender;

@end
