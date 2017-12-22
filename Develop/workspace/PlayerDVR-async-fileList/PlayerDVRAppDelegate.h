//
//  PlayerDVRAppDelegate.h
//  PlayerDVR
//
//  Created by Kelvin on 2/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PlayerDVRAppDelegate : NSObject <NSApplicationDelegate> {
	NSWindow *window;
	//IBOutlet NSWindow *previewWin;
	//IBOutlet NSView *videoPrev;
}

@property (assign) IBOutlet NSWindow *window;

@end
