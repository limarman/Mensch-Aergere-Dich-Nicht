#ifndef MADN_H
#define MADN_H

#include <cstdint>
#include <vector>
using namespace std;


//Defines and Structs
struct Possibility
{
	uint64_t setFinishFirst;
	uint64_t setFinishSecond;
	uint64_t setGameFirst;
};

struct AbstractPosition
{
	int inGame[2];
	int inFinish[2];
};

//Methods

/*
Calculates the binomial coefficient n choose k.
*/
uint64_t binomialCoefficient(int n, int k);

/*
Calculates the number of different positions given the piecenumber and squarenumber.
Thereby (color)symmetrical positions are ignored.
*/
uint64_t calculateNumberOfPositions(void);

/*
Calculates the number of possibilities of the given configuration of pieces in form of the Abstract Position.
*/
uint64_t possibilitiesConfiguration(AbstractPosition* ap);

/**
* Sets pieceNumber and squareNumber and precomuptes eveything needed: 
* baseNumber, numberOfPositions, distributionIDs, configurationBases and possibilities
*/
void initialize(int pieces, int squares);

//Attributes

/**
* number of pieces each player has. Should never be lower than 1
* Needs to be initialized with initialize method
*/
extern int pieceNumber;

/**
 * number of squares a piece has to pass before being able to get into the SafeZone.
 * Should be divisible by two.
 * Needs to be initialized with initialize method
 * */
extern int squareNumber;

/**
 * The total number of different possible (legal) positions
 * Needs to be initialized with initialize method
 */
extern uint64_t numberOfPositions;


/*
Number of different bases or configurations for the given number of pieces and squares
Needs to be initialized via the initialize method.
*/
extern int baseNumber;

/**
* Consisting of one entry for every possible variation of piece distribution onto the different "fields" for both players
* Each entry holds boundary of IDs
* Needs to be initialized with initialize method
*/
extern vector<uint64_t> configurationBases;
//extern uint64_t configurationBases[baseNumber];

/**
* Mapping piece distribution ID to represented piece distribution
* Needs to be initialized with initialize method
*/
extern vector<AbstractPosition> distributionIDs;
//extern AbstractPosition distributionIDs[baseNumber];

/**
* Holding a triple for important possibility numbers for every configuration:
*  + first finished stone set
*  + second finished stone set
*  + first inGame stone set
* Needs to be initialized with initialize method
*/
extern vector<Possibility> possibilities;
//extern Possibility possibilities[baseNumber];


#endif
