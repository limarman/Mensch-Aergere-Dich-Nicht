#include "MadN.h"
#include <iostream>
#include <chrono>
#include "Calculator.h"
#include "IOResults.h"
#include "Position.h"
#include "Compressor.h"
#include "OptimalPlayer.h"
#include "Game.h"
#include "Tournament.h"
#include <random>
#include "SimplePlayer.h"
#include "RandomPlayer.h"
#include "ImprovedSimplePlayer.h"
#include "OptimalPlayerBM.h"
#include "FastOptimalPlayerBMS.h"
#include <cstring>
#include <thread>
#include "Analyser.h"

void selectivityTest(void);
void compressorConsistencyTest(void);
void percentageOfCorrectMoves(GamePlayer*);
void runTimeTest(void);

int main(int argc, char *argv[])
{
	//initialize();
	//srand(time(0));
	//TODO: user enters the specifications (number of pieces, number of squares)

	if (argc <= 3)
	{

		if (argc >= 2 && strcmp(argv[1], "help") == 0) 
		{
			//TODO:
			//output a help page, explaining all the available commands
			std::cout << "The commands all have the form:\n\n" <<
				"madn.out <number_of_pieces> <number_of_squares> <command>\n\n" <<
				"where the commands are the following:\n\n" <<
				"compress <threads>[compressor_type] \n" << 
				" - available compressor types:\n" <<
				"  -- default: creates one file (if no argument is given this is the compressor used)\n" <<
				"  -- r6: creates one file for every die roll (in total 6 files)\n" <<
				"  -- r6s: r6 but handles one dieroll at a time. Using less RAM but taking more time\n"<<
				" descr: condenses the result file at results/ResultsC.bin to just the best possible moves using \"threads\" number of threads " <<
				"and saves results in results/bestMoves.bin or results/bestMoves(1-6).bin respectively.\n\n" <<
				"calc <number_of_threads> [init_file]\n" <<
				" - probabilites from init_file are loaded for initialization if file name is given.\n" <<
				" - calculating is distributed to given number of threads.\n" <<
				" descr: calculates the winning probabilites for every position and saves results to results/ResultsC.bin.\n\n" <<
				"compareGP <number_of_threads> <game_player>\n" <<
				" - calculations is distributed among \"number_of_threads\" threads.\n" <<
				" - game_player which is used in comparison. Available GamePlayers:\n" <<
				" -- SimplePlayer, ImprovedSimplePlayer, OptimalPlayerBM, OptimalPlayerBMS, FastOptimalPlayerBMS, RandomPlayer\n"<<
				" descr: compares the given \"game_player\" to the optimal GamePlayer and counts the number of overlapped decisions.\n\n"<<
				"getBestMoves <compressor_type> <position_id>\n" <<
				" descr: extracts the best moves for every die roll given the position ID.\n" <<
				"        Assumes the best move file(s) have been generated with the given compressor type.\n" <<
				"        note that the best move is given by the index of moving piece if sorted.\n\n" <<
				"getPosID <pieces_of_player_to_move> <pieces_of_other_player>\n" <<
				" - format: entering pieces at positions a, b, c, d as a,b,c,d." <<
				"           Thereby still out of game is position -1, the start square 0 and then counting up until numberOfSquares+numberOfPieces-1.\n" << 
				" descr: gets the id representing the position which is given by the piece positions of both players.\n\n" <<
				"getPos <positionID>\n" <<
				" - positionID should be in bounds [0,numberOfPositions-1]\n" <<
				" descr: gets and shows the position for given positionID.\n";

			
		}
		else
		{
			std::cout << "Invalid command. (for help see madn.out help)" << "\n";
			return -1;
		}
		
	}
	else if (argc >= 4) 
	{
		//first reading the configuration (number of pieces, number of squares)
		int numberOfPieces, numberOfSquares;

		//TODO:
		//check whether the input strings can be casted to int or not

		numberOfPieces = stoi(argv[1]);
		numberOfSquares = stoi(argv[2]);

		if (numberOfPieces <= 0 || numberOfSquares < 0 || numberOfSquares % 2 != 0) 
		{

			std::cout <<
				"configuration values are invalid! It should be: pieceNumber >= 1, numberOfSquares >= 0 and divisible by two"
				<< "\n";
			return -1;
		}

		//TODO: setting the global variables pieceNumber and squareNumber
		std::cout << "initialize...\n";

		initialize(numberOfPieces, numberOfSquares);


		std::cout << "initialization completed.\n";

		std::cout << "Number of Positions: " << numberOfPositions << "\n";

		if (strcmp(argv[3], "test") == 0)
		{
			//used for developer tests
			

			runTimeTest();
			//Analyser a(48, nullptr);
			//a.idMapBijectionTest();
			//selectivityTest();
			//compressorConsistencyTest();
			//bestMoveTest(true, 2);
			//OptimalPlayerBM sp(true);
			//percentageOfCorrectMoves(&sp);
		}
		else if (strcmp(argv[3],"compress") == 0) //user chose the compress function
		{
			int number_of_threads = 0;
			try 
			{
				number_of_threads = stoi(argv[4]);
			}
			catch (...) 
			{
				std::cout << "Invalid command. (for help see madn.out help)" << "\n";
				return -1;
			}
			 

			if (argc < 6) //assuming "default" compressor 
			{
				vector<float> probabilities(numberOfPositions);
				std::cout << "Loading probabilites from result file...\n";
				loadProbabilities(probabilities, resultFileBinary);
				std::cout << "Starting to compress with default compresssor...\n";
				saveBestMoves(probabilities, false);
			}

			//TODO:
			//user can determine the files where the results are written as well as where he wants the compressed file to be saved to.

			else if (strcmp(argv[5], "default") == 0)
			{
				//TODO: implement parallelization for default compressor
				vector<float> probabilities(numberOfPositions);
				std::cout << "Loading probabilites from result file...\n";
				loadProbabilities(probabilities, resultFileBinary);
				std::cout << "Starting to compress with default compresssor...\n";

				auto start = chrono::steady_clock::now();
				saveBestMoves(probabilities, false);

				auto end = chrono::steady_clock::now();
				cout << "Elapsed time in seconds : "
					<< chrono::duration_cast<chrono::seconds>(end - start).count()
					<< " s" << endl;
			}
			else if (strcmp(argv[5], "r6") == 0)
			{
				stringstream ss;
				
				vector<float> probabilities(numberOfPositions);
				
				ss.str(string());
				ss << "Loading probabilites from result file...\n";
				logCout(ss);
				loadProbabilities(probabilities, resultFileBinary);
				ss.str(string());
				ss << "Starting to compress with r6 compressor...\n";
				logCout(ss);

				auto start = chrono::steady_clock::now();
				CompressorR6 compr(number_of_threads);
				vector<optimalMoveUnit4_r> optimalMoves[6];
				vector<optimalMoveUnit4_r>* optimalMovesRef[6];
				for (int i = 0; i < 6; i++) 
				{
					optimalMovesRef[i] = &optimalMoves[i];
				}

				compr.calculateBestMoves(probabilities, optimalMovesRef);
				for (int roll = 1; roll <= 6; roll++)
				{
					ss.str(string());
					ss << "Saving for dieroll " << roll << "...\n";
					logCout(ss);
					string filename = resultDirectory + "/" + bestMovesFileName + std::to_string(roll) + binEnding;
					saveBestMoves_r(optimalMoves[roll-1], filename);
				}

				auto end = chrono::steady_clock::now();
				ss.str(string());
				ss << "Elapsed time in seconds : "
					<< chrono::duration_cast<chrono::seconds>(end - start).count()
					<< " s" << endl;
				logCout(ss);
				//saveBestMoves(probabilities, true);
			}
			else if (strcmp(argv[5], "r6s") == 0) 
			{
				stringstream ss;

				vector<float> probabilities(numberOfPositions);
				CompressorR6 compr(number_of_threads);

				ss.str(string());
				ss << "Loading probabilites from result file...\n";
				logCout(ss);
				loadProbabilities(probabilities, resultFileBinary);
				ss.str(string());
				ss << "Starting to compress with r6s compressor...\n";
				logCout(ss);

				auto start = chrono::steady_clock::now();

				for (int roll = 1; roll <= 6; roll++) 
				{
					ss.str(string());
					ss << "Compressing for dieroll " << roll << "...\n";
					logCout(ss);

					vector<optimalMoveUnit4_r> optimalMoves;

					compr.calculateBestMoves_r(probabilities, optimalMoves, roll);

					ss.str(string());
					ss << "Saving for dieroll " << roll << "...\n";
					logCout(ss);

					string filename = resultDirectory + "/" + bestMovesFileName + std::to_string(roll) + binEnding;
					saveBestMoves_r(optimalMoves, filename);

					//log runtime
					auto end = chrono::steady_clock::now();
					ss.str(string());
					ss << "Elapsed time in seconds : "
						<< chrono::duration_cast<chrono::seconds>(end - start).count()
						<< " s" << endl;
					logCout(ss);
				}

			}
			else
			{
				std::cout << "Compressor type not found. (for help see madn.out help)" << "\n";
				return -1;
			}



		}
		else if (strcmp(argv[3], "calc") == 0) //calculation command
		{
			//no initialization file is givens
			if (argc >= 5)
			{
				//TODO: checking input, so that no error is thrown
				int number_of_threads = stoi(argv[4]);

				//no value loading - 
				if (argc < 6) 
				{
					Calculator c(number_of_threads, false);
					cout << "init complete\n";
					c.calculateProbabilitiesParallel();
				}
				else 
				{
					string loadFile = argv[5];
					Calculator c(number_of_threads, loadFile);
					c.calculateProbabilitiesParallel();
				}
			}
			else 
			{
				std::cout << "Number of threads have to be specified (for help see madn.out help)" << "\n";
			}
		}
		else if (strcmp(argv[3], "compareGP") == 0) 
		{
			if (argc < 6) 
			{
				std::cout << "Invalid command. See madn.out help for help.\n";
				return -1;
			}
			//parsing number of threads
			int numberOfThreads;
			try 
			{
				numberOfThreads = stoi(argv[4]);
			}
			catch(const char* e)
			{
				std::cout << "Number of Threads needs to be a number. See madn.out help for help.\n";
				return -1;
			}
			//parsing gameplayer
			GamePlayer* gp;
			if (strcmp(argv[5], "SimplePlayer") == 0) 
			{
				gp = new SimplePlayer();
			}
			else if (strcmp(argv[5], "ImprovedSimplePlayer") == 0)
			{
				gp = new ImprovedSimplePlayer();
			}
			else if (strcmp(argv[5], "OptimalPlayerBM") == 0)
			{
				gp = new OptimalPlayerBM(false);
			}
			else if (strcmp(argv[5], "OptimalPlayerBMS") == 0)
			{
				gp = new OptimalPlayerBM(true);
			}
			else if (strcmp(argv[5], "FastOptimalPlayerBMS") == 0) 
			{
				gp = new FastOptimalPlayerBMS("results/bestMoves");
			}
			else if (strcmp(argv[5], "RandomPlayer") == 0)
			{
				gp = new RandomPlayer();
			}
			else 
			{
				std::cout << "Invalid GamePlayer. See madn.out help for help.\n";
				return -1;
			}

			stringstream stream;

			vector<float> probabilities(numberOfPositions);

			loadProbabilities(probabilities, "./results/ResultsC.bin");

			Analyser a(numberOfThreads, &probabilities);

			stream.str(string());
			stream << "Started the comparison of " << argv[5] << " to the optimal player...\n";
			logCout(stream);

			auto start = chrono::steady_clock::now();

			similarityStats stats = a.compareToOptimal(gp);

			stream.str(string());
			stream << "Number of non finished situations: " << stats.non_finished_situations << endl;
			stream << "Number of non trivial situations: " << stats.non_trivial_situations << endl;
			stream << "Number of overlapping decision situations: " << stats.same_output_situations << endl;
			stream << "Percentage of correct decisions: " << stats.same_output_situations / (double)stats.non_finished_situations << endl;
			stream << "Percentage of correct decisions for non-trivial situations: " <<
				(stats.same_output_situations - (stats.non_finished_situations - stats.non_trivial_situations)) / (double)stats.non_trivial_situations << endl << endl;

			auto end = chrono::steady_clock::now();
			stream << "Elapsed time in seconds : "
				<< chrono::duration_cast<chrono::seconds>(end - start).count()
				<< " s" << endl;

			logCout(stream);

			
		}
		else if (strcmp(argv[3], "getBestMoves") == 0) 
		{
			if (argc < 6) 
			{
				std::cout << "Invalid command. See madn.out help for help.\n";
				return -1;
			}

			//TODO: check for number to avoid crash
			uint64_t positionID;

			std::istringstream iss(argv[5]); //stoi does not work for uint64_t length
			iss >> positionID;

			if(strcmp(argv[4], "default") == 0)
			{
				for (int i = 1; i <= 6; i++) 
				{
					int index = getBestMovePieceIndex(positionID, i, false);
					cout << "Roll " << i << ": " << index << "\n";
				}
			}
			else if (strcmp(argv[4], "r6") == 0) 
			{
				for (int i = 1; i <= 6; i++)
				{
					int index = getBestMovePieceIndex(positionID, i, true);
					cout << "Roll " << i << ": " << index << "\n";
				}
			}
			else 
			{
				std::cout << "Unknown compressor type. Please see madn.out help for help\n";
			}
		}
		else if (strcmp(argv[3], "getPosID") == 0)
		{
			if (argc < 6) 
			{
				std::cout << "invalid command. See madn.out help for help.\n";
				return -1;
			}

			string piecePlayer1;
			string piecePlayer2;

			istringstream argumentPlayer1(argv[4]);
			istringstream argumentPlayer2(argv[5]);

			vector<int> pp1(pieceNumber);
			vector<int> pp2(pieceNumber);

			//TODO: check for parsing errors, there are so many possibilities to confuse the program here.

			for (int i = 0; i < pieceNumber; i++) 
			{
				//reached end of input -> parsing error 
				if (!(getline(argumentPlayer1, piecePlayer1, ',') &&
					getline(argumentPlayer2, piecePlayer2, ',')))
				{
					cout << "Invalid piece position input. Please see madn.out help.\n";
					return -1;
				}
				
				pp1[i] = stoi(piecePlayer1); //TODO: check whether castable
				pp2[i] = stoi(piecePlayer2);
			}

			//check whether the entered piece positions are inside ouf bounds
			for (int i = 0; i < pieceNumber; i++) 
			{
				if (pp1[i] < -1 || pp1[i] > pieceNumber + squareNumber - 1
					|| pp2[i] < -1 || pp2[i] > pieceNumber + squareNumber - 1)
				{
					//parsing error
					cout << "Piece position input out of bounds. Please see madn.out help.\n";
					return -1;
				}
			}

			Player player1(pp1);
			Player player2(pp2);
			Position pos(player1, player2, false);
			//check whether entered position is legitamte position - 
			//no square used multiple times, not both players in finish etc.
			cout << "ID of position is: " << pos.encodePosition() << "\n";
		}
		else if (strcmp(argv[3], "getPos") == 0) //user chose the getPos command
		{ 
			if (argc < 5)
			{
				std::cout << "invalid command. See madn.out help for help.\n";
				return -1;
			}

			//TODO: check for number to avoid crash
			uint64_t positionID;

			std::istringstream iss(argv[4]); //stoi does not work for uint64_t length
			iss >> positionID;

			if (positionID < 0 || positionID >= numberOfPositions) 
			{
				std::cout << "position ID out of bounds. See madn.out help for help.\n";
				return -1;
			}

			std:cout << "The Position for ID " << positionID << " is: \n" << Position::positionToString(positionID) << "\n";
		}
	}

//	uint64_t posID = 38332;
//	Position pos = Position::decodePosition(posID);
//	vector<Position> successors = pos.calculateSuccessors(5);

	//Finding out specific winning probabilitites
	//int pieces0[] = { 0};
	//int pieces1[] = { 0};
	//Player player0(pieces0);
	//Player player1(pieces1);
	//Position pos(player0, player1, false);

	//Calculator c(true);
	//cout << "Winning probability for s_0: " << c.getProbability(pos.encodePosition()) << "\n";

	//int roll = 6;

	//cout << "Best index: " << getBestMovePieceIndex(pos.encodePosition(), roll) << "\n";

	//optimalMoveBitSequence s1 = 0b111111111111, s2 = 0b111111111111;

	//optimalMoveUnit unit = createOptimalMoveUnit(s1, s2);
	//cout <<"Sequence in Chars: " << (int)unit.sequence1 << " "
	//	<< (int)unit.sequence12 << " " << (int)unit.sequence2 << "\n";

	//optimalMoveBitSequence sequences[2];
	//decodeOptimalMoveUnit(unit, sequences);
	//cout << "First Sequence: " << sequences[0] << " Second Sequence: " << sequences[1] << "\n";

	/*
	Calculation of Results.
	*/
	
	//Calculator c(true);
	//printPosition(2000);

	//auto start = chrono::steady_clock::now();


	//Calculator c(true);
	//uint64_t minWinProb = c.getMinWinningProbabilityPositionID();
	//uint64_t maxWinProb = c.getMaxWinningProbabilityPositionID();
	//cout << "Winning Probabilitiy for start position: "
	//	<< c.getProbability(pos.encodePosition()) << "\n";
	//cout << "Minimal Winning Probability: " << c.getProbability(minWinProb) << " in PositionID " << minWinProb << ":\n";
	//printPosition(minWinProb);
	//cout << "Maximal Winning Probability: " << c.getProbability(maxWinProb) << " in PositionID " << maxWinProb << ":\n";
	//printPosition(maxWinProb);
	//c.outputOptimalMoves();

	//analyseRatios();

	//c.calculateProbabilitiesParallel();
	//checkBijection1();

	//cout << "Selectivity: " << c.getSelectivity() << "\n";

	//ImprovedSimplePlayer isp1;
	//SimplePlayer sp2;
	//Tournament t(&sp2, &isp1, 1000000);
	//t.startTournament();

	//auto end = chrono::steady_clock::now();

	//cout << "Elapsed time in seconds : "
	//	<< chrono::duration_cast<chrono::seconds>(end - start).count()
	//	<< " s" << endl;


	return 0;
}

