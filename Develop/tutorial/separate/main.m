#import "Fraction.h"
#import <stdio.h>

int main(int argc, char *argv[])
{
	Fraction *myFraction = [[Fraction alloc] init];

	//set fraction to 1/3
	[myFraction setNumerator: 1];
	[myFraction setDenominator: 3];

	//display the fraction
	printf("The value of myFraction is: ");
	[myFraction print];
	printf("\n");
	[myFraction free];

	return 0;
}
