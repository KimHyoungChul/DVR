//
//  PlayerDVRAppDelegate.m
//  PlayerDVR
//
//  Created by Kelvin on 2/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "PlayerDVRAppDelegate.h"
#import "DvrManage.h"
#include "kt_core_handler.h"
#include "kt_dvr_manage.h"

DvrManage *getDvrManage();

@implementation PlayerDVRAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application 
	NSLog(@"application launching finish");
//	kt_core_fwInit();
//	kt_dvrMgr_init();
//	kt_dvrMgr_loadDvrList();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
	return YES;
}

- (void)applicationWillTerminate:(NSNotification *)notification {
	// perform any final cleanup before the application terminate
	NSLog(@"application will terminate");
	//DvrManage *DvrMgr = nil;
	//NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	//[previewWin close];
	//[previewWin release];
	//[videoPrev release];
	//[pool release];
	
	//DvrMgr = getDvrManage();
	//[DvrMgr release];
	
    MAC_DVS_Cleanup();
	kt_language_close();
	
	kt_dvrMgr_fini();
	kt_core_fwFini();
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
	//
	NSLog(@"application terminate");
	return NSTerminateNow;
}

- (BOOL)windowShouldClose:(id)window{
	NSLog(@"window should close");
	NSAlert* alert = [[NSAlert alloc] init];
	[alert setAlertStyle:NSInformationalAlertStyle];
	[alert setMessageText:@"Are you sure you want to quit?"];
	[alert addButtonWithTitle:@"Yes"];
	[alert addButtonWithTitle:@"No"];
	NSInteger result = [alert runModal];
	if(result == NSAlertFirstButtonReturn)
	{
		[alert release];
		return YES;
	}
	[alert release];
	return NO;
}

@end
