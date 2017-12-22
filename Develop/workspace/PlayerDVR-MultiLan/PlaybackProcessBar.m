//
//  PlaybackProcessBar.m
//  DVRPlayer
//
//  Created by Kelvin on 4/1/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "PlaybackProcessBar.h"


@implementation PlaybackProcessBar

- (void)awakeFromNib {
	isMouseDown=NO;
	leftColor=[[NSColor colorWithCalibratedRed:0.0/256.0 green:150.0/256.0 blue:1.0 alpha:1.0] retain];
	{
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
		CGColorSpaceRelease(colorSpace);
		NSColor *deviceColor = [leftColor colorUsingColorSpaceName:NSDeviceRGBColorSpace];
		CGFloat components[4];
		[deviceColor getRed:&components[0] green:&components[1] blue:&components[2] alpha:&components[3]];
		glowingColor = CGColorCreate(colorSpace, components);
	}
	leftColor2 = [[NSColor colorWithCalibratedRed:10.0/256.0 green:50.0/256.0 blue:81.0/256.0 alpha:1.0] retain];
	
	glowingSize=7.0;
	
	lastpostdate=[[NSDate date] retain];
	
	_minimum = 0.0;
	_maximum = 100.0;
	_value = 0.0;
	_value2 = 0.0;
	enableValue2 = NO;
}

- (id)init {
	self = [super init];
	if(self)
	{
		NSLog(@"Playback ProcessBar init");
		_minimum = 0.0;
		_maximum = 100.0;
		_value = 0.0;
		_value2 = 0.0;
		enableValue2 = NO;
	}
	
	return self;
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if(self)
	{
		NSLog(@"Playback ProcessBar initWithFrame");
        [self awakeFromNib];
		_minimum = 0.0;
		_maximum = 100.0;
		_value = 0.0;
		_value2 = 0.0;
		enableValue2 = NO;
    }
    return self;
}

- (void)dealloc {
	NSLog(@"PlaybackProcessBar dealloc");
	CGColorRelease (glowingColor);
	[leftColor release];
	[leftColor2 release];
	[lastpostdate release];
	[super dealloc];
}

- (void)setGlowingSize:(CGFloat)size {
	glowingSize = size;
	[self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect {
	
	NSImage *canvas = [[[NSImage alloc] initWithSize:[self bounds].size] autorelease];
	NSRect canvasRect = NSMakeRect(0, 0, [canvas size].width, [canvas size].height);
	
	[canvas lockFocus];
	
	NSRect slideRect = canvasRect;
	slideRect.size.height=4;
	slideRect.origin.y += roundf((canvasRect.size.height - slideRect.size.height) / 2)-1;
	slideRect.size.width-=6;
	slideRect.origin.x += 2;
	
	[[NSColor grayColor] set];
	NSRectFill(slideRect);
	
	if(enableValue2 == YES)
	{
		[leftColor2 set];
		NSRect rect2 = slideRect;
		rect2.size.width = rect2.size.width*[self doubleValue2]/[self maxValue];
		NSRectFill(rect2);
	}
	
	[leftColor set];
	
	if (glowingSize>0.0)
	{
		CGContextRef c = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		CGContextSetShadowWithColor(c, CGSizeZero, glowingSize, glowingColor);		
	}
	
	slideRect.size.width=slideRect.size.width*[self doubleValue]/[self maxValue];
	NSRectFill(slideRect);
	//NSLog(@"double value: %f, maxValue: %f, minValue: %f", [self doubleValue], [self maxValue], [self minValue]);
	
	NSRect knobRect=slideRect;
	knobRect.origin.x+=knobRect.size.width-12;	
	if (knobRect.origin.x<0)
	{
		knobRect.origin.x=0;
	}
	knobRect.size.width=16;
	knobRect.size.height=10;
	knobRect.origin.y-=3;
	
	[leftColor set];
	NSBezierPath *p = [NSBezierPath bezierPath];
	[p appendBezierPathWithOvalInRect:knobRect];
	[p fill];	
	
	[canvas unlockFocus];
	
	[canvas drawInRect:dirtyRect fromRect:dirtyRect operation:NSCompositeSourceOver fraction:1.0];
	
}


- (void)mouseDown:(NSEvent *)theEvent {
	NSPoint thePoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	if(NSPointInRect(thePoint, [self bounds]))
	{
		[playbackSrf setUpdateBar:NO];
		isMouseDown=YES;
		[self mouseDragged:theEvent];
	}
}

- (void)mouseUp:(NSEvent *)theEvent {
	NSLog(@"Playback process bar mouse up");
	
	NSPoint thePoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	double theValue;
	int pos;
	double maxX = [self bounds].size.width;
	
	if(thePoint.x < 0)
		theValue = [self minValue];
	else if(thePoint.x >= [self bounds].size.width)
		theValue = [self maxValue];
	else
	{
		theValue = [self minValue] + (([self maxValue] - [self minValue]) * (thePoint.x - 0)/(maxX - 0));
	}
	pos = (int)(theValue*10);
	[playbackSrf resetPlaybackRestart:pos];
	[playbackSrf setUpdateBar:YES];
	
	NSLog(@"Target: %@", [self target]);
	
	[self sendAction:self.action to:self.target];
	[[self target] performSelector:[self action]];
	isMouseDown=NO;
}

- (void)mouseDragged:(NSEvent *)theEvent {
	if (isMouseDown)
	{
		NSPoint thePoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
		double theValue;
		double maxX=[self bounds].size.width;
		
		if (thePoint.x < 0)
			theValue = [self minValue];
		else if (thePoint.x >= [self bounds].size.width)
			theValue = [self maxValue];
		else
			theValue = [self minValue] + (([self maxValue] - [self minValue]) *
										  (thePoint.x - 0) / (maxX - 0));
		[self setDoubleValue:theValue];
				
		[self setNeedsDisplay:YES];
	}
}

- (BOOL)mouseDownCanMoveWindow {
	return NO;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent {
	return NO;
}

- (BOOL)isFlipped {	
	return NO;
}

- (double)doubleValue {
	return _value;
}

- (void)setDoubleValue:(double)doubleValue {
	_value = doubleValue;
}

- (double)minValue {
	return _minimum;
}

- (double)maxValue {
	return _maximum;
}

- (void)setEnableValue2:(BOOL)flag {
	enableValue2 = flag;
}

- (double)doubleValue2 {
	return _value2;
}

- (void)setDoubleValue2:(double)doubleValue {
	_value2 = doubleValue;
}


@end
