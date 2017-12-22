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

- (id)initWithFrame:(CGRect)frame {
	NSLog(@"init with frame");
    self = [super initWithFrame:frame];
    if(self)
	{
        //Initialization code here.
		columnNum = 15;
		rowNum = 10;
		memset(regionMap, 0x0, sizeof(regionMap));
    }
    return self;
}

- (void)awakeFromNib {
	NSLog(@"awake from nib");
	[self setIsEnable:YES];
	columnNum = 15;
	rowNum = 10;
}

- (void)drawRect:(CGRect)rect {
    //Drawing code here.
	CGPoint p1, p2;
	CGRect rtmp;
	int i = 0, j = 0, idx;
	UIBezierPath *tPath;
	float gridWidth, gridHeight;

	CGRect r = self.bounds;
	UIBezierPath *thePath;
	thePath = [UIBezierPath bezierPathWithRect:r];
	
	/*draw background*/
	[[UIColor whiteColor] set];
	[thePath fill];
	
	
	UIColor *theColor = [UIColor blackColor];
	[theColor set];
	[thePath setLineWidth:1];
	[thePath stroke];
	
	gridWidth = r.size.width/columnNum;
	gridHeight = r.size.height/rowNum;	
	
#if 1
	[[UIColor redColor] set];
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
				//[UIBezierPath fillRect:rtmp];
				tPath = [UIBezierPath bezierPathWithRect:rtmp];
				[tPath fill];
			}
		}
	}
#endif
	
	[[UIColor blueColor] set];
	tPath = [UIBezierPath bezierPathWithRect:selectedRect];
	[tPath fill];
	
	/*draw the grids*/
	[[UIColor blackColor] set];
	[thePath setLineWidth:1];
	for(i = 1; i < columnNum; i++)
	{
		p1.x = gridWidth*i;
		p1.y = 0;
		p2.x = gridWidth*i;
		p2.y = r.size.height;
		[thePath moveToPoint:p1];
		[thePath addLineToPoint:p2];
	}
	for(j = 1; j < rowNum; j++)
	{
		p1.x = 0;
		p1.y = gridHeight*j;
		p2.x = r.size.width;
		p2.y = p1.y;
		[thePath moveToPoint:p1];
		[thePath addLineToPoint:p2];
	}
	[thePath stroke];
	
}

- (void)dealloc {
	NSLog(@"GridSelectView dealloc");

	//call super
	[super dealloc];
}

// -----------------------------------
// Handle Mouse Events 
// -----------------------------------
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"GridSelectView Phase: Touches begin");
	float gridWidth, gridHeight;
	CGRect r = self.bounds;
	int column, row;
	
	if([self isEnable])
	{
		gridWidth = r.size.width/columnNum;
		gridHeight = r.size.height/rowNum;

		NSUInteger numTaps = [[touches anyObject] tapCount]; /*double clicked or clicked*/
		NSLog(@"num Taps: %d", numTaps);
	
		NSLog(@"num Touch: %d", touches.count);
		UITouch *touch = [[touches allObjects] objectAtIndex:0];
		CGPoint touchPoint = [touch locationInView:self];
		NSLog(@"x: %lf, y: %lf", touchPoint.x, touchPoint.y);
		
		column = (int)(touchPoint.x/gridWidth);
		row = (int)(touchPoint.y/gridHeight);
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

		[self setNeedsDisplay];

		dragging=YES;
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"GridSelectView Phase: touchesMoved");
	float gridWidth, gridHeight;
	CGRect r = self.bounds;
	int column, row;

	if([self isEnable])
	{
		gridWidth = r.size.width/columnNum;
		gridHeight = r.size.height/rowNum;

		if (dragging) {
			UITouch *touch = [[touches allObjects] objectAtIndex:0];
			CGPoint touchPoint = [touch locationInView:self];
			NSLog(@"x: %lf, y: %lf", touchPoint.x, touchPoint.y);

			if(touchPoint.x < 0 || touchPoint.y < 0 || touchPoint.x > r.size.width || touchPoint.y > r.size.height)
				return;
			column = (int)(touchPoint.x/gridWidth);
			row = (int)(touchPoint.y/gridHeight);
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

			[self setNeedsDisplay];

		}
	}
	
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"GridSelectView Phase: touchesEnded");
	int i, j, idx;
	float gridWidth, gridHeight;
	int selColumn, selRow;
	int baseColumn, baseRow, basePos;
	CGRect r = self.bounds;

	if([self isEnable])
	{
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

		[self setNeedsDisplay];
		
		dragging = NO;
	}
}

- (void)clearRegoin {
	if([self isEnable])
	{
		memset(regionMap, 0x0, sizeof(regionMap));
		[self setNeedsDisplay];
	}
}

- (void)setColumn:(int)column andRow:(int)row {
	columnNum = column;
	rowNum = row;
	
	[self setNeedsDisplay];
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
		[self setNeedsDisplay];
	}
}

@end

@implementation GridView

- (void)drawRect:(CGRect)rect {
	NSLog(@"GridView drawRect");
	[[UIColor whiteColor] set];
    //[UIBezierPath fillRect:rect];	
}

@end
