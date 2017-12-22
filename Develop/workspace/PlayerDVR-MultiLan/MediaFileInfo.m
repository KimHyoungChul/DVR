//
//  FileInfo.m
//  DVRPlayer
//
//  Created by Kelvin on 3/30/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "MediaFileInfo.h"


@implementation MediaFileInfo

@synthesize name;
@synthesize size;
@synthesize channel;
@synthesize path;
@synthesize status;

- (id)init {
	self = [super init];
	if(self)
	{
		name = nil;
		size = nil;
		channel = nil;
		path = nil;
		status = nil;
	}
	
	return self;
}

- (void)dealloc {
	//NSLog(@"MediaFileInfo dealloc");
	[name release];
	[size release];
	[channel release];
	[path release];
	[status release];
	name = nil;
	size = nil;
	channel = nil;
	path = nil;
	status = nil;
	
	[super dealloc];
}

@end
