//================================ Includes =====================================================
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
#include "stack.h"
//================================ Constants ====================================================
#define ERROR_NOT_ENOUGH_MEMORY "ERROR - Not enough memory!!!"
#define DIVISION_BY_ZERO "Division by 0!\n"
#define POSTFIX_TITLE "Postfix: "
#define INFIX_TITLE "Infix: "
#define PRINT_RESULT "The value is %d\n"
#define NUMBER_PRINT_PATTERN " %s "
#define OPERATOR_AND_PARENTHESIS_PRINT_PATTERN "%s"
#define NEW_LINE "\n"
#define PLUS_CHAR '+'
#define MINUS_CHAR '-'
#define MULTIPLY_CHAR '*'
#define DIVIDE_CHAR '/'
#define POWER_CHAR '^'
#define LEFT_PARENTHESIS_CHAR '('
#define RIGHT_PARENTHESIS_CHAR ')'
#define EMPTY_CHAR '\0'
#define MAX_SIZE_OF_EXPRESSION 102
#define BASE_OF_COUNTING 10
#define TRUE 1
#define FALSE 0
#define PRECEDENCE_SCORE_FOR_POWER_OPERATOR 3
#define PRECEDENCE_SCORE_FOR_MULTIPLY_OR_DIVIDE_OPERATOR 2
#define PRECEDENCE_SCORE_FOR_PLUS_OR_MINUS_OPERATOR 1
#define FIRST_CHAR_INDEX 0
#define SECOND_CHAR_INDEX 1
#define INTEGER_TO_THE_POWER_OF_ZERO 1
#define DUMMY_VALUE 0
//================================ Code Segment =================================================

/**
 * This function checks if a given pointer is a NULL pointer.
 * @param pointerToCheck - the input pointer.
 */
void nullPointerCheckerForAllocatedMemory(const void *pointerToCheck)
{
	if(pointerToCheck == NULL)
	{
		fprintf(stderr, ERROR_NOT_ENOUGH_MEMORY);
		exit(EXIT_FAILURE);
	}
}

/**
 * This function checks if a given char is equal to right or left parenthesis.
 * @param c - the input char.
 * @return 1 if c is a bracket char, 0 otherwise.
 */
int isBracketsChar(const char c)
{
	if(c == LEFT_PARENTHESIS_CHAR || c == RIGHT_PARENTHESIS_CHAR)
	{
		return TRUE;
	}

	return FALSE;
}

/**
 * This function checks if a given char is equal to a binary operator char ('^' or '*' or '/' or
 * '+' or '-').
 * @param c - the input char.
 * @return 1 if c is a binary operator char, 0 otherwise.
 */
int isOperatorChar(const char c)
{
	if(c == PLUS_CHAR || c == MINUS_CHAR || c == MULTIPLY_CHAR || c == DIVIDE_CHAR ||
	   c == POWER_CHAR)
	{
		return TRUE;
	}

	return FALSE;
}

/**
 * This function calculates precedence's score of a binary operator ('^' > '*' = '/' > '+' = '-')
 * @param op - the given operator.
 * @return the operator's precedence.
 */
int calculatePrecedence(const char op)
{
	if(op == POWER_CHAR)
	{
		return PRECEDENCE_SCORE_FOR_POWER_OPERATOR;
	}
	else if(op == MULTIPLY_CHAR || op == DIVIDE_CHAR)
	{
		return PRECEDENCE_SCORE_FOR_MULTIPLY_OR_DIVIDE_OPERATOR;
	}
	else
	{
		return PRECEDENCE_SCORE_FOR_PLUS_OR_MINUS_OPERATOR;
	}
}

/**
 * This function compares between the precedences of two given binary operators.
 * @param op1 - first operator.
 * @param op2 - second operator.
 * @return 1 (TRUE) if op1 <= op2, 0 (FALSE) otherwise.
 */
int compareOperators(const char op1, const char op2)
{
	return calculatePrecedence(op1) <= calculatePrecedence(op2);
}

/**
 * This function calculates integer b by the power of integer a.
 * @param base - first integer.
 * @param exponent - second integer.
 * @return b ^ a
 */
int calculatePowerForInts(const int base, const int exponent)
{
	int result = base;

	if(exponent == 0)
	{
		return INTEGER_TO_THE_POWER_OF_ZERO;
	}

	int i;

	for(i = 1 ; i < exponent ; i++)
	{
		result *= base;
	}

	return result;
}

