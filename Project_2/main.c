/*
 * main.c
 *
 *  	Created on: Apr 20, 2015
 *      Author: GameTime
 */

#include<stdio.h>
#include<time.h>
#include<stdlib.h>

int** createMatrix(int size);
void printMatrix(int **matrix, int size);
int** multiplyMatrix (int **firstMatrix, int **secondMatrix, int size);

int main(int argc, char** argv) {
  //Create the instance for the Rand()
  srand(time(NULL));
  int matrixSize = 16;

  //Set both matrices with random numbers 0-99
  int** firstMatrix = createMatrix(matrixSize);
  int** secondMatrix = createMatrix(matrixSize);

  //Print the first two matrices
  printf("First Matrix!\n");
  printMatrix(firstMatrix, matrixSize);
  printf("Second Matrix!\n");
  printMatrix(secondMatrix, matrixSize);

  //Product of the first two matrices
  int** finalMatrix = multiplyMatrix(firstMatrix, secondMatrix, matrixSize);

  //Print the Product of the first two matrices
  printf("Product Matrix!\n");
  printMatrix(finalMatrix, matrixSize);

  return 0;
}

int** createMatrix(int size) {
	int** matrix = malloc(size * sizeof(int *));

	int i, j;

	for (i = 0; i < size; i++) {
		matrix[i] = malloc(size * sizeof(int));
	}

	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			matrix[i][j] = rand() % 100;
		}
	}
	return matrix;
}

void printMatrix(int **matrix, int size) {
	int i, j;
	for (i = 0; i < size; i++) {
		printf("[ ");
		for (j = 0; j < size; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("]\n");
	}
}

int** multiplyMatrix (int **firstMatrix, int **secondMatrix, int size) {
	int** finalMatrix = malloc(size * sizeof(int *));
	int i, j, z, sum;

	for (i = 0; i < size; i++) {
		finalMatrix[i] = malloc(size * sizeof(int));
	}

	for (i = 0; i < size; i++) {
	      for (j = 0; j < size; j++) {
	        for (z = 0; z < size; z++) {
	          sum = sum + firstMatrix[i][z]*secondMatrix[z][j];
	        }

	        finalMatrix[i][j] = sum;
	        sum = 0;
	      }
	 }
	return finalMatrix;
}
