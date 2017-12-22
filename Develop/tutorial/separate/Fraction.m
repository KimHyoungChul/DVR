#import "Fraction.h"
#import <stdio.h>

@implementation Fraction;
-(void) print
{
	printf("%i/%i", numerator, denominator);
}

-(void) setNumerator: (int) n
{
	numerator = n;
}

-(void) setDenominator: (int) d
{
	denominator = d;
}

-(int) numerator
{
	return numerator;
}

-(int) denominator
{
	return denominator;
}

-(double) convertToNum
{
	if(denominator != 0)
		return (double)numerator/denominator;
	else
		return 1.0;
}
@end
