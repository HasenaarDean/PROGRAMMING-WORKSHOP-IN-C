//================================ Includes =====================================================
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
//================================ Constants ====================================================
#define NUMBER_OF_ARGS 5
#define MIN_NUM_OF_SEQUENCES 2
#define MAX_SIZE_OF_LINE 101
#define MAX_NUM_OF_SEQUENCES 100
#define INDEX_OF_FIRST_ROW_OR_COLUMN 0
#define INDEX_OF_FILE_PATH_ARGUMENT 1
#define INDEX_OF_MATCH_ARGUMENT 2
#define INDEX_OF_MISMATCH_ARGUMENT 3
#define INDEX_OF_GAP_ARGUMENT 4
#define BASE_OF_COUNTING 10
#define TRUE 1
#define FALSE 0
#define FIRST_CHAR_OF_HEADER_LINE '>'
#define EMPTY_CHAR '\0'
#define READ_MODE "r"
#define ARGS_ERROR "Error of usage: CompareSequences <path_to_sequences_file> <m> <s> <g> ...\n"
#define FILE_DOES_NOT_EXIST_ERROR "Error opening file: %s\n"
#define INVALID_INTEGER_FORMAT_ERROR_MESSAGE "Error in input argument conversion %s!\n"
#define NO_SEQUENCES_ERROR_MESSAGE "Error of usage: %d (< 2) sequences were found in file %s\n"
#define ERROR_NOT_ENOUGH_MEMORY "ERROR - Not enough memory!!!"
#define PRINT_RESULT_LINE "Score for alignment of %s to %s is %d\n\nSolution:\n\n"
#define TYPE_OF_MATCH 'm'
#define TYPE_OF_MISMATCH 's'
#define TYPE_OF_GAP_IN_STR1 '1'
#define TYPE_OF_GAP_IN_STR2 '2'
#define TYPE_OF_FIRST_CELL 'f'
#define TWO_NEW_LINES "\n\n"
#define ZERO_CHAR '0'
#define NINE_CHAR '9'
#define SEPARATOR_CHAR_FOR_GAP '-'
#define MINUS_CHAR '-'
#define EMPTY_SEQUENCE {0, "", ""}

//================================ Code Segment =================================================

/**
 * This is a global static variable char array which keeps track of the way we are doing from the
 * last cell of score matrix to the first cell of it, in order to decode later the match
 * restoration.
 */
static char *gMatchRestorationDecoder = NULL;


/**
 * this structure is for a sequence, which includes the size of its string value, the string value
 * itself and the sequence's name.
 */
typedef struct Sequence
{
	unsigned int sizeOfValue;
	char name[MAX_SIZE_OF_LINE];
	char *value;
} Sequence;

/**
 * This structure represents a cell in the scores matrix, which has a pointer to the best cell it
 * came from in the calculation of the maximum value from 3 adjacent previous cells, the type of
 * the previous cell, and the integer value of the cell itself.
 */
typedef struct CellOfScoresMatrix
{
	char typeOfPrevCellPointer;
	struct CellOfScoresMatrix *prevCellPointer;
	int value;
} CellOfScoresMatrix;


/**
 * This is a global static variable integer which stores the number of all sequences we found in
 * the input file.
 */
static int gNumOfSequences = 0;

/**
 * This is a global static variable Sequences array which stores all the sequences we found in
 * the input file.
 */
static Sequence *gSequencesArray = NULL;


/**
 * This function returns weather or not the input string is in an integer format.
 * @param str - the input string we have to check its format.
 */
void checkIfInteger(const char *str)
{
	errno = 0;
	int i = 0;
	while(str[i] != EMPTY_CHAR)
	{
		if(i == 0 && str[i] == MINUS_CHAR)
		{
			i++;
			continue;
		}
		if(str[i] >= ZERO_CHAR && str[i] <= NINE_CHAR)
		{
			i++;
			continue;
		}
		else
		{
			fprintf(stderr, INVALID_INTEGER_FORMAT_ERROR_MESSAGE, str);
			exit(EXIT_FAILURE);
		}
	}
}

/**
 * This function checks if we don't have enough sequences in the file, i.e. less then 2.
 * @param numOfSequences - the number of all the sequences in the file.
 * @param filePath - the path of the file.
 */
void checkNotEnoughSequences(const int numOfSequences, const char filePath[])
{
	if(numOfSequences < MIN_NUM_OF_SEQUENCES)
	{
		fprintf(stderr, NO_SEQUENCES_ERROR_MESSAGE, numOfSequences, filePath);
		exit(EXIT_FAILURE);
	}
}

