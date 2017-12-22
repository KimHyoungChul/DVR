//
//  MainViewController.h
//  DVRPlayer
//
//  Created by Kelvin on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MainSubViewController.h"
#import "ChannelTableViewController.h"

@interface SettingViewControler : NSObject
{
	IBOutlet UISegmentedControl *segmentedControl;
	IBOutlet UILabel *testStatus;
	UIView *contentView;
	
	IBOutlet UIView *settingRecordView;
	IBOutlet UIView *settingAlarmView;
	IBOutlet UIView *settingEmailView;
	IBOutlet UIView *settingPTZView;
	IBOutlet UIView *settingNetworkView;
	IBOutlet UIView *settingManageView;
	IBOutlet UIView *settingInfoView;
}
@property (nonatomic, retain) IBOutlet UIView *contentView;

- (IBAction) segmentedHandle:(id)sender;
- (void) setSegmentedIndex:(NSInteger)index;
- (void)setCurrentView:(UIView *)view;

@end

@interface MainViewController : UIViewController {
	IBOutlet UIBarButtonItem *barPlayBtn;
	IBOutlet ChannelTableViewController *channelView;
	IBOutlet SettingViewControler *settingControl;
	
	UIView *contentView;
	UIView *mainView;
	UIView *playbackView;
	UIView *settingView;

	BOOL settingPopup;
	NSInteger viewIndex;	/*0: mainView; 1: playbackView; 2: settingView*/
	UIPopoverController	*currentPopover;
	MainSubViewController *regionSelectionViewController;
}
@property (nonatomic, retain) IBOutlet UIView *contentView;
@property (nonatomic, retain) IBOutlet UIView *mainView;
@property (nonatomic, retain) IBOutlet UIView *playbackView;
@property (nonatomic, retain) IBOutlet UIView *settingView;

@property (nonatomic, retain) UIPopoverController *currentPopover;
@property (nonatomic, retain) MainSubViewController *regionSelectionViewController;

- (IBAction)playbackSwitch:(id)sender;
- (IBAction)backHandler:(id)sender;
- (IBAction)displayRegionSelection:(id)sender;

- (void)setupNewPopOverControllerViewController:(UIViewController  *)vc;
-(void)setCurrentView:(UIView *)view;
- (void)setCurrentViewByIndex:(NSInteger)index;

@end