/*
* checks whether default and r6 compressor have the same best move index for all non-terminal positions
*/
void compressorConsistencyTest() 
{
	//check whether best move is consistent (same for default and r6 compressor)
	bool consistent = true;

	for (uint64_t id = 0; id < numberOfPositions; id++)
	{
		//iterate through all positions

		//check whether the position is non-terminal
		Position p = Position::decodePosition(id);
		if (p.isFinished())
		{
			continue;
		}

		//if not terminal position
		for (int roll = 1; roll <= 6; roll++)
		{
			int bestIndexDefault = getBestMovePieceIndex(id, roll, false);
			int bestIndexR6 = getBestMovePieceIndex(id, roll, true);

			if (bestIndexR6 != bestIndexDefault)
			{
				consistent = false;
				std::cout << "Failed for positionID " << id << " and roll " << roll << "\n";
			}
		}
	}

	if (consistent)
	{
		std::cout << "r6 and default compressor are consistent";
	}
}

void selectivityTest() 
{
	float minSelec = 0;
	Calculator c(2, resultFileBinary);
	c.getSelectivity(&minSelec);
	cout << "The selectivity is: " << minSelec << endl;
}

void runTimeTest() 
{
	int reachedMinute = 0;
	auto start = chrono::steady_clock::now();

	while (true) 
	{
		auto now = chrono::steady_clock::now();
		if (chrono::duration_cast<chrono::minutes>(now - start).count() > reachedMinute) 
		{
			reachedMinute++;
			stringstream ss;
			ss.str(string());
			ss << "Reached Minute " << reachedMinute << endl;
			logCout(ss);
		}
	}
	

}