/**
 * This function calculates integer b (given binary operator) integer a. For example: b / a, if
 * the given operator is '/' (division operator).
 * @param firstNum - first integer.
 * @param secondNum - second integer.
 * @param operation - a char represents a binary operation ('^' or '*' or '/' or '+' or '-').
 * @return b operation a.
 */
int calculateBinaryOperation(const int firstNum, const int secondNum, const char operation)

{

	if(operation == POWER_CHAR)
	{
		return calculatePowerForInts(firstNum, secondNum);
	}
	else if(operation == MULTIPLY_CHAR)
	{
		return firstNum * secondNum;
	}
	else if(operation == DIVIDE_CHAR)
	{
		return firstNum / secondNum;
	}
	else if(operation == PLUS_CHAR)
	{
		return firstNum + secondNum;
	}

	return firstNum - secondNum;
}

/**
 * This function adds a given char to an arithmetic expression (infix or postfix).
 * @param sizeOfExpression - the expression's size.
 * @param expression - the expression we add a char to.
 * @param c - the char to add to the expression.
 * @return the new size of the expression.
 */
int addCharToExpression(int sizeOfExpression, char *expression[MAX_SIZE_OF_EXPRESSION],
						const char c)
{
	expression[sizeOfExpression] = NULL;

	expression[sizeOfExpression] = (char*) calloc(sizeof(char) + 1, sizeof(char));

	nullPointerCheckerForAllocatedMemory(expression[sizeOfExpression]);

	expression[sizeOfExpression][FIRST_CHAR_INDEX] = c;

	expression[sizeOfExpression][SECOND_CHAR_INDEX] = EMPTY_CHAR;

	sizeOfExpression++;

	return sizeOfExpression;
}

/**
 * This function is parsing the input expression and saves its parts (operands, operators and
 * brackets) in an infix expression form.
 * @param infixExpression - the infix expression to adjust.
 * @param sizeOfExpression - the size of the input expression.
 * @param expression - the input expression we need to parse.
 * @return the new size of the infix expression.
 */
int calculateInfixExpression(char *infixExpression[MAX_SIZE_OF_EXPRESSION],
							 const size_t sizeOfExpression, char *expression)
{
	size_t j = 0;
	int countDigits = 0;
	size_t startOfNumber = 0;
	int sizeOfInfix = 0;

	for(j = 0 ; j < sizeOfExpression ; j++)
	{
		if(isdigit(expression[j]))
		{
			countDigits = 1;
			startOfNumber = j;

			while(isdigit(expression[j + 1]))
			{
				countDigits++;
				j++;
			}

			infixExpression[sizeOfInfix] = NULL;
			infixExpression[sizeOfInfix] = (char*) calloc((size_t)countDigits + 1, sizeof(char));
			nullPointerCheckerForAllocatedMemory(infixExpression[sizeOfInfix]);

			size_t i;

			for(i = startOfNumber ; i < startOfNumber + countDigits ; i++)
			{
				infixExpression[sizeOfInfix][i - startOfNumber] = expression[i];
			}

			infixExpression[sizeOfInfix][countDigits] = EMPTY_CHAR;

			sizeOfInfix++;

		}
		else if(isBracketsChar(expression[j]) || isOperatorChar(expression[j]))
		{
			sizeOfInfix = addCharToExpression(sizeOfInfix, infixExpression, expression[j]);
		}

	}

	return sizeOfInfix;
}

/**
 * This function prints an expression (infix or postfix).
 * @param sizeOfExpression - the expression's size.
 * @param expression - the expression we have to print.
 */
void printExpression(const int sizeOfExpression, char **expression)
{

	int i;

	for(i = 0 ; i < sizeOfExpression ; i++)
	{

		//if it is a number - print spaces before and after. Else: print without spaces.
		if(isdigit(expression[i][FIRST_CHAR_INDEX]))
		{
			printf(NUMBER_PRINT_PATTERN, expression[i]);
		}
		else
		{
			printf(OPERATOR_AND_PARENTHESIS_PRINT_PATTERN, expression[i]);
		}

	}

	printf(NEW_LINE);
}

/**
 * This function updates the temp data (which is the data of the top node of the char stack),
 * if and only if the stack is not empty.
 * @param charStack - the stack we are using while converting infix to postfix expression.
 * @param tempData - the data of the top node of the char stack we are using in our algorithm.
 * @return the new value of the temp data.
 */
char *updateData(Stack *charStack, char *tempData)
{

	if(!(isEmptyStack(charStack)))
	{
		tempData = (char*) charStack->_top->_data;
	}

	return tempData;
}

