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
#include <math.h>

#include "Game.h"
#include "mechanicalTurk.h"

#define NUMBER_OF_DISCIPLINES 6

#define STARTING_CAMPUS_A1 "LB"
#define STARTING_ARC1_A1 "R"
#define STARTING_ARC2_A1 "RL"
#define STARTING_ARC1A_A1 "L"
#define STARTING_ARC2A_A1 "LR"
#define STARTING_CAMPUS_A2 "RLRLRLRLRLL"
#define STARTING_ARC1_A2 "RLRLRLRLRLLL"
#define STARTING_ARC2_A2 "RLRLRLRLRLLLL"
#define STARTING_ARC1A_A2 "RLRLRLRLRLL"
#define STARTING_ARC2A_A2 "RLRLRLRLRL"
#define STARTING_CAMPUS_B1 "RRLRL"
#define STARTING_ARC1_B1 "RRLRLL"
#define STARTING_ARC2_B1 "RRLRLLL"
#define STARTING_ARC1A_B1 "RRLRL"
#define STARTING_ARC2A_B1 "RRLR"
#define STARTING_CAMPUS_B2 "LRLRLRRLRL"
#define STARTING_ARC1_B2 "LRLRLRRLRLB"
#define STARTING_ARC2_B2 "LRLRLRRLRLBL"
#define STARTING_ARC1A_B2 "LRLRLRRLRLR"
#define STARTING_ARC2A_B2 "LRLRLRRLRLRR"
#define STARTING_CAMPUS_C1 "LRLRL"
#define STARTING_ARC1_C1 "LRLRLB"
#define STARTING_ARC2_C1 "LRLRLBL"
#define STARTING_ARC1A_C1 "LRLRLR"
#define STARTING_ARC2A_C1 "LRLRLRR"
#define STARTING_CAMPUS_C2 "RRLRLLRLRL"
#define STARTING_ARC1_C2 "RRLRLLRLRLL"
#define STARTING_ARC2_C2 "RRLRLLRLRLLL"
#define STARTING_ARC1A_C2 "RRLRLLRLRL"
#define STARTING_ARC2A_C2 "RRLRLLRLR"

#define BUILD_FIRST_CAMPUS_LOCATIONS 4

#define ADDED_LENGTH 17
#define BINARY_COUNT 131072

#define MEDIUM_ARC_COUNT 10
#define HIGH_ARC_COUNT 19
#define SUPER_HIGH_ARC_COUNT 29
#define MEDIUM_CAMPUS_COUNT 4
#define HIGH_CAMPUS_COUNT 6
#define SUPER_HIGH_CAMPUS_COUNT 8
#define MAX_GO8 8
#define HIGH_KPI 138
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

typedef struct _region {
    int diceValue;
    int discipline;
    int rollWeighting;
} region;

typedef struct _gameInfo {
    region board[NUM_REGIONS];
    player uni[NUM_UNIS + 1];
    int currentTurn;
    int currentPlayer;
    int mostPublications;
    int mostARCs;
    int numGO8s;
} gameInfo;

action testLegality (Game g, action nextAction);
action simpleBuild (Game g, gameInfo i);
action exchangeCampus (Game g, gameInfo i);
action exchangeGO8 (Game g, gameInfo i);
action exchangeARC (Game g, gameInfo i);
int changeBuild (Game g, int actionCode);
void intToBinary (int input, int addedPath[]);
action simpleSpinoff (Game g, gameInfo i);
action simpleExchange (Game g, gameInfo i);
action excessResources (Game g, gameInfo i, action nextAction);
int safeExchangeTo (Game g, gameInfo i);

