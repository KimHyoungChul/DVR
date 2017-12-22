//
//  Login.h
//  PlayerDVR
//
//  Created by Kelvin on 2/28/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface Login : NSObject {
	IBOutlet NSTextField *userName;
	IBOutlet NSTextField *password;
	IBOutlet NSTextField *IPAddress;
	IBOutlet NSTextField *netPort;
	IBOutlet NSButton	 *rememberMe;
	IBOutlet NSButton	 *signedIn;
	
	IBOutlet NSButton	 *offLineBtn;
	
	IBOutlet NSComboBox  *networkType;
	IBOutlet NSComboBox	 *language;
	IBOutlet NSWindow *loginWin;
	
	IBOutlet NSTextField *labelIPAddress;
	IBOutlet NSTextField *labelMediaPort;
	IBOutlet NSTextField *labelPassword;
	IBOutlet NSTextField *labelNetwork;
	IBOutlet NSTextField *labelLanguage;
	
	IBOutlet NSTextField *notify;
	
	IBOutlet NSWindowController *add_window;
}

- (IBAction)signedIn:(id)sender;
- (IBAction)offLineClicked:(id)sender;


- (IBAction)changeLanguage:(id)sender;
- (void)InitLanguage;

@end
