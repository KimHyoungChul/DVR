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
@synthesize type;

- (id)init {
	self = [super init];
	if(self)
	{
		name = nil;
		size = nil;
		channel = nil;
		path = nil;
		status = nil;
		type = nil;
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
	[type release];
	name = nil;
	size = nil;
	channel = nil;
	path = nil;
	status = nil;
	type = nil;
	
	[super dealloc];
}

@end
