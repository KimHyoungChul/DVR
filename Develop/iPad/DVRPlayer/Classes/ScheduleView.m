//
//  ScheduleView.m
//  DVRPlayer
//
//  Created by Kelvin on 3/29/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "ScheduleView.h"


@implementation ScheduleView

@synthesize isEnable;

// -----------------------------------
// Initialize the View
// -----------------------------------
- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
		columnNum = 25;
		rowNum = 8;
		mode = 0;
		memset(hour, 0x0, sizeof(hour));
		NSLog(@"sizeof(hour): %d", sizeof(hour));
    }
	
    return self;
}

- (void)awakeFromNib {
	NSLog(@"awake from nib");
	[self setIsEnable:YES];
	columnNum = 25;
	rowNum = 8;
	
	mode = 1;
}

// -----------------------------------
// Release the View
// -----------------------------------
- (void)dealloc {
	
	NSLog(@"ScheduleView dealloc");
    
    // call super
    [super dealloc];  
}

// -----------------------------------
// Draw the View Content
// -----------------------------------
- (void)drawRect:(CGRect)rect {
	CGPoint p1, p2;
	CGRect rtmp;
	int i = 0, j = 0;
	float gridWidth, gridHeight;
	p1.x = 0.0;
	p1.y = 10.0;
	p2.x = 40.0;
	p2.y = 10.0;

	CGRect r = self.bounds;
	UIBezierPath *thePath;
	thePath = [UIBezierPath bezierPathWithRect:r];
	
	/*draw background*/
	[[UIColor whiteColor] set];
	[thePath fill];
	
	/*draw rec around*/
	UIColor *theColor = [UIColor blackColor];
	[theColor set];
	[thePath setLineWidth:1];
	[thePath stroke];

	gridWidth = r.size.width/columnNum;
	gridHeight = r.size.height/rowNum;	
	
	UIBezierPath *tPath = nil;
#if 1
	/*draw content*/
	for(i = 0; i < 7; i++)
	{
		for(j = 0; j < 24; j++)
		{
			if(i != 6)
			{
				if(j != 23)
				{
					rtmp.origin.x = (j+2)*gridWidth;
				}
				else
				{
					rtmp.origin.x = gridWidth;
				}

				rtmp.origin.y = r.size.height - (i+2)*gridHeight - gridHeight;
				//rtmp.origin.y = r.size.height - (i+2)*gridHeight;
				rtmp.size.width = gridWidth;
				rtmp.size.height = gridHeight;
			}
			else
			{
				if(j != 23)
				{
					rtmp.origin.x = (j+2)*gridWidth;
				}
				else
				{
					rtmp.origin.x = gridWidth;
				}				
				//rtmp.origin.x = (j+1)*gridWidth;
				rtmp.origin.y = r.size.height - 2*gridHeight;
				rtmp.size.width = gridWidth;
				rtmp.size.height = gridHeight;
			}

			if(hour[i][j] == 2)
			{
				[[UIColor greenColor] set];
			}
			else if(hour[i][j] == 1)
			{
				[[UIColor redColor] set];
			}
			else
			{
				[[UIColor whiteColor] set];
			}
			//[UIBezierPath bezierPathWithRect:rtmp];
			tPath = [UIBezierPath bezierPathWithRect:rtmp];
			[tPath fill];
		}
	}
#endif
	
	/*draw selected region*/
	[[UIColor blueColor] set];
	tPath = [UIBezierPath bezierPathWithRect:selectedRect];
	[tPath fill];
	
	/*draw info*/
	[[UIColor grayColor] set];
	CGRect tRect;
	tRect.origin.x = 0;
	tRect.origin.y = 0;
	tRect.size.width = gridWidth;
	tRect.size.height = r.size.height;
	tPath = [UIBezierPath bezierPathWithRect:tRect];
	[tPath fill];
	
	/**/
	[[UIColor blackColor] set];
	UIFont *font = [UIFont fontWithName:@"Arial" size:18];
	for(i = 0; i < rowNum; i++)
	{
		rtmp.origin.x = 0;
		rtmp.origin.y = i*gridHeight;
		rtmp.size.width = gridWidth;
		rtmp.size.height = gridHeight;
		
		if(i == 1)
		{
			NSString *theStr = @"S";
			CGPoint thePos = CGPointMake(rtmp.origin.x + 3, rtmp.origin.y + 3);
			[theStr drawAtPoint:thePos withFont:font];
		}
		if(i == 2)
		{
			NSString *theStr = @"M";
			CGPoint thePos = CGPointMake(rtmp.origin.x + 3, rtmp.origin.y + 3);
			[theStr drawAtPoint:thePos withFont:font];
		}
		if(i == 3)
		{
			NSString *theStr = @"T";
			CGPoint thePos = CGPointMake(rtmp.origin.x + 3, rtmp.origin.y + 3);
			[theStr drawAtPoint:thePos withFont:font];
		}
		if(i == 4)
		{
			NSString *theStr = @"W";
			CGPoint thePos = CGPointMake(rtmp.origin.x + 3, rtmp.origin.y + 3);
			[theStr drawAtPoint:thePos withFont:font];			
		}
		if(i == 5)
		{
			NSString *theStr = @"T";
			CGPoint thePos = CGPointMake(rtmp.origin.x + 3, rtmp.origin.y + 3);
			[theStr drawAtPoint:thePos withFont:font];			
		}
		if(i == 6)
		{
			NSString *theStr = @"F";
			CGPoint thePos = CGPointMake(rtmp.origin.x + 3, rtmp.origin.y + 3);
			[theStr drawAtPoint:thePos withFont:font];			
		}
		if(i == 7)
		{
			NSString *theStr = @"S";
			CGPoint thePos = CGPointMake(rtmp.origin.x + 3, rtmp.origin.y + 3);
			[theStr drawAtPoint:thePos withFont:font];			
		}
	}
	
	/**/
	[[UIColor grayColor] set];
	tRect.origin.x = 0;
	tRect.origin.y = 0;
	tRect.size.width = r.size.width;
	tRect.size.height = gridHeight;
	tPath = [UIBezierPath bezierPathWithRect:tRect];
	[tPath fill];
	
	[[UIColor blackColor] set];
	for(i = 1; i < columnNum; i++)
	{
		rtmp.origin.x = i*gridWidth;
		rtmp.origin.y = 0;
		rtmp.size.width = gridWidth;
		rtmp.size.height = gridHeight;
		
		NSString *theStr = [[NSString alloc] initWithFormat:@"%d", (i - 1)];
		CGPoint thePos = CGPointMake(rtmp.origin.x + 1, rtmp.origin.y + 1);
		[theStr drawAtPoint:thePos withFont:font];
		[theStr release];
	}

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


// -----------------------------------
// Handle Mouse Events 
// -----------------------------------
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"Phase: Touches begin");
	float gridWidth, gridHeight;
	CGRect r = self.bounds;
	int column, row;
	
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
	NSLog(@"column: %d, row: %d", column, row);
	if((row == 0) || (column == 0))
	{
		NSLog(@"not in grid area");
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

- (void)dispatchFirstTouchAtPoint:(CGPoint)touchPoint forEvent:(UIEvent *)event {
	NSLog(@"Phase: dispatchFirstTouchAtPoint");
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"Phase: touchesMoved");
	
	float gridWidth, gridHeight;
	CGRect r = self.bounds;
	int column, row;
	
	gridWidth = r.size.width/columnNum;
	gridHeight = r.size.height/rowNum;
	
	UITouch *touch = [[touches allObjects] objectAtIndex:0];
	CGPoint touchPoint = [touch locationInView:self];
	NSLog(@"x: %lf, y: %lf", touchPoint.x, touchPoint.y);
	
	if(dragging == YES)
	{
		column = (int)(touchPoint.x/gridWidth);
		row = (int)(touchPoint.y/gridHeight);
		NSLog(@"colume: %d, row: %d", column, row);
		if((row < 0) || (row > rowNum - 1) || (column <= 0) || (column > columnNum - 1))
		{
			NSLog(@"not in grid area");
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

		[self setNeedsDisplay];
	}
	
}

- (void)dispatchTouchEvent:(UIView *)theView toPosition:(CGPoint)position {
	NSLog(@"Phase: dispatchTouchEvent");
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"Phase: touchesEnded");
	int i, j;
	float gridWidth, gridHeight;
	int selColumn, selRow;
	int baseColumn, baseRow;
	CGRect r = self.bounds;
	
	UITouch *touch = [[touches allObjects] objectAtIndex:0];
	CGPoint touchPoint = [touch locationInView:self];
	NSLog(@"x: %lf, y: %lf", touchPoint.x, touchPoint.y);
	
	if(dragging == YES)
	{
		gridWidth = r.size.width/columnNum;
		gridHeight = r.size.height/rowNum;

		selColumn = selectedRect.size.width/(int)gridWidth;
		selRow = selectedRect.size.height/(int)gridHeight;
		printf("selColumn: %d, selRow: %d \n", selColumn, selRow);
		if(selColumn > columnNum - 1)
		{
			selColumn = columnNum - 1;
		}
		if(selRow > rowNum - 1)
		{
			selRow = rowNum - 1;
		}

		baseColumn = (int)((selectedRect.origin.x + 4)/gridWidth);
		baseRow = (int)((selectedRect.origin.y + selectedRect.size.height - gridHeight + 4)/gridHeight);
		//baseRow = (int)((selectedRect.origin.y - gridHeight + 4)/gridHeight);
		baseRow = rowNum - baseRow - 1;
		//baseRow = baseRow + 1;
		printf("baseColumn: %d, baseRow: %d \n", baseColumn, baseRow);
		for(i = baseRow - 1; i < baseRow + selRow - 1; i++)
		{
			for(j = baseColumn - 1; j < selColumn + baseColumn - 1; j++)
			{
				NSLog(@"row: %d, column: %d", i, j);
				if(i != 0 && i != -1)
				{
					if(j != 0)
					{
						hour[i-1][j - 1] = mode;
					}
					else
					{
						hour[i-1][23] = mode;
					}

				}
				else if(i == 0)
				{
					if(j != 0)
					{
						hour[6][j - 1] = mode;
					}
					else
					{
						hour[6][23] = mode;
					}
				}
			}
		}
		selectedRect.origin.x = 0;
		selectedRect.origin.y = 0;
		selectedRect.size.width = 0;
		selectedRect.size.height = 0;

		[self setNeedsDisplay];
	}
	
	dragging = NO;
}

- (void)dispatchTouchEndEvent:(UIView *)theView toPosition:(CGPoint)position {
	NSLog(@"Phase: dispatchTouchEndEvent");
	
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"Phase: touchesCancelled");
	
}

- (void)setMotionRecMode {
	mode = 1;
	
}

- (void)setNormalRecMode {
	mode = 2;
	
}

- (void)setNoRecordMode {
	mode = 0;
	
}

- (void)setSchedule:(unsigned char *)sche {
	NSLog(@"set schedule");
	
	memcpy((void*)hour, sche, 7*24);
	
	[self setNeedsDisplay];
}

- (unsigned char*)getSchedule {
	return (unsigned char*)hour;
}

@end
