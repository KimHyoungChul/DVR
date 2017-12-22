    //
//  MainSubViewController.m
//  DVRPlayer
//
//  Created by Kelvin on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "MainSubViewController.h"
#import "MainViewController.h"


@implementation MainSubViewController

@synthesize regionTableView = _regionTableView;
@synthesize currentRow = _currentRow;
@synthesize regionArray = _regionArray;

@synthesize appDelegate;

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
    NSLog(@"MainSubViewController viewDidLoad");
    [super viewDidLoad];
    self.appDelegate = (DVRPlayerAppDelegate *)[[UIApplication sharedApplication] delegate];
}

-(void)viewWillAppear:(BOOL)animated {
	NSLog(@"MainSubViewController viewWillAppear");
	[super viewWillAppear:animated];
	[self.regionTableView reloadData];
}


//- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
//    // Overriden to allow any orientation.
//    return YES;
//}


- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}


- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	NSLog(@"MainSubViewController numberOfSectionsInTableView");
    return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    NSLog(@"MainSubViewController tableView:numberOfRowsInSection:");
	return [self.regionArray count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
	NSLog(@"MainSubViewController tableView:cellForRowAtIndexPath:");
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
	cell.textLabel.text = [self.regionArray objectAtIndex:indexPath.row];
	if (indexPath.row == _currentRow) {
		cell.accessoryType = UITableViewCellAccessoryCheckmark;
	}else {
		cell.accessoryType = UITableViewCellAccessoryNone;
	}
    
    return cell;
}
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	NSLog(@"MainSubViewController tableView:didSelectRowAtIndexPath: %d", indexPath.row);

	MainViewController *viewController = self.appDelegate.mainViewController;
	[viewController.currentPopover dismissPopoverAnimated:YES];
	[viewController setCurrentViewByIndex:2];
	
}
-(void)markAddressCell:(NSInteger)cellRow {
	NSLog(@"MainSubViewController markAddressCell");
	if (cellRow == -1) {
		UITableViewCell *Cell = [self.regionTableView 
								 cellForRowAtIndexPath:[NSIndexPath indexPathForRow:_currentRow inSection:0]];
		Cell.accessoryType = UITableViewCellAccessoryNone;
		return;
	}
	UITableViewCell *oldCell = [self.regionTableView 
								cellForRowAtIndexPath:[NSIndexPath indexPathForRow:_currentRow inSection:0]];
	oldCell.accessoryType = UITableViewCellAccessoryNone;
	
	UITableViewCell *newCell = [self.regionTableView 
								cellForRowAtIndexPath:[NSIndexPath indexPathForRow:cellRow inSection:0]];
	newCell.accessoryType = UITableViewCellAccessoryCheckmark;
	
	_currentRow = cellRow;
}


- (void)dealloc {
	NSLog(@"MainSubViewController dealloc");
	[_regionTableView release];
    [_regionArray release];
    [super dealloc];
}


@end
