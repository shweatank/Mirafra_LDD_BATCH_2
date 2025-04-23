#include <stdio.h>

typedef union {
	int   a;
	float b;
} sample;

main()
{
	sample u;
	void f(sample *);
	
	u.a = 20;
	u.b = 1.6;
	printf("%d, %f\n", u.a, u.b);
	
	f(&u);
	printf("%d %f\n", u.a, u.b);
}

void f(sample *x)
{
	x->a = 500;
	printf("%d %f\n", x->a, x->b);
	return;
}
