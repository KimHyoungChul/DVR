//
//  OffLinePlayController.h
//  DVRPlayer
//
//  Created by Kelvin on 6/29/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PlaybackSurface.h"
#import "MediaFileInfo.h"

@interface OffLinePlayController : NSObject {
	IBOutlet NSWindow *ctlWin;
	IBOutlet PlaybackSurface *playbackSrf;
	IBOutlet NSTextField *playbackStatus;
	
	IBOutlet NSTextField *labelStartTime;
	
}

/*Multi Language*/
- (void)InitLanguage;
- (void)updatePlaybackStatus:(NSNotification*)notification;

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

@end
