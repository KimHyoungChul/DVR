//
//  GridSelectView.h
//  DVRPlayer
//
//  Created by Kelvin on 3/28/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface GridSelectView : NSView {
	// private variables that track state
    BOOL dragging;
	BOOL isEnable;
	
	NSRect selectedRect;
	NSRect baseRect;
	int columnNum;
	int rowNum;
	unsigned char regionMap[32];	
	
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

- (void)clearRegoin;
- (void)setColumn:(int)column andRow:(int)row;
- (unsigned char*)getRegionMap;
- (void)setRegionMap:(unsigned char*)region;

@end

@interface GridView : NSView {
	int columnNum;
	int rowNum;
}

- (void)drawRect:(NSRect)rect;

@end