action decideAction (Game g) {
    int counter = 0;
    int enemy1 = 0;
    int enemy2 = 0;
    int ourARC = VACANT_ARC;
    int ourCampus = VACANT_VERTEX;
    int ourGO8 = VACANT_VERTEX;
    int temp = 0;
    int temp2 = 0;
    
    int campusValue[BUILD_FIRST_CAMPUS_LOCATIONS];
    
    gameInfo i;
    
    // store game information
    i.currentTurn = getTurnNumber (g);
    i.currentPlayer = getWhoseTurn (g);
    i.mostARCs = getMostARCs (g);
    i.mostPublications = getMostPublications (g);
    i.numGO8s = getGO8s (g, UNI_A) + 
                getGO8s (g, UNI_B) + 
                getGO8s (g, UNI_C);
    
    action nextAction;
    nextAction.actionCode = PASS;
    nextAction.disciplineFrom = STUDENT_MTV;
    nextAction.disciplineTo = STUDENT_BQN;
    
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
        
    i.uni[UNI_A].kpi = getKPIpoints (g, UNI_A);
    i.uni[UNI_A].numCampuses = getCampuses (g, UNI_A);
    i.uni[UNI_A].numGO8s = getGO8s (g, UNI_A);
    i.uni[UNI_A].numARCs = getARCs (g, UNI_A);
    i.uni[UNI_A].numPublications = getPublications (g, UNI_A);
    i.uni[UNI_A].numPatents = getIPs (g, UNI_A);
    
    i.uni[UNI_A].discipline[STUDENT_BPS] = getStudents (g, UNI_A, STUDENT_BPS);
    i.uni[UNI_A].discipline[STUDENT_BQN] = getStudents (g, UNI_A, STUDENT_BQN);
    i.uni[UNI_A].discipline[STUDENT_MJ] = getStudents (g, UNI_A, STUDENT_MJ);
    i.uni[UNI_A].discipline[STUDENT_MTV] = getStudents (g, UNI_A, STUDENT_MTV);
    i.uni[UNI_A].discipline[STUDENT_MMONEY] = getStudents (g, UNI_A, STUDENT_MMONEY);
    
    i.uni[UNI_A].exchangeRate[STUDENT_BPS] = getExchangeRate (g, UNI_A, STUDENT_BPS, STUDENT_THD);
    i.uni[UNI_A].exchangeRate[STUDENT_BQN] = getExchangeRate (g, UNI_A, STUDENT_BQN, STUDENT_THD);
    i.uni[UNI_A].exchangeRate[STUDENT_MJ] = getExchangeRate (g, UNI_A, STUDENT_MJ, STUDENT_THD);
    i.uni[UNI_A].exchangeRate[STUDENT_MTV] = getExchangeRate (g, UNI_A, STUDENT_MTV, STUDENT_THD);
    i.uni[UNI_A].exchangeRate[STUDENT_MMONEY] = getExchangeRate (g, UNI_A, STUDENT_MMONEY, STUDENT_THD);
    
    i.uni[UNI_B].kpi = getKPIpoints (g, UNI_B);
    i.uni[UNI_B].numCampuses = getCampuses (g, UNI_B);
    i.uni[UNI_B].numGO8s = getGO8s (g, UNI_B);
    i.uni[UNI_B].numARCs = getARCs (g, UNI_B);
    i.uni[UNI_B].numPublications = getPublications (g, UNI_B);
    i.uni[UNI_B].numPatents = getIPs (g, UNI_B);
    
    i.uni[UNI_B].discipline[STUDENT_BPS]= getStudents (g, UNI_B, STUDENT_BPS);
    i.uni[UNI_B].discipline[STUDENT_BQN]= getStudents (g, UNI_B, STUDENT_BQN);
    i.uni[UNI_B].discipline[STUDENT_MJ]= getStudents (g, UNI_B, STUDENT_MJ);
    i.uni[UNI_B].discipline[STUDENT_MTV]= getStudents (g, UNI_B, STUDENT_MTV);
    i.uni[UNI_B].discipline[STUDENT_MMONEY]= getStudents (g, UNI_B, STUDENT_MMONEY);
    
    i.uni[UNI_B].exchangeRate[STUDENT_BPS] = getExchangeRate (g, UNI_B, STUDENT_BPS, STUDENT_THD);
    i.uni[UNI_B].exchangeRate[STUDENT_BQN] = getExchangeRate (g, UNI_B, STUDENT_BQN, STUDENT_THD);
    i.uni[UNI_B].exchangeRate[STUDENT_MJ] = getExchangeRate (g, UNI_B, STUDENT_MJ, STUDENT_THD);
    i.uni[UNI_B].exchangeRate[STUDENT_MTV] = getExchangeRate (g, UNI_B, STUDENT_MTV, STUDENT_THD);
    i.uni[UNI_B].exchangeRate[STUDENT_MMONEY] = getExchangeRate (g, UNI_B, STUDENT_MMONEY, STUDENT_THD);
    
    i.uni[UNI_C].kpi = getKPIpoints (g, UNI_C);
    i.uni[UNI_C].numCampuses = getCampuses (g, UNI_C);
    i.uni[UNI_C].numGO8s = getGO8s (g, UNI_C);
    i.uni[UNI_C].numARCs = getARCs (g, UNI_C);
    i.uni[UNI_C].numPublications = getPublications (g, UNI_C);
    i.uni[UNI_C].numPatents = getIPs (g, UNI_C);

    i.uni[UNI_C].discipline[STUDENT_BPS] = getStudents (g, UNI_C, STUDENT_BPS);
    i.uni[UNI_C].discipline[STUDENT_BQN] = getStudents (g, UNI_C, STUDENT_BQN);
    i.uni[UNI_C].discipline[STUDENT_MJ] = getStudents (g, UNI_C, STUDENT_MJ);
    i.uni[UNI_C].discipline[STUDENT_MTV] = getStudents (g, UNI_C, STUDENT_MTV);
    i.uni[UNI_C].discipline[STUDENT_MMONEY] = getStudents (g, UNI_C, STUDENT_MMONEY);

    i.uni[UNI_C].exchangeRate[STUDENT_BPS] = getExchangeRate (g, UNI_C, STUDENT_BPS, STUDENT_THD);
    i.uni[UNI_C].exchangeRate[STUDENT_BQN] = getExchangeRate (g, UNI_C, STUDENT_BQN, STUDENT_THD);
    i.uni[UNI_C].exchangeRate[STUDENT_MJ] = getExchangeRate (g, UNI_C, STUDENT_MJ, STUDENT_THD);
    i.uni[UNI_C].exchangeRate[STUDENT_MTV] = getExchangeRate (g, UNI_C, STUDENT_MTV, STUDENT_THD);
    i.uni[UNI_C].exchangeRate[STUDENT_MMONEY] = getExchangeRate (g, UNI_C, STUDENT_MMONEY, STUDENT_THD);
    
    
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
            i.board[counter].rollWeighting = 6;
        }
            
        if (i.board[counter].discipline == STUDENT_BPS) {
            i.board[counter].rollWeighting += 3;
        } else if (i.board[counter].discipline == STUDENT_BQN) {
            i.board[counter].rollWeighting += 4;
        } else if (i.board[counter].discipline == STUDENT_MJ) {
            i.board[counter].rollWeighting += 2;
        } else if (i.board[counter].discipline == STUDENT_MTV) {
            i.board[counter].rollWeighting += 1;
        } else if (i.board[counter].discipline == STUDENT_MMONEY) {
            i.board[counter].rollWeighting += 1;
        } else if (i.board[counter].discipline == STUDENT_THD) {
            i.board[counter].rollWeighting -= 7;
        } 
        
        if ((i.board[counter].discipline == STUDENT_MMONEY ||
             i.board[counter].discipline == STUDENT_MTV)&& 
             i.board[counter].diceValue == 7) {
             i.board[counter].rollWeighting -= 8;
        } 
        
        counter++;
    }
    
    i.currentPlayer = getWhoseTurn (g);
	// Hardcoded first turn, always take the closest 3 region vertex as the first overall move
    if (getCampus (g, STARTING_ARC2_A1) != ourCampus && 
        getCampus (g, STARTING_ARC2_A2) != ourCampus &&
        getCampus (g, STARTING_ARC2A_A1) != ourCampus && 
        getCampus (g, STARTING_ARC2A_A2) != ourCampus &&
        getCampus (g, STARTING_ARC2_B1) != ourCampus &&
        getCampus (g, STARTING_ARC2_B2) != ourCampus &&
        getCampus (g, STARTING_ARC2A_B1) != ourCampus &&
        getCampus (g, STARTING_ARC2A_B2) != ourCampus &&
        getCampus (g, STARTING_ARC2_C1) != ourCampus &&
        getCampus (g, STARTING_ARC2_C2) != ourCampus && 
        getCampus (g, STARTING_ARC2A_C1) != ourCampus &&
        getCampus (g, STARTING_ARC2A_C2) != ourCampus && 
        getCampus (g, STARTING_ARC2_A1) != ourGO8 && 
        getCampus (g, STARTING_ARC2_A2) != ourGO8 &&
        getCampus (g, STARTING_ARC2A_A1) != ourGO8 && 
        getCampus (g, STARTING_ARC2A_A2) != ourGO8 &&
        getCampus (g, STARTING_ARC2_B1) != ourGO8 &&
        getCampus (g, STARTING_ARC2_B2) != ourGO8 &&
        getCampus (g, STARTING_ARC2A_B1) != ourGO8 &&
        getCampus (g, STARTING_ARC2A_B2) != ourGO8 &&
        getCampus (g, STARTING_ARC2_C1) != ourGO8 &&
        getCampus (g, STARTING_ARC2_C2) != ourGO8 && 
        getCampus (g, STARTING_ARC2A_C1) != ourGO8 &&
        getCampus (g, STARTING_ARC2A_C2) != ourGO8) {
        nextAction.actionCode = OBTAIN_ARC;
        //starting position A
        if (i.currentPlayer == UNI_A) {
            campusValue[0] = i.board[3].rollWeighting + i.board[7].rollWeighting + 
                             i.board[8].rollWeighting;
            campusValue[1] = i.board[10].rollWeighting + i.board[11].rollWeighting +
                             i.board[15].rollWeighting;
            campusValue[2] = i.board[7].rollWeighting + i.board[12].rollWeighting;
            campusValue[3] = i.board[6].rollWeighting + i.board[11].rollWeighting;
            if (campusValue[0] >= campusValue[1] && 
                campusValue[0] >= campusValue[2] && 
                campusValue[0] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1_A1, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1_A1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_A1, PATH_LIMIT - 1);
                }                
            } else if (campusValue[1] >= campusValue[0] && 
                       campusValue[1] >= campusValue[2] && 
                       campusValue[1] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1_A2, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1_A2) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_A2, PATH_LIMIT - 1);	
                }
            } else if (campusValue[2] >= campusValue[0] && 
                       campusValue[2] >= campusValue[1] && 
                       campusValue[2] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1A_A1, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1A_A1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2A_A1, PATH_LIMIT - 1);	
                }
            } else if (campusValue[3] >= campusValue[0] && 
                       campusValue[3] >= campusValue[1] && 
                       campusValue[3] >= campusValue[2]) {
                strncpy (nextAction.destination, STARTING_ARC1A_A2, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1A_A2) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2A_A2, PATH_LIMIT - 1);	
                }
            }
		// starting position B
        } else if (i.currentPlayer == UNI_B) {
            campusValue[0] = i.board[0].rollWeighting + i.board[1].rollWeighting + 
                             i.board[4].rollWeighting;
            campusValue[1] = i.board[14].rollWeighting + i.board[17].rollWeighting +
                             i.board[18].rollWeighting;
            campusValue[2] = i.board[0].rollWeighting + i.board[3].rollWeighting;
            campusValue[3] = i.board[15].rollWeighting + i.board[18].rollWeighting;
            if (campusValue[0] >= campusValue[1] && 
                campusValue[0] >= campusValue[2] && 
                campusValue[0] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1_B1, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1_B1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_B1, PATH_LIMIT - 1);
                }  
            } else if (campusValue[1] >= campusValue[0] && 
                       campusValue[1] >= campusValue[2] && 
                       campusValue[1] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1_B2, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1_B2) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_B2, PATH_LIMIT - 1);	
                }
            } else if (campusValue[2] >= campusValue[0] && 
                       campusValue[2] >= campusValue[1] && 
                       campusValue[2] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1A_B1, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1A_B1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2A_B1, PATH_LIMIT - 1);	
                }
            } else if (campusValue[3] >= campusValue[0] && 
                       campusValue[3] >= campusValue[1] && 
                       campusValue[3] >= campusValue[2]) {
                    strncpy (nextAction.destination, STARTING_ARC1A_B2, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1A_B2) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2A_B2, PATH_LIMIT - 1);	
                }
            }
		// starting position C
        } else if (i.currentPlayer == UNI_C) {
            campusValue[0] = i.board[12].rollWeighting + i.board[13].rollWeighting + 
                             i.board[16].rollWeighting;
            campusValue[1] = i.board[2].rollWeighting + i.board[5].rollWeighting +
                             i.board[6].rollWeighting;
            campusValue[2] = i.board[16].rollWeighting + i.board[17].rollWeighting;
            campusValue[3] = i.board[1].rollWeighting + i.board[2].rollWeighting;
            if (campusValue[0] >= campusValue[1] && 
                campusValue[0] >= campusValue[2] && 
                campusValue[0] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1_C1, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1_C1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_C1, PATH_LIMIT - 1);
                }                
            } else if (campusValue[1] >= campusValue[0] && 
                       campusValue[1] >= campusValue[2] && 
                       campusValue[1] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1_C2, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1_C2) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2_C2, PATH_LIMIT - 1);	
                }
            } else if (campusValue[2] >= campusValue[0] && 
                       campusValue[2] >= campusValue[1] && 
                       campusValue[2] >= campusValue[3]) {
                strncpy (nextAction.destination, STARTING_ARC1A_C1, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1A_C1) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2A_C1, PATH_LIMIT - 1);	
                }
            } else if (campusValue[3] >= campusValue[0] && 
                       campusValue[3] >= campusValue[1] && 
                       campusValue[3] >= campusValue[2]) {
                strncpy (nextAction.destination, STARTING_ARC1A_C2, PATH_LIMIT - 1);
                if (getARC (g, STARTING_ARC1A_C2) == ourARC) {
                    strncpy (nextAction.destination, STARTING_ARC2A_C2, PATH_LIMIT - 1);	
                }
            }
        }
        i.currentPlayer = getTurnNumber (g) % 3 + 1;
        // split into 2 cases, 
        // if we have the ARCs and resources to build the first campus or not
        if (getARC (g, nextAction.destination) == ourARC && 
            getCampus (g, nextAction.destination) == VACANT_VERTEX &&
            i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
            i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 && 
            i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
            i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
            nextAction.actionCode = BUILD_CAMPUS;
        } else if (getARCs (g, i.currentPlayer) > 1) {
            nextAction = exchangeCampus (g, i);
        }
	} else {
        nextAction = simpleBuild (g, i);
        // make the AI follow a set path
        if (getARCs (g, i.currentPlayer) < MEDIUM_ARC_COUNT && 
            nextAction.actionCode != BUILD_CAMPUS && 
            nextAction.actionCode != BUILD_GO8) {
            // nextAction.actionCode = OBTAIN_ARC;
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1) {
                nextAction = exchangeARC (g, i);
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 &&
                       i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0) {
                simpleBuild (g, i);
                if (isLegalAction (g, nextAction) == FALSE && 
                    nextAction.actionCode == BUILD_CAMPUS) {
                    temp = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                    nextAction = simpleBuild (g, i);
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp;
                }
            }
        } else if (getCampuses (g, i.currentPlayer) < HIGH_CAMPUS_COUNT && 
                  (nextAction.actionCode == OBTAIN_ARC)) {
            // nextAction.actionCode = BUILD_CAMPUS;
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] < 1) {
                nextAction = exchangeCampus (g, i);
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
                temp = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                nextAction = simpleBuild (g, i);
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp;
                if (isLegalAction (g, nextAction) == FALSE && 
                    nextAction.actionCode == BUILD_CAMPUS) {
                    temp = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                    temp2 = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                    nextAction = simpleBuild (g, i);
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp;
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp2;
                    if (isLegalAction (g, nextAction) == FALSE && 
                        nextAction.actionCode == OBTAIN_ARC) {
                        nextAction = simpleSpinoff (g, i);
                    }
                }
            }
        } else if (getCampuses (g, i.currentPlayer) > MEDIUM_CAMPUS_COUNT + 1 && 
                   getGO8s (g, i.currentPlayer) < 1 &&
                   getARCs (g, i.currentPlayer) < HIGH_ARC_COUNT && 
                   i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0 && 
                   i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                  (nextAction.actionCode == BUILD_CAMPUS || 
                   nextAction.actionCode == OBTAIN_ARC)) {
            // stealing priority from ARC_building
            // building these GO8s is just too expensive (15 converted students maximum)
            // willing to convert up to 9 students for it
            // nextAction.actionCode = BUILD_GO8;            
            if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 2 ||
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 3) {
                nextAction = exchangeGO8 (g, i);
            }
        } else if (getARCs (g, i.currentPlayer) < HIGH_ARC_COUNT && 
                   nextAction.actionCode != BUILD_CAMPUS && 
                   nextAction.actionCode != BUILD_GO8) {
            // nextAction.actionCode = OBTAIN_ARC;
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1) {
                nextAction = exchangeARC (g, i);
            }
        } else if (getCampuses (g, i.currentPlayer) <= HIGH_CAMPUS_COUNT &&
                   nextAction.actionCode == OBTAIN_ARC) {
            // nextAction.actionCode = BUILD_CAMPUS;
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] < 1) {
                nextAction = exchangeCampus (g, i);
            }
            /*
        } else if (getCampuses (g, i.currentPlayer) >= HIGH_CAMPUS_COUNT && 
                  (nextAction.actionCode == OBTAIN_ARC || 
                   nextAction.actionCode == BUILD_CAMPUS) && 
                   getGO8s (g, i.currentPlayer) < 2 && 
                  (getGO8s (g, UNI_A) + getGO8s (g, UNI_B) + getGO8s (g, UNI_C) < MAX_GO8)) {
               // stealing priority from ARC_building
            nextAction.actionCode = BUILD_GO8;
            if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 2 ||
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 3) {
                nextAction = exchangeGO8 (g, i);
            } */
        } else if (getCampuses (g, i.currentPlayer) < SUPER_HIGH_CAMPUS_COUNT &&
                   nextAction.actionCode == OBTAIN_ARC) {
            // nextAction.actionCode = BUILD_CAMPUS;
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] < 1) {
                nextAction = exchangeCampus (g, i);
            }
        } else if (getCampuses (g, i.currentPlayer) > HIGH_CAMPUS_COUNT && 
                   nextAction.actionCode == OBTAIN_ARC && 
                   getARCs (g, i.currentPlayer) % 5 == 4) {
            // make a spinoff if it's a good idea with current resources
            // nextAction = simpleSpinoff (g, i);
            // a.k.a smart saving, take stuff away from MTV and MMONEY
            // put it into the un-7-able students
            // only do it when not trying to get a spinoff
            if (isLegalAction (g, nextAction) == FALSE && 
                nextAction.actionCode == BUILD_CAMPUS) {
                temp = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                nextAction = simpleBuild (g, i);
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp;
                if (isLegalAction (g, nextAction) == FALSE && 
                    nextAction.actionCode == OBTAIN_ARC) {
                    nextAction = simpleSpinoff (g, i);
                }
            } else {
                nextAction = simpleExchange (g, i);
            }
        } else if (getARCs (g, i.currentPlayer) > MEDIUM_ARC_COUNT + 4 && 
                   getARCs (g, i.currentPlayer) < HIGH_ARC_COUNT + 4 && 
                   getCampuses (g, i.currentPlayer) >= HIGH_CAMPUS_COUNT && 
                   getIPs (g, i.currentPlayer) < 3 && 
                   getPublications (g, i.currentPlayer) < 3 && 
                   i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                   i.uni[i.currentPlayer].discipline[STUDENT_MTV] + 
                   i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1 && 
                   nextAction.actionCode != BUILD_CAMPUS && 
                   nextAction.actionCode != BUILD_GO8) {
            // nextAction.actionCode = OBTAIN_ARC;
            nextAction = simpleSpinoff (g, i);
            if ((i.uni[i.currentPlayer].discipline[STUDENT_MTV] < 1 ||
                 i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1) && 
                 isLegalAction (g, nextAction) == FALSE) {
                nextAction = exchangeARC (g, i);
            }
        } else if (getARCs (g, i.currentPlayer) < SUPER_HIGH_ARC_COUNT && 
                   getCampuses (g, i.currentPlayer) >= SUPER_HIGH_CAMPUS_COUNT && 
                   nextAction.actionCode != BUILD_CAMPUS && 
                   nextAction.actionCode != BUILD_GO8) {
            // nextAction.actionCode = OBTAIN_ARC;
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 ||
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1) {
                nextAction = exchangeARC (g, i);
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 &&
                       i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0) {
                simpleBuild (g, i);
                if (isLegalAction (g, nextAction) == FALSE && 
                    nextAction.actionCode == BUILD_CAMPUS) {
                    temp = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                    nextAction = simpleBuild (g, i);
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp;
                    if (isLegalAction (g, nextAction) == FALSE && 
                        nextAction.actionCode == OBTAIN_ARC) {
                        nextAction = simpleSpinoff (g, i);
                    }
                }
            }
        } else if (isLegalAction (g, nextAction) == FALSE || 
                   nextAction.actionCode == PASS){
            if (nextAction.actionCode == BUILD_CAMPUS && 
                i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
                // check if i can build an ARC instead if a campus
                // cannot be found an appropriate spot to be placed
                temp = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                nextAction = simpleBuild (g, i);
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp;
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                       i.uni[i.currentPlayer].discipline[STUDENT_MTV] + 
                       i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1) {
                nextAction = simpleSpinoff (g, i);
            } else {
                nextAction = excessResources (g, i, nextAction);
            }   
        }

        if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
            i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0 && 
            i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0) {
            if ((nextAction.actionCode == OBTAIN_ARC && 
                 getARCs (g, i.currentPlayer) >= MEDIUM_ARC_COUNT) || 
                (getPublications (g, UNI_A) + 
                 getPublications (g, UNI_B) + 
                 getPublications (g, UNI_C) < 3)) {
                // with sufficient resources, start a spinoff instead of arcs
                nextAction.actionCode = START_SPINOFF;       
            }              
        } else if (getCampuses (g, i.currentPlayer) >= MEDIUM_CAMPUS_COUNT && 
                   getARCs (g, i.currentPlayer) >= HIGH_ARC_COUNT && 
                   getARCs (g, i.currentPlayer) < SUPER_HIGH_ARC_COUNT && 
                   nextAction.actionCode != BUILD_CAMPUS && 
                  (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 ||
                   i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1)) {
            nextAction = exchangeARC (g, i);
        } else if (isLegalAction (g, nextAction) == FALSE && 
                    nextAction.actionCode == BUILD_CAMPUS) {
            temp = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
            i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
            nextAction = simpleBuild (g, i);
            i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp;
            if (isLegalAction (g, nextAction) == FALSE && 
                nextAction.actionCode == OBTAIN_ARC) {
                nextAction = simpleSpinoff (g, i);
            } else {
                nextAction = simpleExchange (g, i);
            }
        }
        
        
        
        // if you're close to winning, just make a campus/arc/spinoff
        if (i.uni[i.currentPlayer].kpi >= HIGH_KPI &&             
           (isLegalAction (g, nextAction) == FALSE || 
            nextAction.actionCode == PASS)) {
            if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                      (i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0 || 
                       i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0)) {
                nextAction = simpleSpinoff (g, i);
            }
            
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&             
               (isLegalAction (g, nextAction) == FALSE || 
                nextAction.actionCode == PASS)) {
                temp = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                temp2 = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                nextAction = simpleBuild (g, i);
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp;
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp2;
            }
                
            if ((i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                 i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0 && 
                 i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0) &&             
                (isLegalAction (g, nextAction) == FALSE || 
                 nextAction.actionCode == PASS)) {
                nextAction.actionCode = START_SPINOFF;
            }                
            
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0 &&             
               (isLegalAction (g, nextAction) == FALSE || 
                nextAction.actionCode == PASS)) {
                temp = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                nextAction = simpleBuild (g, i);
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp;
            } 
        }
    }
    
    // use variables randomly for now
    // maybe later our AI can expand to torment opponents
    if (ourGO8 == UNI_A + 999) {
        ourGO8 = enemy2 + enemy1 + ourCampus;
    }
    
    /*
    if (i.currentPlayer == UNI_B) {
        nextAction.actionCode = PASS;
    }
    */
    
    printf ("End move.\n\
            actionCode = %d\n\
            destination = %s\n\
            disciplineFrom = %d\n\
            disciplineTo = %d\n", 
            nextAction.actionCode, 
            nextAction.destination, 
            nextAction.disciplineFrom, 
            nextAction.disciplineTo);
    
    nextAction = testLegality (g, nextAction);
    
    return nextAction;
}

