#import "Fraction.h"
#import <stdio.h>

int main(int argc, char *argv[])
{
	Fraction *aFraction = [[Fraction alloc] init];
	Fraction *sum = [[Fraction alloc] init], *sum2;
	int i, n, pow2;

	[sum setTo: 0 over: 1];

	printf("Enter your value for n: ");
	scanf("%i", &n);

	pow2 = 2;
	for(i = 1; i <= n; ++i)
	{
		[aFraction setTo: 1 over: pow2];
		sum2 = [sum add: aFraction];
		[sum free];
		sum = sum2;
		pow2 *= 2;
	}

	printf("After %i iterations, the sum is %g \n", n, [sum convertToNum]);
	[aFraction free];
	[sum free];

	return 0;
}
