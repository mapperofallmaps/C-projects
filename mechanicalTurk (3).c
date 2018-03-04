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

#define NUMBER_OF_DISCIPLINES 6

#define STARTING_CAMPUS_A1 "LB"
#define STARTING_ARC1_A1 "R"
#define STARTING_ARC2_A1 "RL"
#define STARTING_CAMPUS_A2 "RLRLRLRLRLL"
#define STARTING_ARC1_A2 "RLRLRLRLRLLL"
#define STARTING_ARC2_A2 "RLRLRLRLRLLLL"
#define STARTING_CAMPUS_B1 "RRLRL"
#define STARTING_ARC1_B1 "RRLRLL"
#define STARTING_ARC2_B1 "RRLRLLL"
#define STARTING_CAMPUS_B2 "LRLRLRRLRL"
#define STARTING_ARC1_B2 "LRLRLRRLRLB"
#define STARTING_ARC2_B2 "LRLRLRRLRLBL"
#define STARTING_CAMPUS_C1 "LRLRL"
#define STARTING_ARC1_C1 "LRLRLB"
#define STARTING_ARC2_C1 "LRLRLBL"
#define STARTING_CAMPUS_C2 "RRLRLLRLRL"
#define STARTING_ARC1_C2 "RRLRLLRLRLL"
#define STARTING_ARC2_C2 "RRLRLLRLRLLL"



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
typedef struct _player {
    int discipline[NUMBER_OF_DISCIPLINES];
    int kpi;
    int numCampuses;
    int numGO8s;
    int numARCs;
    int numPublications;
    int numPatents;
    int exchangeRate[NUMBER_OF_DISCIPLINES];
} player;

typedef struct _campusLocation {
    path campusLocations[100];
} campusLocation;

typedef struct _region {
    int diceValue;
    int discipline;
    int rollWeighting;
} region;

typedef struct _gameInfo {
    region board[NUM_REGIONS];
    player uni[NUM_UNIS];
    int currentTurn;
    int currentPlayer;
    int mostPublications;
    int mostARCs;
    int numGO8s;
} gameInfo;

action testLegality (Game g, action nextAction);
action simpleExchange (Game g, gameInfo i);
action simpleSpinoff (Game g, gameInfo i);
campusLocation getCampusLocation (Game g);
int getRollWeighting (int region, path vertexLoc, gameInfo i);


