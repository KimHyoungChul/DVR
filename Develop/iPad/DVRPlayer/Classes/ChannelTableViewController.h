#import <UIKit/UIKit.h>
#import "ChannelCellView.h"

@interface ChannelTableViewController : UIViewController
{
	ChannelTableViewCell *tmpCell;
    NSArray *data;
	UITableView	*tableView;
	
	// referring to our xib-based UITableViewCell ('IndividualSubviewsBasedApplicationCell')
	UINib *cellNib;
}

@property (nonatomic, retain) IBOutlet UITableView *tableView;
@property (nonatomic, retain) IBOutlet ChannelTableViewCell *tmpCell;
@property (nonatomic, retain) NSArray *data;

@property (nonatomic, retain) UINib *cellNib;

@end