action testLegality (Game g, action nextAction) {
    
    if (isLegalAction (g, nextAction) == FALSE) {
        // makes sure the AI would rather pass than make an illegal move
        nextAction.actionCode = PASS;
        printf ("The move is not legal.\n");
    }
    
    return nextAction;
}

action exchangeCampus (Game g, gameInfo i) {
    action nextAction;
    nextAction.actionCode = RETRAIN_STUDENTS;
    nextAction.disciplineTo = STUDENT_THD;
    
    printf ("Exchanging for campus\n");
    
    // try and exchange the resources needed to make a campus     
    // when any of our Campus-building resources is ready to be exchanged
    // because it is being held in excess i.e. > 1 + exchangeRate
    // note: MTVs will never be exchanged unless they are > 1 + exchangeRate
    // could produce a THD leak if we never get the first campus fast enough

    if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
         i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS]) || 
        (i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
         i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN]) || 
        (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 
         i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ]) || 
        (i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
         i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV])) {
        nextAction.disciplineFrom = STUDENT_MJ;
        nextAction.disciplineTo = STUDENT_BPS;
        // if it's MTV, must exchange into MJ, BQN, BPS
        if (i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
            i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV]) {
            nextAction.disciplineFrom = STUDENT_MTV;
            // exchange into whichever is lowest/lower
            // priority given to MJ since it is non-arc building
            if  ((i.uni[i.currentPlayer].discipline[STUDENT_MJ] <= 
                  i.uni[i.currentPlayer].discipline[STUDENT_BQN]) && 
                 (i.uni[i.currentPlayer].discipline[STUDENT_MJ] <= 
                  i.uni[i.currentPlayer].discipline[STUDENT_BPS])) {
                nextAction.disciplineTo = STUDENT_MJ;
            }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] <= 
                        i.uni[i.currentPlayer].discipline[STUDENT_BPS] && 
                        i.uni[i.currentPlayer].discipline[STUDENT_BQN] <= 
                        i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
                nextAction.disciplineTo = STUDENT_BQN;
            }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] <= 
                        i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                        i.uni[i.currentPlayer].discipline[STUDENT_BPS] <= 
                        i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
                nextAction.disciplineTo = STUDENT_BPS;
            }
        } else {
            // if we're missing MTVs then exchange into MTVs
            // repeat for every other campus-building resource
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 && 
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0) {
                // if we're missing MTVs, then exchange BPS/BQN/MJ into it
                nextAction.disciplineTo = STUDENT_MTV;
                if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                    i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
                    nextAction.disciplineFrom = STUDENT_BPS;
                }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
                    nextAction.disciplineFrom = STUDENT_BQN;
                }  else if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS]) {
                    nextAction.disciplineFrom = STUDENT_MJ;
                }
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                       i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 && 
                       i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
                nextAction.disciplineTo = STUDENT_MJ;
                if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                    i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV]) {
                    nextAction.disciplineFrom = STUDENT_BPS;
                } else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_MTV]) {
                    nextAction.disciplineFrom = STUDENT_BQN;
                }
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                       i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                       i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
                nextAction.disciplineTo = STUDENT_BQN;
                if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MJ] && 
                    i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV]) {
                    nextAction.disciplineFrom = STUDENT_BPS;
                } else if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_MTV]) {
                    nextAction.disciplineFrom = STUDENT_MJ;
                }
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 && 
                       i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                       i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
                nextAction.disciplineTo = STUDENT_BPS;
                if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MJ] && 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV]) {
                    nextAction.disciplineFrom = STUDENT_BQN;
                }  else if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_MTV]) {
                    nextAction.disciplineFrom = STUDENT_MJ;
                }
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 
                       i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ]) {
                nextAction.disciplineFrom = STUDENT_MJ;
                if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN]) {
                    nextAction.disciplineTo = STUDENT_BQN;
                }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS]) {
                    nextAction.disciplineTo = STUDENT_BPS;
                }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_MTV] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_MTV] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                            i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 && 
                            i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0) {
                    nextAction.disciplineTo = STUDENT_MTV;
                }
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
                       i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS]) {
                nextAction.disciplineFrom = STUDENT_BPS;
                if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN]) {
                    nextAction.disciplineTo = STUDENT_BQN;
                }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_MJ] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
                    nextAction.disciplineTo = STUDENT_MJ;
                }
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                       i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN]) {
                nextAction.disciplineFrom = STUDENT_BQN;
                if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MJ] && 
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
                    nextAction.disciplineTo = STUDENT_MJ;
                }  else if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS] && 
                            i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 
                            i.uni[i.currentPlayer].discipline[STUDENT_BPS]) {
                    nextAction.disciplineTo = STUDENT_BPS;
                }
            }
        }
        // apply special cases, i.e. when no campus building resources are
        // ready to be exchanged
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MMONEY]) &&
                i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 1 &&
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 1 && 
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 1) {
        // with excess MMONEY and sufficient BPS/BQN/MJ, then make an MT
        nextAction.disciplineFrom = STUDENT_MMONEY;
        nextAction.disciplineTo = STUDENT_MTV;
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 
               i.uni[i.currentPlayer].exchangeRate[STUDENT_MMONEY]) {
        // else, figure out whether BPS/BQN/MJ needs the extra student more
        nextAction = simpleExchange (g, i);
    }
    
    return nextAction;
}

