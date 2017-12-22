//
//  GridSelectView.m
//  DVRPlayer
//
//  Created by Kelvin on 3/28/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "GridSelectView.h"
#include "kt_util.h"


@implementation GridSelectView

@synthesize isEnable;

- (id)initWithFrame:(NSRect)frame {
	NSLog(@"init with frame");
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
		columnNum = 15;
		rowNum = 10;
		memset(regionMap, 0x0, sizeof(regionMap));
    }
    return self;
}

- (void)awakeFromNib {
	NSLog(@"awake from nib");
	[self setIsEnable:YES];
}

- (void)drawRect:(NSRect)rect {
    // Drawing code here.
	NSPoint p1, p2;
	NSRect rtmp;
	int i = 0, j = 0, idx;
	float gridWidth, gridHeight;

    [[NSColor whiteColor] set];
    [NSBezierPath fillRect:rect];

	NSRect r = self.bounds;
	NSBezierPath *thePath;
	thePath = [NSBezierPath bezierPathWithRect:r];
	
	NSColor *theColor = [NSColor blackColor];
	[theColor set];
	[thePath setLineWidth:1];
	[thePath stroke];
	
	gridWidth = r.size.width/columnNum;
	gridHeight = r.size.height/rowNum;	
	
	[[NSColor redColor] set];
	for(i = 0; i < rowNum; i++)
	{
		for(j = 0; j < columnNum; j++)
		{
			idx = i*columnNum + j;
			rtmp.origin.x = j*gridWidth;
			rtmp.origin.y = r.size.height - i*gridHeight - gridHeight;
			rtmp.size.width = gridWidth;
			rtmp.size.height = gridHeight;
			if(kt_util_bitValue(regionMap, idx))
			{
				[NSBezierPath fillRect:rtmp];
			}
		}
	}
	
	[[NSColor blueColor] set];
	[NSBezierPath fillRect:selectedRect];	
	
	[[NSColor blackColor] set];
	for(i = 1; i < columnNum; i++)
	{
		p1.x = gridWidth*i;
		p1.y = 0;
		p2.x = gridWidth*i;
		p2.y = r.size.height;
		[NSBezierPath strokeLineFromPoint:p1 toPoint:p2];
	}
	for(j = 1; j < rowNum; j++)
	{
		p1.x = 0;
		p1.y = gridHeight*j;
		p2.x = r.size.width;
		p2.y = p1.y;
		[NSBezierPath strokeLineFromPoint:p1 toPoint:p2];
	}
	
}

- (void)dealloc {
	NSLog(@"GridSelectView dealloc");

	//call super
	[super dealloc];
}

// -----------------------------------
// Handle Mouse Events 
// -----------------------------------
- (void)mouseDown:(NSEvent *)event {
    NSPoint clickLocation;
	float gridWidth, gridHeight;
	NSRect r = self.bounds;
	int column, row;

	if([self isEnable])
	{
		gridWidth = r.size.width/columnNum;
		gridHeight = r.size.height/rowNum;
		printf("gridWidth: %f, gridHeight: %f \n", gridWidth, gridHeight);
		printf("bounds.width: %f, bounds.height: %f \n", r.size.width, r.size.height);
    
		//convert the click location into the view coords
		clickLocation = [self convertPoint:[event locationInWindow]
							  fromView:nil];
		NSLog(@"x: %f, y: %f", clickLocation.x, clickLocation.y);
	
		column = (int)(clickLocation.x/gridWidth);
		row = (int)(clickLocation.y/gridHeight);
		printf("column: %d, row: %d \n", column, row);
		if((row == 0) || (row == rowNum - 1) || (column == 0) || (column == columnNum - 1))
		{
			NSLog(@"in grid surround");
			return;
		}

		selectedRect.origin.x = gridWidth*(column);
		selectedRect.origin.y = gridHeight*(row);
		selectedRect.size.width = gridWidth;
		selectedRect.size.height = gridHeight;
		baseRect.origin.x = selectedRect.origin.x;
		baseRect.origin.y = selectedRect.origin.y;
		baseRect.size.width = selectedRect.size.width;
		baseRect.size.height = selectedRect.size.height;

		[self setNeedsDisplay:YES];

		dragging=YES;
	}

}

