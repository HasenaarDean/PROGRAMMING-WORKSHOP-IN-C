#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#define MIN_NUMBER_OF_ARGS 2
#define MAX_SIZE_OF_ATOMS_ARRAY 20000
#define MIN_SIZE_OF_LINE 61
#define MAX_SIZE_OF_LINE 81
#define BEGINNING_OF_X_IN_LINE 30
#define BEGINNING_OF_Y_IN_LINE 38
#define BEGINNING_OF_Z_IN_LINE 46
#define LEN_OF_COORDINATE 8
#define X_COORDINATE 0
#define Y_COORDINATE 1
#define Z_COORDINATE 2
#define SIZE_OF_ATOM 3
#define EMPTY_CHAR '\0'
#define READ_MODE "r"
#define NO_ARGUMENTS_ERROR "Usage: AnalyzeProtein <pdb1> <pdb2> ...\n"
#define ATOM_LINE_IS_TOO_SHORT_MESSAGE_ERROR "ATOM line is too short %d characters\n"
#define FILE_DOES_NOT_EXIST_ERROR "Error opening file: %s\n"
#define INVALID_FLOAT_ERROR_MESSAGE "Error in coordinate conversion %s!\n"
#define NO_ATOMS_ERROR_MESSAGE "Error - 0 atoms were found in the file %s\n"
#define SUCCESS_INFORMATIVE_RESULTS_MESSAGE_LINE_1 "PDB file %s, %d atoms were read\n"
#define SUCCESS_INFORMATIVE_RESULTS_MESSAGE_LINE_2 "Cg = %.3f %.3f %.3f\n"
#define SUCCESS_INFORMATIVE_RESULTS_MESSAGE_LINE_3 "Rg = %.3f\n"
#define SUCCESS_INFORMATIVE_RESULTS_MESSAGE_LINE_4 "Dmax = %.3f\n"

/**
 * This function gets an array of atoms and calculates the molecule's center of
 * gravity (cg), for one coordinate, x or y or z.
 * @param atomsArray an array of atoms
 * @param numOfAtoms number of atoms in array
 * @param coordinate which coordinate are we calculating
 * @return the cg of the given molecule.
 */
float calculateCenterOfGravity(float atomsArray[MAX_SIZE_OF_ATOMS_ARRAY][SIZE_OF_ATOM],
                               int numOfAtoms, int coordinate)
{
    float cgForOneCoordinate = 0.0f;
    int i;

    for(i = 0; i < numOfAtoms; i++)
    {
        cgForOneCoordinate += atomsArray[i][coordinate];
    }

    cgForOneCoordinate /= numOfAtoms;
    return cgForOneCoordinate;
}

/**
 *This function calculates the euclidean distance of 2 three dimensional points A and B.
 * @param x1 first coordinate of point A
 * @param y1 second coordinate of point A
 * @param z1 third coordinate of point A
 * @param x2 first coordinate of point B
 * @param y2 second coordinate of point B
 * @param z2 third coordinate of point B
 * @return the euclidean distance of 2 three dimensional points A and B.
 */
float euclideanDistance(float x1, float y1, float z1, float x2, float y2, float z2)
{

    float distance = 0.0f;
    distance = sqrtf(((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2)));
    return distance;
}

/**
 *This function calculates the euclidean distance (squared) of 2 three dimensional points A and B.
 * @param x1 first coordinate of point A
 * @param y1 second coordinate of point A
 * @param z1 third coordinate of point A
 * @param x2 first coordinate of point B
 * @param y2 second coordinate of point B
 * @param z2 third coordinate of point B
 * @return the euclidean distance (squared) of 2 three dimensional points A and B.
 */
float euclideanDistanceSquared(float x1, float y1, float z1, float x2, float y2, float z2)
{

	float distanceSquared = 0.0f;
	distanceSquared = ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
	return distanceSquared;
}