action decideAction (Game g) {
    int counter = 0;
    int playerIndex = 1;
    int studentIndex = 1;
    int enemy1 = 0;
    int enemy2 = 0;
    int ourARC = VACANT_ARC;
    int ourCampus = VACANT_VERTEX;
    int ourGO8 = VACANT_VERTEX;
    
    gameInfo i;
    i.currentTurn = getTurnNumber (g);
    i.currentPlayer = getWhoseTurn (g);
    i.mostARCs = getMostARCs (g);
    i.mostPublications = getMostPublications (g);
    i.numGO8s = getGO8s (g, UNI_A) + 
                getGO8s (g, UNI_B) + 
                getGO8s (g, UNI_C);
    
    action nextAction;
    nextAction.actionCode = PASS;
    
    if (i.currentPlayer == UNI_A) {
        enemy1 = UNI_B;
        enemy2 = UNI_C;
        ourARC = ARC_A;
        ourCampus = CAMPUS_A;
        ourGO8 = GO8_A;
    } else if (i.currentPlayer == UNI_B) {
        enemy1 = UNI_A;
        enemy2 = UNI_C;
        ourARC = ARC_B;
        ourCampus = CAMPUS_B;
        ourGO8 = GO8_B;
    } else if (i.currentPlayer == UNI_C) {
        enemy1 = UNI_A;
        enemy2 = UNI_B;
        ourARC = ARC_C;
        ourCampus = CAMPUS_C;
        ourGO8 = GO8_C;
    }
    
    while (playerIndex <= UNI_C) {
        while (studentIndex <= STUDENT_MMONEY) {
            i.uni[playerIndex].exchangeRate[studentIndex] = 
            getExchangeRate (g, playerIndex, studentIndex, STUDENT_THD);
            studentIndex++;
        }
        
        i.uni[playerIndex].discipline[STUDENT_BPS] = 
        getStudents (g, playerIndex, STUDENT_BPS);
        i.uni[playerIndex].discipline[STUDENT_BQN] = 
        getStudents (g, playerIndex, STUDENT_BQN);
        i.uni[playerIndex].discipline[STUDENT_MJ] = 
        getStudents (g, playerIndex, STUDENT_MJ);
        i.uni[playerIndex].discipline[STUDENT_MTV] = 
        getStudents (g, playerIndex, STUDENT_MTV);
        i.uni[playerIndex].discipline[STUDENT_MMONEY] = 
        getStudents (g, playerIndex, STUDENT_MMONEY);

        studentIndex = 1;
        i.uni[playerIndex].kpi = getKPIpoints (g, playerIndex);
        i.uni[playerIndex].numCampuses = getCampuses (g, playerIndex);
        i.uni[playerIndex].numGO8s = getGO8s (g, playerIndex);
        i.uni[playerIndex].numARCs = getARCs (g, playerIndex);
        i.uni[playerIndex].numPublications = getPublications (g, playerIndex);
        i.uni[playerIndex].numPatents = getIPs (g, playerIndex);

        playerIndex++;
    }
    	    
    
    //assigning rollWeighting to every region
    while (counter < NUM_REGIONS) {
        i.board[counter].diceValue = getDiceValue (g, counter);
        i.board[counter].discipline = getDiscipline (g, counter);
        if (i.board[counter].diceValue == 2 || i.board[counter].diceValue == 12) {
            i.board[counter].rollWeighting = 1;
        } else if (i.board[counter].diceValue == 3 || i.board[counter].diceValue == 11) {
            i.board[counter].rollWeighting = 2;
        } else if (i.board[counter].diceValue == 4 || i.board[counter].diceValue == 10) {
            i.board[counter].rollWeighting = 3;
        } else if (i.board[counter].diceValue == 5 || i.board[counter].diceValue == 9) {
            i.board[counter].rollWeighting = 4;
        } else if (i.board[counter].diceValue == 6 || i.board[counter].diceValue == 8) {
            i.board[counter].rollWeighting = 5;
        } else if (i.board[counter].diceValue == 7) {
            i.board[counter].rollWeighting = 0;
        }
            
        if (i.board[counter].discipline == STUDENT_BPS) {
            i.board[counter].rollWeighting += 3;
        } else if (i.board[counter].discipline == STUDENT_BQN) {
            i.board[counter].rollWeighting += 3;
        } else if (i.board[counter].discipline == STUDENT_MJ) {
            i.board[counter].rollWeighting += 2;
        } else if (i.board[counter].discipline == STUDENT_MTV) {
            i.board[counter].rollWeighting += 1;
        } else if (i.board[counter].discipline == STUDENT_MMONEY) {
            i.board[counter].rollWeighting += 1;
        }
        counter++;
    }

    int x = getRollWeighting(12, "RL", i);
    int y = i.board[7].rollWeighting;
    int z = i.board[12].rollWeighting;
    int a = i.board[8].rollWeighting;
    printf("this is y %d\n", y);
    printf("this is z %d\n", z);
    printf("this is a %d\n", a);
    printf("this is WEIGHTING %d\n", x);

	//Hardcoded first turn
    if (i.currentTurn < 3) {
    nextAction.actionCode = OBTAIN_ARC;
    //starting position A
        if (i.currentPlayer == UNI_A) {
            if ( (getRollWeighting(8, "RL", i)) > (getRollWeighting(11, "R", i)) ) {
                if (getARC (g, STARTING_ARC1_A1) == VACANT_ARC) {
                    strncpy (nextAction.destination, STARTING_ARC1_A1, PATH_LIMIT - 1);
                } else if (getARC (g, STARTING_ARC1_A1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_A1, PATH_LIMIT - 1);
                }                
            } else if ( (i.board[3].rollWeighting + i.board[7].rollWeighting + 
                         i.board[8].rollWeighting) <
                        (i.board[10].rollWeighting + i.board[11].rollWeighting +
                         i.board[15].rollWeighting) ) {
                if (getARC (g, STARTING_ARC1_A2) == VACANT_ARC) {
                    strncpy (nextAction.destination, STARTING_ARC1_A2, PATH_LIMIT - 1);
                } else if (getARC (g, STARTING_ARC1_A2) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_A2, PATH_LIMIT - 1);	
                }
            }
		// starting position B
        } else if (i.currentPlayer == UNI_B) {
            if ( (i.board[0].rollWeighting + i.board[1].rollWeighting + 
                  i.board[4].rollWeighting) >
                 (i.board[18].rollWeighting + i.board[17].rollWeighting +
                  i.board[14].rollWeighting) ) {
                if (getARC (g, STARTING_ARC1_B1) == VACANT_ARC) {
                    strncpy (nextAction.destination, STARTING_ARC1_B1, PATH_LIMIT - 1);
                } else if (getARC (g, STARTING_ARC1_B1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_B1, PATH_LIMIT - 1);
                }
            } else if ( (i.board[0].rollWeighting + i.board[1].rollWeighting + 
                         i.board[4].rollWeighting) <
                        (i.board[18].rollWeighting + i.board[17].rollWeighting +
                         i.board[14].rollWeighting) ) {
                if (getARC (g, STARTING_ARC1_B2) == VACANT_ARC) {
                    strncpy (nextAction.destination, STARTING_ARC1_B2, PATH_LIMIT - 1);
                } else if (getARC (g, STARTING_ARC1_B1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_B2, PATH_LIMIT - 1);
                }
            }
		// starting position C
        } else if (i.currentPlayer == UNI_C) {
            if ( (i.board[16].rollWeighting + i.board[13].rollWeighting + 
                  i.board[12].rollWeighting) >
                 (i.board[2].rollWeighting + i.board[5].rollWeighting +
                  i.board[6].rollWeighting) ) {
                if (getARC (g, STARTING_ARC1_C1) == VACANT_ARC) {
                    strncpy (nextAction.destination, STARTING_ARC1_C1, PATH_LIMIT - 1);
                } else if (getARC (g, STARTING_ARC1_C1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_C1, PATH_LIMIT - 1);
                }
            } else if ( (i.board[16].rollWeighting + i.board[13].rollWeighting + 
                     i.board[12].rollWeighting) <
                    (i.board[2].rollWeighting + i.board[5].rollWeighting +
                     i.board[6].rollWeighting) ) {
                if (getARC (g, STARTING_ARC1_C2) == VACANT_ARC) {
                    strncpy (nextAction.destination, STARTING_ARC1_C1, PATH_LIMIT - 1);
                } else if (getARC (g, STARTING_ARC1_C2) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_C1, PATH_LIMIT - 1);
                }
             }
        }
        
        if (getARC (g, nextAction.destination) == ourARC) {
            nextAction.actionCode = BUILD_CAMPUS;
        }
	} else if (i.currentTurn >= 3) {
        nextAction = simpleSpinoff (g, i);
        if (nextAction.actionCode != START_SPINOFF && 
            nextAction.actionCode != RETRAIN_STUDENTS) {
            // a.k.a smart saving, take stuff away from MTV and MMONEY
            // put it into the un-7-able students
            // only do it when not trying to get a spinoff
            nextAction = simpleExchange (g, i);
        }
    }
    
    if (ourGO8 == UNI_A) {
        i.currentTurn++;
        ourGO8 = enemy2 + enemy1 + ourCampus;
    }

    nextAction = testLegality (g, nextAction);
    
    return nextAction;
}

