#include "IOResults.h"
#include <fstream>
#include <iostream>
#include "Compressor.h"
#include "CompressorR6.h"

bool canAccess(string filename);

void saveProbabilities(vector<float>& resultVector, bool binary, string filename)
{
	ofstream resultFile;

	if (binary) {
		//opening a binary file
		resultFile.open(filename, ios::binary | ios::out);

		for (uint64_t i = 0; i < numberOfPositions; i++)
		{
			resultFile.write(reinterpret_cast<const char*>(&resultVector[i]), sizeof(float));
		}

		if (!resultFile.good())
		{
			stringstream ss;
			ss << "Es gab einen Fehler beim Schreiben in die Datei " << filename << "\n";
			logCout(ss);
		}
	}
	else {
		//opening a file
		resultFile.open(filename, ios::out);


		for (uint64_t i = 0; i < numberOfPositions; i++)
		{
			resultFile.precision(8);
			resultFile << i << ":" << resultVector[i] << "\n";
		}

		if (!resultFile.good())
		{
			stringstream ss;
			ss << "Es gab einen Fehler beim Schreiben in die Datei " << filename << "\n";
			logCout(ss);
		}

	}

	resultFile.close();
}

void loadProbabilities(vector<float>& into, string filename) 
{
	ifstream loadFile;
	loadFile.open(filename, ios::binary | ios::in);
	//loadFile.seekg(0);

	//for (uint64_t i = 0; i < numberOfPositions; i++) 
	//{
	//	//loadFile.seekg(i * 4); //every position was saved with 4 bytes
	//	loadFile.read(reinterpret_cast<char*>(&into[i]), sizeof(float));

	//	if (!loadFile.good())
	//	{
	//		stringstream ss;
	//		ss << "Es gab einen Fehler beim Lesen der Datei " << filename << "\n";
	//		logCout(ss);
	//	}
	//}

	/*into.assign(istreambuf_iterator<char>(loadFile), istreambuf_iterator<char>());*/

	loadFile.read(reinterpret_cast<char*>(&into[0]), into.size() * sizeof(float));

	if (!loadFile.good())
	{
		stringstream ss;
		ss << "Es gab einen Fehler beim Lesen der Datei " << filename << "\n";
		logCout(ss);
	}

	loadFile.close();
}

float readProbability(uint64_t positionID, string filename) 
{
	float prob = 0;

	ifstream resultFile;
	resultFile.open(filename, ios::binary | ios::in);
	resultFile.seekg(positionID * 4); //every position was saved with 4 bytes
	resultFile.read(reinterpret_cast<char*>(&prob), sizeof(float));

	if (!resultFile.good())
	{
		stringstream ss;
		ss << "Es gab einen Fehler beim Lesen der Datei! " << filename << "\n";
		logCout(ss);
	}

	return prob;
}

void backupProbabilites(vector<float>& resultVector, string filename)
{

	ofstream resultFile;

	if (!canAccess(filename)) //if cannot be accessed, we ASSUME that the file does not exist
	{
		//opening a binary file
		resultFile.open(filename, ios::binary | ios::out); //create file
	}
	else 
	{
		//opening a binary file
		resultFile.open(filename, ios::binary | ios::out | ios::in); //in and out to not truncate the file
		resultFile.seekp(0);
	}


	for (uint64_t i = 0; i < numberOfPositions; i++)
	{
		resultFile.write(reinterpret_cast<const char*>(&resultVector[i]), sizeof(float));
	}

	if (!resultFile.good())
	{
		stringstream ss;
		ss << "Es gab einen Fehler beim Schreiben in die Datei " << filename << "\n";
		logCout(ss);
	}
}

void logCout(stringstream& out)
{
	cout << out.str();
	ofstream log;
	log.open(logFile, ios::app | ios::out);
	log << out.str();
	log.close();
}

void saveBestMoves_r(vector<optimalMoveUnit4_r>& optimalMoves, string filename) 
{
	ofstream resultFile;

	//opening a binary file
	resultFile.open(filename, ios::binary | ios::out);

	for (uint64_t i = 0; i < optimalMoves.size(); i++)
	{
		resultFile.write(reinterpret_cast<const char*>(&optimalMoves[i]), sizeof(optimalMoveUnit4_r));
	}

	if (!resultFile.good())
	{
		stringstream ss;
		ss << "Es gab einen Fehler beim Schreiben in die Datei " << filename << "\n";
		logCout(ss);
	}

}

void loadBestMoves_r(vector<optimalMoveUnit4_r>& into, string filename)
{
	ifstream loadFile;
	loadFile.open(filename, ios::binary | ios::in);
	loadFile.read(reinterpret_cast<char*>(&into[0]), into.size() * sizeof(optimalMoveUnit4_r));

	if (!loadFile.good())
	{
		stringstream ss;
		ss << "Es gab einen Fehler beim Lesen der Datei " << filename << "\n";
		logCout(ss);
	}

	loadFile.close();
}

