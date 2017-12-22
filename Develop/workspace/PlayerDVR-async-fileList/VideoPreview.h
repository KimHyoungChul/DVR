//
//  VideoPreview.h
//  PlayerDVR
//
//  Created by Kelvin on 2/28/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "kt_stream_playback.h"

@interface VideoItem : NSObject {
	Stream_Playback *playback;
	int channel;
	BOOL startFlag;
	char serverName[64];

	BOOL reqCapture;
	BOOL reqRecord;
	BOOL doDraw; /*YES: draw; NO not draw*/
	
	CGRect rect;
	
	/*parent*/
	NSView *view;
	BOOL focusFlag;
}

- (VideoItem *)initWithRect:(CGRect)r atParent:(NSView*)parent;
- (void)setChannel:(int)ch andServer:(char*)name;
- (int)createStreamPlaybackWithChannel:(unsigned int)ch;
- (int)destroyStreamPlayback;
- (int)resetUsrName:(char*)name andPwd:(char*)pwd;
- (int)resetNetServer:(char*)addr andPort:(int)port andType:(unsigned int)type;
- (int)resetRect:(CGRect)r;
- (int)startPlayback;
- (int)stopPlayback;
- (void)displayYUVFrame;
- (void)dropYUVFrame;
- (void)displayFrame:(RGB_Entry *)pEntry;
- (void)drawBorder;
- (BOOL)pointInside:(NSPoint)p;
- (void)setFocus:(BOOL)flag;
- (BOOL)getFocus;
- (BOOL)getStartFlag;
- (BOOL)isRecording;
- (void)setNeedCapture;
- (void)startRecord;
- (void)stopRecord;
- (void)drawRecFlag;

- (void)disableScreenDraw;
- (void)enableScreenDraw;

- (void)ptzUp;
- (void)ptzDown;
- (void)ptzLeft;
- (void)ptzRight;
- (void)ptzZoomin;
- (void)ptzZoomout;
- (void)ptzFocusFar;
- (void)ptzFocusNear;
- (void)ptzIRISOpen;
- (void)ptzIRISClose;
- (void)ptzStop;

@end

typedef enum
{
	ONE_GRID = 1,
	FOUR_GRID = 2,
	NINE_GRID = 3,
	SISTEEN_GRID = 4
}GridType;

typedef enum
{
	GROUP_NONE,
	GROUP_ONE,
	GROUP_TWO,
	GROUP_THREE,
	GROUP_FOUR,
	GROUP_FIVE
}GroupNum;


//NSMutableArray *itemArray;
@interface VideoPreview : NSView {
	NSTimer *animationTimer;
	NSTimeInterval animationInterval;
	
	int gridDrawFlag;
	int lastFocus;
	GridType gridType;
	int channelNum;
	GroupNum group;
	
	NSMutableArray *itemArray;
}

- (void)focusCapture;
- (void)focusFullScreen;
- (void)focusPtzUp;
- (void)focusPtzDown;
- (void)focusPtzLeft;
- (void)focusPtzRight;
- (void)focusPtzZoomin;
- (void)focusPtzZoomout;
- (void)focusPtzFocusFar;
- (void)focusPtzFocusNear;
- (void)focusPtzIRISOpen;
- (void)focusPtzIRISClose;
- (void)focusPtzStop;

- (void)recordAllChannel;
- (void)stopRecordAllChannel;

- (void)disableDrawAllChannel;
- (void)enableDrawAllChannel;

- (void)resetAllChannel;
- (void)ontimer;
- (void)start;
- (void)stop;
- (void)logout;

- (void)setGridType:(GridType)type;
- (void)initAllChannel;
- (void)finiAllChannel;

//---------------------------------------
// Handle Mouse Events
//---------------------------------------
- (void)mouseDown:(NSEvent *)theEvent;
- (void)mouseUp:(NSEvent *)theEvent;

@end