action testLegality (Game g, action nextAction) {
    
    if (isLegalAction (g, nextAction) == FALSE) {
        // makes sure the AI would rather pass than make an illegal move
        nextAction.actionCode = PASS;
    }
    
    return nextAction;
}

action simpleSpinoff (Game g, gameInfo i) {
    action spinoffAction;
    spinoffAction.actionCode = PASS;
    int excessStudents = 0;
    
    if ((i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 1) && 
        (i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 1) && 
        (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 1)) {
        spinoffAction.actionCode = START_SPINOFF;
    } else {
        if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] / 
             i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS]) + 
            (i.uni[i.currentPlayer].discipline[STUDENT_BQN] /
             i.uni[i.currentPlayer].discipline[STUDENT_BQN]) >=
            (3 - i.uni[i.currentPlayer].discipline[STUDENT_MJ] -
             i.uni[i.currentPlayer].discipline[STUDENT_MTV] - 
             i.uni[i.currentPlayer].discipline[STUDENT_MMONEY])) {
            // spinoff needs some exchanges before it can spinoff
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN]) {
                excessStudents = STUDENT_BPS;
            } else {
                excessStudents = STUDENT_BQN;
            }
            
            spinoffAction.disciplineFrom = excessStudents;
            
            if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 1) {
                spinoffAction.disciplineTo = STUDENT_MJ; 
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_MTV] < 1) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 1) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        }
    }

    return spinoffAction;
}

