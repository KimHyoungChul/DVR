//
//  GridSelectView.h
//  DVRPlayer
//
//  Created by Kelvin on 3/28/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface GridSelectView : UIView {
	// private variables that track state
    BOOL dragging;
	BOOL isEnable;
	
	CGRect selectedRect;
	CGRect baseRect;
	int columnNum;
	int rowNum;
	unsigned char regionMap[32];	
	
}

@property BOOL isEnable;

- (id)initWithFrame:(CGRect)frame;

// -----------------------------------
// Draw the View Content
// -----------------------------------
- (void)drawRect:(CGRect)rect;

// -----------------------------------
// Handle Mouse Events 
// -----------------------------------
//- (void)mouseDown:(NSEvent *)event;
//- (void)mouseDragged:(NSEvent *)event;
//- (void)mouseUp:(NSEvent *)event;

- (void)clearRegoin;
- (void)setColumn:(int)column andRow:(int)row;
- (unsigned char*)getRegionMap;
- (void)setRegionMap:(unsigned char*)region;

@end

@interface GridView : UIView {
	int columnNum;
	int rowNum;
}

- (void)drawRect:(CGRect)rect;

@end
