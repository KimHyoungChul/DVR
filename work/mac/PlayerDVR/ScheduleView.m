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
- (id)initWithFrame:(NSRect)frame {
	NSLog(@"init With Frame");
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
- (void)drawRect:(NSRect)rect {
    // erase the background by drawing white
	NSPoint p1, p2;
	NSRect rtmp;
	int i = 0, j = 0;
	float gridWidth, gridHeight;
	p1.x = 0.0;
	p1.y = 10.0;
	p2.x = 40.0;
	p2.y = 10.0;
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
				rtmp.origin.y = r.size.height - 2*gridHeight;
				rtmp.size.width = gridWidth;
				rtmp.size.height = gridHeight;
			}

			if(hour[i][j] == 2)
			{
				[[NSColor greenColor] set];
			}
			else if(hour[i][j] == 1)
			{
				[[NSColor redColor] set];
			}
			else
			{
				[[NSColor whiteColor] set];
			}
			[NSBezierPath fillRect:rtmp];
		}
	}
	
	/*draw selected region*/
	[[NSColor blueColor] set];
	[NSBezierPath fillRect:selectedRect];
	
	/*draw info*/
	for(i = 0; i < rowNum; i++)
	{
		[[NSColor grayColor] set];		
		rtmp.origin.x = 0;
		rtmp.origin.y = r.size.height - i*gridHeight - gridHeight;
		rtmp.size.width = gridWidth;
		rtmp.size.height = gridHeight;
		[NSBezierPath fillRect:rtmp];
		[[NSColor blackColor] set];
		if(i == 1)
		{
			NSString *theStr = @"S";
			NSPoint thePos = NSMakePoint(rtmp.origin.x + 1, rtmp.origin.y + 1);
			[theStr drawAtPoint:thePos withAttributes:nil];
		}
		if(i == 2)
		{
			NSString *theStr = @"M";
			NSPoint thePos = NSMakePoint(rtmp.origin.x + 1, rtmp.origin.y + 1);
			[theStr drawAtPoint:thePos withAttributes:nil];
		}
		if(i == 3)
		{
			NSString *theStr = @"T";
			NSPoint thePos = NSMakePoint(rtmp.origin.x + 1, rtmp.origin.y + 1);
			[theStr drawAtPoint:thePos withAttributes:nil];
		}
		if(i == 4)
		{
			NSString *theStr = @"W";
			NSPoint thePos = NSMakePoint(rtmp.origin.x + 1, rtmp.origin.y + 1);
			[theStr drawAtPoint:thePos withAttributes:nil];			
		}
		if(i == 5)
		{
			NSString *theStr = @"T";
			NSPoint thePos = NSMakePoint(rtmp.origin.x + 1, rtmp.origin.y + 1);
			[theStr drawAtPoint:thePos withAttributes:nil];			
		}
		if(i == 6)
		{
			NSString *theStr = @"F";
			NSPoint thePos = NSMakePoint(rtmp.origin.x + 1, rtmp.origin.y + 1);
			[theStr drawAtPoint:thePos withAttributes:nil];			
		}
		if(i == 7)
		{
			NSString *theStr = @"S";
			NSPoint thePos = NSMakePoint(rtmp.origin.x + 1, rtmp.origin.y + 1);
			[theStr drawAtPoint:thePos withAttributes:nil];			
		}
	}
	for(i = 1; i < columnNum; i++)
	{
		[[NSColor grayColor] set];
		rtmp.origin.x = i*gridWidth;
		rtmp.origin.y = rowNum*gridHeight - gridHeight;
		rtmp.size.width = gridWidth;
		rtmp.size.height = gridHeight;
		[NSBezierPath fillRect:rtmp];
		NSString *theStr = [[NSString alloc] initWithFormat:@"%d", (i - 1)];
		NSPoint thePos = NSMakePoint(rtmp.origin.x + 1, rtmp.origin.y + 1);
		[theStr drawAtPoint:thePos withAttributes:nil];
		[theStr release];
	}
	
	/*draw the grids*/
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
    
		// convert the click location into the view coords
		clickLocation = [self convertPoint:[event locationInWindow]
								  fromView:nil];
		NSLog(@"x: %f, y: %f", clickLocation.x, clickLocation.y);
	
		column = (int)(clickLocation.x/gridWidth);
		row = (int)(clickLocation.y/gridHeight);
		printf("column: %d, row: %d \n", column, row);
		if((row == rowNum - 1) || (column == 0))
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
	
		if (dragging)
		{
			NSPoint newDragLocation=[self convertPoint:[event locationInWindow]
											  fromView:nil];
		
			NSLog(@"x: %f, y: %f", newDragLocation.x, newDragLocation.y);
			if(newDragLocation.x < 0 || newDragLocation.y < 0 || newDragLocation.x > r.size.width || newDragLocation.y > r.size.height)
				return;
			column = (int)(newDragLocation.x/gridWidth);
			row = (int)(newDragLocation.y/gridHeight);
			if((row < 0) || (row >= rowNum - 1) || (column <= 0) || (column > columnNum - 1))
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
		
			[self setNeedsDisplay:YES];
		
		}
	}
}

- (void)mouseUp:(NSEvent *)event {
	int i, j;
	float gridWidth, gridHeight;
	int selColumn, selRow;
	int baseColumn, baseRow;//, basePos;
	NSRect r = self.bounds;
	
	if([self isEnable])
	{
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
			baseRow = rowNum - baseRow - 1;
			printf("baseColumn: %d, baseRow: %d \n", baseColumn, baseRow);
			for(i = baseRow - 1; i < baseRow + selRow - 1; i++)
			{
				for(j = baseColumn - 1; j < selColumn + baseColumn - 1; j++)
				{
					NSLog(@"row: %d, column: %d", i, j);
					if(i != 0)
					{
						if(j != 0)
						{
							hour[i-1][j - 1] = mode;
						}
						else {
							hour[i-1][23] = mode;
						}

					}
					else
					{
						if(j != 0)
						{
							hour[6][j - 1] = mode;
						}
						else {
							hour[6][23] = mode;
						}

					}

				}
			}
		
			selectedRect.origin.x = 0;
			selectedRect.origin.y = 0;
			selectedRect.size.width = 0;
			selectedRect.size.height = 0;
		
			[self setNeedsDisplay:YES];
		}
		dragging = NO;
	}
    
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
	
	if([self isEnable])
	{
		NSLog(@"++++++++++++++++++>>>>>>>>>>>>>>>>>>>");
		memcpy((void*)hour, sche, 7*24);
		[self setNeedsDisplay:YES];
	}
}

- (unsigned char*)getSchedule {
	return (unsigned char*)hour;
}

@end
