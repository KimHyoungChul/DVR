//
//  DVRPlayerViewController.m
//  DVRPlayer
//
//  Created by Kelvin on 5/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "DVRPlayerViewController.h"
#import "DVRPlayerAppDelegate.h"
#import "MainViewController.h"

@implementation DVRPlayerViewController

@synthesize backgroundView;
/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	backgroundView = nil;
	[self setBackgroundImage];
    [super viewDidLoad];
}


- (void)setBackgroundImage {
	NSLog(@"set backgroud image");
	UIImageView *bg = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"background2.png"]];
	self.backgroundView = bg;
	[bg release];
	
	self.backgroundView.frame = self.view.bounds;
	[self.view addSubview:backgroundView];
	[self.view sendSubviewToBack:backgroundView];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	if(interfaceOrientation == UIInterfaceOrientationLandscapeLeft)
	{
		NSLog(@"UIInterfaceOrientationLandscapeLeft");
		return NO;
	}
	else if(interfaceOrientation == UIInterfaceOrientationLandscapeRight)
	{
		NSLog(@"UIInterfaceOrientationLandscapeRight");
		return NO;
	}
	else if(interfaceOrientation == UIInterfaceOrientationPortrait)
	{
		NSLog(@"UIInterfaceOrientationPortrait");
	}
	else if(interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown)
	{
		NSLog(@"UIInterfaceOrientationPortraitUpsideDown");
	}
	
    return YES;
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (BOOL)textFieldShouldReturn:(UITextField *)theTextField {
	if(theTextField == ipAddress)
	{
		[mediaPort becomeFirstResponder];
	}
	else if(theTextField == mediaPort)
	{
		[password becomeFirstResponder];
	}
	else if(theTextField == password)
	{
		[password resignFirstResponder];
		[loginBtn becomeFirstResponder];
	}

	return YES;
}

- (IBAction)loginHandler:(id)sender {
	NSLog(@"Login button clicked");
	DVRPlayerAppDelegate *appDelegate = (DVRPlayerAppDelegate*)[[UIApplication sharedApplication] delegate];
	[appDelegate.viewController.view removeFromSuperview];
	if(appDelegate.mainViewController == nil)
	{
		appDelegate.mainViewController = [[MainViewController alloc] initWithNibName:@"MainViewController" bundle:nil];
	}
	[appDelegate.window addSubview:appDelegate.mainViewController.view];
	[appDelegate.window makeKeyAndVisible];
}

- (void)dealloc {
	NSLog(@"dealloc");
	[ipAddress release];
	[mediaPort release];
	[password release];
	[loginBtn release];
    [super dealloc];
}

@end
