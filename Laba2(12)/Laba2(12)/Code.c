#include <stdio.h>
#include <malloc.h>

int getint(int *add);
int getnat(int *add);
int check(int a);
int counter(int *A, int l);
void matrixprint(int **M, int m, int *N);

int getint(int *add)
{
	int n=0;
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

int getnat(int *add)
{
	int isint=1;
	do
	{
		isint = getint(add);
		if ((isint) && (*add <= 0)) printf("ERROR: Enter natural number.\n");
	} while ((isint) && (*add <= 0));
	return isint;
}

int check(int a)
{
	int r = 1;
	while (a != 0)
	{
		if (((a % 10) % 2) != 0) r = 0;
		a = a / 10;
	}
	return (r);
}

int counter(int *A, int l)
{
	int c = 0;
	for (int k = 0; k < l; k++) c = c + check(A[k]);
	return c;
}

void matrixprint(int **M, int m, int *N)
{
	int i = 0, j = 0;
	for (i = 0; i < m; i++)
	{
		printf("%d |", i);
		for (j = 0; j < N[i]; j++) printf(" %d", M[i][j]);
		printf("\n");
	}
}

int main()
{
	//creating start matrix
	int **M = NULL; //matrix
	int *N = NULL; //lengths
	int m = 0, cont = 1;
	printf("Enter number of lines: ");
	cont = getnat(&m);
	if (cont)
	{
		M = (int**)malloc(m * sizeof(int*));
		N = (int*)malloc(m * sizeof(int));
	}
	int i = 0, j = 0;
	for (i = 0; (i < m) && (cont); i++)
	{
		printf("Enter length of line %d : ", i);
		cont = getnat(&N[i]);
		if (cont)
		{
			M[i] = (int*)malloc(N[i] * sizeof(int));
			for (j = 0; (j < N[i]) && (cont); j++)
			{

				printf("Enter element [%d][%d]: ", i, j);
				cont = getint(&M[i][j]);
			}
		}
	}
	//creating new matrix
	int **R = NULL; //matrix
	int *S = NULL; //lengths
	if (cont)
	{
		R = (int**)malloc(m * sizeof(int*));
		S = (int*)malloc(m * sizeof(int));
		for (i = 0; i < m; i++) S[i] = counter(M[i], N[i]);
		for (i = 0; i < m; i++) R[i] = (int*)malloc(S[i] * sizeof(int)); 
		int k = 0;
		for (i = 0; i < m; i++)
		{
			k = 0;
			for (j = 0; j < N[i]; j++) if (check(M[i][j]))
			{
				R[i][k] = M[i][j];
				k++;
			}
		}
		printf("Entered matrix:\n");
		matrixprint(M, m, N);
		printf("Resultant matrix:\n");
		matrixprint(R, m, S);
	}
	if (cont == 0) printf("ERROR: End of file reached. Program is stopped.");
	system("pause");
	return 0;
}