action exchangeGO8 (Game g, gameInfo i) {
    action nextAction;
    nextAction.actionCode = RETRAIN_STUDENTS;
    nextAction.disciplineFrom = STUDENT_THD;
    nextAction.disciplineTo = STUDENT_THD;
    
    printf ("Exchanging for GO8\n");
    
    if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 2) {
        nextAction.disciplineTo = STUDENT_MJ;
        if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
            i.uni[i.currentPlayer].discipline[STUDENT_BPS] && 
            i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
            i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
            nextAction.disciplineFrom = STUDENT_BQN;
        }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] <= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                    i.uni[i.currentPlayer].discipline[STUDENT_BPS] <= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
            nextAction.disciplineFrom = STUDENT_BPS;
        }
    } else if (((int)((double) i.uni[i.currentPlayer].discipline[STUDENT_BPS] / 
                      (double) i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] + 
                      (double) i.uni[i.currentPlayer].discipline[STUDENT_BQN] /  
                      (double) i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN] + 
                      (double) i.uni[i.currentPlayer].discipline[STUDENT_MTV] / 
                      (double) i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV]) > 
                           3 - i.uni[i.currentPlayer].discipline[STUDENT_MMONEY]) && 
                              (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
                               i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] || 
                               i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                               i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN] || 
                               i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
                               i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV] || 
                               i.uni[i.currentPlayer].discipline[STUDENT_MJ] - 2 > 
                               i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ] * 2)) {
        nextAction.disciplineFrom = STUDENT_BPS;
        nextAction.disciplineTo = STUDENT_MMONEY;
        if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] - 2 > 
            i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ] * 2) {
            nextAction.disciplineFrom = STUDENT_MJ;
        } else if (i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 
                   i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV]) {
            nextAction.disciplineFrom = STUDENT_MTV;
        } else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_BPS] && 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
                nextAction.disciplineFrom = STUDENT_BQN;
        }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                        i.uni[i.currentPlayer].discipline[STUDENT_BQN] && 
                        i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                        i.uni[i.currentPlayer].discipline[STUDENT_MJ]) {
                nextAction.disciplineFrom = STUDENT_BPS;
        }
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 
               i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ] * 3) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_MJ;
        safeExchangeTo (g, i);
        if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
             i.uni[i.currentPlayer].discipline[STUDENT_BQN]) && 
            (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
             i.uni[i.currentPlayer].discipline[STUDENT_BQN])) {
            nextAction.disciplineTo = STUDENT_BQN;
        } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BPS]) && 
                   (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_BPS])) {
            nextAction.disciplineTo = STUDENT_BPS;
        } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY]) && 
                   (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY])) {
            nextAction.disciplineTo = STUDENT_MJ;
        }
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MJ] - 2 > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ] * 2) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_MJ;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MJ] - 2 > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_MJ;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
               i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] * 3 && 
               i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 1) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_BPS;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] * 2) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_BPS;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_BPS;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN] * 3) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 1)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_BQN;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN] * 2) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_BQN;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_BQN;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV] * 3) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 1)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_MTV;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV] * 2) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_MTV;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        nextAction.disciplineFrom = STUDENT_MTV;
        nextAction.disciplineTo = STUDENT_MMONEY;
    }
    
    return nextAction;
}

