/*
 *  Obelisk the Tormentor
 *	-things to do . tormenting, being an AI, winning, seg faulting, bad variables
 *  Proundly Created by Daniel Lau z3376221 & Wei Hong Chen zsomething
 *  what to write here? hello, it"s me. you"re way ahead of the curve.
 *	THU 12 MAY 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "Game.h"
#include "mechanicalTurk.h"

#define NUM_REGIONS 19
#define NUM_PLAYERS 2

#define STARTING_CAMPUS_A1 "RB"
#define STARTING_CAMPUS_A2 "LRLRLRRLRLRRLRL"
#define STARTING_CAMPUS_B1 "RRLRLL"
#define STARTING_CAMPUS_B2 "LRLRLRRLRL"
#define STARTING_CAMPUS_C1 "RRLRLLRLRLL"
#define STARTING_CAMPUS_C2 "LRLRL"

#define PATH_LIMIT 150
		/*
		left side A B C starting arcs
		nextAction.destination[0] = ";
		nextAction.destination[0] = "RRLRLL";
		nextAction.destination[0] = "RRLRLLRLRLL";

		right side C B A starting arcs
		nextAction.destination[0] = "LRLRL";
		nextAction.destination[0] = "LRLRLRRLRL";
		nextAction.destination[0] = "LRLRLRRLRLRRLRL";
		*/
    typedef struct _p {
    	int ottBPS;
		int ottBQN;
		int ottMJ;
		int ottMTV;
		int ottMMONEY;

    } p;

    typedef struct _region {
    	int diceValue;
    	int discipline;
    	int rollWeighting;
    } region;

    typedef struct _game {
    	region board[NUM_REGIONS];
    	p uni[NUM_PLAYERS];
    } game;

    game i;

