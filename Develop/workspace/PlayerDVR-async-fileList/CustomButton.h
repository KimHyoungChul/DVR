//
//  CustomButton.h
//  PlayerDVR
//
//  Created by Kelvin on 3/3/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface CustomButton : NSButton {
	int btnStatus; /*0: normal 1: pressed 2:clicked*/
}

- (int)getBtnStatus;
@end