action exchangeARC (Game g, gameInfo i) {
    action nextAction;
    nextAction.actionCode = RETRAIN_STUDENTS;
    
    printf ("Exchanging for ARC\n");
    
    nextAction = simpleExchange (g, i);
    if (nextAction.disciplineTo == STUDENT_MJ) {
        if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
            i.uni[i.currentPlayer].discipline[STUDENT_BQN]) {
        nextAction.disciplineTo = STUDENT_BQN;
        } else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                   i.uni[i.currentPlayer].discipline[STUDENT_BPS]) {
        nextAction.disciplineTo = STUDENT_BPS;
       }
    }
    
    if (nextAction.disciplineFrom != STUDENT_MTV && 
        nextAction.disciplineFrom != STUDENT_MMONEY) {
        if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 
            i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ]) {
            nextAction.disciplineFrom = STUDENT_MJ;
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN]) {
                nextAction.disciplineTo = STUDENT_BQN;
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                       i.uni[i.currentPlayer].discipline[STUDENT_BPS]) {
                nextAction.disciplineTo = STUDENT_BPS;
            }  
        } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
                   i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] &&
                   i.uni[i.currentPlayer].discipline[STUDENT_BQN] == 0) {
            nextAction.disciplineFrom = STUDENT_BPS;
            nextAction.disciplineTo = STUDENT_BQN;
        } else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                   i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN] &&
                   i.uni[i.currentPlayer].discipline[STUDENT_BPS] == 0) {
            nextAction.disciplineFrom = STUDENT_BQN;
            nextAction.disciplineTo = STUDENT_BPS;
        }
    }
    
    return nextAction;
}