action simpleExchange (Game g, gameInfo i) {
    action exchangeAction;
    exchangeAction.actionCode = RETRAIN_STUDENTS;
    
    if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >=
        i.uni[i.currentPlayer].exchangeRate[STUDENT_MMONEY] + 1) {
        exchangeAction.disciplineFrom = STUDENT_MMONEY;
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 
        i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV] + 1) {
        exchangeAction.disciplineFrom = STUDENT_MTV;
    }
    
    if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
         i.uni[i.currentPlayer].discipline[STUDENT_BQN]) && 
        (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 
         i.uni[i.currentPlayer].discipline[STUDENT_BQN])) {
        exchangeAction.disciplineTo = STUDENT_BQN;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                i.uni[i.currentPlayer].discipline[STUDENT_BPS]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 
                i.uni[i.currentPlayer].discipline[STUDENT_BPS])) {
        exchangeAction.disciplineTo = STUDENT_BPS;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                i.uni[i.currentPlayer].discipline[STUDENT_BPS]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 
                i.uni[i.currentPlayer].discipline[STUDENT_BPS])) {
        exchangeAction.disciplineTo = STUDENT_MTV;
    }
    
    return exchangeAction;
}



//weird function, actions N NE SE S SW NW probs need an array or something
//not sure what ahppening, needs array to be run multiple times
action buildLoc (Game g, gameInfo i) {

	int counter = 0;
    int currentPlayer = getWhoseTurn (g);
    int vertexValue[100] = {0};
    int maximum = 0;
    int location = 0;
    int pathlength;

	action actionN, actionNE, actionSE, actionS, actionSW, actionNW;
    action checkLoc;
    action buildLoc;
    action nextAction;
    action playerARC;

    if (currentPlayer == UNI_A) {
        if (getCampus(g, STARTING_ARC2_A2) == currentPlayer) {
            strcpy (checkLoc.destination, STARTING_ARC2_A2);
            vertexValue[0] = getRollWeighting (15, "R", i);
            vertexValue[1] = getRollWeighting (10, "R", i);
            vertexValue[2] = getRollWeighting (10, "RLL", i);
            vertexValue[3] = getRollWeighting (11, "RLL", i);
            vertexValue[4] = getRollWeighting (8, "RL", i);
            strcpy (playerARC.destination, STARTING_ARC2_A1);
        } else if (getCampus(g, STARTING_ARC2_A1) == currentPlayer) {
            strcpy (checkLoc.destination, STARTING_ARC2_A1);
            vertexValue[0] = getRollWeighting (4, "RL", i);
            vertexValue[1] = getRollWeighting (9, "RL", i);
            vertexValue[2] = getRollWeighting (13, "RL", i);
            vertexValue[3] = getRollWeighting (12, "RL", i);
            vertexValue[4] = getRollWeighting (11, "R", i);
            strcpy (playerARC.destination, STARTING_ARC2_A2);
        }
    } else if (currentPlayer == UNI_B) {
        if (getCampus(g, STARTING_ARC2_B2) == currentPlayer) {
            strcpy (checkLoc.destination, STARTING_ARC2_B2);
            vertexValue[0] = getRollWeighting (17, "RL", i);
            vertexValue[1] = getRollWeighting (14, "RL", i);
            vertexValue[2] = getRollWeighting (15, "RL", i);
            vertexValue[3] = getRollWeighting (18, "RLLL", i);
            vertexValue[4] = getRollWeighting (1, "R", i);
            strcpy (playerARC.destination, STARTING_ARC2_B1);
        } else if (getCampus(g, STARTING_ARC2_B1) == currentPlayer) {
            strcpy (checkLoc.destination, STARTING_ARC2_B1);
            vertexValue[0] = getRollWeighting (2, "R", i);
            vertexValue[1] = getRollWeighting (5, "R", i);
            vertexValue[2] = getRollWeighting (4, "R", i);
            vertexValue[3] = getRollWeighting (0, "R", i);
            vertexValue[4] = getRollWeighting (18, "RL", i);
            strcpy (playerARC.destination, STARTING_ARC2_B2);
        }     
    } else if (currentPlayer == UNI_C) {
        if (getCampus(g, STARTING_ARC2_C1) == currentPlayer) {
            strcpy (checkLoc.destination, STARTING_ARC2_C1);
            vertexValue[0] = getRollWeighting (8, "R", i);
            vertexValue[1] = getRollWeighting (9, "R", i);
            vertexValue[2] = getRollWeighting (14, "R", i);
            vertexValue[3] = getRollWeighting (17, "R", i);
            vertexValue[4] = getRollWeighting (6, "RL", i);
            strcpy (playerARC.destination, STARTING_ARC2_C2);
        } else if (getCampus(g, STARTING_ARC2_C2) == currentPlayer) {
            strcpy (checkLoc.destination, STARTING_ARC2_C2);
            vertexValue[0] = getRollWeighting (11, "RL", i);
            vertexValue[1] = getRollWeighting (10, "RL", i);
            vertexValue[2] = getRollWeighting (5, "RL", i);
            vertexValue[3] = getRollWeighting (2, "RL", i);
            vertexValue[4] = getRollWeighting (13, "R", i);
            strcpy (playerARC.destination, STARTING_ARC2_C1);
        }
    }

	pathlength = strlen (checkLoc.destination);

	strcpy (actionN.destination, checkLoc.destination);
	strcpy (actionNE.destination, checkLoc.destination);
	strcpy (actionSE.destination, checkLoc.destination);
	strcpy (actionS.destination, checkLoc.destination);
	strcpy (actionSW.destination, checkLoc.destination);
	strcpy (actionNW.destination, checkLoc.destination);
	

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
	actionS.destination[pathlength + 4] = 'L';

	actionSW.destination[pathlength + 1] = 'L';
	actionSW.destination[pathlength + 2] = 'L';

	actionNW.destination[pathlength + 1] = 'L';
	actionNW.destination[pathlength + 2] = 'R';

    maximum = vertexValue[0];

    while (counter < 100) {
        if (vertexValue[counter] > maximum) {
            maximum = vertexValue[counter];
            location = counter;
        }
        counter++;
    }

    nextAction.actionCode = OBTAIN_ARC;

    if (location == 0 && (getCampus (g, actionNE.destination) == VACANT_VERTEX)) {
        strcpy (buildLoc.destination, actionNE.destination);
            buildLoc.destination[strlen (buildLoc.destination) - 1] = 0;
        if (getARC (g, buildLoc.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, buildLoc.destination);
        } else if (getARC (g, actionNE.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, actionNE.destination);
        }    
    } else if (location == 1 && (getCampus (g, actionN.destination) == VACANT_VERTEX)) {
        strcpy (buildLoc.destination, actionN.destination);
            buildLoc.destination[strlen (buildLoc.destination) - 1] = 0;
        if (getARC (g, buildLoc.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, buildLoc.destination);
        } else if (getARC (g, actionN.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, actionN.destination);
        }
    } else if (location == 2 && (getCampus (g, actionNW.destination) == VACANT_VERTEX)) {
        strcpy (buildLoc.destination, actionNW.destination);
            buildLoc.destination[strlen (buildLoc.destination) - 1] = 0;
        if (getARC (g, buildLoc.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, buildLoc.destination);
        } else if (getARC (g, actionNW.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, actionNW.destination);
        }    
    } else if (location == 3 && (getCampus (g, actionSW.destination) == VACANT_VERTEX)) {
        strcpy (buildLoc.destination, actionSW.destination);
            buildLoc.destination[strlen (buildLoc.destination) - 1] = 0;
        if (getARC (g, buildLoc.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, buildLoc.destination);
        } else if (getARC (g, actionSW.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, actionSW.destination);
        }    
    } else if (location == 4 && (getCampus (g, playerARC.destination) == VACANT_VERTEX)) {
        strcpy (buildLoc.destination, playerARC.destination);
            buildLoc.destination[strlen (buildLoc.destination) - 1] = 0;
        if (getARC (g, buildLoc.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, buildLoc.destination);
        } else if (getARC (g, playerARC.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, playerARC.destination);
        }    
    }

    if ( (getARC (g, nextAction.destination) == currentPlayer) ) {
        nextAction.actionCode = BUILD_CAMPUS;
    }
    return nextAction;
}

