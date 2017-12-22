#import <stdio.h>
#import <objc/Object.h>

// @interface section
@interface Fraction: Object
{
	int numerator;
	int denominator;
}

-(void)	print;
-(void)	setNumerator: (int)n;
-(void) setDenominotor: (int)d;
-(int) numerator;
-(int) denominator;

@end

// @implementation section
@implementation Fraction;
-(void) print
{
	printf("%i/%i", numerator, denominator);
}

-(void) setNumerator: (int) n
{
	numerator = n;
}

-(void) setDenominotor: (int) d
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

@end


// program section

int main(int argc, char *argv[])
{
	Fraction *frac1 = [[Fraction alloc] init];
	Fraction *frac2 = [[Fraction alloc] init];

	//Set 1st fraction to 2/3
	[frac1 setNumerator: 2];
	[frac1 setDenominotor:3];

	//Set 2st fraction to 3/7
	[frac2 setNumerator: 3];
	[frac2 setDenominotor: 7];

	//Display the fractions
	printf("First fraction is: ");
	[frac1 print];
	printf("\n");
	printf("Second fraction is: ");
	[frac2 print];
	printf("\n");

	[frac1 free];
	[frac2 free];

	return 0;
}