/**
 * This function gets an array of atoms and the molecule's cg and
 * calculates the molecule's orbital radius.
 * @param atomsArray an array of atoms.
 * @param numOfAtoms number of atoms in array.
 * @param cgX first coordinate of center of gravity.
 * @param cgY second coordinate of center of gravity.
 * @param cgZ third coordinate of center of gravity.
 * @return the molecule's orbital radius.
 */
float calculateOrbitalRadius(float atomsArray[MAX_SIZE_OF_ATOMS_ARRAY][SIZE_OF_ATOM],
                             int numOfAtoms, float cgX, float cgY, float cgZ)
{

    float orbitalRadius = 0.0f;
    int i;
    float distanceSquared;

    for(i = 0; i < numOfAtoms; i++)
    {
        distanceSquared = euclideanDistanceSquared(cgX, cgY, cgZ, atomsArray[i][X_COORDINATE],
                                     atomsArray[i][Y_COORDINATE], atomsArray[i][Z_COORDINATE]);
        orbitalRadius += distanceSquared;
    }

    orbitalRadius /= numOfAtoms;
    orbitalRadius = sqrtf(orbitalRadius);
    return orbitalRadius;
}

/**
 * This function gets an array of atoms and calculates the molecule's maximal distance
 * between any two atoms of the molecule.
 * @param atomsArray an array of atoms.
 * @param numOfAtoms number of atoms in array.
 * @return the molecule's maximal distance between any two atoms of the molecule.
 */
float calculateMaximalDistance(float atomsArray[MAX_SIZE_OF_ATOMS_ARRAY][SIZE_OF_ATOM],
                               int numOfAtoms)
{
    float dMax = 0.0f;
    float tempDistance = 0.0f;
    int i;
    int j;

    for(i = 0; i < numOfAtoms; i++)
    {
        //the loops run through all combinations of 2 atoms and save the maximum distance.
        //notice that the order i, j does not matter, because euclidean distance is commutative.
        for(j = i; j < numOfAtoms; j++)
        {
            tempDistance = euclideanDistance(atomsArray[i][X_COORDINATE],
                                             atomsArray[i][Y_COORDINATE],
                                             atomsArray[i][Z_COORDINATE],
                                             atomsArray[j][X_COORDINATE],
                                             atomsArray[j][Y_COORDINATE],
                                             atomsArray[j][Z_COORDINATE]);

            //we want the maximal distance in dMax, so we do this check
            if(dMax < tempDistance)
            {
                dMax = tempDistance;
            }
        }
    }

    return dMax;
}

/**
 * This is the main function of the program. It analyzes all input proteins and
 * prints the results (or the errors) on the screen.
 * @param argc arguments counter.
 * @param argv arguments values.
 * @return 0 if succeeds, 1 otherwise.
 */
