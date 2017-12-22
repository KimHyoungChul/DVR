//
//  VideoPreview.m
//  PlayerDVR
//
//  Created by Kelvin on 2/28/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "VideoPreview.h"
#import "DvrManage.h"
#include "kt_util.h"
#include "kt_core_config.h"

/**/
DvrManage* getDvrManage();
void dumpDvrConfig(DvrConfig *cfg);

#define SURFACE_WIDTH	708.0
#define SURFACE_HEIGHT	580.0
#define SURFACE_BORDER	1.0

#define CHANNEL_NUM		4

@implementation VideoPreview

- (id)initWithFrame:(NSRect)frame {
	DvrConfig *cfg;
    if (self = [super initWithFrame:frame])
	{
		NSLog(@"initWithFrame");
		lastFocus = 0;

		cfg = kt_dvrMgr_getCurDvr();
		dumpDvrConfig(cfg);
		gridType = 3;
		channelNum = CHANNEL_NUM;
		gridDrawFlag = 0;
		channelNum = cfg->channelNum;
		if(cfg->channelNum == 4)
		{
			gridType = FOUR_GRID;
		}
		else if(cfg->channelNum == 8)
		{
			gridType = NINE_GRID;
		}
		else if(cfg->channelNum == 16)
		{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
			gridType = FOUR_GRID;
#else
			gridType = SISTEEN_GRID;
#endif /*KT_DEBASE_CHANNEL*/
		}
		
		if(channelNum == 16)
		{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
			group = GROUP_ONE;
#else
			group = GROUP_NONE;
#endif /*KT_DEBASE_CHANNEL*/
		}
		else
		{
			group = GROUP_NONE;
		}

	
		itemArray = [[NSMutableArray alloc] initWithCapacity:16];
	}
    return self;
}

- (void)initAllChannel {
	int i = 0, j = 0;
	int tWidth, tHeight;
	CGRect r;
	DvrConfig *cfg;
	VideoItem *item;

	cfg = kt_dvrMgr_getCurDvr();
	
	tWidth = SURFACE_WIDTH/SISTEEN_GRID;
	tHeight = SURFACE_HEIGHT/SISTEEN_GRID;
	if(((int)SURFACE_WIDTH % SISTEEN_GRID) != 0)
	{
		tWidth = (SURFACE_WIDTH + (SISTEEN_GRID -  ((int)SURFACE_WIDTH % SISTEEN_GRID)))/SISTEEN_GRID;
	}
	if(((int)SURFACE_HEIGHT % SISTEEN_GRID) != 0)
	{
		tHeight = (SURFACE_HEIGHT + (SISTEEN_GRID - ((int)SURFACE_HEIGHT % SISTEEN_GRID)))/SISTEEN_GRID;
	}
	NSLog(@"tWidth: %d, tHeight: %d", tWidth, tHeight);
	
	for(i = 0; i < SISTEEN_GRID; i++)
	{
		for(j = 0; j < SISTEEN_GRID; j++)
		{
			r.origin.x = tWidth*j;
			r.origin.y = SURFACE_HEIGHT - tHeight*(i+1);
			r.size.width = tWidth;
			r.size.height = tHeight;
			
			printf("x: %f, y: %f \n", (r.origin.x), (r.origin.y));
			item = [[VideoItem alloc] initWithRect:r atParent:self];
			[item setChannel:1 andServer:"test"];
			[item createStreamPlaybackWithChannel:i*SISTEEN_GRID + j];
			NSLog(@"User Name: %s, Password: %d", cfg->userName, cfg->password);
			[item resetUsrName:(char*)cfg->userName andPwd:(char*)cfg->password];
			[item resetNetServer:(char*)cfg->serverAddr andPort:cfg->port andType:cfg->netType];
			if(i == 0 && j == 0)
			{
				[item setFocus:YES];
			}
			else
			{
				[item setFocus:NO];
			}
			[itemArray addObject:item];
		}
	}
}

- (void)finiAllChannel {
	[itemArray removeAllObjects];
}

- (void)getItemWidth:(int*)width andHeight:(int*)height {
	int tWidth, tHeight;

	tWidth = SURFACE_WIDTH/gridType;
	tHeight = SURFACE_HEIGHT/gridType;
	if(((int)SURFACE_WIDTH % gridType) != 0)
	{
		tWidth = (SURFACE_WIDTH + (gridType -  ((int)SURFACE_WIDTH % gridType)))/gridType;
	}
	if(((int)SURFACE_HEIGHT % gridType) != 0)
	{
		tHeight = (SURFACE_HEIGHT + (gridType - ((int)SURFACE_HEIGHT % gridType)))/gridType;
	}

	*width = tWidth;
	*height = tHeight;
	
	NSLog(@"width: %d, height: %d", *width, *height);
}