/**
 * This function calculates the maximum integer from three input integers x, y, z.
 * @param x - integer 1.
 * @param y - integer 2.
 * @param z - integer 3.
 * @return the maximum of the three.
 */
int maxOfThreeCalculator(const int x, const int y, const int z)
{
	if(x > y)
	{
		if(x > z)
		{
			return x;
		}
		else
		{
			return z;
		}

	}
	else
	{
		if(y > z)
		{
			return y;
		}
		else
		{
			return z;
		}
	}
}

/**
 * This function checks if we have the right number of input arguments.
 * @param numOfArgs - the number of input arguments.
 */
void checkNumOfInputArgs(const int numOfArgs)
{
	if(numOfArgs != NUMBER_OF_ARGS)
	{
		fprintf(stderr, ARGS_ERROR);
		exit(EXIT_FAILURE);
	}
}

/**
 * This function checks if we can open the given file correctly.
 * @param fp - the file pointer.
 * @param fileName - the name of the file.
 */
void checkNoFile(const FILE *fp, const char fileName[])
{
	if (fp == NULL)
	{
		fprintf(stderr, FILE_DOES_NOT_EXIST_ERROR, fileName);
		exit(EXIT_FAILURE);

	}
}

/**
 * This function checks if a given pointer is a NULL pointer.
 * @param p - the input pointer.
 */
void nullPointerCheckerForAllocatedMemory(const void *p)
{
	if(p == NULL)
	{
		fprintf(stderr, ERROR_NOT_ENOUGH_MEMORY);
		exit(EXIT_FAILURE);
	}
}

/**
 * This function calculates the score of best alignment for two input strings.
 * @param str1Rows - input string 1.
 * @param str2Cols - input string 2.
 * @param sizeStr1Rows - the size of input string 1.
 * @param sizeStr2Cols - the size of input string 2.
 * @param match - the score for match argument.
 * @param mismatch - the score for mismatch argument.
 * @param gap - the score for gap argument.
 * @return - score of best alignment of two strings.
 */