/**
 * This function manage the behaviour of the stack while encountering an operator char,
 * when converting an infix expression into a postfix expression.
 * @param infixExpression - the infix expression we convert.
 * @param postfixExpression - the postfix expression we covert to.
 * @param charStack - the stack we are using for the conversion algorithm.
 * @param sizeOfPostfix - the postfix expression's size.
 * @param tempData - the first popped data from the stack.
 * @param index - the index of the iteration we are in right now in our algorithm.
 */
int handleOperatorChar(char *infixExpression[MAX_SIZE_OF_EXPRESSION],
					   char *postfixExpression[MAX_SIZE_OF_EXPRESSION], Stack *charStack,
					   int sizeOfPostfix, char *tempData, int index)
{
	while(!(isEmptyStack(charStack)) && tempData[FIRST_CHAR_INDEX] != LEFT_PARENTHESIS_CHAR &&
		  compareOperators(infixExpression[index][FIRST_CHAR_INDEX], tempData[FIRST_CHAR_INDEX]))
	{

		char tempHeadData;
		pop(charStack, &tempHeadData);

		sizeOfPostfix = addCharToExpression(sizeOfPostfix, postfixExpression, tempHeadData);

		if(!(isEmptyStack(charStack)))
		{
			tempData = (char*) charStack->_top->_data;
		}

	}
	push(charStack, &infixExpression[index][FIRST_CHAR_INDEX]);

	return sizeOfPostfix;
}

/**
 * This function calculates the postfix expression form, from a given infix expression.
 * @param sizeOfInfix - infix expression's size.
 * @param infixExpression - the infix expression.
 * @param postfixExpression - the postfix expression we need to adjust.
 * @return the new size of the new postfix expression.
 */
int calculatePostfixExpression(const int sizeOfInfix,
							   char *infixExpression[MAX_SIZE_OF_EXPRESSION],
							   char *postfixExpression[MAX_SIZE_OF_EXPRESSION])
{
	Stack *charStack = stackAlloc(sizeof(char));

	int sizeOfPostfix = 0;

	int i;

	for(i = 0 ; i < sizeOfInfix ; i++)
	{

		if(isdigit(infixExpression[i][FIRST_CHAR_INDEX]))
		{
			size_t tempLenOfNum = strlen(infixExpression[i]);

			postfixExpression[sizeOfPostfix] = NULL;
			postfixExpression[sizeOfPostfix] = (char*) calloc(tempLenOfNum + 1, sizeof(char));
			nullPointerCheckerForAllocatedMemory(postfixExpression[sizeOfPostfix]);

			strcpy(postfixExpression[sizeOfPostfix], infixExpression[i]);
			postfixExpression[sizeOfPostfix][tempLenOfNum] = EMPTY_CHAR;
			sizeOfPostfix++;
		}

		if(infixExpression[i][FIRST_CHAR_INDEX] == LEFT_PARENTHESIS_CHAR)
		{
			push(charStack, &infixExpression[i][FIRST_CHAR_INDEX]);
		}

		if(infixExpression[i][FIRST_CHAR_INDEX] == RIGHT_PARENTHESIS_CHAR)
		{
			char *tempData = (char*) charStack->_top->_data;
			while(!(isEmptyStack(charStack)) &&
				  tempData[FIRST_CHAR_INDEX] != LEFT_PARENTHESIS_CHAR)
			{
				char tempHeadData;
				pop(charStack, &tempHeadData);

				sizeOfPostfix = addCharToExpression(sizeOfPostfix, postfixExpression,
													tempHeadData);

				tempData = updateData(charStack, tempData);
			}
			char discardLeftParenthesis;
			pop(charStack, &discardLeftParenthesis);
		}

		if(isOperatorChar(infixExpression[i][FIRST_CHAR_INDEX]))
		{
			if(isEmptyStack(charStack))
			{
				push(charStack, &infixExpression[i][FIRST_CHAR_INDEX]);
			}
			else
			{
				char *tempData = (char*) charStack->_top->_data;

				if(tempData[FIRST_CHAR_INDEX] == LEFT_PARENTHESIS_CHAR)
				{
					push(charStack, &infixExpression[i][FIRST_CHAR_INDEX]);
					continue;
				}

				sizeOfPostfix = handleOperatorChar(infixExpression, postfixExpression, charStack,
												   sizeOfPostfix, tempData, i);
			}
		}
	}

	while(!(isEmptyStack(charStack)))
	{
		char tempHeadData;
		pop(charStack, &tempHeadData);

		sizeOfPostfix = addCharToExpression(sizeOfPostfix, postfixExpression, tempHeadData);
	}

	freeStack(&charStack);

	return sizeOfPostfix;
}