- (void)stopAllOtherChannel {
	NSLog(@"Stop All Other Channel, gridType: %d", gridType);
	int idx;
	int i = 0, j = 0;
	VideoItem *item;
	
	for(i = 0; i < SISTEEN_GRID; i++)
	{
		for(j = 0; j < SISTEEN_GRID; j++)
		{
			idx = i * SISTEEN_GRID + j;
			if(idx >= 16)
			{
				break;
			}
			item = [itemArray objectAtIndex:idx];
			if(idx >= channelNum)
			{
				if([item getStartFlag])
				{
					[item stopPlayback];
				}
			}
			else// if(idx >= gridType*gridType)
			{
				/*make sure that recording can't stop*/
				if(group != GROUP_NONE)
				{
					if(channelNum != 16)
					{
						if(idx >= group*gridType*gridType || idx < (group-1)*gridType*gridType)
						{
							if([item getStartFlag])
							{
								if([item isRecording] == NO)
								{
									[item stopPlayback];
								}
							}
						}
					}
					else
					{
						if(gridType != NINE_GRID)
						{
							if(idx >= group*gridType*gridType || idx < (group-1)*gridType*gridType)
							{
								if([item getStartFlag])
								{
									if([item isRecording] == NO)
									{
										[item stopPlayback];
									}
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
									if(channelNum == 16)
									{
										[item stopPlayback];
										[self stopRecordAllChannel];
									}
#endif /*KT_DEBASE_CHANNEL*/
								}
							}
						}
						else
						{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1) /*lan*/
							/*8channel each grid*/
							if(group == GROUP_ONE)
							{
								if(idx > group*gridType*gridType -2 && idx < channelNum)
								{
									if([item getStartFlag])
									{
										if([item isRecording] == NO)
										{
											[item stopPlayback];
										}
									}
								}
							}
							else
							{
								if(idx < (group-1)*gridType*gridType - 1)
								{
									if([item getStartFlag])
									{
										if([item isRecording] == NO)
										{
											[item stopPlayback];
										}
									}
								}
							}
#else
							if(group == GROUP_ONE)
							{
								if(idx >= group*gridType*gridType && idx < channelNum)
								{
									if([item getStartFlag])
									{
										if([item isRecording] == NO)
										{
											[item stopPlayback];
										}
									}
								}
							}
							else {
								if(idx < 7)
								{
									if([item getStartFlag])
									{
										if([item isRecording] == NO)
										{
											[item stopPlayback];
										}
									}
								}
							}
#endif /*KT_DEBASE_CHANNEL*/
						}

					}

				}
			}
		}
	}
	
}

- (void)startAllChannel {
	NSLog(@"Start All Channel, gridType: %d", gridType);
	int idx;
	int i = 0, j = 0;
	VideoItem *item;
	int baseIdx = 0;
	
	if(group != GROUP_NONE)
	{
		if(channelNum != 16)
		{
			baseIdx = (group - 1)*gridType*gridType;
		}
		else
		{
			if(gridType == NINE_GRID)
			{
				if(group == GROUP_TWO)
				{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
					baseIdx = (group - 1)*gridType*gridType - 1;
#else
					baseIdx = (group - 1)*gridType*gridType - 2;
#endif /*KT_DEBASE_CHANNEL*/
				}
				else
				{
					baseIdx = (group - 1)*gridType*gridType;
				}

			}
			else
			{
				baseIdx = (group - 1)*gridType*gridType;
			}

		}

	}
	
	for(i = 0; i < gridType; i++)
	{
		for(j = 0; j < gridType; j++)
		{
			idx = baseIdx + i*gridType + j;
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
			if(idx >= channelNum)
			{
				continue;
			}
			else if(channelNum == 16 && gridType == NINE_GRID && i == 2 && j == 2)
			{
				continue;
			}
#else
			if(idx >= channelNum)
			{
				continue;
			}
#endif /*KT_DEBASE_CHANNEL*/
			item = [itemArray objectAtIndex:idx];
			if([item getStartFlag] == NO)
			{
				[item startPlayback];
			}
		}
	}
}

- (void)setFocusItem {
	int i = 0;
	VideoItem *item = NULL;
	
	for(i = 0; i < 16; i++)
	{
		item = [itemArray objectAtIndex:i];
		if(i == lastFocus)
		{
			[item setFocus:YES];
		}
		else
		{
			[item setFocus:NO];
		}
	}
}

- (void)awakeFromNib {
	NSLog(@"awake from nib");
	
	[self initAllChannel];
		
	/*start all available channel*/
	[self resetAllChannel];
	
	/*start timer*/
	animationTimer = nil;
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/60.0) target:self selector:@selector(ontimer) userInfo:nil repeats:YES];
}

- (void)focusFullScreen {
	int i, j, idx;
	VideoItem *item;
	int tWidth, tHeight;
	CGRect r;
	
	gridType = ONE_GRID;
	[self getItemWidth:&tWidth andHeight:&tHeight];
	r.origin.x = 0.0;
	r.origin.y = 0.0;
	r.size.width = tWidth;
	r.size.height = tHeight;
	
	for(i = 0; i < SISTEEN_GRID; i++)
	{
		for(j = 0; j < SISTEEN_GRID; j++)
		{
			NSLog(@"i: %d, j: %d", i, j);
			idx = i * SISTEEN_GRID + j;
			item = [itemArray objectAtIndex:idx];
			if(idx == lastFocus)
			{
				[item resetRect:r];
				if([item getStartFlag] == NO)
				{
					[item startPlayback];
					[item setFocus:YES];
				}
			}
			else {
				if(idx < channelNum)
				{
					if([item isRecording] == NO)
					{
						[item stopPlayback];
						[item setFocus:NO];
					}
				}
				else
				{
					[item stopPlayback];
					[item setFocus:NO];
				}
			}
		}
	}
	
	[self setNeedsDisplay:YES];
}

