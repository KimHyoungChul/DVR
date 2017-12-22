#import <Foundation/Foundation.h>
#import "ChannelTableViewCell.h"

@interface IndividualChannelCell : ChannelTableViewCell
{
    IBOutlet UIImageView *iconView;
    IBOutlet UILabel *publisherLabel;
    IBOutlet UILabel *nameLabel;
    IBOutlet UILabel *numRatingsLabel;
    IBOutlet UILabel *priceLabel;
}

@end