action simpleBuild (Game g, gameInfo i) {
    action nextAction;
    nextAction.actionCode = PASS;
    nextAction.disciplineFrom = STUDENT_THD;
    nextAction.disciplineTo = STUDENT_THD;

    int counter = 0;
    int length = 0;
    int index = 0;
    int addedPath[ADDED_LENGTH + 1] = {0};
      
      
    printf ("Simply Building...\n");
    
    /*
    if (getARCs (g, i.currentPlayer) <= 
        (int) pow (2, (double) getCampuses (g, i.currentPlayer) + 1) + 
        (int) pow (3, (double) getGO8s (g, i.currentPlayer))) {
        if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 1 &&
            i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 1) {
            nextAction.actionCode = OBTAIN_ARC;
            printf ("Trying to obtain an arc %d\n", nextAction.actionCode);
        } else {
            nextAction = exchangeARC (g, i);
        }
    } else {
        nextAction = exchangeGO8 (g, i);
        if (nextAction.actionCode != RETRAIN_STUDENTS) {
            nextAction = exchangeCampus (g, i);
        }
    }
    */
    // non-exchange actionCode deciding
    if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 2 &&
        i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 3 && 
        (getGO8s (g, UNI_A) + getGO8s (g, UNI_B) + getGO8s (g, UNI_C) < MAX_GO8)) {
        nextAction.actionCode = BUILD_GO8;
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 1 &&
               i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 1 &&
               i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 1 &&
               i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 1) {
        nextAction.actionCode = BUILD_CAMPUS;
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 1 &&
               i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 1) {
        nextAction.actionCode = OBTAIN_ARC;
    }
    
    if (getARCs (g, i.currentPlayer) >= HIGH_ARC_COUNT && 
        getARCs (g, i.currentPlayer) < SUPER_HIGH_ARC_COUNT && 
        i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 1 &&
        i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 1 && 
        nextAction.actionCode != BUILD_CAMPUS) {     
        nextAction.actionCode = OBTAIN_ARC;
    }
    
    /*
    if (nextAction.actionCode != BUILD_CAMPUS) {
        if (nextAction.actionCode != BUILD_GO8 && 
            nextAction.actionCode != OBTAIN_ARC) {
            if (getCampuses (g, i.currentPlayer) > 3 && 
                nextAction.actionCode == PASS) {
                nextAction = exchangeGO8 (g, i);
                if (getARCs (g, i.currentPlayer) % 3 == 0) {
                    nextAction.actionCode = OBTAIN_ARC;
                }
            }
            if (nextAction.actionCode != RETRAIN_STUDENTS) {
                nextAction = exchangeARC (g, i);
            }
        }  
        if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 1 &&
            i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 1 &&
            i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 1 && 
            getARCs (g, i.currentPlayer) > 6 * getCampuses (g, i.currentPlayer) && 
            nextAction.actionCode != BUILD_GO8) {
            nextAction = exchangeCampus (g, i);  
        } 
    }
    */
    if (i.currentPlayer == UNI_A) {
        strncpy (nextAction.destination, STARTING_CAMPUS_A1, PATH_LIMIT - 1);
        nextAction.destination[2] = '\0';
    } else if (i.currentPlayer == UNI_B) {
        strncpy (nextAction.destination, STARTING_CAMPUS_B1, PATH_LIMIT - 1);
        nextAction.destination[5] = '\0';
    } else if (i.currentPlayer == UNI_C) {
        strncpy (nextAction.destination, STARTING_CAMPUS_C1, PATH_LIMIT - 1);
        nextAction.destination[5] = '\0';
    }
    
    while (nextAction.destination[counter] != '\0') {
        counter++;
    }
    
    length = counter;
    index = length;
    counter = 0;
    if (nextAction.actionCode != RETRAIN_STUDENTS && nextAction.actionCode != PASS) {
        // convert an integer into binary so it can represent
        // LR combinations easier
        while (counter < BINARY_COUNT) {
            intToBinary (counter, addedPath);
            // turn an array of ints into additions onto path
            while (index < length + ADDED_LENGTH) {
                if (addedPath[length + ADDED_LENGTH - index - 1] == 0) {
                    nextAction.destination[index] = 'L';
                } else if (addedPath[length + ADDED_LENGTH - index - 1] == 1) {
                    nextAction.destination[index] = 'R';
                }
                index++;
            }        
            index = length; 
            if (isLegalAction (g, nextAction) == TRUE) {
                counter = BINARY_COUNT + 1;
            }
            counter++;
        }
    }
    return nextAction;
}

