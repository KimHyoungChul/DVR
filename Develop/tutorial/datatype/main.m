#import <stdio.h>

int main(int argc, char *argv[])
{
	int integerVar = 100;
	float floatingVar = 331.79;
	double doubleVar = 8.44e+11;
	char charVar = 'W';

	printf("integerVar = %i \n", integerVar);
	printf("floatingVar = %f \n", floatingVar);
	printf("doubleVar = %e \n", doubleVar);
	printf("doubleVar = %g \n", doubleVar);
	printf("charVar = %c \n", charVar);

	return 0;
}
