/*

*/

#ifndef IORESULTS_H
#define IORESULTS_H

#include "MadN.h"
#include <string>
#include <sstream>
#include <chrono>
#include "CompressorR6.h"

/*
Directory for logging.
*/
const string logDirectory = "./log";
const string logFile = logDirectory + "/cout.log";

/*
Directory for results.
*/
const string resultDirectory = "./results";

//file ending for binaries
const string binEnding = ".bin";

//bestMoves naming scheme
const string bestMovesFileName = "bestMoves";

//results file name
const string resultFileName = "ResultsC";

//bestMoves file
const string bestMovesFile = resultDirectory + "/" + bestMovesFileName + binEnding;

//resultsFile binary
const string resultFileBinary = resultDirectory + "/" + resultFileName + binEnding;

//resultsFile txt
const string resultFileTxt = resultDirectory + "/" + resultFileName + ".txt";

//backUpFiles

/*
Saves the calculated results (resultArray) in binary or not (binary) into the given file (filename)
*/
void saveProbabilities(vector<float>& resultArray, bool binary, string filename);


/*
Saves the calculated results (resultArray) in binary into the given file (filename),
by overwriting the existing file from beginning.
Therefore data is valid at any point of the writing procedure.
*/
void backupProbabilites(vector<float>& resultArray, string filename);


/*
Writes the given out into the standard out as well as in the logfile (appending)
*/
void logCout(stringstream& out);


/*
Reads the winning probability for the given positionID in the binary written file (filename).
*/
float readProbability(uint64_t positionID, string filename);

/*
Loads the probabilites from filename into the given vector
*/
void loadProbabilities(vector<float>& into, string filename);


/*
writes the vector of optimalMoveUnits (caclculated by CompressorR6) to the hard-drive under the given "filename"
*/
void saveBestMoves_r(vector<optimalMoveUnit4_r>& optimalMoves, string filename);

/*
loads the best moves int the given vector "into" from the file filename
*/
void loadBestMoves_r(vector<optimalMoveUnit4_r>& into, string filename);

/*
Condenses the best moves out of the probabilites and saves them to the hard disk. 
split decides whether the results are saved as one file using Compressor class
or split into 6 files (one for every die roll) and then saved using CompressorR6 class
*/
void saveBestMoves(vector<float>& probabilities, bool split);

/*
Reads the bestMovesFile at the positionID and extracts the piece-index, which has to advance for the
best move.
split decides whether the best moves are assumed to be in one file, compressed using Compressor class, or
in one file per die roll (total 6), compressed using CompressorR6 class
*/
int getBestMovePieceIndex(uint64_t posID, int roll, bool split);
#endif