/*
 ============================================================================
 Name        : Sequential-Collatz-Conjecture.c
 Author      : Chaitra
 Version     :
 Copyright   : Your copyright notice
 Description : Program to generate Collatz Conjecture sequence
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

void generateSequence(int n);
void handleEvenNumber(int n);
void handleOddNumber(int n);

int main(void) {
	int n;
	// Start measuring time
	struct timeval begin, end;
	printf("Enter a positive integer to generate collatz conjecture.\n");
	scanf("%d", &n);
	printf("Generated Collatz conjecture is: \n");
	printf("%d", n);
	if (n < 1) {
		printf("Enter positive number greater than or equal to 1. \n");
		return EXIT_SUCCESS;
	}
	//Starting to measure time
	gettimeofday(&begin, 0);
	generateSequence(n);
	gettimeofday(&end, 0);
	float endSeqTime = (float) (((end.tv_sec * 1000000) + end.tv_usec)
			- ((begin.tv_sec * 1000000) + begin.tv_usec));
	printf(
			"\nTime measured for generating Collatz conjecture sequentially is: %.9f microseconds.\n",
			endSeqTime);
	return EXIT_SUCCESS;
}

/**
 * Method to generate sequence.
 * Takes user input as method parameter
 */
void generateSequence(int n) {
	/*Determine whether the given integer is even or odd*/
	if (n > 1) {
		printf(" ,");
		if (n % 2 == 0) {
			handleEvenNumber(n);

		} else {
			handleOddNumber(n);
		}

	}
}

/**
 * Method to handle Even numbers
 */
void handleEvenNumber(int n) {
	/*calculate sequence for even number*/
	n = n / 2;
	printf("%d", n);
	generateSequence(n);
}

/**
 * Method to handle Odd numbers
 */
void handleOddNumber(int n) {
	/*calculate sequence for even number*/
	n = n * 3 + 1;
	printf("%d", n);
	generateSequence(n);
}

