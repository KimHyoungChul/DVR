//
//  DVRPlayerViewController.h
//  DVRPlayer
//
//  Created by Kelvin on 5/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
@class MainViewController;

@interface DVRPlayerViewController : UIViewController {
	IBOutlet UITextField *ipAddress;
	IBOutlet UITextField *mediaPort;
	IBOutlet UITextField *password;
	IBOutlet UIButton *loginBtn;
	UIImageView *backgroundView;
	
}

@property (nonatomic, retain) UIImageView *backgroundView;

- (IBAction)loginHandler:(id)sender;
- (void)setBackgroundImage;

@end

