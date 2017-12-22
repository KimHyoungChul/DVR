//
//  ScheduleView.h
//  DVRPlayer
//
//  Created by Kelvin on 3/29/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ScheduleView : UIView {
	// private variables that track state
    BOOL dragging;
	BOOL isEnable;
	
	CGRect selectedRect;
	CGRect baseRect;
	int columnNum;
	int rowNum;
	unsigned char hour[7][24]; /*one week*/
	int mode; /*0: no rec; 1: normal rec; 2: alarm rec*/	
}

@property BOOL isEnable;

- (id)initWithFrame:(CGRect)frame;

// -----------------------------------
// Draw the View Content
// -----------------------------------
- (void)drawRect:(CGRect)rect;

- (void)setMotionRecMode;
- (void)setNormalRecMode;
- (void)setNoRecordMode;

- (void)setSchedule:(unsigned char *)sche;
- (unsigned char*)getSchedule;


@end
