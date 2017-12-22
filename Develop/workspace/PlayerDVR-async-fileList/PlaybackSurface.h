//
//  PlaybackSurface.h
//  DVRPlayer
//
//  Created by Kelvin on 3/22/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "kt_stream_playback.h"
#import "PlaybackProcessBar.h"
#include "kt_core_config.h"

typedef enum _UIStreamControl
{
	UI_STREAM_NOFILESEL,
	UI_STREAM_NORMAL,
	UI_STREAM_STOP,
	UI_STREAM_PAUSE,
	UI_STREAM_FINISH,
	UI_STREAM_FF_2X,
	UI_STREAM_FF_4X,
	UI_STREAM_FF_8X,
	UI_STREAM_SF_2X,
	UI_STREAM_SF_4X,
	UI_STREAM_SF_8X,
	UI_STREAM_STEP,
	
}UIStreamControl;


@interface PlaybackSurface : NSView {
	NSTimer *animationTimer;
	NSTimeInterval animationInterval;	
	Stream_Playback *playback;
#if defined(KT_AUDIO_ENABLE) && (KT_AUDIO_ENABLE == 1)
	void *audioOutput;
	BOOL outputFlag;
#endif /*KT_AUDIO_ENABLE*/
	UIStreamControl streamCtl; /*0: normal or pause, 1: ff 1x, 2: ff 2x, 0x0f: step, -1: sf 1x, -2: sf 2x*/
	int counter;
	int counter2;
	BOOL updateBar;
	int stepUp;
	BOOL reqCapture;
	int channel;
	IBOutlet PlaybackProcessBar *processBar;
	int onlineFileSize;
	
	BOOL startFlag;
}

- (void)ontimer;
- (int)createPlayback:(int)type;
- (int)destroyPlayback;
- (int)startPlayback;
- (int)stopPlayback;
- (int)normalPlayback;
- (int)pausePlayback;
- (int)fastForwardPlayback;
- (int)slowForwardPlayback;
- (int)stepPlayback;
- (int)resetPlaybackRestart:(int)pos;
- (void)setPlaybackFile:(const char*)path andChannel:(int)ch;
- (void)displayYUVFrame;
- (void)displayFrame:(RGB_Entry *)pEntry;
- (UIStreamControl)getPlaybackStatus;
- (void)setNeedCapture;
- (void)setUpdateBar:(BOOL)update;

- (void)setDownloadProcess:(BOOL)flag;
- (void)setDownloadPrecent:(int)precent;
- (void)setDownloadFileSize:(int)size;

@end