/**
 * This function calculates the result of the given postfix expression.
 * @param sizeOfPostfix - the postfix expression's size.
 * @param postfixExpression - the postfix expression we have to calulate its value.
 */
void calculateResultAndPrintIt(const int sizeOfPostfix,
							   char *postfixExpression[MAX_SIZE_OF_EXPRESSION])
{
	int result = 0;

	Stack *intStack = stackAlloc(sizeof(int));

	int i;

	int divisionByZero = FALSE;

	for(i = 0 ; i < sizeOfPostfix ; i++)
	{

		if(isdigit(postfixExpression[i][FIRST_CHAR_INDEX]))
		{
			int tempInt = (int) strtol(postfixExpression[i], NULL, BASE_OF_COUNTING);
			push(intStack, &tempInt);
		}

		if(isOperatorChar(postfixExpression[i][FIRST_CHAR_INDEX]))
		{
			int a;
			pop(intStack, &a);

			int b;
			pop(intStack, &b);

			if(a == 0 && postfixExpression[i][FIRST_CHAR_INDEX] == DIVIDE_CHAR)
			{
				divisionByZero = TRUE;
				break;
			}
			else
			{
				int tempResult;

				tempResult = calculateBinaryOperation(b, a, postfixExpression[i][FIRST_CHAR_INDEX]);

				push(intStack, &tempResult);
			}
		}
	}

	if(!(isEmptyStack(intStack)))
	{
		pop(intStack, &result);
	}
	else
	{
		result = DUMMY_VALUE;
	}

	if(divisionByZero)
	{
		fprintf(stderr, DIVISION_BY_ZERO);
		exit(EXIT_FAILURE);
	}
	else
	{
		printf(PRINT_RESULT, result);
	}

	freeStack(&intStack);
}

/**
 * This function frees all the expressions memory allocations we used in our program (for the
 * infix expression and the postfix expression).
 * @param sizeOfPostfix - the postfix expression's size.
 * @param postfixExpression - the postfix expression we calculated in our program.
 * @param sizeOfInfix - the infix expression's size.
 * @param infixExpression - the infix expression we calculated in our program.
 */
void freeExpressions(const int sizeOfPostfix, char *postfixExpression[MAX_SIZE_OF_EXPRESSION],
					 const int sizeOfInfix, char *infixExpression[MAX_SIZE_OF_EXPRESSION])
{
	int i;

	for(i = 0 ; i < sizeOfPostfix ; i++)
	{
		free(postfixExpression[i]);
		postfixExpression[i] = NULL;
	}

	for(i = 0 ; i < sizeOfInfix ; i++)
	{
		free(infixExpression[i]);
		infixExpression[i] = NULL;
	}
}

/**
 * This is the main function of the program. It gets an input expression from the user each time,
 * and calculates its infix form. Then, it calculates its postfix form, and eventually calculates
 * its integer value, and prints out all these results.
 * @return 0 if succeeded, non-zero otherwise.
 */
int main()
{
	char expression[MAX_SIZE_OF_EXPRESSION] = {EMPTY_CHAR};

	while (fgets(expression, MAX_SIZE_OF_EXPRESSION, stdin) != NULL)
	{
		//get size of the input expression.
		size_t sizeOfExpression = strlen(expression);

		//calculate infix expression.
		char *infixExpression[MAX_SIZE_OF_EXPRESSION];
		int sizeOfInfix = calculateInfixExpression(infixExpression, sizeOfExpression, expression);

		//print infix expression.
		printf("%s", INFIX_TITLE);
		printExpression(sizeOfInfix, infixExpression);

		//calculate postfix expression.
		char *postfixExpression[MAX_SIZE_OF_EXPRESSION];
		int sizeOfPostfix = calculatePostfixExpression(sizeOfInfix, infixExpression,
													   postfixExpression);

		//print postfix expression.
		printf("%s", POSTFIX_TITLE);
		printExpression(sizeOfPostfix, postfixExpression);

		//calculate mathematical value of the expression and print it.
		calculateResultAndPrintIt(sizeOfPostfix, postfixExpression);

		//free all allocated memory in the heap for both the infix and postfix expressions.
		freeExpressions(sizeOfPostfix, postfixExpression, sizeOfInfix, infixExpression);
	}

	return 0;
}