- (void)focusCapture {
	int i, j, idx;
	VideoItem *item;
	
	for(i = 0; i < SISTEEN_GRID; i++)
	{
		for(j = 0; j < SISTEEN_GRID; j++)
		{
			idx = i * SISTEEN_GRID + j;
			item = [itemArray objectAtIndex:idx];
			if([item getFocus] == YES)
			{
				NSLog(@"set need capture");
				[item setNeedCapture];
			}
			else
			{
				NSLog(@"not this item");
			}

		}
	}
	
}

- (void)focusPtzUp {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzUp];
	}
}

- (void)focusPtzDown {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzDown];
	}
}

- (void)focusPtzLeft {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzLeft];
	}
}

- (void)focusPtzRight {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzRight];
	}
}

- (void)focusPtzZoomin {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzZoomin];
	}
}

- (void)focusPtzZoomout {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzZoomout];
	}
}

- (void)focusPtzFocusFar {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzFocusFar];
	}
}

- (void)focusPtzFocusNear {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzFocusNear];
	}
}

- (void)focusPtzIRISOpen {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzIRISOpen];
	}
}

- (void)focusPtzIRISClose {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzIRISClose];
	}
}

- (void)focusPtzStop {
	VideoItem *item;
	item = [itemArray objectAtIndex:lastFocus];
	if(item)
	{
		[item ptzStop];
	}
}

- (void)recordAllChannel {
	int i, j, idx;
	int baseIdx = 0;
	VideoItem *item;
	
	if(group != GROUP_NONE)
	{
		baseIdx = (group - 1)*gridType*gridType;
	}
	
	if(gridType != ONE_GRID)
	{
		for(i = 0; i < gridType; i++)
		{
			for(j = 0; j < gridType; j++)
			{
				idx = baseIdx + i * gridType +j;
				item = [itemArray objectAtIndex:idx];
				[item startRecord];
			}
		}
	}
	else
	{
		item = [itemArray objectAtIndex:lastFocus];
		[item startRecord];
	}

}

- (void)stopRecordAllChannel {
	int i, j, idx;
	VideoItem *item;
	for(i = 0; i < SISTEEN_GRID; i++)
	{
		for(j = 0; j < SISTEEN_GRID; j++)
		{
			idx = i * SISTEEN_GRID + j;
			item = [itemArray objectAtIndex:idx];
			[item stopRecord];
		}
	}
}

- (void)disableDrawAllChannel {
	int i, j, idx;
	VideoItem *item;
	
	for(i = 0; i < SISTEEN_GRID; i++)
	{
		for(j = 0; j < SISTEEN_GRID; j++)
		{
			idx = i * SISTEEN_GRID + j;
			item = [itemArray objectAtIndex:idx];
			[item disableScreenDraw];
		}
	}
	[self setNeedsDisplay:YES];
}

- (void)enableDrawAllChannel {
	int i, j, idx;
	VideoItem *item;
	
	for(i = 0; i < SISTEEN_GRID; i++)
	{
		for(j = 0; j < SISTEEN_GRID; j++)
		{
			idx = i * SISTEEN_GRID + j;
			item = [itemArray objectAtIndex:idx];
			[item enableScreenDraw];
		}
	}
}

- (void)resetAllChannel {
	int i, j, idx;
	int baseIdx = 0;
	VideoItem *item;
	int tWidth, tHeight;
	CGRect r;
	
	if(group != GROUP_NONE)
	{
		//baseIdx = (group - 1)*gridType*gridType;
		if(channelNum != 16)
		{
			baseIdx = (group - 1)*gridType*gridType;
		}
		else
		{
			if(gridType == NINE_GRID)
			{
				if(group == GROUP_TWO)
				{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
					baseIdx = (group - 1)*gridType*gridType - 1;
#else
					baseIdx = (group - 1)*gridType*gridType - 2;
#endif /*KT_DEBASE_CHANNEL*/
				}
				else
				{
					baseIdx = (group - 1)*gridType*gridType;
				}
				
			}
			else {
				baseIdx = (group - 1)*gridType*gridType;
			}
			
		}
	}

	if(gridType != ONE_GRID)
	{
		[self getItemWidth:&tWidth andHeight:&tHeight];

		for(i = 0; i < gridType; i++)
		{
			for(j = 0; j < gridType; j++)
			{
				r.origin.x = tWidth*j;
				r.origin.y = SURFACE_HEIGHT - tHeight*(i+1);
				r.size.width = tWidth;
				r.size.height = tHeight;
				if(i == 2 && gridType == NINE_GRID)
				{
					r.size.height = SURFACE_HEIGHT - i * tHeight;
					r.origin.y = 0.0;
				}
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
				if(gridType == NINE_GRID && channelNum == 16 && i == 2 && j == 2)
				{
					break;
				}
#endif /*KT_DEBASE_CHANNEL*/

				printf("x: %f, y: %f \n", (r.origin.x), (r.origin.y));
				idx = baseIdx + i * gridType + j;
				//if(idx >= channelNum)
				//	break;
				item = [itemArray objectAtIndex:idx];
				[item resetRect:r];
			}
		}

		[self startAllChannel];
		[self stopAllOtherChannel];

		[self setNeedsDisplay:YES];
	}
	else
	{
		[self focusFullScreen];
	}

}