////tests the corresponding compressor, whether it gives all the best moves
//void bestMoveTest(bool split, int numberOfThreads) 
//{
//	cout << "Starting bestMove test for compressor " << (split ? "r6" : "default") << "\n";
//	bool correct = true;
//	OptimalPlayer optProb;
//	OptimalPlayerBM optBM(split);
//
//	vector<uint64_t> ranges(numberOfThreads); //start index for every thread
//	vector<bool> correctPerThread(numberOfThreads); //minimum for every single thread
//
//	uint64_t positionsPerThread = numberOfPositions / numberOfThreads;
//
//	for (int i = 0; i < numberOfThreads; i++)
//	{
//		ranges[i] = i * positionsPerThread;
//	}
//	vector<thread> threads;
//
//	//Creating the Threads
//	for (int i = 0; i < numberOfThreads - 1; i++)
//	{
//		threads.emplace_back(bestMoveTestFromTo, ranges[i], ranges[i + 1] - 1,
//			&optProb, &optBM, &correctPerThread[i]);
//	}
//	threads.emplace_back(bestMoveTestFromTo, ranges[numberOfThreads-1], numberOfPositions - 1,
//		&optProb, &optBM, &correctPerThread[numberOfThreads-1]);
//
//	for (auto& t : threads)
//	{
//		t.join();
//	}
//
//	for (bool b : correctPerThread) 
//	{
//		if (!b) 
//		{
//			correct = false;
//		}
//	}
//	
//	if (correct)
//	{
//		std::cout << "The " << (split ? "r6" : "default") << " compressor gives the best move for every position\n";
//	}
//	else 
//	{
//		std::cout << "The " << (split ? "r6" : "default") << " compressor fails at least at one position.\n";
//	}
//}