int calculateBestAlignment(const char *str1Rows, const char *str2Cols,
		                   const int sizeStr1Rows, const int sizeStr2Cols, const int match,
		                   const int mismatch, const int gap)
{
	int i;
	int mResult = 0;
	int sResult = 0;
	int gapResult1 = 0;
	int gapResult2 = 0;
	int result = 0;

	CellOfScoresMatrix **scoresMatrix = NULL;
	scoresMatrix = (CellOfScoresMatrix**) malloc((sizeStr1Rows + 1) *
												 sizeof(CellOfScoresMatrix*));

	nullPointerCheckerForAllocatedMemory(scoresMatrix);

	for(i = 0 ; i < sizeStr1Rows + 1 ; i++)
	{
		scoresMatrix[i] = NULL;
		scoresMatrix[i] = (CellOfScoresMatrix*) malloc((sizeStr2Cols + 1) *
													   sizeof(CellOfScoresMatrix));
		nullPointerCheckerForAllocatedMemory(scoresMatrix[i]);
	}

	int j;
	for(j = 0; j < sizeStr1Rows + 1 ; j++)
	{
		scoresMatrix[j][INDEX_OF_FIRST_ROW_OR_COLUMN].value = j * gap;
		if(j == 0)
		{
			scoresMatrix[j][INDEX_OF_FIRST_ROW_OR_COLUMN].prevCellPointer =
					&scoresMatrix[j][INDEX_OF_FIRST_ROW_OR_COLUMN];

			scoresMatrix[j][INDEX_OF_FIRST_ROW_OR_COLUMN].typeOfPrevCellPointer =
					TYPE_OF_FIRST_CELL;
		}
		else
		{
			scoresMatrix[j][INDEX_OF_FIRST_ROW_OR_COLUMN].prevCellPointer =
					&scoresMatrix[j - 1][INDEX_OF_FIRST_ROW_OR_COLUMN];

			scoresMatrix[j][INDEX_OF_FIRST_ROW_OR_COLUMN].typeOfPrevCellPointer =
					TYPE_OF_GAP_IN_STR2;
		}

	}
	for(j = 0 ; j < sizeStr2Cols + 1 ; j++)
	{
		scoresMatrix[INDEX_OF_FIRST_ROW_OR_COLUMN][j].value = j * gap;
		if(j == 0)
		{
			scoresMatrix[INDEX_OF_FIRST_ROW_OR_COLUMN][j].prevCellPointer =
					&scoresMatrix[INDEX_OF_FIRST_ROW_OR_COLUMN][j];

			scoresMatrix[INDEX_OF_FIRST_ROW_OR_COLUMN][j].typeOfPrevCellPointer =
					TYPE_OF_FIRST_CELL;
		}
		else
		{
			scoresMatrix[INDEX_OF_FIRST_ROW_OR_COLUMN][j].prevCellPointer =
					&scoresMatrix[INDEX_OF_FIRST_ROW_OR_COLUMN][j - 1];

			scoresMatrix[INDEX_OF_FIRST_ROW_OR_COLUMN][j].typeOfPrevCellPointer =
					TYPE_OF_GAP_IN_STR1;
		}

	}

	for(i = 0; i < sizeStr1Rows ; i++)
	{
		for(j = 0 ; j < sizeStr2Cols ; j++)
		{
			gapResult1 = scoresMatrix[i][j + 1].value + gap;
			gapResult2 = scoresMatrix[i + 1][j].value + gap;

			if(str1Rows[i] == str2Cols[j])
			{
				mResult = scoresMatrix[i][j].value + match;
				result = maxOfThreeCalculator(mResult, gapResult1, gapResult2);

				if(result == mResult)
				{
					scoresMatrix[i + 1][j + 1].typeOfPrevCellPointer = TYPE_OF_MATCH;
					scoresMatrix[i + 1][j + 1].prevCellPointer = &scoresMatrix[i][j];
				}
				else if(result == gapResult1)
				{
					scoresMatrix[i + 1][j + 1].prevCellPointer = &scoresMatrix[i][j + 1];
					scoresMatrix[i + 1][j + 1].typeOfPrevCellPointer = TYPE_OF_GAP_IN_STR2;
				}
				else
				{
					scoresMatrix[i + 1][j + 1].prevCellPointer = &scoresMatrix[i + 1][j];
					scoresMatrix[i + 1][j + 1].typeOfPrevCellPointer = TYPE_OF_GAP_IN_STR1;
				}

			}
			else
			{
				sResult = scoresMatrix[i][j].value + mismatch;
				result = maxOfThreeCalculator(sResult, gapResult1, gapResult2);

				if(result == sResult)
				{
					scoresMatrix[i + 1][j + 1].typeOfPrevCellPointer = TYPE_OF_MISMATCH;
					scoresMatrix[i + 1][j + 1].prevCellPointer = &scoresMatrix[i][j];
				}
				else if(result == gapResult1)
				{
					scoresMatrix[i + 1][j + 1].prevCellPointer = &scoresMatrix[i][j + 1];
					scoresMatrix[i + 1][j + 1].typeOfPrevCellPointer = TYPE_OF_GAP_IN_STR2;

				}
				else
				{
					scoresMatrix[i + 1][j + 1].prevCellPointer = &scoresMatrix[i + 1][j];
					scoresMatrix[i + 1][j + 1].typeOfPrevCellPointer = TYPE_OF_GAP_IN_STR1;
				}

			}

			scoresMatrix[i + 1][j + 1].value = result;

		}
	}

	int finalResult = 0;
	finalResult = scoresMatrix[sizeStr1Rows][sizeStr2Cols].value;

	CellOfScoresMatrix tempCell = scoresMatrix[sizeStr1Rows][sizeStr2Cols];

	int strIndex = 0;
	int maxSizeDoubled = 0;

	if(sizeStr1Rows < sizeStr2Cols)
	{
		maxSizeDoubled = sizeStr2Cols;
	}
	else
	{
		maxSizeDoubled = sizeStr1Rows;
	}

	maxSizeDoubled *= 2;

	gMatchRestorationDecoder = NULL;
	gMatchRestorationDecoder = (char*) calloc((size_t) maxSizeDoubled + 1, sizeof(char));
	nullPointerCheckerForAllocatedMemory(gMatchRestorationDecoder);

	while(tempCell.typeOfPrevCellPointer != TYPE_OF_FIRST_CELL)
	{

		if(tempCell.typeOfPrevCellPointer == TYPE_OF_MATCH)
		{

			gMatchRestorationDecoder[strIndex] = TYPE_OF_MATCH;
			strIndex++;
			tempCell = *(tempCell.prevCellPointer);
			continue;
		}
		else if(tempCell.typeOfPrevCellPointer == TYPE_OF_MISMATCH)
		{
			gMatchRestorationDecoder[strIndex] = TYPE_OF_MISMATCH;
			strIndex++;
			tempCell = *(tempCell.prevCellPointer);
			continue;
		}
		else if(tempCell.typeOfPrevCellPointer == TYPE_OF_GAP_IN_STR2)
		{
			gMatchRestorationDecoder[strIndex] = TYPE_OF_GAP_IN_STR2;
			strIndex++;
			tempCell = *(tempCell.prevCellPointer);
			continue;
		}
		else if(tempCell.typeOfPrevCellPointer == TYPE_OF_GAP_IN_STR1)
		{
			gMatchRestorationDecoder[strIndex] = TYPE_OF_GAP_IN_STR1;
			strIndex++;
			tempCell = *(tempCell.prevCellPointer);
			continue;
		}

	}

	gMatchRestorationDecoder[strIndex] = EMPTY_CHAR;

	//Now we free all the allocated memory we used in the heap for the scores Matrix.
	for(i = 0 ; i < sizeStr1Rows + 1 ; i++)
	{
		free(scoresMatrix[i]);
		scoresMatrix[i] = NULL;
	}

	free(scoresMatrix);
	scoresMatrix = NULL;

	return finalResult;
}