- (void)ontimer {
	int i, j, idx;
	int baseIdx = 0;
	VideoItem *item;
	

	i = 0;j = 0;idx = 0;
	
	if(group != GROUP_NONE)
	{
		//baseIdx = (group - 1)*gridType*gridType;
		if(channelNum != 16)
		{
			baseIdx = (group - 1)*gridType*gridType;
		}
		else
		{
			if(gridType == NINE_GRID)
			{
				if(group == GROUP_TWO)
				{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
					baseIdx = (group - 1)*gridType*gridType - 1;
#else
					baseIdx = (group - 1)*gridType*gridType - 2;
#endif /*KT_DEBASE_CHANNEL*/
				}
				else
				{
					baseIdx = (group - 1)*gridType*gridType;
				}
				
			}
			else {
				baseIdx = (group - 1)*gridType*gridType;
			}
			
		}
	}

	[self lockFocus];	
	if(gridType != ONE_GRID)
	{
		for(i = 0; i < gridType; i++)
		{
			for(j = 0; j < gridType; j++)
			{
				if(i == 2 && j == 2 && gridType == NINE_GRID && channelNum <= 8)
				{
					break;
				}
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
				if(gridType == NINE_GRID && channelNum == 16 && i == 2 && j == 2)
				{
					break;
				}
#endif /*KT_DEBASE_CHANNEL*/
			
				idx = baseIdx + i * gridType + j;
				if(idx < channelNum)
				{
					item = [itemArray objectAtIndex:idx];
					[item displayYUVFrame];
				}
			}
		}
	}
	else
	{
		/*display spec channel*/
		item = [itemArray objectAtIndex:lastFocus];
		[item displayYUVFrame];
	}
	
	for(i = 0; i < channelNum; i++)
	{
		if(gridType != ONE_GRID)
		{
			if(i < baseIdx || i >= (baseIdx + gridType*gridType))
			{
				item = [itemArray objectAtIndex:i];
				if([item isRecording] == YES && [item getStartFlag] == YES)
				{
					[item dropYUVFrame];
				}
			}
		}
		else {
			if(i != lastFocus)
			{
				item = [itemArray objectAtIndex:i];
				if([item isRecording] == YES && [item getStartFlag] == YES)
				{
					[item dropYUVFrame];
				}
			}
		}

	}

	CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];	
	[self unlockFocus];

	CGContextFlush(context);
}

- (void)setGridType:(GridType)type {
	if(gridType == type)
	{
		if(gridType == ONE_GRID)
		{
			lastFocus++;
			if(lastFocus < channelNum)
			{
				[self setFocusItem];
			
			}
			else
			{
				lastFocus = 0;
				[self setFocusItem];
			}
		}
		
		group = group + 1;
		if(group*gridType*gridType > channelNum && group > 2 && channelNum == 16)
		{
			NSLog(@"xxxxxxxxxxxxxxxxxxxx>>>>>>>>>>>");
			group = GROUP_ONE;
		}
		else if(group*gridType*gridType > channelNum && channelNum != 16 && gridType != NINE_GRID)
		{
			group = GROUP_ONE;
		}
		else if(group*gridType*gridType > channelNum)
		{
			group = GROUP_ONE;
		}

		[self resetAllChannel];
		return;
	}
	
	gridType = type;
	if(channelNum > gridType*gridType)
	{
		group = GROUP_ONE;
	}
	else
	{
		group = GROUP_NONE;
	}

	[self resetAllChannel];
}

- (void)drawRect:(NSRect)rect {
	NSLog(@"drawRect");
	
	int i, j, idx;
	int baseIdx = 0;
	VideoItem *item;
	
	if(group != GROUP_NONE)
	{
		//baseIdx = (group - 1)*gridType*gridType;
		if(channelNum != 16)
		{
			baseIdx = (group - 1)*gridType*gridType;
		}
		else
		{
			if(gridType == NINE_GRID)
			{
				if(group == GROUP_TWO)
				{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
					baseIdx = (group - 1)*gridType*gridType - 1;
#else
					baseIdx = (group - 1)*gridType*gridType - 2;
#endif /*KT_DEBASE_CHANNEL*/
				}
				else
				{
					baseIdx = (group - 1)*gridType*gridType;
				}
				
			}
			else {
				baseIdx = (group - 1)*gridType*gridType;
			}
			
		}
	}
	
	if(gridType != ONE_GRID)
	{
		//if(gridDrawFlag == 0)
		{
			for(i = 0; i < gridType; i++)
			{
				for(j = 0; j < gridType; j++)
				{
					idx = baseIdx + i * gridType + j;
					NSLog(@"idx: %d", idx);
					//if(idx >= channelNum)
					//	break;
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
					if(gridType == NINE_GRID && channelNum == 16 && i == 2 && j == 2)
					{
						break;
					}
#endif /*KT_DEBASE_CHANNEL*/
					item = [itemArray objectAtIndex:idx];
					//if([item getFocus])
					//{
					//	[item drawBorder];
					//}
					[item drawBorder];
				}
			}
			gridDrawFlag = 1;
		}
	
		for(i = 0; i < gridType; i++)
		{
			for(j = 0; j < gridType; j++)
			{
				idx = baseIdx + i * gridType + j;
				if(idx >= channelNum)
					break;
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
				if(gridType == NINE_GRID && channelNum == 16 && i == 2 && j == 2)
				{
					break;
				}
#endif /*KT_DEBASE_CHANNEL*/
				item = [itemArray objectAtIndex:idx];
				if([item getFocus])
				{
					[item drawBorder];
				}
			}
		}
	}
	else
	{
		item = [itemArray objectAtIndex:lastFocus];
		[item drawBorder];
	}
}

