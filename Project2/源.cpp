#include <stdio.h>
#include<stdlib.h>
int main()
{
	int **pointer;
	int *p[3];
	int a = 1;
	int b = 2;
	int c = 3;
	p[0] = &a;
	p[1] = &b;
	p[2] = &c;

	pointer = p;

	printf("%d\n", *(*(pointer + 2)));
	system("pause");
	return 0;
}
