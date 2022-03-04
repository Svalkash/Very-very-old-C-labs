#include <stdio.h>
#include <malloc.h>

typedef struct aoi //array of integer
{
	int *arr; //array
	int len; //length
} Line;

typedef struct aoaoiaaoi //array of array of integer and array of integer
{
	Line *mat; //array of lines
	int nol; //number of lines
} Matrix;

//typedef struct aoaoiaaoi Matrix;

int getint(int *add);
int getnat(int *add);
int check(int a);
int counter(Line *L);
void matrixprint(Matrix *M);
int getmatrix(Matrix *M);
void transformatrix(Matrix *M, Matrix *R);
void freedom(Matrix *M);


int getint(int *add)
{
	int n = 0;
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
	int isint = 1;
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

int counter(Line *L)
{
	int c = 0;
	int *pj = (*L).arr;
	for (int k = 0; k < (*L).len; k++)
	{
		c = c + check(*pj);
		pj++;
	}
	return c;
}

void matrixprint(Matrix *M)
{
	int i = 0, j = 0;
	Line *pi = (*M).mat;
	int *pj;
	for (i = 0; i < (*M).nol; i++)
	{
		pj = (*pi).arr;
		printf("%d |", i);
		for (j = 0; j < (*pi).len; j++)
		{
			printf(" %d", *pj);
			pj++;
		}
		printf("\n");
		pi++;
	}
}

int getmatrix(Matrix *M)
{
	int cont = 1;
	printf("Enter number of lines: ");
	cont = getnat(&(*M).nol);
	if (cont)
	{
		(*M).mat = (Line*)malloc((*M).nol * sizeof(Line));
	}
	int i = 0, j = 0;
	Line *pi = (*M).mat;
	int *pj;
	for (i = 0; (i < (*M).nol) && (cont); i++)
	{
		printf("Enter length of line %d : ", i);
		do
		{
			cont = getint(&(*pi).len);
			if ((*pi).len < 0) printf("ERROR: Seriously?\n");
		} while ((cont) && ((*pi).len < 0));
		if (cont)
		{
			(*pi).arr = (int*)malloc((*pi).len * sizeof(int));
			pj = (*pi).arr;
			for (j = 0; (j < (*pi).len) && (cont); j++)
			{

				printf("Enter element [%d][%d]: ", i, j);
				cont = getint(pj);
				pj++;
			}
		}
		pi++;
	}
	return(cont);
}

void transformatrix(Matrix *M, Matrix *R)
{
	(*R).nol = (*M).nol;
	(*R).mat = (Line*)malloc((*R).nol * sizeof(Line));
	int i, j;
	Line *pim = (*M).mat;
	Line *pir = (*R).mat;
	int *pjm, *pjr;
	for (i = 0; i < (*R).nol; i++)
	{
		(*pir).len = counter(pim)+1;
		(*pir).arr = (int*)malloc((*pir).len * sizeof(int));
		pjm = (*pim).arr;
		pjr = (*pir).arr;
		for (j = 0; j < (*pim).len; j++)
		{
			if (check(*pjm))
			{
				*pjr = *pjm;
				pjr++;
			}
			pjm++;
		}
		*pjr = (*pir).len-1;
		pim++;
		pir++;
	}
}

void freedom(Matrix *M)
{
	Line *pi = (*M).mat;
	int i;
	for (i = 0; i < (*M).nol; i++)
	{
		free((*pi).arr);
		pi++;
	}
	free((*M).mat);
}

int main()
{
	//creating start matrix
	Matrix M = { NULL, 0 }, R = { NULL, 0 }; //struct
	if (getmatrix(&M))
	{
		//creating new matrix
		transformatrix(&M, &R);
		printf("Entered matrix:\n");
		matrixprint(&M);
		printf("Resultant matrix:\n");
		matrixprint(&R);
		freedom(&M);
		freedom(&R);
		printf("Matrixes was removed from memory.\n");
	}
	else printf("ERROR: End of file reached. Program is stopped.");
	system("pause");
	return 0;
}