- (void)mouseDragged:(NSEvent *)event {
	float gridWidth, gridHeight;
	NSRect r = self.bounds;
	int column, row;

	if([self isEnable])
	{
		gridWidth = r.size.width/columnNum;
		gridHeight = r.size.height/rowNum;

		if (dragging) {
			NSPoint newDragLocation=[self convertPoint:[event locationInWindow]
											  fromView:nil];

			if(newDragLocation.x < 0 || newDragLocation.y < 0 || newDragLocation.x > r.size.width || newDragLocation.y > r.size.height)
				return;
			column = (int)(newDragLocation.x/gridWidth);
			row = (int)(newDragLocation.y/gridHeight);
			if((row <= 0) || (row >= rowNum - 1) || (column <= 0) || (column >= columnNum - 1))
			{
				NSLog(@"in grid surround");
				return;
			}		

			r.origin.x = gridWidth*column;
			r.origin.y = gridHeight*row;
			r.size.width = gridWidth;
			r.size.height = gridHeight;

			if(baseRect.origin.x > r.origin.x)
			{
				selectedRect.origin.x = r.origin.x;
				selectedRect.size.width = baseRect.origin.x - r.origin.x + gridWidth;
			}
			else
			{
				selectedRect.origin.x = baseRect.origin.x;
				selectedRect.size.width = r.origin.x - baseRect.origin.x + gridWidth;
			}

			if(baseRect.origin.y > r.origin.y)
			{
				selectedRect.origin.y = r.origin.y;
				selectedRect.size.height = baseRect.origin.y - r.origin.y + gridHeight;
			}
			else
			{
				selectedRect.origin.y = baseRect.origin.y;
				selectedRect.size.height = r.origin.y - baseRect.origin.y + gridHeight;
			}

			if(selectedRect.origin.x < 0)
			{
				selectedRect.origin.x = 0;
			}
			if(selectedRect.origin.y < 0)
			{
				selectedRect.origin.y = 0;
			}

			r = self.bounds;
			if(selectedRect.size.width > r.size.width)
			{
				selectedRect.size.width = r.size.width;
			}
			if(selectedRect.size.height > r.size.height)
			{
				selectedRect.size.height = r.size.height;
			}

			NSLog(@"select x: %f, select y: %f", selectedRect.origin.x, selectedRect.origin.y);
			NSLog(@"select w: %f, select h: %f", selectedRect.size.width, selectedRect.size.height);
		
			[self setNeedsDisplay:YES];

		}
	}
}

- (void)mouseUp:(NSEvent *)event {
	int i, j, idx;
	float gridWidth, gridHeight;
	int selColumn, selRow;
	int baseColumn, baseRow, basePos;
	NSRect r = self.bounds;
	
	if([self isEnable])
	{
		dragging = NO;
	
		NSLog(@"bound.x: %f, bound.y: %f", r.origin.x, r.origin.y);
		NSLog(@"bound.w: %f, bound.h: %f", r.size.width, r.size.height);
		gridWidth = r.size.width/columnNum;
		gridHeight = r.size.height/rowNum;
		NSLog(@"gridWidth: %f, gridHeight: %f", gridWidth, gridHeight);
		NSLog(@"select x: %f, select y: %f", selectedRect.origin.x, selectedRect.origin.y);
		NSLog(@"select w: %f, select h: %f", selectedRect.size.width, selectedRect.size.height);	
		selColumn = (int)(selectedRect.size.width/(int)gridWidth);
		selRow = (int)(selectedRect.size.height/(int)gridHeight);
		printf("selColumn: %d, selRow: %d \n", selColumn, selRow);
		if(selColumn > columnNum)
		{
			selColumn = columnNum;
		}
		if(selRow > rowNum)
		{
			selRow = rowNum;
		}
	
		baseColumn = (int)((selectedRect.origin.x + 2)/gridWidth);
		baseRow = (int)((selectedRect.origin.y + selectedRect.size.height - gridHeight + 2)/gridHeight);
		printf("baseColumn: %d, baseRow: %d \n", baseColumn, baseRow);
		baseRow = rowNum - baseRow - 1;
		basePos = baseRow*columnNum + baseColumn;
	
		for(i = 0; i < selRow; i++)
		{
			for(j = 0; j < selColumn; j++)
			{
				idx = basePos + i * columnNum + j;
				kt_util_bitXOR(regionMap, idx);
			}
		}
	
		selectedRect.origin.x = 0;
		selectedRect.origin.y = 0;
		selectedRect.size.width = 0;
		selectedRect.size.height = 0;
	
		[self setNeedsDisplay:YES];
	}
    
}

- (void)clearRegoin {
	if([self isEnable])
	{
		memset(regionMap, 0x0, sizeof(regionMap));
		[self setNeedsDisplay:YES];
	}
}

- (void)setColumn:(int)column andRow:(int)row {
	columnNum = column;
	rowNum = row;
	
	[self setNeedsDisplay:YES];
}

- (unsigned char*)getRegionMap {
	
	return regionMap;
}

- (void)setRegionMap:(unsigned char*)region {
	if([self isEnable])
	{
		if(region != NULL)
		{
			memcpy(regionMap, region, 32);
		}
		[self setNeedsDisplay:YES];
	}
}

@end

@implementation GridView

- (void)drawRect:(NSRect)rect {
	NSLog(@"GridView drawRect");
	[[NSColor whiteColor] set];
    [NSBezierPath fillRect:rect];	
}

@end
