#import <objc/Object.h>

//Define the Fraction class
@interface Fraction : Object
{
	int numerator;
	int denominator;
}

-(void) print;
-(void) setNumerator: (int) n;
-(void) setDenominator: (int) d;
-(void) setTo: (int) n over: (int) d;
-(void) reduce;
//-(void) add: (Fraction *) f;
-(int) numerator;
-(int) denominator;
-(double) convertToNum;
-(Fraction *) add: (Fraction *) f;
@end
