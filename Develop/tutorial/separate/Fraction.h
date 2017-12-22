#import <objc/Object.h>

//The Fraction class
@interface Fraction: Object
{
	int numerator;
	int denominator;
}

-(void) print;
-(void) setNumerator: (int) n;
-(void) setDenominator: (int) d;
-(int) numerator;
-(int) denominator;
-(double) convertToNum;

@end