action decideAction (Game g) {

	action nextAction;
    nextAction.actionCode = PASS;
    //weird structs

    int counter = 0;


    //assigning rollweighting to every region
    while (counter < 19) {
    	i.board[counter].diceValue = getDiceValue (g, counter);
    	i.board[counter].discipline = getDiscipline (g, counter);
    	if (i.board[counter].diceValue == 2) {
    		i.board[counter].rollWeighting = 1;
    	} else if (i.board[counter].diceValue == 3) {
    		i.board[counter].rollWeighting = 2;
    	} else if (i.board[counter].diceValue == 4) {
    		i.board[counter].rollWeighting = 3;
    	} else if (i.board[counter].diceValue == 5) {
    		i.board[counter].rollWeighting = 4;
    	} else if (i.board[counter].diceValue == 6) {
    		i.board[counter].rollWeighting = 5;
    	} else if (i.board[counter].diceValue == 7) {
    		i.board[counter].rollWeighting = 0;
    	} else if (i.board[counter].diceValue == 8) {
    		i.board[counter].rollWeighting = 5;
    	} else if (i.board[counter].diceValue == 9) {
    		i.board[counter].rollWeighting = 4;
    	} else if (i.board[counter].diceValue == 10) {
    		i.board[counter].rollWeighting = 3;
    	} else if (i.board[counter].diceValue == 11) {
    		i.board[counter].rollWeighting = 2;
    	} else if (i.board[counter].diceValue == 12) {
    		i.board[counter].rollWeighting = 1;
    	}

    	if (i.board[counter].discipline == STUDENT_BPS) {
    		i.board[counter].rollWeighting += 2;
    	} else if (i.board[counter].discipline == STUDENT_BQN) {
    		i.board[counter].rollWeighting += 2;
    	} else if (i.board[counter].discipline == STUDENT_MJ) {
    		i.board[counter].rollWeighting += 1;
    	} else if (i.board[counter].discipline == STUDENT_MTV) {
    		i.board[counter].rollWeighting += 1;
    	} else if (i.board[counter].discipline == STUDENT_MMONEY) {
    		i.board[counter].rollWeighting += 1;
		}
    	counter++;
    }

	int currentPlayer = getWhoseTurn (g);
	int currentTurn = getTurnNumber (g);

	//figuring out which player is 1 and which player is 2
	int enemyPlayer = 0;

	if (currentPlayer == 1) {
		enemyPlayer = 2;
	} else if (currentPlayer == 2) {
		enemyPlayer = 1;
	}

	//defining variables
	//not sure if i should do it this way
	//might change it later into the player struct
	int mostARCs = getMostARCs (g);
	int ottARCs = getARCs (g, currentPlayer);
	int enemyARCs = getARCs (g, enemyPlayer);

	int mostPublications = getMostPublications (g);
	int ottPublications = getPublications (g, currentPlayer);
	int enemyPublications = getPublications (g, enemyPlayer);

	int ottKPI = getKPIpoints (g, currentPlayer);
	int enemyKPI = getKPIpoints (g, enemyPlayer);

	int ottGO8s = getGO8s (g, currentPlayer);
	int enemyGO8s = getGO8s (g, enemyPlayer);
	int totalGO8s = ottGO8s + enemyGO8s;

	int ottCampuses = getCampuses (g, currentPlayer);
	int enemyCampuses = getCampuses (g, enemyPlayer);

	counter = 0;

	while (counter < NUM_PLAYERS) {
		i.uni[counter].ottBPS = getStudents (g, counter, STUDENT_BPS);
		i.uni[counter].ottBQN = getStudents (g, counter, STUDENT_BQN);
		i.uni[counter].ottMJ = getStudents (g, counter, STUDENT_MJ);
		i.uni[counter].ottMTV = getStudents (g, counter, STUDENT_MTV);
		i.uni[counter].ottMMONEY = getStudents (g, counter, STUDENT_MMONEY);
	}
	
	int missingDiscipline = 0;

	

	//Hardcoded first turn
	if (currentTurn < 3) {
		nextAction.actionCode = OBTAIN_ARC;
		//starting position A
		if ( (getCampus (g, "")) == currentPlayer ) {
			if ( (i.board[3].rollWeighting + i.board[7].rollWeighting + 
				  i.board[8].rollWeighting) >
				  (i.board[10].rollWeighting + i.board[11].rollWeighting +
				  i.board[15].rollWeighting) ) {
				strcpy (nextAction.destination, "R");
				makeAction (g, nextAction);
				strcpy (nextAction.destination, "RL");
				
			} else if ( (i.board[3].rollWeighting + i.board[7].rollWeighting + 
				  		 i.board[8].rollWeighting) <
						 (i.board[10].rollWeighting + i.board[11].rollWeighting +
				         i.board[15].rollWeighting) ) {
				strcpy (nextAction.destination, "LRLRLRRLRLRRLRL");
				makeAction (g, nextAction);
				strcpy (nextAction.destination, "LRLRLRRLRLRRLRR");	
			}	
		// starting position B
		} else if ( (getCampus (g, "RRLRL")) == currentPlayer ) {
			if ( (i.board[0].rollWeighting + i.board[1].rollWeighting + 
				  i.board[4].rollWeighting) >
				  (i.board[18].rollWeighting + i.board[17].rollWeighting +
				  i.board[14].rollWeighting) ) {
				strcpy (nextAction.destination, "RRLRLL");
				makeAction (g, nextAction);
				strcpy (nextAction.destination, "RRLRLLL");
			} else if ( (i.board[0].rollWeighting + i.board[1].rollWeighting + 
				  		 i.board[4].rollWeighting) <
				  		 (i.board[18].rollWeighting + i.board[17].rollWeighting +
				 		 i.board[14].rollWeighting) ) {
				strcpy (nextAction.destination, "LRLRLRRLRL");
				makeAction (g, nextAction);
				strcpy (nextAction.destination, "LRLRLRRLRR");
			}
		// starting position C
		} else if ( (getCampus (g, "LRLRL")) == currentPlayer ) {
			if ( (i.board[2].rollWeighting + i.board[5].rollWeighting + 
				  i.board[6].rollWeighting) >
				  (i.board[16].rollWeighting + i.board[13].rollWeighting +
				  i.board[12].rollWeighting) ) {
				strcpy (nextAction.destination, "RRLRLLRLRLL");
				makeAction (g, nextAction);
				strcpy (nextAction.destination, "RRLRLLRLRLLL");
			} else if ( (i.board[2].rollWeighting + i.board[5].rollWeighting + 
				  		 i.board[6].rollWeighting) >
				  		 (i.board[16].rollWeighting + i.board[13].rollWeighting +
				  		 i.board[12].rollWeighting) ) {
				strcpy (nextAction.destination, "LRLRL");
				makeAction (g, nextAction);
				strcpy (nextAction.destination, "LRLRR");
		}
		makeAction (g, nextAction);
		nextAction.actionCode = BUILD_CAMPUS;
		makeAction (g, nextAction);
	
	//exchanging M$ & MTV for other resources
		//i think this is wrong, top kek, it"s wrong,
		if (i.uni[currentPlayer].ottMMONEY >= ottExchangeRateMMONEY) {
			if (i.uni[currentPlayer].ottBPS > i.uni[currentPlayer].ottBQN) {
				nextAction.disciplineTo = STUDENT_BQN;
			} else if (i.uni[currentPlayer].ottBPS < i.uni[currentPlayer].ottBQN) {
				nextAction.disciplineTo = STUDENT_BPS;
			}
		}
		nextAction.actionCode = RETRAIN_STUDENTS;
		nextAction.disciplineFrom = STUDENT_MMONEY;
		makeAction (g, nextAction);
		
		if (i.uni[currentPlayer].ottMTV >= ottExchangeRateMTV) {
			if (i.uni[currentPlayer].ottBPS > i.uni[currentPlayer].ottBQN) {
				nextAction.disciplineTo = STUDENT_BQN;
			} else if (i.uni[currentPlayer].ottBPS < i.uni[currentPlayer].ottBQN) {
				nextAction.disciplineTo = STUDENT_BPS;
			}
		}
		nextAction.disciplineFrom = STUDENT_MTV;
		makeAction (g, nextAction);
	}





    return nextAction;
	}
}
//weird function, actions N NE SE S SW NW probs need an array or something
//not sure what ahppening, needs array to be run multiple times
action buildLoc (path campusLocations[getCampuses(g, currentPlayer)]) {

	int counter = 0;

	action actionN, actionNE, actionSE, actionS, actionSW, actionNW;

	actionN.actionCode = BUILD_CAMPUS;
	actionNE.actionCode = BUILD_CAMPUS;
	actionSE.actionCode = BUILD_CAMPUS;
	actionS.actionCode = BUILD_CAMPUS;
	actionSW.actionCode = BUILD_CAMPUS;
	actionNW.actionCode = BUILD_CAMPUS;


	int pathlength;

	while (counter < ottCampuses) {

		strlen (campusLocations[counter]) = pathlength;

		strcpy (actionN.destination, campusLocations[counter]);
		strcpy (actionNE.destination, campusLocations[counter]);
		strcpy (actionSE.destination, campusLocations[counter]);
		strcpy (actionS.destination, campusLocations[counter]);
		strcpy (actionSW.destination, campusLocations[counter]);
		strcpy (actionNW.destination, campusLocations[counter]);
	}

	actionN.destination[pathlength + 1] = 'R';
	actionN.destination[pathlength + 2] = 'L';

	actionNE.destination[pathlength + 1] = 'R';
	actionNE.destination[pathlength + 2] = 'R';

	actionSE.destination[pathlength + 1] = 'R';
	actionSE.destination[pathlength + 2] = 'R';
	actionSE.destination[pathlength + 3] = 'R';
	actionSE.destination[pathlength + 4] = 'R';

	actionS.destination[pathlength + 1] = 'L';
	actionS.destination[pathlength + 2] = 'L';
	actionS.destination[pathlength + 3] = 'L';
	actions.destination[pathlength + 4] = 'L';

	actionSW.destination[pathlength + 1] = 'L';
	actionSW.destination[pathlength + 2] = 'L';

	actionNW.destination[pathlength + 1] = 'L';
	actionNW.destination[pathlength + 2] = 'R';

	//isLegalAction (g, actionN) =
}
//for a certain region it compares the regions surrounding it and returns highest
//valued region, didn"t implement when one region = region in terms of weighting
//will fail, needs to take into account when regionvalue = regionvalue
int checkRegion (path a) {

	int buildRegion;

	if (getCampus (g, STARTING_CAMPUS_A1) == currentPlayer) {
		int startingRegion = 7;
		if (startingRegion == 0) {
			if (i.board[1].rollWeighting > i.board[3].rollWeighting &&
				i.board[1].rollWeighting > i.board[4].rollWeighting) {
				buildRegion = 1;
			} else if (i.board[3].rollWeighting > i.board[3].rollWeighting &&
					   i.board[3].rollWeighting > i.board[1].rollWeighting) {
				buildRegion = 3;
			} else if (i.board[4].rollWeighting > i.board[1].rollWeighting &&
					   i.board[4].rollWeighting > i.board[3].rollWeighting) {
				buildRegion = 4;
			}
		} else if (startingRegion == 1) {
			if (i.board[0].rollWeighting > i.board[4].rollWeighting &&
				i.board[0].rollWeighting > i.board[5].rollWeighting &&
				i.board[0].rollWeighting > i.board[2].rollWeighting) {
				buildRegion = 0;
			} else if (i.board[4].rollWeighting > i.board[0].rollWeighting &&
					   i.board[4].rollWeighting > i.board[5].rollWeighting &&
				       i.board[4].rollWeighting > i.board[2].rollWeighting) {
				buildRegion = 4;
			} else if (i.board[5].rollWeighting > i.board[0].rollWeighting &&
					   i.board[5].rollWeighting > i.board[4].rollWeighting &&
				       i.board[5].rollWeighting > i.board[2].rollWeighting) {
				buildRegion = 5;
			} else if (i.board[2].rollWeighting > i.board[0].rollWeighting &&
					   i.board[2].rollWeighting > i.board[5].rollWeighting &&
				       i.board[2].rollWeighting > i.board[4].rollWeighting) {
				buildRegion = 2;
			} 
		} else if (startingRegion == 2) {
			if (i.board[1].rollWeighting > i.board[5].rollWeighting &&
				i.board[1].rollWeighting > i.board[6].rollWeighting) {
				buildRegion = 1;
			} else if (i.board[5].rollWeighting > i.board[6].rollWeighting &&
					   i.board[5].rollWeighting > i.board[1].rollWeighting) {
				buildRegion = 5;
			} else if (i.board[6].rollWeighting > i.board[5].rollWeighting &&
					   i.board[6].rollWeighting > i.board[1].rollWeighting) {
				buildRegion = 6;
			}
		} else if (startingRegion == 3) {
			if (i.board[0].rollWeighting > i.board[4].rollWeighting &&
				i.board[0].rollWeighting > i.board[7].rollWeighting &&
				i.board[0].rollWeighting > i.board[8].rollWeighting) {
				buildRegion = 0;
			} else if (i.board[4].rollWeighting > i.board[0].rollWeighting &&
					   i.board[4].rollWeighting > i.board[7].rollWeighting &&
				       i.board[4].rollWeighting > i.board[8].rollWeighting) {
				buildRegion = 4;
			} else if (i.board[7].rollWeighting > i.board[0].rollWeighting &&
					   i.board[7].rollWeighting > i.board[4].rollWeighting &&
				       i.board[7].rollWeighting > i.board[8].rollWeighting) {
				buildRegion = 7;
			} else if (i.board[8].rollWeighting > i.board[0].rollWeighting &&
					   i.board[8].rollWeighting > i.board[8].rollWeighting &&
				       i.board[8].rollWeighting > i.board[4].rollWeighting) {
				buildRegion = 2;
			} 
		} else if (startingRegion == 4) {
			if (i.board[0].rollWeighting > i.board[1].rollWeighting &&
				i.board[0].rollWeighting > i.board[3].rollWeighting &&
				i.board[0].rollWeighting > i.board[5].rollWeighting &&
				i.board[0].rollWeighting > i.board[8].rollWeighting &&
				i.board[0].rollWeighting > i.board[9].rollWeighting) {
				buildRegion = 0;
			} else if (i.board[1].rollWeighting > i.board[0].rollWeighting &&
					   i.board[1].rollWeighting > i.board[3].rollWeighting &&
				       i.board[1].rollWeighting > i.board[5].rollWeighting &&
				       i.board[1].rollWeighting > i.board[8].rollWeighting &&
				       i.board[1].rollWeighting > i.board[9].rollWeighting) {
				buildRegion = 1;
			} else if (i.board[3].rollWeighting > i.board[0].rollWeighting &&
					   i.board[3].rollWeighting > i.board[5].rollWeighting &&
				       i.board[3].rollWeighting > i.board[8].rollWeighting &&
				       i.board[3].rollWeighting > i.board[9].rollWeighting &&
				       i.board[3].rollWeighting > i.board[1].rollWeighting) {
				buildRegion = 3;
			} else if (i.board[5].rollWeighting > i.board[0].rollWeighting &&
					   i.board[5].rollWeighting > i.board[3].rollWeighting &&
				       i.board[5].rollWeighting > i.board[8].rollWeighting &&
				       i.board[5].rollWeighting > i.board[9].rollWeighting &&
				       i.board[5].rollWeighting > i.board[1].rollWeighting) {
				buildRegion = 5;
			} else if (i.board[8].rollWeighting > i.board[0].rollWeighting &&
					   i.board[8].rollWeighting > i.board[3].rollWeighting &&
				       i.board[8].rollWeighting > i.board[5].rollWeighting &&
				       i.board[8].rollWeighting > i.board[1].rollWeighting &&
				       i.board[8].rollWeighting > i.board[9].rollWeighting) {
				buildRegion = 8;
			} else if (i.board[9].rollWeighting > i.board[0].rollWeighting &&
					   i.board[9].rollWeighting > i.board[1].rollWeighting &&
				       i.board[9].rollWeighting > i.board[3].rollWeighting &&
				       i.board[9].rollWeighting > i.board[5].rollWeighting &&
				       i.board[9].rollWeighting > i.board[8].rollWeighting) {
				buildRegion = 9;	
		} else if (startingRegion == 5) {
			if (i.board[1].rollWeighting > i.board[2].rollWeighting &&
				i.board[1].rollWeighting > i.board[4].rollWeighting &&
				i.board[1].rollWeighting > i.board[6].rollWeighting &&
				i.board[1].rollWeighting > i.board[9].rollWeighting &&
				i.board[1].rollWeighting > i.board[10].rollWeighting) {
				buildRegion = 1;
			} else if (i.board[2].rollWeighting > i.board[4].rollWeighting &&
					   i.board[2].rollWeighting > i.board[6].rollWeighting &&
				       i.board[2].rollWeighting > i.board[9].rollWeighting &&
				       i.board[2].rollWeighting > i.board[10].rollWeighting &&
				       i.board[2].rollWeighting > i.board[1].rollWeighting) {
				buildRegion = 2;
			} else if (i.board[4].rollWeighting > i.board[2].rollWeighting &&
					   i.board[4].rollWeighting > i.board[6].rollWeighting &&
				       i.board[4].rollWeighting > i.board[10].rollWeighting &&
				       i.board[4].rollWeighting > i.board[9].rollWeighting &&
				       i.board[4].rollWeighting > i.board[1].rollWeighting) {
				buildRegion = 4;
			} else if (i.board[6].rollWeighting > i.board[1].rollWeighting &&
					   i.board[6].rollWeighting > i.board[2].rollWeighting &&
				       i.board[6].rollWeighting > i.board[4].rollWeighting &&
				       i.board[6].rollWeighting > i.board[9].rollWeighting &&
				       i.board[6].rollWeighting > i.board[10].rollWeighting) {
				buildRegion = 6;
			} else if (i.board[10].rollWeighting > i.board[1].rollWeighting &&
					   i.board[10].rollWeighting > i.board[2].rollWeighting &&
				       i.board[10].rollWeighting > i.board[4].rollWeighting &&
				       i.board[10].rollWeighting > i.board[6].rollWeighting &&
				       i.board[10].rollWeighting > i.board[9].rollWeighting) {
				buildRegion = 10;
			} else if (i.board[9].rollWeighting > i.board[1].rollWeighting &&
					   i.board[9].rollWeighting > i.board[2].rollWeighting &&
				       i.board[9].rollWeighting > i.board[4].rollWeighting &&
				       i.board[9].rollWeighting > i.board[6].rollWeighting &&
				       i.board[9].rollWeighting > i.board[10].rollWeighting) {
				buildRegion = 9;	
		} else if (startingRegion == 6) {
			if (i.board[2].rollWeighting > i.board[5].rollWeighting &&
				i.board[2].rollWeighting > i.board[10].rollWeighting &&
				i.board[2].rollWeighting > i.board[11].rollWeighting) {
				buildRegion = 2;
			} else if (i.board[5].rollWeighting > i.board[2].rollWeighting &&
					   i.board[5].rollWeighting > i.board[10].rollWeighting &&
				       i.board[5].rollWeighting > i.board[11].rollWeighting) {
				buildRegion = 5;
			} else if (i.board[10].rollWeighting > i.board[2].rollWeighting &&
					   i.board[10].rollWeighting > i.board[5].rollWeighting &&
				       i.board[10].rollWeighting > i.board[11].rollWeighting) {
				buildRegion = 10;
			} else if (i.board[11].rollWeighting > i.board[2].rollWeighting &&
					   i.board[11].rollWeighting > i.board[5].rollWeighting &&
				       i.board[11].rollWeighting > i.board[10].rollWeighting) {
				buildRegion = 11;
			} 
		} else if (startingRegion == 7) {
			if (i.board[3].rollWeighting > i.board[8].rollWeighting &&
				i.board[3].rollWeighting > i.board[12].rollWeighting) {
				buildRegion = 3;
			} else if (i.board[8].rollWeighting > i.board[3].rollWeighting &&
					   i.board[8].rollWeighting > i.board[12].rollWeighting) {
				buildRegion = 8;
			} else if (i.board[12].rollWeighting > i.board[8].rollWeighting &&
					   i.board[12].rollWeighting > i.board[3].rollWeighting) {
				buildRegion = 12;
			}
		} else if (startingRegion == 8) {
			if (i.board[3].rollWeighting > i.board[4].rollWeighting &&
				i.board[3].rollWeighting > i.board[7].rollWeighting &&
				i.board[3].rollWeighting > i.board[9].rollWeighting &&
				i.board[3].rollWeighting > i.board[12].rollWeighting &&
				i.board[3].rollWeighting > i.board[13].rollWeighting) {
				buildRegion = 3;
			} else if (i.board[4].rollWeighting > i.board[3].rollWeighting &&
					   i.board[4].rollWeighting > i.board[7].rollWeighting &&
				       i.board[4].rollWeighting > i.board[9].rollWeighting &&
				       i.board[4].rollWeighting > i.board[12].rollWeighting &&
				       i.board[4].rollWeighting > i.board[13].rollWeighting) {
				buildRegion = 4;
			} else if (i.board[7].rollWeighting > i.board[3].rollWeighting &&
					   i.board[7].rollWeighting > i.board[4].rollWeighting &&
				       i.board[7].rollWeighting > i.board[9].rollWeighting &&
				       i.board[7].rollWeighting > i.board[12].rollWeighting &&
				       i.board[7].rollWeighting > i.board[13].rollWeighting) {
				buildRegion = 7;
			} else if (i.board[9].rollWeighting > i.board[3].rollWeighting &&
					   i.board[9].rollWeighting > i.board[4].rollWeighting &&
				       i.board[9].rollWeighting > i.board[7].rollWeighting &&
				       i.board[9].rollWeighting > i.board[12].rollWeighting &&
				       i.board[9].rollWeighting > i.board[13].rollWeighting) {
				buildRegion = 9;
			} else if (i.board[12].rollWeighting > i.board[3].rollWeighting &&
					   i.board[12].rollWeighting > i.board[4].rollWeighting &&
				       i.board[12].rollWeighting > i.board[7].rollWeighting &&
				       i.board[12].rollWeighting > i.board[9].rollWeighting &&
				       i.board[12].rollWeighting > i.board[13].rollWeighting) {
				buildRegion = 12;
			} else if (i.board[13].rollWeighting > i.board[3].rollWeighting &&
					   i.board[13].rollWeighting > i.board[4].rollWeighting &&
				       i.board[13].rollWeighting > i.board[7].rollWeighting &&
				       i.board[13].rollWeighting > i.board[9].rollWeighting &&
				       i.board[13].rollWeighting > i.board[12].rollWeighting) {
				buildRegion = 13;
			}
		} else if (startingRegion == 9) {
			if (i.board[4].rollWeighting > i.board[5].rollWeighting &&
				i.board[4].rollWeighting > i.board[8].rollWeighting &&
				i.board[4].rollWeighting > i.board[10].rollWeighting &&
				i.board[4].rollWeighting > i.board[13].rollWeighting &&
				i.board[4].rollWeighting > i.board[14].rollWeighting) {
				buildRegion = 4;
			} else if (i.board[5].rollWeighting > i.board[4].rollWeighting &&
					   i.board[5].rollWeighting > i.board[8].rollWeighting &&
				       i.board[5].rollWeighting > i.board[10].rollWeighting &&
				       i.board[5].rollWeighting > i.board[13].rollWeighting &&
				       i.board[5].rollWeighting > i.board[14].rollWeighting) {
				buildRegion = 5;
			} else if (i.board[8].rollWeighting > i.board[4].rollWeighting &&
					   i.board[8].rollWeighting > i.board[5].rollWeighting &&
				       i.board[8].rollWeighting > i.board[9].rollWeighting &&
				       i.board[8].rollWeighting > i.board[13].rollWeighting &&
				       i.board[8].rollWeighting > i.board[14].rollWeighting) {
				buildRegion = 8;
			} else if (i.board[10].rollWeighting > i.board[4].rollWeighting &&
					   i.board[10].rollWeighting > i.board[5].rollWeighting &&
				       i.board[10].rollWeighting > i.board[8].rollWeighting &&
				       i.board[10].rollWeighting > i.board[13].rollWeighting &&
				       i.board[10].rollWeighting > i.board[14].rollWeighting) {
				buildRegion = 10;
			} else if (i.board[13].rollWeighting > i.board[4].rollWeighting &&
					   i.board[13].rollWeighting > i.board[5].rollWeighting &&
				       i.board[13].rollWeighting > i.board[8].rollWeighting &&
				       i.board[13].rollWeighting > i.board[10].rollWeighting &&
				       i.board[13].rollWeighting > i.board[14].rollWeighting) {
				buildRegion = 13;
			} else if (i.board[14].rollWeighting > i.board[4].rollWeighting &&
					   i.board[14].rollWeighting > i.board[5].rollWeighting &&
				       i.board[14].rollWeighting > i.board[8].rollWeighting &&
				       i.board[14].rollWeighting > i.board[10].rollWeighting &&
				       i.board[14].rollWeighting > i.board[13].rollWeighting) {
				buildRegion = 14;
			}
		} else if (startingRegion == 10) {
			if (i.board[5].rollWeighting > i.board[6].rollWeighting &&
				i.board[5].rollWeighting > i.board[9].rollWeighting &&
				i.board[5].rollWeighting > i.board[11].rollWeighting &&
				i.board[5].rollWeighting > i.board[14].rollWeighting &&
				i.board[5].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 5;
			} else if (i.board[6].rollWeighting > i.board[5].rollWeighting &&
					   i.board[6].rollWeighting > i.board[9].rollWeighting &&
				       i.board[6].rollWeighting > i.board[11].rollWeighting &&
				       i.board[6].rollWeighting > i.board[14].rollWeighting &&
				       i.board[6].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 6;
			} else if (i.board[9].rollWeighting > i.board[5].rollWeighting &&
					   i.board[9].rollWeighting > i.board[6].rollWeighting &&
				       i.board[9].rollWeighting > i.board[11].rollWeighting &&
				       i.board[9].rollWeighting > i.board[14].rollWeighting &&
				       i.board[9].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 9;
			} else if (i.board[11].rollWeighting > i.board[5].rollWeighting &&
					   i.board[11].rollWeighting > i.board[6].rollWeighting &&
				       i.board[11].rollWeighting > i.board[11].rollWeighting &&
				       i.board[11].rollWeighting > i.board[14].rollWeighting &&
				       i.board[11].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 11;
			} else if (i.board[14].rollWeighting > i.board[5].rollWeighting &&
					   i.board[14].rollWeighting > i.board[6].rollWeighting &&
				       i.board[14].rollWeighting > i.board[9].rollWeighting &&
				       i.board[14].rollWeighting > i.board[11].rollWeighting &&
				       i.board[14].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 14;
			} else if (i.board[15].rollWeighting > i.board[5].rollWeighting &&
					   i.board[15].rollWeighting > i.board[6].rollWeighting &&
				       i.board[15].rollWeighting > i.board[9].rollWeighting &&
				       i.board[15].rollWeighting > i.board[11].rollWeighting &&
				       i.board[15].rollWeighting > i.board[14].rollWeighting) {
				buildRegion = 15;
			}
		} else if (startingRegion == 11) {
			if (i.board[6].rollWeighting > i.board[10].rollWeighting &&
				i.board[6].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 6;
			} else if (i.board[10].rollWeighting > i.board[6].rollWeighting &&
					   i.board[10].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 10;
			} else if (i.board[15].rollWeighting > i.board[6].rollWeighting &&
					   i.board[15].rollWeighting > i.board[10].rollWeighting) {
				buildRegion = 15;
			}
		} else if (startingRegion == 12) {
			if (i.board[7].rollWeighting > i.board[8].rollWeighting &&
				i.board[7].rollWeighting > i.board[13].rollWeighting &&
				i.board[7].rollWeighting > i.board[16].rollWeighting) {
				buildRegion = 7;
			} else if (i.board[8].rollWeighting > i.board[7].rollWeighting &&
					   i.board[8].rollWeighting > i.board[13].rollWeighting &&
				       i.board[8].rollWeighting > i.board[16].rollWeighting) {
				buildRegion = 8;
			} else if (i.board[13].rollWeighting > i.board[7].rollWeighting &&
					   i.board[13].rollWeighting > i.board[8].rollWeighting &&
				       i.board[13].rollWeighting > i.board[16].rollWeighting) {
				buildRegion = 13;
			} else if (i.board[16].rollWeighting > i.board[7].rollWeighting &&
					   i.board[16].rollWeighting > i.board[8].rollWeighting &&
				       i.board[16].rollWeighting > i.board[13].rollWeighting) {
				buildRegion = 16;
			} 
		} else if (startingRegion == 13) {
			if (i.board[8].rollWeighting > i.board[9].rollWeighting &&
				i.board[8].rollWeighting > i.board[12].rollWeighting &&
				i.board[8].rollWeighting > i.board[14].rollWeighting &&
				i.board[8].rollWeighting > i.board[16].rollWeighting &&
				i.board[8].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 8;
			} else if (i.board[9].rollWeighting > i.board[8].rollWeighting &&
					   i.board[9].rollWeighting > i.board[12].rollWeighting &&
				       i.board[9].rollWeighting > i.board[14].rollWeighting &&
				       i.board[9].rollWeighting > i.board[16].rollWeighting &&
				       i.board[9].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 9;
			} else if (i.board[12].rollWeighting > i.board[8].rollWeighting &&
					   i.board[12].rollWeighting > i.board[9].rollWeighting &&
				       i.board[12].rollWeighting > i.board[14].rollWeighting &&
				       i.board[12].rollWeighting > i.board[16].rollWeighting &&
				       i.board[12].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 12;
			} else if (i.board[14].rollWeighting > i.board[8].rollWeighting &&
					   i.board[14].rollWeighting > i.board[9].rollWeighting &&
				       i.board[14].rollWeighting > i.board[12].rollWeighting &&
				       i.board[14].rollWeighting > i.board[16].rollWeighting &&
				       i.board[14].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 14;
			} else if (i.board[16].rollWeighting > i.board[8].rollWeighting &&
					   i.board[16].rollWeighting > i.board[9].rollWeighting &&
				       i.board[16].rollWeighting > i.board[12].rollWeighting &&
				       i.board[16].rollWeighting > i.board[14].rollWeighting &&
				       i.board[16].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 16;
			} else if (i.board[17].rollWeighting > i.board[8].rollWeighting &&
					   i.board[17].rollWeighting > i.board[9].rollWeighting &&
				       i.board[17].rollWeighting > i.board[12].rollWeighting &&
				       i.board[17].rollWeighting > i.board[14].rollWeighting &&
				       i.board[17].rollWeighting > i.board[16].rollWeighting) {
				buildRegion = 17;
			}
		} else if (startingRegion == 14) {
			if (i.board[9].rollWeighting > i.board[10].rollWeighting &&
				i.board[9].rollWeighting > i.board[12].rollWeighting &&
				i.board[9].rollWeighting > i.board[13].rollWeighting &&
				i.board[9].rollWeighting > i.board[15].rollWeighting &&
				i.board[9].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 9;
			} else if (i.board[10].rollWeighting > i.board[9].rollWeighting &&
					   i.board[10].rollWeighting > i.board[12].rollWeighting &&
				       i.board[10].rollWeighting > i.board[13].rollWeighting &&
				       i.board[10].rollWeighting > i.board[15].rollWeighting &&
				       i.board[10].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 10;
			} else if (i.board[12].rollWeighting > i.board[9].rollWeighting &&
					   i.board[12].rollWeighting > i.board[10].rollWeighting &&
				       i.board[12].rollWeighting > i.board[13].rollWeighting &&
				       i.board[12].rollWeighting > i.board[15].rollWeighting &&
				       i.board[12].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 12;
			} else if (i.board[13].rollWeighting > i.board[9].rollWeighting &&
					   i.board[13].rollWeighting > i.board[10].rollWeighting &&
				       i.board[13].rollWeighting > i.board[12].rollWeighting &&
				       i.board[13].rollWeighting > i.board[15].rollWeighting &&
				       i.board[13].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 13;
			} else if (i.board[15].rollWeighting > i.board[9].rollWeighting &&
					   i.board[15].rollWeighting > i.board[10].rollWeighting &&
				       i.board[15].rollWeighting > i.board[12].rollWeighting &&
				       i.board[15].rollWeighting > i.board[13].rollWeighting &&
				       i.board[15].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 15;
			} else if (i.board[18].rollWeighting > i.board[9].rollWeighting &&
					   i.board[18].rollWeighting > i.board[10].rollWeighting &&
				       i.board[18].rollWeighting > i.board[12].rollWeighting &&
				       i.board[18].rollWeighting > i.board[13].rollWeighting &&
				       i.board[18].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 18;
			}
		} else if (startingRegion == 15) {
			if (i.board[10].rollWeighting > i.board[11].rollWeighting &&
				i.board[10].rollWeighting > i.board[14].rollWeighting &&
				i.board[10].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 10;
			} else if (i.board[11].rollWeighting > i.board[10].rollWeighting &&
					   i.board[11].rollWeighting > i.board[14].rollWeighting &&
				       i.board[11].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 11;
			} else if (i.board[14].rollWeighting > i.board[10].rollWeighting &&
					   i.board[14].rollWeighting > i.board[11].rollWeighting &&
				       i.board[14].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 14;
			} else if (i.board[18].rollWeighting > i.board[10].rollWeighting &&
					   i.board[18].rollWeighting > i.board[11].rollWeighting &&
				       i.board[18].rollWeighting > i.board[14].rollWeighting) {
				buildRegion = 18;
			} 
		} else if (startingRegion == 16) {
			if (i.board[12].rollWeighting > i.board[13].rollWeighting &&
				i.board[12].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 12;
			} else if (i.board[13].rollWeighting > i.board[12].rollWeighting &&
					   i.board[13].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 13;
			} else if (i.board[17].rollWeighting > i.board[12].rollWeighting &&
					   i.board[17].rollWeighting > i.board[13].rollWeighting) {
				buildRegion = 17;
			}
		} else if (startingRegion == 17) {
			if (i.board[13].rollWeighting > i.board[14].rollWeighting &&
				i.board[13].rollWeighting > i.board[16].rollWeighting &&
				i.board[13].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 13;
			} else if (i.board[14].rollWeighting > i.board[13].rollWeighting &&
					   i.board[14].rollWeighting > i.board[16].rollWeighting &&
				       i.board[14].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 14;
			} else if (i.board[16].rollWeighting > i.board[13].rollWeighting &&
					   i.board[16].rollWeighting > i.board[14].rollWeighting &&
				       i.board[16].rollWeighting > i.board[18].rollWeighting) {
				buildRegion = 16;
			} else if (i.board[18].rollWeighting > i.board[13].rollWeighting &&
					   i.board[18].rollWeighting > i.board[14].rollWeighting &&
				       i.board[18].rollWeighting > i.board[16].rollWeighting) {
				buildRegion = 18;
			} 
		} else if (startingRegion == 18) {
			if (i.board[14].rollWeighting > i.board[15].rollWeighting &&
				i.board[14].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 14;
			} else if (i.board[15].rollWeighting > i.board[14].rollWeighting &&
					   i.board[15].rollWeighting > i.board[17].rollWeighting) {
				buildRegion = 15;
			} else if (i.board[17].rollWeighting > i.board[14].rollWeighting &&
					   i.board[17].rollWeighting > i.board[15].rollWeighting) {
				buildRegion = 17;
			}
		}
		return buildRegion;
	}
}














