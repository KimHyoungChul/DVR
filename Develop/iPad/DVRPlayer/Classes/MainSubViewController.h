//
//  MainSubViewController.h
//  DVRPlayer
//
//  Created by Kelvin on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DVRPlayerAppDelegate.h"


@interface MainSubViewController : UIViewController {
	UITableView			*_regionTableView;
	NSArray				*_regionArray;
	NSInteger			_currentRow;
	
	DVRPlayerAppDelegate *appDelegate;
}

@property (nonatomic, retain) IBOutlet UITableView		*regionTableView;
@property (nonatomic, retain) NSArray					*regionArray;
@property (nonatomic, assign) NSInteger					currentRow;

@property (nonatomic, assign) DVRPlayerAppDelegate   *appDelegate;
-(void)markAddressCell:(NSInteger)cellRow;

@end
