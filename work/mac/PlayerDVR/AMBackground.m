//
//  AMBackground.m
//  PlayerDVR
//
//  Created by Kelvin on 2/28/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "AMBackground.h"


@implementation AMBackground

- (id)initWithFrame:(NSRect)frame {
    if (self = [super initWithFrame:frame]) {
    }
    return self;
}

- (void)drawRect:(NSRect)rect {
    
    NSRect bounds = self.bounds;
    
    // Draw background gradient
    NSGradient *gradient = [[NSGradient alloc] initWithColorsAndLocations:
                            //[NSColor colorWithDeviceWhite:0.15f alpha:1.0f], 0.0f, 
							[NSColor colorWithDeviceWhite:0.35f alpha:1.0f], 0.0f,							
                            //[NSColor colorWithDeviceWhite:0.19f alpha:1.0f], 0.5f, 
							[NSColor colorWithDeviceWhite:0.39f alpha:1.0f], 0.5f,							
                            //[NSColor colorWithDeviceWhite:0.20f alpha:1.0f], 0.5f, 
							[NSColor colorWithDeviceWhite:0.40f alpha:1.0f], 0.5f, 							
                            //[NSColor colorWithDeviceWhite:0.25f alpha:1.0f], 1.0f, 
							[NSColor colorWithDeviceWhite:0.45f alpha:1.0f], 1.0f,							
                            nil];
    
    [gradient drawInRect:bounds angle:90.0f];
    [gradient release];
    
    // Stroke bounds
    [[NSColor blackColor] setStroke];
    [NSBezierPath strokeRect:bounds];
}

@end
