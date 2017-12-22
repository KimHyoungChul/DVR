//
//  VideoController.h
//  PlayerDVR
//
//  Created by Kelvin on 3/2/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "VideoPreview.h"
#import "MediaFileController.h"

@class DvrSettingControl;

//NSWindowController *setupWin = nil;
NSWindowController *playbackWin = nil; /*for playback window*/
@interface VideoController : NSObject {
	IBOutlet VideoPreview *videoPreview;
	IBOutlet NSWindow *previewWin;
	IBOutlet NSButton *setupBtn;
	IBOutlet NSButton *sisteenBtn;
	IBOutlet NSButton *nineBtn;
	
	IBOutlet NSButton *ptzCruiseSet;
	IBOutlet NSButton *ptzCruiseStart;
	IBOutlet NSButton *ptzCruiseStop;
	IBOutlet NSButton *ptzCruiseRecall;
	IBOutlet NSButton *ptzCruiseLoad;
	IBOutlet NSButton *ptzCruiseSave;
	IBOutlet NSButton *ptzCruiseClear;
	
	IBOutlet NSButton *playbackBtn;
	IBOutlet NSButton *logoutBtn;
	IBOutlet NSTextField *labelPreviewCtl;
	IBOutlet NSTextField *labelPTZCtl;
	IBOutlet NSTextField *labelPTZCtlZoom;
	IBOutlet NSTextField *labelPTZCtlFocus;
	IBOutlet NSTextField *labelPTZCtlIris;
	
	IBOutlet NSTextField *information;
	
	BOOL recStatus;
	BOOL previewStatus; /*YES: ON; NO: OFF*/
	BOOL playbackStatus; /*YES: ON; NO: OFF*/
	BOOL settingStatus; /*YES: ON; NO: OFF*/
	BOOL infoStatus; /*YES:ON; NO: OFF*/
	
	NSWindowController *add_window; /*for logout to login window*/
	
	DvrSettingControl *settingCtl;
	
	NSTimer *animationTimer;
	
	int ptzPointValue_;
	IBOutlet NSStepper *ptzPointStep;
	IBOutlet NSTextField *ptzPointEdit;
}

@property (nonatomic, readwrite) int ptzPointValue;

- (void)onTimer;

- (void)playbackExitHandler:(NSNotification*)notification;
- (void)settingExitHandler:(NSNotification*)notification;
- (void)notifyInfoHandler:(NSNotification*)notification;

/*multi language*/
- (void)InitLanguage;

- (IBAction)Logout:(id)sender;
- (IBAction)StartVideo:(id)sender;
- (IBAction)StopVideo:(id)sender;
- (IBAction)oneChannel:(id)sender;
- (IBAction)fourChannel:(id)sender;
- (IBAction)nineChannel:(id)sender;
- (IBAction)sisteenChannel:(id)sender;
- (IBAction)imgCapture:(id)sender;
- (IBAction)dvrSetup:(id)sender;
- (IBAction)doRecord:(id)sender;
- (IBAction)startPlayback:(id)sender;
- (IBAction)switchPreview:(id)sender;

- (IBAction)testButton:(id)sender;

/*PTZ Control*/
- (IBAction)ptzUp:(id)sender;
- (IBAction)ptzDown:(id)sender;
- (IBAction)ptzLeft:(id)sender;
- (IBAction)ptzRight:(id)sender;
- (IBAction)ptzZoomin:(id)sender;
- (IBAction)ptzZoomout:(id)sender;
- (IBAction)ptzFocusFar:(id)sender;
- (IBAction)ptzFocusNear:(id)sender;
- (IBAction)ptzIRISOpen:(id)sender;
- (IBAction)ptzIRISClose:(id)sender;
- (IBAction)ptzCruisePreSet:(id)sender;
- (IBAction)ptzCruiseLoad:(id)sender;
- (IBAction)ptzCruiseStart:(id)sender;
- (IBAction)ptzCruiseStop:(id)sender;
- (IBAction)ptzCruiseRecall:(id)sender;
- (IBAction)ptzCruiseSave:(id)sender;
- (IBAction)ptzCruiseClear:(id)sender;
- (BOOL)windowShouldClose:(id)window;

@end

@interface previewDelegate : NSObject {
	IBOutlet VideoPreview *videoPreview;
}

- (BOOL)windowShouldClose:(id)window;

@end

@interface playbackDelegate : NSObject {
	IBOutlet MediaFileController *MediaControl;
	
}

- (BOOL)windowShouldClose:(id)window;

@end

@interface settingDelegate : NSObject {
	
}

- (BOOL)windowShouldClose:(id)window;

@end
