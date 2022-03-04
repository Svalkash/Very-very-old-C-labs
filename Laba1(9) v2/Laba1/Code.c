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

int transform(int a, int *c)
{
	int b = 0, t = 0, k = 1;
	while (a != 0) //transforming
	{
		t = a % 10;
		if (t == 0) t = 1;
			else if (t == 1) t = 0;
		b = b + (k*t);
		k = k * 10;
		a = a / 10;
	}
	a = b;
	*c = 0; //counting
	while (a != 0)
	{
		*c = *c + 1;
		a = a / 10;
	}
	return(b);
}

int getarray(int *A, int size)
{
	int conta = 1;
	printf("Enter array elements.\n");
	for (int i = 0; (i<size)&&(conta==1); i++)
	{
		printf("Enter element #%d:", i);
		conta = getnat(&A[i]);
	}
	return conta == 0 ? 0 : 1;
}

void sort(int *A, int *LA, int size)
{
	int b = 0;
	int t = 0;
	do
	{
		b = 1;
		for (int i = 0; i < size - 1; i++) if (LA[i] < LA[i + 1])
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
	int s = 0, cont = 1, i=0;
	printf("Max array size: %d\n", N);
	printf("Enter array size:");
	do
	{
		cont = getnat(&s);
		if (s > N) printf("ERROR: Array size can't be more than %d!\n", N);
	}
	while ((s>N) && (cont==1));
	if (cont == 1) cont = getarray(M, s);
	if (cont == 1)
	{
		for (i = 0; i < s; i++) R[i] = transform(M[i], &L[i]);
		sort(R, L, s);
		printf("Input array:\n");
		for (i = 0; i < s; i++) printf("#%d | %d\n", i, M[i]);
		printf("Resultant array:\n");
		for (i = 0; i < s; i++) printf("#%d | %d\n", i, R[i]);
	}
	if (cont == 0) printf("End of file reached, program stopped...");
	system("pause");
	return 0;
}
