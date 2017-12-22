//
//  KtEditFormatter.h
//  DVRPlayer
//
//  Created by Kelvin on 1/10/12.
//  Copyright 2012 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface KtEditFormatter : NSFormatter {
	int maxLength;
}

- (void)setMaximumLength:(int)len;
- (int)maximumLength;

@end
