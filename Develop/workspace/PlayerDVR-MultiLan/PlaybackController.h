//
//  PlaybackController.h
//  DVRPlayer
//
//  Created by Kelvin on 3/22/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PlaybackSurface.h"
#import "MediaFileInfo.h"

@interface PlaybackController : NSObject {
	IBOutlet NSWindow *ctlWin;
	IBOutlet PlaybackSurface *playbackSrf;
	IBOutlet NSTextField *playbackStatus;
	IBOutlet NSTextField *playbackStartTime;
	
	IBOutlet NSTabView *tabView;
	IBOutlet NSTextField *labelStartTime;

}

- (void)InitLanguage;

- (void)playbackStatusHandler:(NSNotification*)notification;
- (void)updatePlaybackStatus:(NSNotification*)notification;

//- (IBAction)openFileinFS:(id)sender;
- (IBAction)startPlayback:(id)sender;
- (IBAction)pausePlayback:(id)sender;
- (IBAction)stopPlayback:(id)sender;
- (IBAction)fastForwardPlayback:(id)sender;
- (IBAction)slowForwardPlayback:(id)sender;
- (IBAction)stepPlayback:(id)sender;
- (IBAction)imgCapture:(id)sender;
- (IBAction)dragProcessBar:(id)sender;
- (void)startMediaFile:(MediaFileInfo *)mediaFile withType:(int)type;
- (void)startMediaFileByPath:(char*)path withType:(int)type andChannel:(int)channel andStartTime:(NSString*)start;
- (void)statusLisstener;

- (void)openFileinFS;

- (void)setDownloadProcess:(BOOL)flag;
- (void)setDownloadPrecent:(int)precent;
- (void)setDownloadFileSize:(int)size;

@end

/*not used*/
@interface PlaybackWinController : NSWindowController {
	
}

@end