//void bestMoveTestFromTo(uint64_t from, uint64_t to, GamePlayer* gp1, GamePlayer* gp2, bool* correct) 
//{
//	//vector<float> probabilities(numberOfPositions);
//	//loadProbabilities(probabilities, "results/ResultsC.bin");
//
//
//	//ImprovedSimplePlayer optBM = ImprovedSimplePlayer();
//	//RandomPlayer optBM;
//	//SimplePlayer optBM = SimplePlayer();
//
//	for (uint64_t id = from; id <= to; id++)
//	{
//		//programflow:
//		//iterate through all positions
//		//for every non-terminal position:
//		//get the best successor through comparing values in prob vector
//		//get the best successor by getting the optimal index and applying the move
//		//compare the the positionIDs of both successors obtained to check whether it is the same
//
//		//check whether the position is non-terminal
//		Position p = Position::decodePosition(id);
//		if (p.isFinished())
//		{
//			continue;
//		}
//
//		//if not terminal position
//		for (int roll = 1; roll <= 6; roll++)
//		{
//			//find the optimal successor based on the proability vector
//			vector<Position> succs = p.calculateSuccessors(roll);
//
//			Position p1 = gp1->chooseSuccessor(p, roll);
//			Position p2 = gp2->chooseSuccessor(p, roll);
//
//			uint64_t succIDBM = p2.encodePosition();
//			uint64_t succIDProb = p1.encodePosition();
//
//			//First check whether chosen successor by BM is legal:
//			bool contained = false;
//			for (Position succ : succs) 
//			{
//				if (succ.encodePosition() == p2.encodePosition())
//				{
//					contained = true;
//					break;
//				}
//			}
//
//			if (!contained) 
//			{
//				std::cout << "The Gameplayer2 gives an illegal move for positionID, dieroll: " << id << ", " << roll << "\n";
//			}
//
//			//check whether the Gameplayers give the same result
//			if (succIDBM != succIDProb) 
//			{
//				*correct = false;
//				std::cout << "The Gameplayers' move differs for positionID, dieroll: " << id << ", " << roll << "\n";
//			}
//		}
//	}
//}

