#import <Foundation/Foundation.h>
#import "ChannelTableViewCell.h"

@interface ChannelCell : ChannelTableViewCell
{
    UIView *cellContentView;
}

@end

@interface ChannelCellContentView : UIView
{
    ChannelTableViewCell *_cell;
    BOOL _highlighted;
}

@end