- (void)start {
	int i, j, idx;
	int baseIdx = 0;
	VideoItem *item;
	NSLog(@"start");
	
	if(group != GROUP_NONE)
	{
		//baseIdx = (group - 1)*gridType*gridType;
		if(channelNum != 16)
		{
			baseIdx = (group - 1)*gridType*gridType;
		}
		else
		{
			if(gridType == NINE_GRID)
			{
				if(group == GROUP_TWO)
				{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
					baseIdx = (group - 1)*gridType*gridType - 1;
#else
					baseIdx = (group - 1)*gridType*gridType - 2;
#endif /*KT_DEBASE_CHANNEL*/
				}
				else
				{
					baseIdx = (group - 1)*gridType*gridType;
				}
				
			}
			else {
				baseIdx = (group - 1)*gridType*gridType;
			}
			
		}
	}

	if(gridType != ONE_GRID)
	{
		for(i = 0; i < gridType; i++)
		{
			for(j = 0; j < gridType; j++)
			{
				if(i == 2 && j == 2 && gridType == NINE_GRID && channelNum <= 8)
				{
					break;
				}
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
				if(gridType == NINE_GRID && channelNum == 16 && i == 2 && j == 2)
				{
					break;
				}
#endif /*KT_DEBASE_CHANNEL*/
				idx = baseIdx + i * gridType + j;
				if(idx < channelNum)
				{
					item = [itemArray objectAtIndex:idx];
					[item startPlayback];
				}
			}
		}
	}
	else
	{
		item = [itemArray objectAtIndex:lastFocus];
		[item startPlayback];
	}

}

- (void)stop {
	int i, j, idx;
	int baseIdx = 0;
	VideoItem *item;
	NSLog(@"stop");
	
	/*stop the timer*/
	//[animationTimer invalidate];
	
	if(group != GROUP_NONE)
	{
		//baseIdx = (group - 1)*gridType*gridType;
		if(channelNum != 16)
		{
			baseIdx = (group - 1)*gridType*gridType;
		}
		else
		{
			if(gridType == NINE_GRID)
			{
				if(group == GROUP_TWO)
				{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
					baseIdx = (group - 1)*gridType*gridType - 1;
#else
					baseIdx = (group - 1)*gridType*gridType - 2;
#endif /*KT_DEBASE_CHANNEL*/
				}
				else
				{
					baseIdx = (group - 1)*gridType*gridType;
				}
				
			}
			else {
				baseIdx = (group - 1)*gridType*gridType;
			}
			
		}
	}

	if(gridType != ONE_GRID)
	{
		for(i = 0; i < gridType; i++)
		{
			for(j = 0; j < gridType; j++)
			{
				if(i == 2 && j == 2 && gridType == NINE_GRID && channelNum <= 8)
				{
					break;
				}
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
				if(gridType == NINE_GRID && channelNum == 16 && i == 2 && j == 2)
				{
					break;
				}
#endif /*KT_DEBASE_CHANNEL*/
				idx = baseIdx + i * gridType + j;
				if(idx < channelNum)
				{
					item = [itemArray objectAtIndex:idx];
					[item stopPlayback];
				}
			}
		}
	}
	else {
		item = [itemArray objectAtIndex:lastFocus];
		[item stopPlayback];
	}

}

- (void)logout {
	int i, j, idx;
	VideoItem *item;
	NSLog(@"logout");
	[animationTimer invalidate];

	/*release all video item*/
	for(i = 0; i < SISTEEN_GRID; i++)
	{
		for(j = 0; j < SISTEEN_GRID; j++)
		{
			idx = i * SISTEEN_GRID + j;
			item = [itemArray objectAtIndex:idx];
			[item destroyStreamPlayback];
		}
	}
}

- (void)mouseDown:(NSEvent *)theEvent {
	int i, j, idx;
	int baseIdx = 0;
	VideoItem *item;
	NSLog(@"mouse down event");
	NSPoint clickLocation;
	
	clickLocation = [self convertPoint:[theEvent locationInWindow] fromView:[[self window] contentView]];
	printf("x: %f, y: %f \n", clickLocation.x, clickLocation.y);
	
	if(group != GROUP_NONE)
	{
		//baseIdx = (group - 1)*gridType*gridType;
		if(channelNum != 16)
		{
			baseIdx = (group - 1)*gridType*gridType;
		}
		else
		{
			if(gridType == NINE_GRID)
			{
				if(group == GROUP_TWO)
				{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
					baseIdx = (group - 1)*gridType*gridType - 1;
#else
					baseIdx = (group - 1)*gridType*gridType - 2;
#endif /*KT_DEBASE_CHANNEL*/
				}
				else
				{
					baseIdx = (group - 1)*gridType*gridType;
				}
				
			}
			else {
				baseIdx = (group - 1)*gridType*gridType;
			}
			
		}
	}
	
	if(gridType != ONE_GRID)
	{
		for(i = 0; i < gridType; i++)
		{
			for(j = 0; j < gridType; j++)
			{
				idx = baseIdx + i * gridType + j;
				item = [itemArray objectAtIndex:idx];
				
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
				if(gridType == NINE_GRID && channelNum == 16 && i == 2 && j == 2)
				{
					break;
				}
#endif /*KT_DEBASE_CHANNEL*/
			
				if(idx < channelNum)
				{
					if([item pointInside:clickLocation] == YES)
					{
						NSLog(@"index: %d", idx);
						//if(idx == lastFocus)
						[item setFocus:YES];
						if(idx == lastFocus)
						{
							continue;
							//return;
							//[self setNeedsDisplay:YES];
						}
						else
						{
							[self setNeedsDisplay:YES];
							lastFocus = idx;
							[item setFocus:YES];
							//return;
						}
					}
					else
					{
						[item setFocus:NO];
					}
				}
			}
		}
	
		[self setFocusItem];
	}
	
	if([theEvent clickCount] > 1)
	{
		NSLog(@"double click ???");
		if(gridType == ONE_GRID)
		{
			if(channelNum == 4)
			{
				gridType = FOUR_GRID;
			}
			else if(channelNum == 8)
			{
				gridType = NINE_GRID;
			}
			else if(channelNum == 16)
			{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
				gridType = FOUR_GRID;
#else
				gridType = SISTEEN_GRID;
#endif /*KT_DEBASE_CHANNEL*/			
			}

			if(channelNum == 16)
			{
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
				group = GROUP_ONE;
#else
				group = GROUP_NONE;
#endif /*KT_DEBASE_CHANNEL*/
			}
			else
			{
				group = GROUP_NONE;
			}
			
			//group = GROUP_NONE;
			
			[self resetAllChannel];
		}
		else
		{
			[self focusFullScreen];
		}
	}

}

