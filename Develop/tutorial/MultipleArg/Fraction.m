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

-(void) setTo: (int) n over: (int) d
{
	numerator = n;
	denominator = d;
}

-(void) reduce
{
	int u = numerator;
	int v = denominator;
	int temp;

	while(v != 0)
	{
		temp = u % v;
		u = v;
		v = temp;
	}

	numerator /= u;
	denominator /= u;
}

//-(void) add: (Fraction *) f
//{
//	//To add two fractions:
//	//a/b + c/d = ((a*d) + (b*c)) / (b * d)
//
//	numerator = (numerator * [f denominator]) + 
//		(denominator * [f numerator]);
//	denominator = denominator * [f denominator];
//
//	[self reduce];
//}

-(Fraction *) add: (Fraction *) f
{
	//To add two fractions:
	//a/b + c/d = ((a*d) + (b*c)) / (b * d)

	//result will store the result of the addition
	Fraction *result = [[Fraction alloc] init];
	int resultNum, resultDenom;

	resultNum = (numerator * [f denominator]) + 
		(denominator * [f numerator]);
	resultDenom = denominator * [f denominator];

	[result setTo: resultNum over: resultDenom];
	[result reduce];

	return result;
}
@end