void saveBestMoves(vector<float>& probabilities, bool split)
{
	if (split) 
	{

		//ofstream resultFiles[6];

		////create one file for every die roll
		//for (int roll = 1; roll <= 6; roll++)
		//{

		//	resultFiles[roll-1].open(resultDirectory + "/" + bestMovesFileName + std::to_string(roll) + binEnding,
		//		ios::out | ios::binary);
		//}

		////iterating in steps of four (one unit contains for positions) through the positions
		////and writing the bit-sequences of the best
		////moves in units to the hard disk.
		//for (uint64_t posID = 0; posID < numberOfPositions; posID += 4)
		//{
		//	if (posID % 500000 == 0) 
		//	{
		//		cout << "Reached position ID " << posID << "\n";
		//	}

		//	//for every die roll (every file)
		//	for (int roll = 1; roll <= 6; roll++) 
		//	{
		//		//optimal move bits for the next for positions
		//		optimalMoveBits_r bits[4] = { 0 };

		//		for (int i = 0; i < 4; i++)
		//		{
		//			if (posID + i < numberOfPositions)
		//			{
		//				bits[i] = getOptimalMoveBits_r(posID + i, probabilities, roll);
		//			}
		//		}

		//		optimalMoveUnit4_r unit = createOptimalMoveUnit4_r(bits);
		//		resultFiles[roll-1].write(reinterpret_cast<const char*>(&unit), sizeof(optimalMoveUnit4_r));
		//	}
		//}


		////close all files
		//for (int roll = 1; roll <= 6; roll++) 
		//{
		//	if (!resultFiles[roll-1].good())
		//	{
		//		stringstream ss;
		//		ss << "Es gab einen Fehler beim Schreiben in die Datei " <<
		//			resultDirectory + "/" + bestMovesFileName + std::to_string(roll) + binEnding << "\n";
		//		logCout(ss);
		//	}

		//	resultFiles[roll-1].close();
		//}
	}
	else 
	{
		ofstream resultFile;

		resultFile.open(bestMovesFile, ios::out | ios::binary);

		//iterating in steps of two through the positions and writing the bit-sequences of the best
		//moves in units to the hard disk.
		for (uint64_t posID = 0; posID < numberOfPositions; posID += 2)
		{

			if (posID % 500000 == 0)
			{
				cout << "Reached position ID " << posID << "\n";
			}

			optimalMoveBitSequence sequence1 = getOptimalMoveBitSequence(posID, probabilities);

			//if number of positions is odd the last entry has to be taken care of
			optimalMoveBitSequence sequence2 = 0;
			if (posID + 1 < numberOfPositions)
			{
				sequence2 = getOptimalMoveBitSequence(posID + 1, probabilities);
			}

			optimalMoveUnit unit = createOptimalMoveUnit(sequence1, sequence2);

			resultFile.write(reinterpret_cast<const char*>(&unit), sizeof(optimalMoveUnit));
		}

		if (!resultFile.good())
		{
			stringstream ss;
			ss << "Es gab einen Fehler beim Schreiben in die Datei " << bestMovesFile << "\n";
			logCout(ss);
		}

		resultFile.close();
	}
}

int getBestMovePieceIndex(uint64_t posID, int roll, bool split) 
{
	if (split) 
	{
		ifstream resultFile;
		string filename = resultDirectory + "/" + bestMovesFileName + std::to_string(roll) + binEnding;

		resultFile.open(filename, ios::in | ios::binary);

		//positions are saved into units of four positions
		//therefore unit number i holds the information for positionID 4i, 4i+1, 4i+2, 4i+3
		uint64_t unitID = posID / 4;
		resultFile.seekg(unitID); //Every unit consists of 1 Byte

		optimalMoveUnit4_r unit;
		resultFile.read(reinterpret_cast<char*>(&unit), sizeof(optimalMoveUnit4_r));

		if (!resultFile.good())
		{
			stringstream ss;
			ss << "Es gab einen Fehler beim Lesen aus der Datei " << filename << "\n";
			logCout(ss);
		}

		resultFile.close();

		optimalMoveBits_r bits[4];
		CompressorR6::decodeOptimalMoveUnit4_r(unit, bits); //decode unit in its optimal Move bits

		int optimalPieceIndex = 0;
		optimalPieceIndex |= (bits[posID % 4] & 0b00000011); //picking out the bits which are are relevant in the unit
		

		return optimalPieceIndex;
	}
	else
	{
		ifstream resultFile;

		resultFile.open(bestMovesFile, ios::in | ios::binary);

		//positions are saved into units of two positions
		//therefore unit number i holds the information for positionID 2i and 2i+1
		uint64_t unitID = posID / 2;
		resultFile.seekg(unitID * 3); //Every unit consists of 3 Bytes

		optimalMoveUnit unit;
		resultFile.read(reinterpret_cast<char*>(&unit), sizeof(optimalMoveUnit));

		if (!resultFile.good())
		{
			stringstream ss;
			ss << "Es gab einen Fehler beim Lesen aus der Datei " << bestMovesFile << "\n";
			logCout(ss);
		}

		resultFile.close();

		optimalMoveBitSequence sequences[2];
		decodeOptimalMoveUnit(unit, sequences);

		optimalMoveBitSequence s = (posID % 2 == 0 ? sequences[0] : sequences[1]);

		return getOptimalMovePieceIndex(s, roll);
	}
	
}

bool canAccess(string filename) 
{
	fstream accessFile;
	accessFile.open(filename, ios::in | ios::out);

	if (accessFile.good())
	{
		accessFile.close();
		return true;
	}
	else 
	{
		accessFile.close();
		return false;
	}
}