/**
 * This function prints the results (i.e. one of the best alignments example and the score) of two
 * strings according to their locations in the score matrix given by input integers str1Location
 * and str2Location.
 * @param sequencesArray - the array of all sequences in the file.
 * @param str1Location - location in the score matrix for string 1.
 * @param str2Location - location in the score matrix for string 2.
 * @param result - the result score of best alignments of two strings.
 */
void printResultOfPair(Sequence *sequencesArray, int str1Location, int str2Location, int result)
{
	printf(PRINT_RESULT_LINE, sequencesArray[str1Location].name,
		   sequencesArray[str2Location].name, result);

	int decoderLen = (int) strlen(gMatchRestorationDecoder);
	int index;
	int s = 0;

	for(index = decoderLen - 1 ; index >= 0 ; index--)
	{

		if(gMatchRestorationDecoder[index] == TYPE_OF_MATCH ||
		   gMatchRestorationDecoder[index] == TYPE_OF_MISMATCH ||
		   gMatchRestorationDecoder[index] == TYPE_OF_GAP_IN_STR2)
		{
			printf("%c", sequencesArray[str1Location].value[s]);
			s++;
		}
		else if(gMatchRestorationDecoder[index] == TYPE_OF_GAP_IN_STR1)
		{
			printf("%c", SEPARATOR_CHAR_FOR_GAP);
		}

	}

	printf("%s", TWO_NEW_LINES);
	s = 0;

	for(index = decoderLen - 1 ; index >= 0 ; index--)
	{
		if(gMatchRestorationDecoder[index] == TYPE_OF_MATCH ||
		   gMatchRestorationDecoder[index] == TYPE_OF_MISMATCH ||
		   gMatchRestorationDecoder[index] == TYPE_OF_GAP_IN_STR1)
		{
			printf("%c", sequencesArray[str2Location].value[s]);
			s++;
		}
		else if(gMatchRestorationDecoder[index] == TYPE_OF_GAP_IN_STR2)
		{
			printf("%c", SEPARATOR_CHAR_FOR_GAP);
		}

	}
}

/**
 * This function parses the input file to an array of sequences.
 * @param fp - the file pointer.
 */
void parseFile(FILE *fp)

{
	char line[MAX_SIZE_OF_LINE] = {EMPTY_CHAR};
	int thereIsSequence = FALSE;
	unsigned int rowNumForSequence = 0;
	Sequence tempSequence;
	tempSequence = (Sequence) EMPTY_SEQUENCE;
	unsigned int sizeOfPrevLine = 0;

	while (fgets(line, MAX_SIZE_OF_LINE, fp) != NULL)
	{
		unsigned int sizeOfLine = 0;
		while(line[sizeOfLine] != EMPTY_CHAR && (isalpha(line[sizeOfLine]) ||
			  (line[sizeOfLine] >= ZERO_CHAR && line[sizeOfLine] <= NINE_CHAR) ||
			  line[sizeOfLine] == FIRST_CHAR_OF_HEADER_LINE))

		{
			sizeOfLine++;
		}

		if(line[0] == FIRST_CHAR_OF_HEADER_LINE)
		{
			thereIsSequence = TRUE;
			if(rowNumForSequence > 0)
			{
				tempSequence.sizeOfValue = (unsigned int) strlen(tempSequence.value);
				gSequencesArray[gNumOfSequences] = tempSequence;
				gNumOfSequences++;
				rowNumForSequence = 0;
			}

			unsigned int j;
			for(j = 1 ; j < sizeOfLine ; j++)
			{
				if(isalpha(line[j]) || (line[j] >= ZERO_CHAR && line[j] <= NINE_CHAR))
				{
					tempSequence.name[j - 1] = line[j];
				}
			}
			tempSequence.name[sizeOfLine - 1] = EMPTY_CHAR;

			continue;

		}

		if(rowNumForSequence == 0)
		{
			tempSequence.value = NULL;
			tempSequence.value = (char*) calloc(sizeOfLine + 1, sizeof(char));
			nullPointerCheckerForAllocatedMemory(tempSequence.value);
			unsigned int i;
			for(i = 0 ; i < sizeOfLine ; i++)
			{
				if(isalpha(line[i]))
				{
					tempSequence.value[i] = line[i];
				}
			}

			rowNumForSequence++;
			sizeOfPrevLine = sizeOfLine;
		}
		else
		{
			tempSequence.value = (char*) realloc(tempSequence.value, (rowNumForSequence + 1) *
			                                     MAX_SIZE_OF_LINE + 1);

			nullPointerCheckerForAllocatedMemory(tempSequence.value);
			rowNumForSequence++;

			unsigned int i;
			for(i = 0 ; i < sizeOfLine ; i++)
			{
				if(isalpha(line[i]))
				{
					tempSequence.value[i + sizeOfPrevLine] = line[i];
				}
			}
			sizeOfPrevLine += sizeOfLine;
		}

	}

	if(thereIsSequence)
	{
		tempSequence.sizeOfValue = (unsigned int) strlen(tempSequence.value);
		gSequencesArray[gNumOfSequences] = tempSequence;
		gNumOfSequences++;

	}

	//we don't forget to close the file after finishing using it!
	fclose(fp);
}