campusLocation getCampusLocation (Game g) {

    campusLocation storeCampus;

    int currentPlayer = getWhoseTurn (g);
    int pathlength = 0;
    int counter = 0;
    int counter1 = 0;
    int counter2 = 0;

    char north[PATH_LIMIT];
    char northEast[PATH_LIMIT];
    char southEast[PATH_LIMIT];
    char south[PATH_LIMIT];
    char southWest[PATH_LIMIT];
    char northWest[PATH_LIMIT];
    char newCampusLocation[PATH_LIMIT];
    
    while (counter2 < 2) {

        if (counter2 == 0) {
            if (currentPlayer == UNI_A) {
                strcpy (newCampusLocation, STARTING_CAMPUS_A1);
            } else if (currentPlayer == UNI_B) {
                strcpy (newCampusLocation, STARTING_CAMPUS_B1);
            } else if (currentPlayer == UNI_C) {
                strcpy (newCampusLocation, STARTING_CAMPUS_C1);
            }
            counter2++; 
        } else if (counter2 == 1) {
            if (currentPlayer == UNI_A) {
                strcpy (newCampusLocation, STARTING_CAMPUS_A2);
            } else if (currentPlayer == UNI_B) {
                strcpy (newCampusLocation, STARTING_CAMPUS_B2);
            } else if (currentPlayer == UNI_C) {
                strcpy (newCampusLocation, STARTING_CAMPUS_C2);
            }
            counter2++; 
        }

        while (counter1 < 10) {

            pathlength = strlen (newCampusLocation);

            strcpy (north, newCampusLocation);
            strcpy (northEast, newCampusLocation);
            strcpy (southEast, newCampusLocation);
            strcpy (south, newCampusLocation);
            strcpy (southWest, newCampusLocation);
            strcpy (northWest, newCampusLocation);
            
            north[pathlength + 1] = 'R';
            north[pathlength + 2] = 'L';

            northEast[pathlength + 1] = 'R';
            northEast[pathlength + 2] = 'R';

            southEast[pathlength + 1] = 'R';
            southEast[pathlength + 2] = 'R';
            southEast[pathlength + 3] = 'R';
            southEast[pathlength + 4] = 'R';

            south[pathlength + 1] = 'L';
            south[pathlength + 2] = 'L';
            south[pathlength + 3] = 'L';
            south[pathlength + 4] = 'L';

            southWest[pathlength + 1] = 'L';
            southWest[pathlength + 2] = 'L';

            northWest[pathlength + 1] = 'L';
            northWest[pathlength + 2] = 'R';

            if ( (getCampus (g, southEast)) == currentPlayer) {
                strcpy (storeCampus.campusLocations[counter], southEast);
                counter++;
                strcpy (newCampusLocation, southEast);
            }
            if ( (getCampus (g, south)) == currentPlayer) {
                strcpy (storeCampus.campusLocations[counter], south);
                counter++;
                strcpy (newCampusLocation, south);
            }
            if ( (getCampus (g, northEast)) == currentPlayer) {
                strcpy (storeCampus.campusLocations[counter], northEast);
                counter++;
                strcpy (newCampusLocation, northEast);
            }
            if ( (getCampus (g, southWest)) == currentPlayer) {
                strcpy (storeCampus.campusLocations[counter], southWest);
                counter++;
                strcpy (newCampusLocation, southWest);
            }
            if ( (getCampus (g, northWest)) == currentPlayer) {
                strcpy (storeCampus.campusLocations[counter], northWest);
                counter++;
                strcpy (newCampusLocation, northWest);
            }
            if ( (getCampus (g, north)) == currentPlayer) {
                strcpy (storeCampus.campusLocations[counter], north);
                counter++;
                strcpy (newCampusLocation, north);
            }
            counter1++;
        }
    }
    return storeCampus;
}

