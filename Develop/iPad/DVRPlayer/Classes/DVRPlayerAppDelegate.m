//
//  DVRPlayerAppDelegate.m
//  DVRPlayer
//
//  Created by Kelvin on 5/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "DVRPlayerAppDelegate.h"
#import "DVRPlayerViewController.h"

@implementation DVRPlayerAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize mainViewController;


#pragma mark -
#pragma mark Application lifecycle

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    NSLog(@"application:didFinishLaunchingWithOptions:");
	
    // Override point for customization after app launch. 
    [self.window addSubview:viewController.view];
    [self.window makeKeyAndVisible];
	mainViewController = nil;

	return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
	NSLog(@"applicationWillResignActive");
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
	NSLog(@"applicationDidBecomeActive");
}


- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
	NSLog(@"applicationWillTerminate");
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
	NSLog(@"applicationDidReceiveMemoryWarning");
}


- (void)dealloc {
	NSLog(@"DVRPlayerAppDelegate dealloc");
	
    [viewController release];
	[mainViewController release];
    [window release];
    [super dealloc];
}


@end
