//
//  ScheduleView.h
//  DVRPlayer
//
//  Created by Kelvin on 3/29/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface ScheduleView : NSView {
	// private variables that track state
    BOOL dragging;
	BOOL isEnable;
	
	NSRect selectedRect;
	NSRect baseRect;
	int columnNum;
	int rowNum;
	unsigned char hour[7][24]; /*one week*/
	int mode; /*0: no rec; 1: normal rec; 2: alarm rec*/	
}

@property BOOL isEnable;

- (id)initWithFrame:(NSRect)frame;

// -----------------------------------
// Draw the View Content
// -----------------------------------
- (void)drawRect:(NSRect)rect;

// -----------------------------------
// Handle Mouse Events 
// -----------------------------------
- (void)mouseDown:(NSEvent *)event;
- (void)mouseDragged:(NSEvent *)event;
- (void)mouseUp:(NSEvent *)event;

- (void)setMotionRecMode;
- (void)setNormalRecMode;
- (void)setNoRecordMode;

- (void)setSchedule:(unsigned char *)sche;
- (unsigned char*)getSchedule;


@end
