//
//  DVRPlayerAppDelegate.h
//  DVRPlayer
//
//  Created by Kelvin on 5/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DVRPlayerViewController;
@class MainViewController;

@interface DVRPlayerAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    DVRPlayerViewController *viewController;
	MainViewController *mainViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet DVRPlayerViewController *viewController;
@property (nonatomic, retain) IBOutlet MainViewController *mainViewController;

@end