- (void)mouseUp:(NSEvent *)theEvent {
	NSLog(@"mouse up event");
}

- (void)dealloc {
	/*release here*/
	NSLog(@"------------>---------------->VideoPreview dealloc");
	
	[self logout];
	[self finiAllChannel];
	
	[super dealloc];
}

@end

@implementation VideoItem

- (VideoItem *)initWithRect:(CGRect)r atParent:(NSView*)parent{
	if(self = [super init])
	{
		/*set rect*/
		rect.origin.x = r.origin.x;
		rect.origin.y = r.origin.y;
		rect.size.width = r.size.width;
		rect.size.height = r.size.height;
		
		/*init*/
		channel = 0;
		startFlag = NO;
		memset(serverName, 0x0, sizeof(serverName));
		playback = NULL;
		
		reqCapture = NO;
		reqRecord = NO;
		doDraw = YES;
		
	}
	
	view = parent;
	focusFlag = NO;
	return self;
}

- (void)setNeedCapture {
	NSLog(@" set capture");
	reqCapture = YES;
}

- (void)setChannel:(int)ch andServer:(char*)name {
	channel = ch;
	memcpy(serverName, name, strlen(name));
}

- (void)startRecord {
	NSLog(@"start Record");
	if(playback)
	{
		if(reqRecord == NO)
		{
			stream_playback_startRecord(playback);
			reqRecord = YES;
		}
	}
}

- (void)stopRecord {
	NSLog(@"stop Record");
	if(playback)
	{
		if(reqRecord == YES)
		{
			stream_playback_stopRecord(playback);
			reqRecord = NO;
		}
	}
	
}

- (void)disableScreenDraw {
	doDraw = NO;
}

- (void)enableScreenDraw {
	doDraw = YES;
}

- (int)createStreamPlaybackWithChannel:(unsigned int)ch {
	playback = stream_playback_init(STREAM_LIVE);
	if(playback != NULL)
	{
		stream_playback_setup(playback, ch);
	}
	else
	{
		return -1;
	}
	channel = ch;
	
	stream_playback_setDstSize(playback, (int)(rect.size.width) - 1, (int)(rect.size.height) - 1);
	
	return 0;
}

- (int)destroyStreamPlayback {
	if(playback)
	{
		stream_playback_fini(playback);
		playback = NULL;
	}		
	
	return 0;
}

- (int)resetUsrName:(char*)name andPwd:(char*)pwd {
	NSLog(@"reset user name");
	if(name == NULL || pwd == NULL)
	{
		NSLog(@"user name or password is NULL");
	}
	if(playback)
	{
		stream_playback_setUsrInfo(playback, (UPSint8*)name, (UPSint8*)pwd);		
	}
	
	return 0;
}

- (int)resetNetServer:(char*)addr andPort:(int)port andType:(unsigned int)type {
	NSLog(@"reset network server");
	if(addr == NULL)
	{
		NSLog(@"server address is NULL");
	}
	if(playback)
	{
		stream_playback_setNetInfo(playback, (UPSint8*)addr, port, type);		
	}
	
	return 0;
}

- (int)resetRect:(CGRect)r {
	if(rect.size.width == r.size.width && rect.size.height == r.size.height)
	{
		rect.origin.x = r.origin.x;
		rect.origin.y = r.origin.y;
		
		/*just move to elsewhere*/
		return 0;
	}
	
	/*set rect*/
	rect.origin.x = r.origin.x;
	rect.origin.y = r.origin.y;
	rect.size.width = r.size.width;
	rect.size.height = r.size.height;
	
	stream_playback_setDstSize(playback, (int)(rect.size.width) - 1, (int)(rect.size.height) - 1);	
	
	return 0;
}

- (int)startPlayback {
	if(playback)
	{
		if(startFlag)
		{
			/*restart this channel*/
			stream_playback_stop(playback);
		}
		stream_playback_start(playback);
		startFlag = YES;
	}	
	
	return 0;
}