void intToBinary (int input, int addedPath[]) {    
    double n = 0;
    if (input == 1) {
        addedPath[0] = 1;
    } else if (input == 2) {
        addedPath[1] = 1;
    } else {
        while (pow (2, n) < (double) input) {
            n++;
        }        
        while (input > 0) {
            if (n >= 0) {
                if (pow (2, n) > (double) input) {
                    addedPath[(int) n] = 0;
                } else if (pow (2, n) <= (double) input) {
                    addedPath[(int) n] = 1;
                    input -= pow (2, n);
                }
            }
            n--;
        }
        
    }
    /*
    while (i < ADDED_LENGTH) {
        addedPath[i] = input % 2;
        i++;
        input = input / 2;
    }
    */
}

/*
int changeBuild (Game g, int actionCode) {
    int newCode = OBTAIN_ARC;
    
    if (actionCode == BUILD_GO8) {
        newCode = BUILD_CAMPUS;
    } else if (actionCode == BUILD_CAMPUS) {
        newCode = OBTAIN_ARC;
    } else if (actionCode == OBTAIN_ARC) {
        newCode = BUILD_GO8;
    }
    
    return newCode;
}
*/

action simpleSpinoff (Game g, gameInfo i) {
    action spinoffAction;
    spinoffAction.actionCode = START_SPINOFF;
    int currentPlayer = getTurnNumber (g) % 3 + 1;
    
    printf ("Exchanging for spinoff\n");
    
    if ((i.uni[currentPlayer].discipline[STUDENT_MJ] > 0) && 
        (i.uni[currentPlayer].discipline[STUDENT_MTV] > 0) && 
        (i.uni[currentPlayer].discipline[STUDENT_MMONEY] > 0)) {
        // with sufficient resources, start a spinoff
        spinoffAction.actionCode = START_SPINOFF;
    } else {  
        if (((int) ((double)(i.uni[currentPlayer].discipline[STUDENT_BPS] / 
                             i.uni[currentPlayer].exchangeRate[STUDENT_BPS])) + 
                   (double)((i.uni[currentPlayer].discipline[STUDENT_BQN] /
                             i.uni[currentPlayer].exchangeRate[STUDENT_BQN])) >= 
                        (2 - i.uni[currentPlayer].discipline[STUDENT_MTV] - 
                             i.uni[currentPlayer].discipline[STUDENT_MMONEY])) && 
                            (i.uni[currentPlayer].discipline[STUDENT_MJ] > 0) && 
                            (i.uni[currentPlayer].discipline[STUDENT_MTV] > 0 || 
                             i.uni[currentPlayer].discipline[STUDENT_MMONEY] > 0)) {
            // spinoff needs some exchanges before it can spinoff
            // if BPS/exchangeRateBPS + BQN/exchangeRateBQN > 2 - MTVs - MMONEYs
            // assuming MJ > 0
            // then we have enough to make a spinoff if we convert BPS and BQN
            // into MTVs or MMONEYs
            // we don't want to pre-emptively convert and risk getting 7-ed
            spinoffAction.actionCode = RETRAIN_STUDENTS;

            if ((i.uni[currentPlayer].discipline[STUDENT_BPS] >= 
                 i.uni[currentPlayer].discipline[STUDENT_BQN]) && 
                (i.uni[currentPlayer].discipline[STUDENT_BPS] >= 
                 i.uni[currentPlayer].discipline[STUDENT_MJ])) {
                spinoffAction.disciplineFrom = STUDENT_BPS;
                
            } else if ((i.uni[currentPlayer].discipline[STUDENT_BQN] >= 
                        i.uni[currentPlayer].discipline[STUDENT_BPS]) && 
                       (i.uni[currentPlayer].discipline[STUDENT_BQN] >= 
                        i.uni[currentPlayer].discipline[STUDENT_MJ])) {
                spinoffAction.disciplineFrom = STUDENT_BQN;
            } else if ((i.uni[currentPlayer].discipline[STUDENT_MJ] >= 
                        i.uni[currentPlayer].discipline[STUDENT_BPS]) &&
                       (i.uni[currentPlayer].discipline[STUDENT_MJ] >= 
                        i.uni[currentPlayer].discipline[STUDENT_BQN])) {
                spinoffAction.disciplineFrom = STUDENT_MJ;  
            }       
            // does MTV or MMONEY need it more?
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] < 1) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (i.uni[currentPlayer].discipline[STUDENT_MMONEY] < 1) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_MJ] == 0) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineTo = STUDENT_MJ;
            // if we're missing an MJ, make BPS or BQN exchange into it
            if ( (int)((double)(i.uni[currentPlayer].discipline[STUDENT_BPS] / 
                                i.uni[currentPlayer].exchangeRate[STUDENT_BPS]) >= 
                       (double)(i.uni[currentPlayer].discipline[STUDENT_BQN] /
                                i.uni[currentPlayer].exchangeRate[STUDENT_BQN]))) {
                 spinoffAction.disciplineFrom = STUDENT_BPS;
            } else if ( (int)((double)(i.uni[currentPlayer].discipline[STUDENT_BPS] / 
                                       i.uni[currentPlayer].exchangeRate[STUDENT_BPS]) < 
                              (double)(i.uni[currentPlayer].discipline[STUDENT_BQN] /
                                       i.uni[currentPlayer].exchangeRate[STUDENT_BQN]))){
                spinoffAction.disciplineFrom = STUDENT_BQN;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_MJ] > 
                   i.uni[currentPlayer].exchangeRate[STUDENT_MJ] * 2) {
            // if we have MJs such that we conver them into 1 MTV and 1 MMONEY
            // such that in total we have enough to make a spinoff, then start the exchange
            // repeat this concept for BPS, BQN, MTV, MMONEY
            // this ensures if we have excess of one of the resources, we can quickly
            // conver it into spinoff-able resources then create the spin-off
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MJ;
            spinoffAction.disciplineTo = STUDENT_MTV;
        } else if ((i.uni[currentPlayer].discipline[STUDENT_MJ] > 
                    i.uni[currentPlayer].exchangeRate[STUDENT_MJ]) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0 ||
                    i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MJ;
            
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_BPS] > 
                   i.uni[currentPlayer].exchangeRate[STUDENT_BPS] * 2) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_BPS;
            spinoffAction.disciplineTo = STUDENT_MTV;
        } else if ((i.uni[currentPlayer].discipline[STUDENT_BPS] > 
                    i.uni[currentPlayer].exchangeRate[STUDENT_BPS]) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0 ||
                    i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_BPS;
            
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_BQN] > 
                   i.uni[currentPlayer].exchangeRate[STUDENT_BQN] * 2) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_BQN;
            
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if ((i.uni[currentPlayer].discipline[STUDENT_BQN] > 
                    i.uni[currentPlayer].exchangeRate[STUDENT_BQN]) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0 ||
                    i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_BQN;
            
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_MTV] > 
                   i.uni[currentPlayer].exchangeRate[STUDENT_MTV] * 2) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MTV;
            spinoffAction.disciplineTo = STUDENT_MJ;
        } else if ((i.uni[currentPlayer].discipline[STUDENT_MTV] > 
                    i.uni[currentPlayer].exchangeRate[STUDENT_MTV]) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MJ] == 0 ||
                    i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MTV;
            
            if (i.uni[currentPlayer].discipline[STUDENT_MJ] == 0) {
                spinoffAction.disciplineTo = STUDENT_MJ;
            } else if (i.uni[currentPlayer].discipline[STUDENT_MMONEY] == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_MMONEY] > 
                   i.uni[currentPlayer].exchangeRate[STUDENT_MMONEY] * 2) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MMONEY;
            spinoffAction.disciplineTo = STUDENT_MJ;
        } else if ((i.uni[currentPlayer].discipline[STUDENT_MMONEY] > 
                    i.uni[currentPlayer].exchangeRate[STUDENT_MMONEY]) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0 ||
                    i.uni[currentPlayer].discipline[STUDENT_MJ] == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MMONEY;
            
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (i.uni[currentPlayer].discipline[STUDENT_MJ] == 0) {
                spinoffAction.disciplineTo = STUDENT_MJ;
            }
        }
    }

    return spinoffAction;
}

