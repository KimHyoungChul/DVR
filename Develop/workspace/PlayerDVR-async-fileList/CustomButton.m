//
//  CustomButton.m
//  PlayerDVR
//
//  Created by Kelvin on 3/3/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "CustomButton.h"


@implementation CustomButton

- (void)mouseDown:(NSEvent *)theEvent {
	if([self isEnabled])
	{
		NSLog(@"custom button mouse down");
		[self highlight:YES];
		btnStatus = 1;/*pressed*/
		[self sendAction:self.action to:self.target];
	}
	
}

- (void)mouseUp:(NSEvent *)theEvent {
	if([self isEnabled])
	{
		NSLog(@"custom button mouse up");
		[self highlight:NO];
		btnStatus = 2;/*clicked*/
		//[self doSomethingSignificant];
		[self sendAction:self.action to:self.target];
	}
}

- (int)getBtnStatus {
	return btnStatus;
}


@end