- (int)stopPlayback {
	if(playback && startFlag == YES)
	{
		stream_playback_stop(playback);
	}
	startFlag = NO;
	
	return 0;
}

- (void)setFocus:(BOOL)flag {
	focusFlag = flag;
	//[self drawBorder];
}

- (BOOL)getFocus {
	return focusFlag;
}

- (BOOL)getStartFlag {
	return startFlag;
}

- (BOOL)isRecording {
	return reqRecord;
}

- (void)drawBorder {
	CGColorSpaceRef rgbColorspace = CGColorSpaceCreateDeviceRGB();
	CGFloat values[4] = {1.0, 0.0, 0.0, 1.0};
	if(focusFlag == NO)
	{
		values[0] = 1.0;
		values[1] = 1.0;
		values[2] = 1.0;
		//return;
	}

	CGColorRef red = CGColorCreate(rgbColorspace, values);
	CGColorSpaceRelease(rgbColorspace);
	//[view lockFocus];
	/*get current CGCtx*/
	CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	
	/*draw border*/
	CGContextSetLineWidth(context, SURFACE_BORDER);
	CGContextSetStrokeColorWithColor(context, red);
	CGContextSetFillColorWithColor(context, red);
	
	CGContextMoveToPoint(context, rect.origin.x, rect.origin.y);
	CGContextAddLineToPoint(context, rect.origin.x, rect.origin.y + rect.size.height);
	
	CGContextSetLineWidth(context, SURFACE_BORDER);	
	CGContextMoveToPoint(context, rect.origin.x, rect.origin.y);
	CGContextAddLineToPoint(context, rect.origin.x + rect.size.width, rect.origin.y);
	
	CGContextSetLineWidth(context, SURFACE_BORDER);	
	CGContextMoveToPoint(context, rect.origin.x, rect.origin.y + rect.size.height);
	CGContextAddLineToPoint(context, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
	
	CGContextSetLineWidth(context, SURFACE_BORDER);	
	CGContextMoveToPoint(context, rect.origin.x + rect.size.width, rect.origin.y);
	CGContextAddLineToPoint(context, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
	
	CGContextStrokePath(context);	
	CGColorRelease(red);
	//[view unlockFocus];
	
	//CGContextFlush(context);	
}

- (void)drawRecFlag {
	CGRect r;
	CGColorSpaceRef rgbColorspace = CGColorSpaceCreateDeviceRGB();
	CGFloat values[4] = {1.0, 0.0, 0.0, 1.0};
	CGColorRef red = CGColorCreate(rgbColorspace, values);
	CGColorSpaceRelease(rgbColorspace);
	
	CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	CGContextSetFillColorWithColor(context, red);
	//CGContextSetStrokeColorWithColor(context, red);
	CGContextSelectFont(context, "Times", 1.5, kCGEncodingMacRoman);
	
	r.origin.x = rect.origin.x + SURFACE_BORDER + 8;
	r.origin.y = rect.origin.y + SURFACE_BORDER + 8;
	r.size.width = 16;
	r.size.height = 16;
	CGContextFillRect(context, r);
	//CGContextSetTextDrawingMode(context, kCGTextStroke);
	CGContextSetRGBFillColor(context, 1.0, 1.0, 1.0, 1.0);
	CGContextShowTextAtPoint(context, r.origin.x + 2, r.origin.y + 2, "R", 1);
	
	CGColorRelease(red);
	
	
}

- (void)dropYUVFrame {
	RGB_Entry *pEntry = NULL;
	
	pEntry = stream_playback_listPop2(playback);
	if(pEntry == NULL)
	{
		if(stream_playback_getNetworkStatus(playback) == 1)
		{
			NSLog(@"111restart stream playback");
			[self stopPlayback];
			[self startPlayback];
		}
		return;
	}
	if(pEntry != NULL)
	{
		stream_playback_poolPush2(playback, pEntry);
	}
}

- (void)displayYUVFrame {
	RGB_Entry *pEntry = NULL;
	
	pEntry = stream_playback_listPop2(playback);
	if(pEntry == NULL)
	{
		if(stream_playback_getNetworkStatus(playback) == 1)
		{
			NSLog(@"222restart stream playback");
			[self stopPlayback];
			[self startPlayback];
		}		
		return;
	}
	
	if(pEntry != NULL)
	{
		if(pEntry->width == ((int)(rect.size.width) - 1) && pEntry->height == ((int)(rect.size.height) - 1))
		{
			if(doDraw == YES)
			{
				[self displayFrame: pEntry];
			}
		}
		stream_playback_poolPush2(playback, pEntry);
	}

}

- (void)displayFrame:(RGB_Entry *)pEntry {
	RGB_Render *pRender = NULL;
	
	if(pEntry == NULL)
	{
		return ;
	}
	
	pRender = (RGB_Render*)pEntry->data;
	
	//[view lockFocus];

	CGColorSpaceRef rgbColorspace = CGColorSpaceCreateDeviceRGB();	
	CGContextRef srcCtx = CGBitmapContextCreate(pRender->outData, rect.size.width - 1, rect.size.height - 1, 8, 4*(rect.size.width - 1), rgbColorspace, kCGImageAlphaNoneSkipFirst);
	CGColorSpaceRelease(rgbColorspace);
	CGImageRef srcCtx_img = CGBitmapContextCreateImage(srcCtx);

	
	/*screen capture*/
	if(reqCapture == YES)
	{
		char *homePath = getenv("HOME");
		char capPath[128] = {0};
		char capName[128] = {0};
		DvrConfig *dvrCfg = NULL;
		dvrCfg = kt_dvrMgr_getCurDvr();
		sprintf(capPath, "%s/DvrRecord/%s/capture", homePath, dvrCfg->serverAddr);
		//NSString *path = [@"~/Pictures" stringByExpandingTildeInPath];
		
		struct tm *curTime = kt_util_getLocalTime();
		printf("data: %d%d%d time: %d:%d:%d \n", (1900 + curTime->tm_year), (1 + curTime->tm_mon), 
			   curTime->tm_mday, (curTime->tm_hour + 8), curTime->tm_min, curTime->tm_sec);
		//NSString *name = [[NSString alloc] initWithFormat:@"%@/ch%d-%d%02d%02d-%02d%02d%02d.jpeg", path, (channel+1), (1900+curTime->tm_year), 
		//				  (1+curTime->tm_mon), curTime->tm_mday, (curTime->tm_hour+8), curTime->tm_min, curTime->tm_sec];
		//sprintf(capName, "%s/ch%02d-%d%02d%02d-%02d%02d%02d.jpg", capPath, (channel+1), (1900+curTime->tm_year), 
		//				(1+curTime->tm_mon), curTime->tm_mday, (curTime->tm_hour+8), curTime->tm_min, curTime->tm_sec);
		sprintf(capName, "%s/%d%02d%02d%02d%02d%02d.jpg", capPath, (1900+curTime->tm_year), 
				(1+curTime->tm_mon), curTime->tm_mday, (curTime->tm_hour+8), curTime->tm_min, curTime->tm_sec);
		NSString *name = [[NSString alloc] initWithUTF8String:(const char*)capName];
		
		NSSize tSize;
		tSize.width = rect.size.width - 1;
		tSize.height = rect.size.height - 1;
		NSBitmapImageRep *img = [[NSBitmapImageRep alloc] initWithCGImage:srcCtx_img];
		//NSArray *representations = [img representations];
		//NSData *bitmapData = [NSBitmapImageRep representationOfImageRepsInArray:representations usingType:NSJPEGFileType properties:nil];
		NSData *jpegData = [img representationUsingType:NSJPEGFileType properties:nil];
		//[jpegData writeToFile:[@"~/Pictures/test.jpeg" stringByExpandingTildeInPath] atomically: YES];
		[jpegData writeToFile:name atomically:YES];
		//[jpegData release];
		//[name release];
		[img release];
		reqCapture = NO;
		
		NSDictionary *dict = [NSDictionary dictionaryWithObject:name forKey:@"name"];
		
		[[NSNotificationCenter defaultCenter]
		 postNotificationName:@"Capture Finish" 
		 object:nil userInfo:dict];
		
		[name release];
	}

	CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	
	CGRect r;
	r.origin.x = rect.origin.x + SURFACE_BORDER;
	r.origin.y = rect.origin.y + SURFACE_BORDER;
	//r.size.width = rect.size.width - SURFACE_BORDER*2;
	//r.size.height = rect.size.height - SURFACE_BORDER*2;
	r.size.width = rect.size.width - 1;
	r.size.height = rect.size.height - 1;

	CGContextDrawImage(context, r, srcCtx_img);

	CGImageRelease(srcCtx_img);
	CGContextRelease(srcCtx);
	if(reqRecord)
	{
		[self drawRecFlag];
	}

	
	//[view unlockFocus];
	
	//[self drawBorder];
	
	//CGContextFlush(context);
}

- (void)ptzUp {
	if(playback)
	{
		stream_playback_ptzUp(playback);
	}
	
}

- (void)ptzDown {
	if(playback)
	{
		stream_playback_ptzDown(playback);
	}
	
}

- (void)ptzLeft {
	if(playback)
	{
		stream_playback_ptzLeft(playback);
	}
	
}

- (void)ptzRight {
	if(playback)
	{
		stream_playback_ptzRight(playback);
	}
	
}

- (void)ptzZoomin {
	if(playback)
	{
		stream_playback_ptzZoomin(playback);
	}
	
}

- (void)ptzZoomout {
	if(playback)
	{
		stream_playback_ptzZoomout(playback);
	}
	
}

- (void)ptzFocusFar {
	if(playback)
	{
		stream_playback_ptzFocusFar(playback);
	}
	
}

- (void)ptzFocusNear {
	if(playback)
	{
		stream_playback_ptzFocusNear(playback);
	}
	
}

- (void)ptzIRISOpen {
	if(playback)
	{
		stream_playback_ptzIRISOpen(playback);
	}
	
}

- (void)ptzIRISClose {
	if(playback)
	{
		stream_playback_ptzIRISClose(playback);
	}
	
}

- (void)ptzStop {
	if(playback)
	{
		stream_playback_ptzStop(playback);
	}
}

- (BOOL)pointInside:(NSPoint)p {
	NSRect r;
	
	r.origin.x = rect.origin.x;
	r.origin.y = rect.origin.y;
	r.size.width = rect.size.width;
	r.size.height = rect.size.height;
	
	return NSPointInRect(p, r);
}

- (void)dealloc {
	int ret = 0;
	NSLog(@"VideoItem dealloc");
	
	if(playback)
	{
		ret = [self destroyStreamPlayback];
		playback = NULL;
	}
	
	[super dealloc];
}

@end

