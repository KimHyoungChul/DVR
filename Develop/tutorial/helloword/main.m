#import <Foundation/NSString.h>
#import <stdio.h>

int main(int argc, char *argv[])
{
	NSString *aa = @"Hello World!";
	printf("Hello world! \n");
	printf("Length of aa is: %i \n", [aa length]);
	return 0;
}
