#import "classB.h"

int main(int argc, char *argv[])
{
	ClassB *b = [[ClassB alloc] init];	

	[b initVar];
	[b printVar];

	[b free];

	return 0;
}
