//
//  PlaybackProcessBar.h
//  DVRPlayer
//
//  Created by Kelvin on 4/1/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PlaybackSurface;
@interface PlaybackProcessBar : NSControl {
	IBOutlet PlaybackSurface *playbackSrf;	
	
	BOOL isMouseDown;
	BOOL enableValue2;
	CGFloat glowingSize;
	CGColorRef	glowingColor;
	NSColor *leftColor;
	NSColor *leftColor2;
	NSDate	*lastpostdate;
		
	double _minimum;// 0 by default
    double _maximum;// 100 by default
    
    /* Non-persitent properties */
    double _value;// _minimum by default	
	double _value2;
}

- (id)init;
- (id)initWithFrame:(NSRect)frame;

- (void) mouseDown:(NSEvent *)theEvent;
- (void) mouseDragged:(NSEvent *)theEvent;

- (void) drawRect:(NSRect)aRect;
- (void) setGlowingSize:(CGFloat)size;

- (double)doubleValue;
- (void)setDoubleValue:(double)doubleValue;
- (double)minValue;
- (double)maxValue;

- (void)setEnableValue2:(BOOL)flag;
- (double)doubleValue2;
- (void)setDoubleValue2:(double)doubleValue;

@end
