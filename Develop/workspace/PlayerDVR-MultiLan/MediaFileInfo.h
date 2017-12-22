//
//  FileInfo.h
//  DVRPlayer
//
//  Created by Kelvin on 3/30/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MediaFileInfo : NSObject {
	NSString *name;
	NSString *size;
	NSString *channel;
	NSString *path;
	NSString *status;
}

@property (readwrite, copy) NSString *name;
@property (readwrite, copy) NSString *size;
@property (readwrite, copy) NSString *channel;
@property (readwrite, copy) NSString *path;
@property (readwrite, copy) NSString *status;

@end
