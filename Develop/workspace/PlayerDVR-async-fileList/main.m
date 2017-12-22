//
//  main.m
//  PlayerDVR
//
//  Created by Kelvin on 2/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PlayerDVRApplication : NSApplication
@end

@implementation PlayerDVRApplication

- (void)terminate:(id)sender
{
	NSLog(@"PlayerDVRApplication terminate");
	//[super terminate];
}

@end


int main(int argc, char *argv[])
{
    return NSApplicationMain(argc,  (const char **) argv);
}
