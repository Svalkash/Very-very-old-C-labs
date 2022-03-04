#include <stdio.h>
#define N 100 //max array size

int transform(int a, int *c);
int getint(int *add);
int getarray(int *A, int size);
void sort(int *A, int *LA, int size);

int getnat(int *add)
{
	int n;
	do
	{
		n = scanf_s("%d", add);
		if (n == 0)
		{
			printf("ERROR: Enter natural number.\n");
			scanf_s("%*c[^\n]");
		}
		if (*add <= 0)
		{
			printf("ERROR: Enter natural number.\n");
			n = 0;
		}
	} while (n == 0);
	return n < 0 ? 0 : 1;
}

int getint(int *add)
{
	int n;
	do
	{
		n = scanf_s("%d", add);
		if (n == 0)
		{
			printf("ERROR: Enter integer.\n");
			scanf_s("%*c[^\n]");
		}
	} while (n == 0);
	return n < 0 ? 0 : 1;
}

int transform(int a, int *c, int x, int y)
{
	int b = 0, t = 0, k = 1;
	while (a != 0) //transforming
	{
		t = a % 10;
		if (t == x) t = y;
			else if (t == y) t = x;
		b = b + (k*t);
		k = k * 10;
		a = a / 10;
	}
	a = b;
	*c = 0; //counting
	while (a != 0)
	{
		*c++;
		a = a / 10;
	}
	return(b);
}

int getarray(int *A, int size)
{
	int i=0;
	int conta = 1;
	printf("Enter array elements.\n");
	for (i=0; (i<size)&&(conta==1); i++)
	{
		printf("Enter element #%d:", i);
		conta = getnat(&A[i]);
	}
	return conta == 0 ? 0 : 1;
}

void sort(int *A, int *LA, int size)
{
	int i = 0;
	int b = 0;
	int t = 0;
	do
	{
		b = 1;
		for (i = 0; i < size - 1; i++) if (LA[i] < LA[i + 1])
		{
			t = A[i];
			A[i] = A[i + 1];
			A[i + 1] = t;
			t = LA[i];
			LA[i] = LA[i + 1];
			LA[i + 1] = t;
			b = 0;
		}
	}
	while (b == 0);
}



int main()
{
	int M[N], L[N], R[N];
	int s = 0, cont = 1, i=0, a=0, b=0, dig=1;
	printf("Max array size: %d\n", N);
	printf("Enter array size:");
	do
	{
		cont = getnat(&s);
		if (s > N) printf("ERROR: Array size can't be more than %d!\n", N);
	}
	while ((s>N) && (cont==1));
	if (cont == 1) cont = getarray(M, s);
	if (cont==1)
	{
		printf("Enter 2 digits you want to swap:");
		do
		{
			cont=getint(&a);
			cont=getint(&b);
			dig=0;
			if ((a<0) || (b<0) || (a>9) || (b>9))
			{
				dig=1;
				printf("ERROR: Enter digits.");
			}
		}
		while ((cont==1) && (dig==1));
	}
	if (cont == 1)
	{
		for (i = 0; i < s; i++) R[i] = transform(M[i], &L[i], a, b);
		sort(M, L, s);
		printf("Input array:\n");
		for (i = 0; i < s; i++) printf("#%d | %d\n", i, M[i]);
		printf("Resultant array:\n");
		for (i = 0; i < s; i++) printf("#%d | %d\n", i, R[i]);
	}
	printf("Enter 0 to close.\n");
	scanf_s("%*d");
	return 0;
}