///*
//* Taking one GamePlayer and checking how many moves it manages to guess right for pair positionID, dieroll
//*/
//void percentageOfCorrectMoves(GamePlayer* gp) 
//{
//	cout << "Starting to calculate the percentage of correct moves of given GamePlayer\n";
//	
//	uint64_t correct = 0;
//	uint64_t non_trivial_situation = 0; //situation where there is more than 1 choice
//	uint64_t non_finished_situation = 0; //situation where there are at least 1 choice
//
//	OptimalPlayer optProb;
//
//	for (uint64_t id = 0; id < numberOfPositions; id++)
//	{
//
//		if (id % 100000 == 0)
//		{
//			cout << "Reached positionID " << id << "\n";
//		}
//
//		//check whether the position is non-terminal
//		Position p = Position::decodePosition(id);
//		if (p.isFinished())
//		{
//			continue;
//		}
//
//
//		//if not terminal position
//		for (int roll = 1; roll <= 6; roll++)
//		{
//			non_finished_situation++;
//
//			//find the optimal successor based on the proability vector
//			vector<Position> succs = p.calculateSuccessors(roll);
//
//			if (succs.size() > 1)
//			{
//				non_trivial_situation++;
//			}
//
//			Position p1 = optProb.chooseSuccessor(p, roll);
//			Position p2 = (*gp).chooseSuccessor(p, roll);
//
//			uint64_t succIDBM = p2.encodePosition();
//			uint64_t succIDProb = p1.encodePosition();
//
//			//check whether Prob and BM give the same successor by comparing their position IDs
//			if (succIDBM == succIDProb)
//			{
//				correct++;
//			}
//		}
//	}
//
//	cout << "In total there are " << non_finished_situation << " situations " <<
//		"from which the GamePlayer correctly moves in " << correct << " situations.\n" <<
//		" In total there are " << non_trivial_situation << " non trivial situations\n";
//
//}


//void printPosition(uint64_t posID) 
//{
//	Position pos = Position::decodePosition(posID);
//	cout << "Player0: ";
//	for (int i = 0; i < pieceNumber; i++) 
//	{
//		cout << pos.getPlayer(0)->getPositionAtIndex(i) << ", ";
//	}
//	cout << "\nPlayer1: ";
//	for (int i = 0; i < pieceNumber; i++)
//	{
//		cout << pos.getPlayer(1)->getPositionAtIndex(i) << ", ";
//	}
//	cout << "\nTurn: " << 0 << "\n";
//}
//
//void printPosition(Position pos)
//{
//	cout << "Player0: ";
//	for (int i = 0; i < pieceNumber; i++)
//	{
//		cout << pos.getPlayer(0)->getPositionAtIndex(i) << ", ";
//	}
//	cout << "\nPlayer1: ";
//	for (int i = 0; i < pieceNumber; i++)
//	{
//		cout << pos.getPlayer(1)->getPositionAtIndex(i) << ", ";
//	}
//	cout << "\nTurn: " << 0 << "\n";
//}