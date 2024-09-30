# Mensch Ärgere Dich Nicht - Optimal Strategy
---
This repository provides the code for computing the optimal strategy for the 2-player variant of the famous German board game ["Mensch Ärgere Dich Nicht"](https://en.wikipedia.org/wiki/Mensch_%C3%A4rgere_Dich_nicht).

## Rule Set
The code follows the [official rule set](https://www.schmidtspiele.de/files/Produkte/4/49020%20-%20Standardausgabe/49020_49021_Mensch_aergere_Dich_nicht_DE.pdf) of the game and did not implement commonly added rules such as:
- forced capture
- triple die roll
- capturing backwards

As the rule set is not fully specified, some decisions had to be made:
- rolling a 6 *always* permits you to roll again.
- the finish squares have to be reached *exactly* (cannot decrease the rolled number)

## Method
TODO

## Results
The winning probabilities converged after around 140 compute hours on a 48 core machine with a clock rate of 2.1Ghz. The resulting file containing the winning probabilities for each of the 16053053985 positions (taking out the symmetries) is around 64 GB in size and too large to be stored here. Instead the ```res``` folder contains the probabilties for the subgame (2-Game) with only 2 pieces per player.

**I have created a website ([madnsolver.de](https://www.madnsolver.de)) to visualize the results and make them accessible to the public.** Additionally, you can see a light analysis of the results in Chapter 5 of my thesis (```thesis.pdf```) .

## About
This work is the result of my Bachelor Thesis in Computer Science I conducted at the RWTH Aachen University at the chair for [Theoretical Computer Science](https://tcs.rwth-aachen.de/index.php) supervised by Prof. Peter Rossmanith and Dr. Henri Lotze in the summer term 2019.