int main(int argc, char *argv[])
{
    if(argc < MIN_NUMBER_OF_ARGS)
    {
        printf(NO_ARGUMENTS_ERROR);
        exit(EXIT_FAILURE);
    }

    int i;
    for (i = 1; i < argc; i++)
    {
        FILE *fp;
        char line[MAX_SIZE_OF_LINE] = {EMPTY_CHAR};
        float atomsArray[MAX_SIZE_OF_ATOMS_ARRAY][SIZE_OF_ATOM];
        int numOfAtoms = 0;

        fp = fopen(argv[i], READ_MODE);

        if (fp == NULL)
        {
            fprintf(stderr, FILE_DOES_NOT_EXIST_ERROR, argv[i]);
            exit(EXIT_FAILURE);

        }
        while (fgets(line, MAX_SIZE_OF_LINE, fp) != NULL && numOfAtoms < MAX_SIZE_OF_ATOMS_ARRAY)
        {

            if(line[0] == 'A' && line[1] == 'T' && line[2] == 'O' && line[3] == 'M')
            {
                int sizeOfLine = (int) strlen(line);


                if (sizeOfLine < MIN_SIZE_OF_LINE)
                {

                    fprintf(stderr, ATOM_LINE_IS_TOO_SHORT_MESSAGE_ERROR, sizeOfLine);
                    exit(EXIT_FAILURE);

                }

                char coordinateX[LEN_OF_COORDINATE + 1];
                memcpy(coordinateX, &line[BEGINNING_OF_X_IN_LINE], LEN_OF_COORDINATE);
                coordinateX[LEN_OF_COORDINATE] = EMPTY_CHAR;

                char coordinateY[LEN_OF_COORDINATE + 1];
                memcpy(coordinateY, &line[BEGINNING_OF_Y_IN_LINE], LEN_OF_COORDINATE);
                coordinateY[LEN_OF_COORDINATE] = EMPTY_CHAR;

                char coordinateZ[LEN_OF_COORDINATE + 1];
                memcpy(coordinateZ, &line[BEGINNING_OF_Z_IN_LINE], LEN_OF_COORDINATE);
                coordinateZ[LEN_OF_COORDINATE] = EMPTY_CHAR;


                char *endX;
                float resultX = 0.0f;
                errno = 0;
                resultX = strtof(coordinateX, &endX);
                if(resultX == 0 && (errno != 0 || endX == coordinateX))
                {
                    fprintf(stderr, INVALID_FLOAT_ERROR_MESSAGE, coordinateX);
                    exit(EXIT_FAILURE);
                }

                char *endY;
                float resultY = 0.0f;
                errno = 0;
                resultY = strtof(coordinateY, &endY);
                if(resultY == 0 && (errno != 0 || endY == coordinateY))
                {
                    fprintf(stderr, INVALID_FLOAT_ERROR_MESSAGE, coordinateY);
                    exit(EXIT_FAILURE);
                }

                char *endZ;
                float resultZ = 0.0f;
                errno = 0;
                resultZ = strtof(coordinateZ, &endZ);
                if(resultZ == 0 && (errno != 0 || endZ == coordinateZ))
                {
                    fprintf(stderr, INVALID_FLOAT_ERROR_MESSAGE, coordinateZ);
                    exit(EXIT_FAILURE);
                }

                atomsArray[numOfAtoms][X_COORDINATE] = resultX;
                atomsArray[numOfAtoms][Y_COORDINATE] = resultY;
                atomsArray[numOfAtoms][Z_COORDINATE] = resultZ;
                numOfAtoms++;

            }

        }

        //we don't forget to close the file after finishing using it!
        fclose(fp);

        if(numOfAtoms == 0)
        {
            fprintf(stderr, NO_ATOMS_ERROR_MESSAGE, argv[i]);
            exit(EXIT_FAILURE);
        }

        //calculating the center of gravity for the given molecule.
        float cgX = calculateCenterOfGravity(atomsArray, numOfAtoms, X_COORDINATE);
        float cgY = calculateCenterOfGravity(atomsArray, numOfAtoms, Y_COORDINATE);
        float cgZ = calculateCenterOfGravity(atomsArray, numOfAtoms, Z_COORDINATE);

		// calculating the orbital radius.
        float rg = calculateOrbitalRadius(atomsArray, numOfAtoms, cgX, cgY, cgZ);

        // calculating the maximal distance between any two atoms in the molecule.
        float dMax = calculateMaximalDistance(atomsArray, numOfAtoms);

		// printing the results of the protein analysis.
        printf(SUCCESS_INFORMATIVE_RESULTS_MESSAGE_LINE_1, argv[i], numOfAtoms);
        printf(SUCCESS_INFORMATIVE_RESULTS_MESSAGE_LINE_2, cgX, cgY, cgZ);
        printf(SUCCESS_INFORMATIVE_RESULTS_MESSAGE_LINE_3, rg);
        printf(SUCCESS_INFORMATIVE_RESULTS_MESSAGE_LINE_4, dMax);


    }

    return 0;
}
