//
//  DvrManage.m
//  PlayerDVR
//
//  Created by Kelvin on 3/14/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "DvrManage.h"


@implementation DvrManage

- (id)init {
	NSLog(@"DvrManage init");
	self = [super init];
	
	if(self)
	{
		curDvr = NULL;
		DvrList = NULL;
	}
	
	return self;
}

- (void)dealloc {
	NSLog(@"DvrManage dealloc");
	DvrConfig *cfg;
	
	cfg = DvrList;
	while(cfg)
	{
		//free(cfg);
		cfg = cfg->next;
	}
	[super dealloc];
}

- (BOOL)setCurDvr:(int)idx {
	DvrConfig *cfg;
	int count = 0;
	int flag = 0;
	
	cfg = DvrList;
	while(cfg)
	{
		if(count == idx)
		{
			curDvr = cfg;
			flag = 1;
		}
		cfg = cfg->next;
		count++;
	}
	
	if(flag == 1)
	{
		return YES;
	}
	
	return NO;
}

- (DvrConfig*)getCurDvr {
	return curDvr;
	
}

- (BOOL)addDvr:(DvrConfig*)dvr {
	
	if(DvrList == NULL)
	{
		DvrList = dvr;
		curDvr = dvr;
	}
	
	return YES;
}

- (BOOL)removeAllDvr {
	DvrConfig *cfg;
	
	cfg = DvrList;
	while(cfg)
	{
		free(cfg);
		cfg = cfg->next;
	}
	
	return YES;
}

- (BOOL)removeDvr:(DvrConfig*)dvr {
	
	return YES;
}

- (BOOL)removeDvrByName:(char*)name {
	
	return YES;
}

- (BOOL)removeDvrByIdx:(int)idx {
	
	return YES;
}

@end