int getRollWeighting (int region, path vertexLoc, gameInfo i) {

    int map[7][11] = {{0}};
    int x = 0;
    int y = 0;
    int vertexValue = 0;

    map[1][3] = i.board[0].rollWeighting;
    map[1][5] = i.board[1].rollWeighting;
    map[1][7] = i.board[2].rollWeighting;
    map[2][2] = i.board[3].rollWeighting;
    map[2][4] = i.board[4].rollWeighting;
    map[2][6] = i.board[5].rollWeighting;
    map[2][8] = i.board[6].rollWeighting;
    map[3][1] = i.board[7].rollWeighting;
    map[3][3] = i.board[8].rollWeighting;
    map[3][5] = i.board[9].rollWeighting;
    map[3][7] = i.board[10].rollWeighting;
    map[3][9] = i.board[11].rollWeighting;
    map[4][2] = i.board[12].rollWeighting;
    map[4][4] = i.board[13].rollWeighting;
    map[4][6] = i.board[14].rollWeighting;
    map[4][8] = i.board[15].rollWeighting;
    map[5][3] = i.board[16].rollWeighting;
    map[5][5] = i.board[17].rollWeighting;
    map[5][7] = i.board[18].rollWeighting;

    if (region == 0) {
        x = 1;
        y = 3;
    } else if (region == 1) {
        x = 1;
        y = 5;
    } else if (region == 2) {
        x = 1;
        y = 7;
    } else if (region == 3) {
        x = 2;
        y = 2;
    } else if (region == 4) {
        x = 2;
        y = 4;
    } else if (region == 5) {
        x = 2;
        y = 6;
    } else if (region == 6) {
        x = 2;
        y = 8;
    } else if (region == 7) {
        x = 3;
        y = 1;
    } else if (region == 8) {
        x = 3;
        y = 3;
    } else if (region == 9) {
        x = 3;
        y = 5;
    } else if (region == 10) {
        x = 3;
        y = 7;
    } else if (region == 11) {
        x = 3;
        y = 9;
    } else if (region == 12) {
        x = 4;
        y = 2;
    } else if (region == 13) {
        x = 4;
        y = 4;
    } else if (region == 14) {
        x = 4;
        y = 6;
    } else if (region == 15) {
        x = 4;
        y = 8;
    } else if (region == 16) {
        x = 5;
        y = 3;
    } else if (region == 17) {
        x = 5;
        y = 5;
    } else if (region == 18) {
        x = 5;
        y = 7;
    }

    if (strcmp (vertexLoc, "R") == 0) {
        vertexValue = map[x][y] + map[x][y-2] + map[x+1][y-1];
    } else if (strcmp (vertexLoc, "RL") == 0) {
        vertexValue = map[x][y] + map[x][y-2] + map[x-1][y-1];
    } else if (strcmp (vertexLoc, "RLL") == 0) {
        vertexValue = map[x][y] + map[x-1][y-1] + map[x-1][y+1];
    } else if (strcmp (vertexLoc, "RLLL") == 0) {
        vertexValue = map[x][y] + map[x-1][y+1] + map[x][y+2];
    } else if (strcmp (vertexLoc, "RLLLL") == 0) {
        vertexValue = map[x][y] + map[x][y+2] + map[x+1][y+1];
    } else if (strcmp (vertexLoc, "RLLLLL") == 0) {
        vertexValue = map[x][y] + map[x+1][y+1] + map[x+1][y-1];
    }
    
    if (region == 3 || region == 12) {
        if ((strcmp (vertexLoc, "R") == 0) || (strcmp (vertexLoc, "RL") == 0)) {
            vertexValue += 2;
        }
    }
    if (region == 6) {
        if ((strcmp (vertexLoc, "RLL") == 0) || (strcmp (vertexLoc, "RLLL") == 0)) {
            vertexValue += 2;
        }
    }
    if (region == 15 || region == 17) {
        if ((strcmp (vertexLoc, "RLLLL") == 0) || (strcmp (vertexLoc, "RLLLLL") == 0)) {
            vertexValue += 2;
        }
    }
    return vertexValue;
}
/*
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

*/