action simpleExchange (Game g, gameInfo i) {
    action exchangeAction;
    exchangeAction.actionCode = RETRAIN_STUDENTS;
    exchangeAction.disciplineFrom = STUDENT_THD;
    
    if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 
        i.uni[i.currentPlayer].exchangeRate[STUDENT_MMONEY]) {
        exchangeAction.disciplineFrom = STUDENT_MMONEY;
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
               i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV]) {
        exchangeAction.disciplineFrom = STUDENT_MTV;
    }
    
    safeExchangeTo (g, i);
    
    if (exchangeAction.disciplineFrom == STUDENT_THD) {
        if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 2 && 
            i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
            i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] && 
            i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
            i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN]) {
            exchangeAction.disciplineTo = STUDENT_MJ;
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN]) {
                exchangeAction.disciplineFrom = STUDENT_BPS;
            } else {
                exchangeAction.disciplineFrom = STUDENT_BQN;
            }
        }
    }
    
    return exchangeAction;
}

action excessResources (Game g, gameInfo i, action nextAction) {
    int studentIndex = STUDENT_MMONEY;
    int maxDiscipline = studentIndex;
    int minDiscipline = studentIndex;
    
    if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 10 || 
        i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 10 || 
        i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 10 ||
        i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 10 ||
        i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 10)) {
        nextAction.actionCode = RETRAIN_STUDENTS;
        while (studentIndex > STUDENT_BPS + 1) {
            if (i.uni[i.currentPlayer].discipline[maxDiscipline] >= 
                i.uni[i.currentPlayer].discipline[studentIndex - 1]) {
                nextAction.disciplineFrom = maxDiscipline;
            } else {
                maxDiscipline = studentIndex - 1;
                nextAction.disciplineFrom = maxDiscipline;
            }
            studentIndex--;
        }
        
        studentIndex = STUDENT_MJ;
        while (studentIndex > STUDENT_BPS + 1) {
            if (i.uni[i.currentPlayer].discipline[minDiscipline] <= 
                i.uni[i.currentPlayer].discipline[studentIndex - 1] && 
                minDiscipline != nextAction.disciplineTo) {
                nextAction.disciplineFrom = minDiscipline;
            } else {
                maxDiscipline = studentIndex - 1;
                nextAction.disciplineFrom = minDiscipline;
            }
            studentIndex--;
        }
    }
    
    return nextAction;
}

int safeExchangeTo (Game g, gameInfo i) {
    int disciplineTo = 2;
    
    if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
         i.uni[i.currentPlayer].discipline[STUDENT_BQN]) && 
        (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
         i.uni[i.currentPlayer].discipline[STUDENT_BQN])) {
        disciplineTo = STUDENT_BQN;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_BPS]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_BPS])) {
        disciplineTo = STUDENT_BPS;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY])) {
        disciplineTo = STUDENT_MJ;
    }
    
    return disciplineTo;
}


/*
 
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

*/