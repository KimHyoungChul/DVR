//
//  DvrManage.h
//  PlayerDVR
//
//  Created by Kelvin on 3/14/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "kt_dvr_manage.h"


@interface DvrManage : NSObject {

	DvrConfig *curDvr;
	DvrConfig *DvrList;
}

- (id)init;
- (BOOL)setCurDvr:(int)idx;
- (DvrConfig*)getCurDvr;
- (BOOL)addDvr:(DvrConfig*)dvr;
- (BOOL)removeAllDvr;
- (BOOL)removeDvr:(DvrConfig*)dvr;
- (BOOL)removeDvrByName:(char*)name;
- (BOOL)removeDvrByIdx:(int)idx;

@end