/**
 * This function prints the final results of all the pairs comparisons of strings in the file.
 * @param match - the match score parameter.
 * @param mismatch - the mismatch score parameter.
 * @param gap - the gap score parameter.
 */
void printFinalResultsForFile(const int match, const int mismatch, const int gap)
{
	int i;
	int j;
	int result;

	for(i = 0 ; i < gNumOfSequences ; i++)
	{
		for(j = i; j < gNumOfSequences ; j++)
		{
			if(i == j)
			{
				continue;
			}

			result = calculateBestAlignment(gSequencesArray[i].value,
					                                          gSequencesArray[j].value,
					                                          gSequencesArray[i].sizeOfValue,
					                                          gSequencesArray[j].sizeOfValue,
					                                          match, mismatch, gap);

			//if you are in the last test to print its results - at the end don't print new lines.
			if(i == gNumOfSequences - 2 && j == gNumOfSequences - 1)
			{
				printResultOfPair(gSequencesArray, i, j, result);
			}
			else
			{
				printResultOfPair(gSequencesArray, i, j, result);
				printf("%s", TWO_NEW_LINES);
			}

			free(gMatchRestorationDecoder);
			gMatchRestorationDecoder = NULL;

		}
	}
}

/**
 * This is the main function of the program. It opens the given file and reads all the other input
 * arguments and checks their validity as integers. after that, it analyzes all pairs of
 * sequences in the file and prints the results for all best alignments of strings.
 * @param argc arguments counter.
 * @param argv arguments values.
 * @return 0 if succeeds, 1 otherwise.
 */
int main(int argc, char *argv[])
{
	FILE *fp;
	gSequencesArray = (Sequence*) calloc(sizeof(struct Sequence) * MAX_NUM_OF_SEQUENCES,
										sizeof(struct Sequence));

	nullPointerCheckerForAllocatedMemory(gSequencesArray);

	fp = fopen(argv[INDEX_OF_FILE_PATH_ARGUMENT], READ_MODE);

	checkNumOfInputArgs(argc);

	checkNoFile(fp, argv[INDEX_OF_FILE_PATH_ARGUMENT]);

	int l;
	for(l = INDEX_OF_MATCH_ARGUMENT ; l <= INDEX_OF_GAP_ARGUMENT ; l++)
	{
		checkIfInteger(argv[l]);
	}

	parseFile(fp);

	checkNotEnoughSequences(gNumOfSequences, argv[INDEX_OF_FILE_PATH_ARGUMENT]);

	int match = (int) strtol(argv[INDEX_OF_MATCH_ARGUMENT], NULL, BASE_OF_COUNTING);
	int mismatch = (int) strtol(argv[INDEX_OF_MISMATCH_ARGUMENT], NULL, BASE_OF_COUNTING);
	int gap = (int) strtol(argv[INDEX_OF_GAP_ARGUMENT], NULL, BASE_OF_COUNTING);

	printFinalResultsForFile(match, mismatch, gap);

	//now we will free all the allocated memory we used in the heap:
	int seq;
	for(seq = 0 ; seq < gNumOfSequences ; seq++)
	{
		free(gSequencesArray[seq].value);
		gSequencesArray[seq].value = NULL;
	}

	free(gSequencesArray);
	gSequencesArray = NULL;

	return 0;

}
