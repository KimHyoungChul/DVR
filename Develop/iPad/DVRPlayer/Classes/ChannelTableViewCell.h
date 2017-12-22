#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface ChannelTableViewCell : UITableViewCell
{
    BOOL useDarkBackground;

    UIImage *icon;
    NSString *publisher;
    NSString *name;
    float rating;
    NSInteger numRatings;
    NSString *price;
}

@property BOOL useDarkBackground;

@property(retain) UIImage *icon;
@property(retain) NSString *publisher;
@property(retain) NSString *name;
@property float rating;
@property NSInteger numRatings;
@property(retain) NSString *price;

@end
