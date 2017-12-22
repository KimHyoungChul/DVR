    //
//  MainViewController.m
//  DVRPlayer
//
//  Created by Kelvin on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "MainViewController.h"
#import "DVRPlayerAppDelegate.h"
#import "DVRPlayerViewController.h"


@implementation MainViewController
@synthesize contentView;
@synthesize mainView;
@synthesize playbackView;
@synthesize settingView;

@synthesize currentPopover;
@synthesize regionSelectionViewController;

 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
/*
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization.
    }
    return self;
}
*/

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	NSLog(@"MainViewController viewDidLoad");
	[self.contentView addSubview:self.mainView];
	MainSubViewController *regionViewController = [[MainSubViewController alloc] init];
	self.regionSelectionViewController = regionViewController;
	settingPopup = NO;
	viewIndex = 0;
	[regionViewController release];
    [super viewDidLoad];
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
	if(interfaceOrientation == UIInterfaceOrientationLandscapeLeft)
	{
		NSLog(@"UIInterfaceOrientationLandscapeLeft");
	}
	else if(interfaceOrientation == UIInterfaceOrientationLandscapeRight)
	{
		NSLog(@"UIInterfaceOrientationLandscapeRight");
	}
	else if(interfaceOrientation == UIInterfaceOrientationPortrait)
	{
		NSLog(@"UIInterfaceOrientationPortrait");
		return NO;
	}
	else if(interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown)
	{
		NSLog(@"UIInterfaceOrientationPortraitUpsideDown");
		return NO;
	}
    return YES;
}


- (void)didReceiveMemoryWarning {
	NSLog(@"MainViewController didReceiveMemoryWarning");
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}


- (void)viewDidUnload {
	NSLog(@"MainViewController viewDidUnload");
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (IBAction)backHandler:(id)sender {
	NSLog(@"MainViewController backHandler");
	DVRPlayerAppDelegate *appDelegate = (DVRPlayerAppDelegate*)[[UIApplication sharedApplication] delegate];
	[appDelegate.mainViewController.view removeFromSuperview];
	[appDelegate.window addSubview:appDelegate.viewController.view];
	[appDelegate.window makeKeyAndVisible];
}

- (IBAction)playbackSwitch:(id)sender {
	NSLog(@"MainViewController playbackSwitch");
	if(viewIndex == 0)
	{
		/*switch playback view*/
		viewIndex = 1;
	}
	else if(viewIndex == 1 || viewIndex == 2)	/*1: playback; 2: setting*/
	{
		/*switch main view*/
		viewIndex = 0;
	}
	[self setCurrentViewByIndex:viewIndex];
}

- (IBAction)displayRegionSelection:(id)sender {
	NSLog(@"MainViewController displayRegionSelection");
    UIBarButtonItem *button = (UIBarButtonItem *)sender;
    
	if(settingPopup == NO)
	{
		NSArray *array = [[NSArray alloc] initWithObjects:@"Record",@"Alarm", @"Email", @"PTZ", @"Network", @"Manage", @"Info", nil];
		self.regionSelectionViewController.regionArray = array;
		[array release];
		[self setupNewPopOverControllerViewController:self.regionSelectionViewController];
		self.currentPopover.popoverContentSize = CGSizeMake(200,400);

		[self.currentPopover presentPopoverFromBarButtonItem:button permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
		settingPopup = YES;
	}
	else
	{
		if(self.currentPopover != nil)
		{
			[self.currentPopover dismissPopoverAnimated:YES];
			self.currentPopover = nil;
		}
		settingPopup = NO;
	}

}

- (void)setupNewPopOverControllerViewController:(UIViewController  *)vc {
	if (self.currentPopover != nil)
	{
		[self.currentPopover dismissPopoverAnimated:YES];
		self.currentPopover = nil;
	}
	UIPopoverController *popOver = [[UIPopoverController alloc] initWithContentViewController:vc];
	self.currentPopover = popOver;
	[popOver release];
}

-(void)setCurrentView:(UIView *)view {
    if (self.contentView.subviews !=nil)
	{
		for(UIView *subView in self.contentView.subviews)
		{
			[subView removeFromSuperview];
		}
	}
    [self.contentView addSubview:view];
}

- (void)setCurrentViewByIndex:(NSInteger)index {
    switch (index)
	{
		case 0:
			[self setCurrentView:self.mainView];
			barPlayBtn.title = @"Playback";
			break;
        case 1:
			[self setCurrentView:self.playbackView];
			barPlayBtn.title = @"Preview";
            break;
        case 2:
            [self setCurrentView:self.settingView];
			barPlayBtn.title = @"Preview";
			viewIndex = 2;
			//[settingControl setSegmentedIndex:3];
			break;
        default:
            break;
    }
}

- (void)dealloc {
	NSLog(@"MainViewController dealloc");
	[barPlayBtn release];
	[contentView release];
	[mainView release];
	[playbackView release];
	[settingView release];
	[settingControl release];
	
	[currentPopover release];
	[regionSelectionViewController release];
    [super dealloc];
}

@end

@implementation SettingViewControler
@synthesize contentView;

- (IBAction) segmentedHandle:(id)sender {
	NSLog(@"SettingViewControler segmentedHandle");
	NSLog(@"SettingViewControler %d", [segmentedControl selectedSegmentIndex]);
	NSInteger idx = [segmentedControl selectedSegmentIndex];
	if(idx == 0)
	{
		testStatus.text = @"Record";
		[self setCurrentView:settingRecordView];
	}
	else if(idx == 1)
	{
		testStatus.text = @"Alarm";
		[self setCurrentView:settingAlarmView];
	}
	else if(idx == 2)
	{
		testStatus.text = @"Email";
		[self setCurrentView:settingEmailView];
	}
	else if(idx == 3)
	{
		testStatus.text = @"PTZ";
		[self setCurrentView:settingPTZView];
	}
	else if(idx == 4)
	{
		testStatus.text = @"Network";
		[self setCurrentView:settingNetworkView];
	}
	else if(idx == 5)
	{
		testStatus.text = @"Manage";
		[self setCurrentView:settingManageView];
	}
	else if(idx == 6)
	{
		testStatus.text = @"Info";
		[self setCurrentView:settingInfoView];
	}
}

- (void) setSegmentedIndex:(NSInteger)index {
	NSLog(@"SettingViewControler setSegmentedIndex");
	[segmentedControl setSelectedSegmentIndex:index];
	
}

- (void)setCurrentView:(UIView *)view {
    if (self.contentView.subviews !=nil)
	{
		for(UIView *subView in self.contentView.subviews)
		{
			[subView removeFromSuperview];
		}
	}
    [self.contentView addSubview:view];
}

- (void)dealloc {
	NSLog(@"SettingViewControler dealloc");
	
	[segmentedControl release];
	[testStatus release];
	[settingRecordView release];
	[settingAlarmView release];
	[settingPTZView release];
	[settingEmailView release];
	[settingNetworkView release];
	[settingManageView release];
	[settingInfoView release];
	[contentView release];
	
	[super dealloc];
}

@end
