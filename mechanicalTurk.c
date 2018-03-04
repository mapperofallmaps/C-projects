/*
 *  Obelisk the Tormentor
 *  -things to do . tormenting, being an AI, winning, seg faulting, bad variables
 *  Proundly Created by Daniel Lau z3376221 & Wei Hong Chen zsomething
 *  what to write here? hello, it"s me. you"re way ahead of the curve.
 *  THU 12 MAY 2016
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
#define STARTING_ARC2A_A2 "RLRLRLRLRLB"
#define STARTING_CAMPUS_B1 "RRLRL"
#define STARTING_ARC1_B1 "RRLRLL"
#define STARTING_ARC2_B1 "RRLRLLL"
#define STARTING_ARC1A_B1 "RRLRL"
#define STARTING_ARC2A_B1 "RRLRB"
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
#define STARTING_ARC2A_C2 "RRLRLLRLRB"
 
#define BUILD_FIRST_CAMPUS_LOCATIONS 4
 
#define ADDED_LENGTH 17
#define BINARY_COUNT 131072

#define MEDIUM_ARC_COUNT 7
#define HIGH_ARC_COUNT 18
#define SUPER_HIGH_ARC_COUNT 21
#define MEDIUM_CAMPUS_COUNT 5
#define HIGH_CAMPUS_COUNT 8
#define SUPER_HIGH_CAMPUS_COUNT 10
#define MAX_GO8 8
#define HIGH_KPI 138
 
#define NUMBER_OF_DISCIPLINES 6
  
#define NO_STUDENT -1
#define NO_DICE 0
#define TERRA_NULLIUS_TURN -1

// board dimensions
#define HORIZONTAL_SIZE 25
#define VERTICAL_SIZE 23
  
// extra tile id
#define NULL_ID -1
  
// tiletypes
#define NULL_TILE 0
#define CAMPUS_TILE 1
#define ARC_TILE 2
  
// max number of students/dice per tile
#define MAX_REGION_CONNECTIONS 3
  
// coordinates of CAMPUS_A in region7
#define X_PATH_START 11
#define Y_PATH_START 1
  
// the X-coordinates of each region's eastern-most tile
#define X_REGION_0_1_2 7
#define X_REGION_3_4_5_6 11
#define X_REGION_7_8_9_10_11 15
#define X_REGION_12_13_14_15 19
#define X_REGION_16_17_18 23
  
// the Y-coordinates of each region's eastern-most tile
#define Y_REGION_7 3
#define Y_REGION_3_12 5
#define Y_REGION_0_8_16 7
#define Y_REGION_4_13 9
#define Y_REGION_1_9_17 11
#define Y_REGION_5_14 13
#define Y_REGION_2_10_18 15
#define Y_REGION_6_15 17
#define Y_REGION_11 19
  
// 6 vertex positions of a hexagon
#define EAST 0
#define SOUTH_EAST 1
#define SOUTH_WEST 2
#define WEST 3
#define NORTH_WEST 4
#define NORTH_EAST 5
#define END_OF_VERTEX_POSITIONS 6
  
// distance of a vertex from another vertex. Both X and Y are the same
#define VERTEX_DISTANCE 2
  
// retraining centre types
#define RETRAINING_NULL -1
#define RETRAINING_ANY 0
#define RETRAINING_MTV 1
#define RETRAINING_MMONEY 2
#define RETRAINING_MJ 3
#define RETRAINING_BQN 4
#define RETRAINING_BPS 5
  
#define MAX_ARCS_AROUND_CAMPUS 3

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
 
typedef struct _tile {
    int students[MAX_REGION_CONNECTIONS];
    int dice[MAX_REGION_CONNECTIONS];
    int tileType;
    int id;
    int retrainingType;
    int rollWeighting;
    path vertexPath;  
} tile;
 
typedef struct _region {
    int diceValue;
    int discipline;
    int rollWeighting;
} region;
 
typedef struct _gameInfo {
    region board1[NUM_REGIONS]; 
    tile board[HORIZONTAL_SIZE][VERTICAL_SIZE];
    player uni[NUM_UNIS + 1];
    int currentTurn;
    int currentPlayer;
    int mostPublications;
    int mostARCs;
    int numGO8s;
} gameInfo;
 
typedef struct _coordinates {
    int x;
    int y; 
    int validPath;
} coordinates;
 
typedef struct _buildLoc {
    coordinates buildDirection[50];
} buildLoc;
 
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
int getRollWeighting (int region, path vertexLoc, gameInfo i);
action getBuildLocations (Game g, buildLoc campusCoordinates, gameInfo i);
buildLoc getCampusTiles (Game g, gameInfo i);
gameInfo storeCampus (Game g, gameInfo i);
static coordinates getTile (gameInfo i, path inputPath, int isARC);
gameInfo setInitialCampusesAndARCs (gameInfo i);
gameInfo clearBoard (gameInfo i);
 
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
        i.board1[counter].diceValue = getDiceValue (g, counter);
        i.board1[counter].discipline = getDiscipline (g, counter);
        if (i.board1[counter].diceValue == 2 || i.board1[counter].diceValue == 12) {
            i.board1[counter].rollWeighting = 1;
        } else if (i.board1[counter].diceValue == 3 || i.board1[counter].diceValue == 11) {
            i.board1[counter].rollWeighting = 2;
        } else if (i.board1[counter].diceValue == 4 || i.board1[counter].diceValue == 10) {
            i.board1[counter].rollWeighting = 3;
        } else if (i.board1[counter].diceValue == 5 || i.board1[counter].diceValue == 9) {
            i.board1[counter].rollWeighting = 4;
        } else if (i.board1[counter].diceValue == 6 || i.board1[counter].diceValue == 8) {
            i.board1[counter].rollWeighting = 5;
        } else if (i.board1[counter].diceValue == 7) {
            i.board1[counter].rollWeighting = 6;
        }
             
        if (i.board1[counter].discipline == STUDENT_BPS) {
            i.board1[counter].rollWeighting += 2;
        } else if (i.board1[counter].discipline == STUDENT_BQN) {
            i.board1[counter].rollWeighting += 2;
        } else if (i.board1[counter].discipline == STUDENT_MJ) {
            i.board1[counter].rollWeighting += 1;
        } else if (i.board1[counter].discipline == STUDENT_MTV) {
            i.board1[counter].rollWeighting += 1;
        } else if (i.board1[counter].discipline == STUDENT_MMONEY) {
            i.board1[counter].rollWeighting += 1;
        } else if (i.board1[counter].discipline == STUDENT_THD) {
            i.board1[counter].rollWeighting -= i.board1[counter].rollWeighting;
        } 
         
        if ((i.board1[counter].discipline == STUDENT_MMONEY ||
             i.board1[counter].discipline == STUDENT_MTV)&& 
             i.board1[counter].diceValue == 7) {
             i.board1[counter].rollWeighting -= 7;
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
            campusValue[0] = i.board1[3].rollWeighting + i.board1[7].rollWeighting + 
                             i.board1[8].rollWeighting;
            campusValue[1] = i.board1[10].rollWeighting + i.board1[11].rollWeighting +
                             i.board1[15].rollWeighting;
            campusValue[2] = i.board1[7].rollWeighting + i.board1[12].rollWeighting;
            campusValue[3] = i.board1[6].rollWeighting + i.board1[11].rollWeighting;
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
            campusValue[0] = i.board1[0].rollWeighting + i.board1[1].rollWeighting + 
                             i.board1[4].rollWeighting;
            campusValue[1] = i.board1[14].rollWeighting + i.board1[17].rollWeighting +
                             i.board1[18].rollWeighting;
            campusValue[2] = i.board1[0].rollWeighting + i.board1[3].rollWeighting;
            campusValue[3] = i.board1[15].rollWeighting + i.board1[18].rollWeighting;
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
            campusValue[0] = i.board1[12].rollWeighting + i.board1[13].rollWeighting + 
                             i.board1[16].rollWeighting;
            campusValue[1] = i.board1[2].rollWeighting + i.board1[5].rollWeighting +
                             i.board1[6].rollWeighting;
            campusValue[2] = i.board1[16].rollWeighting + i.board1[17].rollWeighting;
            campusValue[3] = i.board1[1].rollWeighting + i.board1[2].rollWeighting;
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
        
        if (isLegalAction (g, nextAction) == FALSE && 
          ((nextAction.actionCode == RETRAIN_STUDENTS && 
            nextAction.disciplineTo == STUDENT_MTV) || 
           (nextAction.actionCode == BUILD_CAMPUS))) {
            printf ("Might be stuck building first campus.\n");
            nextAction = simpleBuild (g, i);
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 || 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1) {        
                nextAction = exchangeARC (g, i);
            } else {
                nextAction = simpleBuild (g, i);
                if (isLegalAction (g, nextAction) == FALSE) {
                    nextAction = simpleSpinoff (g, i);
                }
            }
        }
        
    } else {
        nextAction = simpleBuild (g, i);
        // make the AI follow a set path
        /*if (getARCs (g, i.currentPlayer) < MEDIUM_ARC_COUNT && 
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
                   nextAction.actionCode == BUILD_CAMPUS && 
                   nextAction.actionCode != OBTAIN_ARC) {
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
        /*} else if (getCampuses (g, i.currentPlayer) < SUPER_HIGH_CAMPUS_COUNT &&
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
            }*/
        // core of the logic
        if (getARCs (g, i.currentPlayer) < (getCampuses (g, i.currentPlayer) + 
                                            getGO8s (g, i.currentPlayer) - 1) * 2) {
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0) {
                nextAction = simpleBuild (g, i);
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 ||  
                       i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1) {
                nextAction = exchangeARC (g, i);
            }   
        } else {
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
                nextAction = simpleBuild (g, i);
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 1 || 
                       i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 1 ||
                       i.uni[i.currentPlayer].discipline[STUDENT_MJ] < 1 ||
                       i.uni[i.currentPlayer].discipline[STUDENT_MTV] < 1) {
                nextAction = exchangeCampus (g, i);
            }
        }
        /*
        if (isLegalAction (g, nextAction) == FALSE || 
            nextAction.actionCode == PASS) {
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
            }
        }
        */
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
        } 
        /*
        else if (getCampuses (g, i.currentPlayer) >= MEDIUM_CAMPUS_COUNT && 
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
        */

         
        // if you're close to winning, just make a campus/arc/spinoff
        if (i.uni[i.currentPlayer].kpi >= HIGH_KPI &&             
           (isLegalAction (g, nextAction) == FALSE || 
            nextAction.actionCode == PASS)) {
            if (isLegalAction (g, nextAction) == FALSE || 
                nextAction.actionCode == PASS) {
                if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&
                    i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 &&
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
                    temp = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                    nextAction = simpleBuild (g, i);
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp;
                } 
                 
                if ((i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                     i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0 && 
                     i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0 && 
                    (isLegalAction (g, nextAction) == FALSE || 
                     nextAction.actionCode == PASS))) {
                    nextAction.actionCode = START_SPINOFF;
                }   
            }

            if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
               (isLegalAction (g, nextAction) == FALSE || 
                nextAction.actionCode == PASS) && 
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
            } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] < 0 && 
                        i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0) || 
                       (i.uni[i.currentPlayer].discipline[STUDENT_BQN] < 0 &&
                        i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0)) {
                exchangeARC (g, i);
            }
        }
         
        if (isLegalAction (g, nextAction) == FALSE || 
            nextAction.actionCode == PASS) {
            printf ("Invalid action, trying to build again\n");
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0) {
                temp = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                nextAction = simpleBuild (g, i);
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp;
                if ((isLegalAction (g, nextAction) == FALSE || 
                    nextAction.actionCode == PASS) && 
                    i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0) {
                    temp = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                    temp2 = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                    nextAction = simpleBuild (g, i);
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp;
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp2;
                    if (isLegalAction (g, nextAction) == FALSE || 
                        nextAction.actionCode == PASS) {
                        nextAction = simpleSpinoff (g, i);
                    }
                }
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                       i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 && 
                       getARCs (g, i.currentPlayer) < 
                      (getCampuses (g, i.currentPlayer) + 
                       getGO8s (g, i.currentPlayer) - 1) * 2 && 
                       nextAction.actionCode != RETRAIN_STUDENTS) { // might need to remove this
                temp = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                temp2 = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                nextAction = simpleBuild (g, i);
                i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp;
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp2;
                if (isLegalAction (g, nextAction) == FALSE || 
                    nextAction.actionCode == PASS) {
                    nextAction = simpleSpinoff (g, i);
                }  
            }
                
            if ((i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 && 
                 i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0 && 
                 i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0 && 
                (isLegalAction (g, nextAction) == FALSE || 
                 nextAction.actionCode == PASS))) {
                nextAction.actionCode = START_SPINOFF;
            }   
        }
        
        if ((getARCs (g, i.currentPlayer) == HIGH_ARC_COUNT || 
             getARCs (g, i.currentPlayer) == HIGH_ARC_COUNT + 1 || 
             getARCs (g, i.currentPlayer) == HIGH_ARC_COUNT + 2) &&  
             getCampuses (g, i.currentPlayer) > HIGH_CAMPUS_COUNT) {
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
                if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 &&
                    i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&             
                   (isLegalAction (g, nextAction) == FALSE || 
                    nextAction.actionCode == PASS)) {
                    temp = i.uni[i.currentPlayer].discipline[STUDENT_MMONEY];
                    temp2 = i.uni[i.currentPlayer].discipline[STUDENT_MTV];
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = 0;
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = 0;
                    nextAction = simpleBuild (g, i);
                    i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] = temp;
                    i.uni[i.currentPlayer].discipline[STUDENT_MTV] = temp2;
                    if (isLegalAction (g, nextAction) == FALSE || 
                        nextAction.actionCode == PASS) {
                        nextAction = simpleSpinoff (g, i);
                    }
                }
            }
        }
        
        if (i.uni[i.currentPlayer].kpi < HIGH_KPI && 
            nextAction.actionCode == OBTAIN_ARC && 
            getARCs (g, i.currentPlayer) >= (getCampuses (g, i.currentPlayer) + 
                                             getGO8s (g, i.currentPlayer) + 1) * 2) {
            nextAction = simpleBuild (g, i);
            if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 0 && 
                i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 0 &&
                i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0 && 
                nextAction.actionCode == OBTAIN_ARC) {
                nextAction = simpleBuild (g, i);
                if (nextAction.actionCode != BUILD_CAMPUS && 
                    isLegalAction (g, nextAction) == FALSE) {
                    nextAction = simpleSpinoff (g, i);
                }
            } else {
                nextAction = exchangeCampus (g, i);
            }
        }
        
        nextAction = excessResources (g, i, nextAction);
    }
     
    // use variables randomly for now
    // maybe later our AI can expand to torment opponents
    if (ourGO8 == UNI_A + 999) {
        ourGO8 = enemy2 + enemy1 + ourCampus + temp + temp2;
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
             
    if (nextAction.disciplineTo == STUDENT_THD) {
        safeExchangeTo (g, i);
    }
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
        nextAction.disciplineFrom = STUDENT_MJ;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MJ] - 2 > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MJ]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.disciplineFrom = STUDENT_MJ;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
               i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] * 3 && 
               i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 1) {
        nextAction.disciplineFrom = STUDENT_BPS;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] * 2) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0)) {
        nextAction.disciplineFrom = STUDENT_BPS;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.disciplineFrom = STUDENT_BPS;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN] * 3) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 1)) {
        nextAction.disciplineFrom = STUDENT_BQN;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN] * 2) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 0)) {
        nextAction.disciplineFrom = STUDENT_BQN;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.disciplineFrom = STUDENT_BQN;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV] * 3) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] < 1)) {
        nextAction.disciplineFrom = STUDENT_MTV;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV] * 2) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
        nextAction.disciplineFrom = STUDENT_MTV;
        nextAction.disciplineTo = STUDENT_MMONEY;
    } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
                i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV]) && 
               (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1)) {
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
 
    /*int counter = 0;
    int length = 0;
    int index = 0;
    int addedPath[ADDED_LENGTH + 1] = {0};
    */
       
       
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
    /*if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 2 &&
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
    }*/
     
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
    }*/
    i = clearBoard (i); 
    i = setInitialCampusesAndARCs (i);
    i = storeCampus (g, i);
 
    buildLoc campusLocations;
    campusLocations = getCampusTiles (g, i);
 
    nextAction = getBuildLocations (g, campusLocations, i);
    path maxCampus;
    int counter = 0;

    if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 2 &&
        i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 1 &&
        i.numGO8s < MAX_GO8) {
        nextAction.actionCode = BUILD_GO8;
        while (campusLocations.buildDirection[counter + 1].x != 0) {
            if (i.board[campusLocations.buildDirection[counter].x]
                [campusLocations.buildDirection[counter].y].rollWeighting <
                i.board[campusLocations.buildDirection[counter + 1].x]
                [campusLocations.buildDirection[counter + 1].y].rollWeighting) {
                strcpy (maxCampus, i.board[campusLocations.buildDirection[counter + 1].x]
                [campusLocations.buildDirection[counter + 1].y].vertexPath );
            }
            counter++;
        }
        strcpy (nextAction.destination, maxCampus);
    } else if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1 &&
        i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 1 &&
        i.numGO8s < MAX_GO8) {
        if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 
            i.uni[i.currentPlayer].exchangeRate[STUDENT_BPS] ||
            i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 
            i.uni[i.currentPlayer].exchangeRate[STUDENT_BQN] ||
            i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 
            i.uni[i.currentPlayer].exchangeRate[STUDENT_MTV]) {
            nextAction = exchangeGO8 (g, i);
        }
    }
    /*action temp1, temp2, temp3;
    int stringlength = strlen (nextAction.destination);
 
    strcpy (temp1.destination, nextAction.destination);
    strcpy (temp2.destination, nextAction.destination);
    strcpy (temp3.destination, nextAction.destination);
 
    temp1.destination[stringlength] = 'R';
    temp1.destination[stringlength + 1] = '\0';
 
    temp2.destination[stringlength] = 'L';
    temp2.destination[stringlength + 1] = '\0';
 
    temp3.destination[stringlength] = 'B';
    temp3.destination[stringlength + 1] = '\0';
 
    printf ("temp1 owner = %d\n", getCampus (g, temp1.destination));
    printf ("temp1 string = %s\n", temp1.destination);
    printf ("temp2 owner = %d\n", getCampus (g, temp2.destination));
    printf ("temp2 string = %s\n", temp2.destination);
    printf ("temp3 owner = %d\n", getCampus (g, temp3.destination));
    printf ("temp3 string = %s\n", temp3.destination);*/
 
 
     
    /*nextAction.actionCode = OBTAIN_ARC;
    stringlength = strlen (nextAction.destination);
 
    action buildposition;
 
    if (getCampus (g, nextAction.destination) == VACANT_VERTEX) {
        strcpy (buildposition.destination, nextAction.destination);
        stringlength = strlen (buildposition.destination);
        printf("this is buildposition %s\n", buildposition.destination);
        buildposition.destination[stringlength - 1] = 0;
        printf("this is arc buildposition %s\n", buildposition.destination);
        if (getARC (g, buildposition.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, buildposition.destination);
        } else if (getARC (g, nextAction.destination) == VACANT_ARC) {
            nextAction.actionCode = OBTAIN_ARC;
        } else if (getARC (g, nextAction.destination) == i.currentPlayer) {
            nextAction.actionCode = BUILD_CAMPUS;
        }
    }*/
    
    printf("nextaction string %s\n", nextAction.destination);
    printf("actionCode is %d\n", nextAction.actionCode);
    printf("the next action is %d\n", isLegalAction (g, nextAction));
    
    
    i.board[0][0].tileType = NULL_TILE;
    i.board[0][0].id = NULL_ID;
    printf ("Printing the game board\n");
    int row = 0;
    int column = 0;
    while (row < VERTICAL_SIZE) {
        while (column < HORIZONTAL_SIZE) {
            if ((column != 0) && (column != 24)) {
                if (i.board[column][row].tileType == NULL_TILE) {
                    printf (" ");
                } else if (i.board[column][row].tileType == CAMPUS_TILE) {
                    if (i.board[column][row].id != VACANT_VERTEX) {
                        printf ("%d", i.board[column][row].id);
                    } else {
                        printf ("C");                
                    }
                } else if (i.board[column][row].tileType == ARC_TILE) {
                    if (i.board[column][row].id != VACANT_VERTEX) {
                        printf ("%d", getARC (g, i.board[column][row].vertexPath));
                    /*if (i.board[column][row].id == ARC_A) {
                        printf ("a");
                    } else if (i.board[column][row].id == ARC_B) {
                        printf ("b");
                    } else if (i.board[column][row].id == ARC_C) {
                        printf ("c");*/
                    } else {
                        printf ("*");
                    }
                }
            } else if ((column == 0) || (column == 24)) {
                printf(" ");
            }
            column++;
        }
        column = 0;
        printf ("\n");
        row++;
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

            if (i.uni[currentPlayer].discipline[STUDENT_MTV] > 
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
     
    printf ("Simply exchange\n");
     
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
    int maxDiscipline = STUDENT_MMONEY;
    int minDiscipline = STUDENT_MMONEY;
     
    if ((i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 10 || 
         i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 10 || 
         i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 10 ||
         i.uni[i.currentPlayer].discipline[STUDENT_MTV] >= 10 ||
         i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 10)) {
        printf ("Exchanging excess\n");
        nextAction.actionCode = RETRAIN_STUDENTS;
        while (studentIndex > STUDENT_BPS) {
            if (i.uni[i.currentPlayer].discipline[maxDiscipline] >= 
                i.uni[i.currentPlayer].discipline[studentIndex - 1] && 
                studentIndex - 1 != STUDENT_THD) {
                nextAction.disciplineFrom = maxDiscipline;
            } else {
                maxDiscipline = studentIndex - 1;
                nextAction.disciplineFrom = maxDiscipline;
            }
            studentIndex--;
        }
         
        studentIndex = STUDENT_MJ;
        while (studentIndex > STUDENT_BPS) {
            if (i.uni[i.currentPlayer].discipline[minDiscipline] <= 
                i.uni[i.currentPlayer].discipline[studentIndex - 1] && 
                minDiscipline != nextAction.disciplineTo && 
                studentIndex - 1 != STUDENT_THD) {
                nextAction.disciplineTo = minDiscipline;
            } else {
                maxDiscipline = studentIndex - 1;
                nextAction.disciplineTo = minDiscipline;
            }
            studentIndex--;
        }
    }
     
    return nextAction;
}
 
int safeExchangeTo (Game g, gameInfo i) {
    int disciplineTo = STUDENT_BQN;
     
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
 
gameInfo clearBoard (gameInfo i) {
    int row = 0;
    int column = 0;
    int counter = 0;
      
    // checking through each row and column of the board
    // set all disciplines and dice values to a lack of production
    while (row < VERTICAL_SIZE) {
        while (column < HORIZONTAL_SIZE) {
            while (counter < MAX_REGION_CONNECTIONS) {
                i.board[column][row].students[counter] = NO_STUDENT;
                i.board[column][row].dice[counter] = NO_DICE;
                counter++;
            }
            counter = 0;
            // fill the board with NULL_TILEs that cannot be walked on
            i.board[column][row].tileType = NULL_TILE;
            i.board[column][row].id = NULL_ID;
            i.board[column][row].retrainingType = RETRAINING_NULL;
            column++;
        }
        column = 0;
        row++;
    }
    return i;
}
  
gameInfo setInitialCampusesAndARCs (gameInfo i) {
    int row = 0;
    int column = 0;
     
    // check each row and column of the board
    while (row < VERTICAL_SIZE) {
        while (column < HORIZONTAL_SIZE) {
            // campuses only appear on odd columns
            if ((column % 2) == 1) {
                // then set out the campus tiles
                if ((column == 1) || (column == 23)) {
                    if (((row - 6) % 4 - 1 == 0) && (row < 16)) {
                        i.board[column][row].tileType = CAMPUS_TILE;  
                        i.board[column][row].id = VACANT_VERTEX;  
                    }
                } else if ((column == 3) || (column == 5) ||
                           (column == 19) || (column == 21)) {
                    if (((row - 4) % 4 - 1 == 0) && (row < 18)) {
                        i.board[column][row].tileType = CAMPUS_TILE;
                        i.board[column][row].id = VACANT_VERTEX;
                    }
                } else if ((column == 7) || (column == 9) ||
                           (column == 15) || (column == 17)) {
                    if (((row - 2) % 4 - 1 == 0) && (row < 20)) {
                        i.board[column][row].tileType = CAMPUS_TILE;
                        i.board[column][row].id = VACANT_VERTEX;
                    }
                } else if ((column == 11) || (column == 13)) {
                    if ((row % 4 - 1 == 0) && (row < 22)) {
                        i.board[column][row].tileType = CAMPUS_TILE;
                        i.board[column][row].id = VACANT_VERTEX;
                    }
                }
            } else if ((column % 2) == 0) {
                // then set out ARC tiles which occur only on even columns
                if ((column == 2) || (column == 22)) {
                    if (((row - 5) % 2 - 1 == 0) && (row < 17)) {
                        i.board[column][row].tileType = ARC_TILE;
                        i.board[column][row].id = VACANT_ARC;
                    }
                } else if ((column == 4) || (column == 20)) {
                    if (((row - 4) % 4 - 1 == 0) && (row < 18)) {
                        i.board[column][row].tileType = ARC_TILE;
                        i.board[column][row].id = VACANT_ARC;
                    }
                } else if ((column == 6) || (column == 18)) {
                    if (((row - 3) % 2 - 1 == 0) && (row < 19)) {
                        i.board[column][row].tileType = ARC_TILE;
                        i.board[column][row].id = VACANT_ARC;
                    }
                } else if ((column == 8) || (column == 16)) {
                    if (((row - 2) % 4 - 1 == 0) && (row < 20)) {
                        i.board[column][row].tileType = ARC_TILE;
                        i.board[column][row].id = VACANT_ARC;
                    }
                } else if ((column == 10) || (column == 14)) {
                    if (((row - 1) % 2 - 1 == 0) && (row < 21)) {
                        i.board[column][row].tileType = ARC_TILE;
                        i.board[column][row].id = VACANT_ARC;
                    }
                } else if (column == 12) {
                    if ((row % 4 - 1 == 0) && (row < 22)) {
                        i.board[column][row].tileType = ARC_TILE;
                        i.board[column][row].id = VACANT_ARC;
                    }
                }
            }
            column++;
        }
        column = 0;
        row++;
    }
     
    // starting positions are always the same
    i.board[11][1].id = CAMPUS_A;
    i.board[13][21].id = CAMPUS_A;
    i.board[1][7].id = CAMPUS_B;
    i.board[23][15].id = CAMPUS_B;
    i.board[21][5].id = CAMPUS_C;
    i.board[3][17].id = CAMPUS_C;
     
    // remove this later on, just for visual purposes
     

    return i;
}
 
static coordinates getTile (gameInfo i, path inputPath, int isARC) {
     
    int counter = 1;
    int pathLength = 0;    
    int distanceX = 0;
    int distanceY = 0;
     
    // always begin at CAMPUS_A at top left
     
    coordinates tileLocation;
    tileLocation.x = X_PATH_START;
    tileLocation.y = Y_PATH_START;    
    tileLocation.validPath = TRUE;
     
    coordinates tileVector;
    tileVector.x = tileLocation.x + 1;
    tileVector.y = tileLocation.y + 1;
     
    coordinates tileL;
    tileL.x = 13;
    tileL.y = 1;
     
    coordinates tileR;
    tileR.x = 9;
    tileR.y = 3;
     
    coordinates tileB;
    tileB.x = tileLocation.x;
    tileB.y = tileLocation.y;
     
    while (inputPath[pathLength] != '\0') {
        pathLength++;
    }
     
    // EOF last char not included in strlen
    assert (pathLength >= 0);
         
    // hard code the first path because it is special
    // and less generic to work with
    if (inputPath[0] == 'R') {
        tileLocation.x = tileR.x;
        tileLocation.y = tileR.y;
        tileVector.x = tileLocation.x - 1;
        tileVector.y = tileLocation.y + 1;
    } else if (inputPath[0] == 'L') {
        tileLocation.x = tileL.x;
        tileLocation.y = tileL.y;
        tileVector.x = tileLocation.x + 1;
        tileVector.y = tileLocation.y;
    } else if (inputPath[0] == 'B') {
        //printf ("Invalid path %s\n", inputPath);
        //printf ("inputPath[%d] == %c\n", counter, inputPath[counter]);
        //printf ("At (%d,%d)\n", tileLocation.x, tileLocation.y);
        tileLocation.validPath = FALSE;
        counter = pathLength;
    }
     
    // because the frist case has been coded
    // can start counting from index 1 rather than 0 of an array
    while (counter < pathLength) {
        if ((inputPath[counter] == 'L') || 
            (inputPath[counter] == 'R') || 
            (inputPath[counter] == 'B')) {
            if ((i.board[tileLocation.x + 1][tileLocation.y + 1].tileType == ARC_TILE) ||
                (i.board[tileLocation.x + 1][tileLocation.y - 1].tileType == ARC_TILE) ||
                (i.board[tileLocation.x - 1][tileLocation.y].tileType == ARC_TILE)) {
                //
                //  **A
                //  AC*
                //  **A
                //             
                if ((tileVector.x == tileLocation.x + 1) &&
                    (tileVector.y == tileLocation.y)) {
                    // -->
                    // NORTH_EAST tile                    
                    tileL.x = tileLocation.x + VERTEX_DISTANCE;
                    tileL.y = tileLocation.y - VERTEX_DISTANCE;                    
                    // SOUTH_EAST tile
                    tileR.x = tileLocation.x + VERTEX_DISTANCE;
                    tileR.y = tileLocation.y + VERTEX_DISTANCE;    
                    // WEST tile                    
                    tileB.x = tileLocation.x - VERTEX_DISTANCE;
                    tileB.y = tileLocation.y;
                } else if ((tileVector.x == tileLocation.x - 1) &&
                           (tileVector.y == tileLocation.y - 1)) {
                    // <-^
                    // WEST tile
                    tileL.x = tileLocation.x - VERTEX_DISTANCE;
                    tileL.y = tileLocation.y;
                    // NORTH_EAST tile
                    tileR.x = tileLocation.x + VERTEX_DISTANCE;
                    tileR.y = tileLocation.y - VERTEX_DISTANCE;
                    // SOUTH_EAST tile
                    tileB.x = tileLocation.x + VERTEX_DISTANCE;
                    tileB.y = tileLocation.y + VERTEX_DISTANCE;                     
               } else if ((tileVector.x == tileLocation.x - 1) &&
                           (tileVector.y == tileLocation.y + 1)) {
                    // <-v
                    // SOUTH_EAST tile
                    tileL.x = tileLocation.x + VERTEX_DISTANCE;
                    tileL.y = tileLocation.y + VERTEX_DISTANCE;
                    // WEST tile
                    tileR.x = tileLocation.x - VERTEX_DISTANCE;
                    tileR.y = tileLocation.y;
                    // NORTH_EAST tile
                    tileB.x = tileLocation.x + VERTEX_DISTANCE;
                    tileB.y = tileLocation.y - VERTEX_DISTANCE;
               }                              
            } else if ((i.board[tileLocation.x - 1][tileLocation.y - 1].tileType == ARC_TILE) ||
                       (i.board[tileLocation.x - 1][tileLocation.y + 1].tileType == ARC_TILE) ||
                       (i.board[tileLocation.x + 1][tileLocation.y].tileType == ARC_TILE)) {
                //
                //  A**
                //  *CA
                //  A**
                //                     
                if ((tileVector.x == tileLocation.x - 1) &&
                    (tileVector.y == tileLocation.y)) {
                    // <--
                    // SOUTH_WEST tile
                    tileL.x = tileLocation.x - VERTEX_DISTANCE;
                    tileL.y = tileLocation.y + VERTEX_DISTANCE;
                    // NORTH_WEST tile
                    tileR.x = tileLocation.x - VERTEX_DISTANCE;
                    tileR.y = tileLocation.y - VERTEX_DISTANCE;
                    // EAST tile
                    tileB.x = tileLocation.x + VERTEX_DISTANCE;
                    tileB.y = tileLocation.y;
                } else if ((tileVector.x == tileLocation.x + 1) &&
                           (tileVector.y == tileLocation.y - 1)) {
                    // ^->
                    // NORTH_WEST tile
                    tileL.x = tileLocation.x - VERTEX_DISTANCE;
                    tileL.y = tileLocation.y - VERTEX_DISTANCE;
                    // EAST tile
                    tileR.x = tileLocation.x + VERTEX_DISTANCE;
                    tileR.y = tileLocation.y;
                    // SOUTH_WEST tile
                    tileB.x = tileLocation.x - VERTEX_DISTANCE;
                    tileB.y = tileLocation.y + VERTEX_DISTANCE;
                } else if ((tileVector.x == tileLocation.x + 1) &&
                           (tileVector.y == tileLocation.y + 1)) {
                    // v->
                    // EAST tile
                    tileL.x = tileLocation.x + VERTEX_DISTANCE;
                    tileL.y = tileLocation.y;
                    // SOUTH_WEST tile
                    tileR.x = tileLocation.x - VERTEX_DISTANCE;
                    tileR.y = tileLocation.y + VERTEX_DISTANCE;
                    // NORTH_WEST tile
                    tileB.x = tileLocation.x - VERTEX_DISTANCE;
                    tileB.y = tileLocation.y - VERTEX_DISTANCE;
                }                              
            } else {
                //printf ("Pathing from an invalid tile.\n");
                //printf ("Invalid path %s\n", inputPath);
                //printf ("inputPath[%d] == %c\n", counter, inputPath[counter]);
                //printf ("At (%d,%d)\n", tileLocation.x, tileLocation.y);
                tileLocation.validPath = FALSE;
                // force the loop to finish
                counter = pathLength;
            }
             
            // apologies for the extremely uncompact triple repetition
            // testing for the pathing direction
            if (inputPath[counter] == 'L' && ((tileL.x >= 0) && (tileL.x < HORIZONTAL_SIZE) &&
                                              (tileL.y >= 0) && (tileL.y < VERTICAL_SIZE))) {
                distanceX = tileL.x - tileLocation.x;
                distanceY = tileL.y - tileLocation.y;
                // alter tileVector.x to new tileVector location
                if (distanceX == VERTEX_DISTANCE) {
                    tileVector.x = tileL.x + 1;
                } else if (distanceX == -VERTEX_DISTANCE) {
                    tileVector.x = tileL.x - 1;
                } else if (distanceX == 0) {
                    tileVector.x = tileL.x;
                }
                // alter tileVector.y to new tileVector location
                if (distanceY == VERTEX_DISTANCE) {
                    tileVector.y = tileL.y + 1;
                } else if (distanceY == -VERTEX_DISTANCE) {
                    tileVector.y = tileL.y - 1; 
                } else if (distanceY == 0) {
                    tileVector.y = tileL.y;
                }
                // set tileLocation to the new campus tile
                if (i.board[tileL.x][tileL.y].tileType == CAMPUS_TILE) {
                    tileLocation.x = tileL.x;
                    tileLocation.y = tileL.y;
                } else if (i.board[tileL.x][tileL.y].tileType == NULL_TILE) {
                    tileLocation.validPath = FALSE;
                    counter = pathLength;
                }
            } else if (inputPath[counter] == 'R' && ((tileR.x >= 0) && (tileR.x < HORIZONTAL_SIZE) &&
                                                     (tileR.y >= 0) && (tileR.y < VERTICAL_SIZE))) {
                distanceX = tileR.x - tileLocation.x;
                distanceY = tileR.y - tileLocation.y;
                if (distanceX == VERTEX_DISTANCE) {
                    tileVector.x = tileR.x + 1;
                } else if (distanceX == -VERTEX_DISTANCE) {
                    tileVector.x = tileR.x - 1;
                } else if (distanceX == 0) {
                    tileVector.x = tileR.x;
                }
                if (distanceY == VERTEX_DISTANCE) {
                    tileVector.y = tileR.y + 1;
                } else if (distanceY == -VERTEX_DISTANCE) {
                    tileVector.y = tileR.y - 1; 
                } else if (distanceY == 0) {
                    tileVector.y = tileR.y;
                }
                if (i.board[tileR.x][tileR.y].tileType == CAMPUS_TILE) {
                    tileLocation.x = tileR.x;
                    tileLocation.y = tileR.y;
                } else if (i.board[tileR.x][tileR.y].tileType == NULL_TILE) {
                    tileLocation.validPath = FALSE;
                    counter = pathLength;
                }
            } else if (inputPath[counter] == 'B' && ((tileB.x >= 0) && (tileB.x < HORIZONTAL_SIZE) &&
                                                     (tileB.y >= 0) && (tileB.y < VERTICAL_SIZE))) {
                distanceX = tileB.x - tileLocation.x;
                distanceY = tileB.y - tileLocation.y;
                if (distanceX == VERTEX_DISTANCE) {
                    tileVector.x = tileB.x + 1;
                } else if (distanceX == -VERTEX_DISTANCE) {
                    tileVector.x = tileB.x - 1;
                } else if (distanceX == 0) {
                    tileVector.x = tileB.x;
                }
                if (distanceY == VERTEX_DISTANCE) {
                    tileVector.y = tileB.y + 1;
                } else if (distanceY == -VERTEX_DISTANCE) {
                    tileVector.y = tileB.y - 1; 
                } else if (distanceY == 0) {
                    tileVector.y = tileB.y;
                }
                if (i.board[tileB.x][tileB.y].tileType == CAMPUS_TILE) {
                    tileLocation.x = tileB.x;
                    tileLocation.y = tileB.y;
                } else if (i.board[tileB.x][tileB.y].tileType == NULL_TILE) {
                    tileLocation.validPath = FALSE;
                    counter = pathLength;
                }
            } else if ((tileL.x < 0) || (tileL.x >= HORIZONTAL_SIZE) ||
                       (tileL.y < 0) || (tileL.y >= VERTICAL_SIZE) ||
                       (tileR.x < 0) || (tileR.x >= HORIZONTAL_SIZE) ||
                       (tileR.y < 0) || (tileR.y >= VERTICAL_SIZE) ||
                       (tileB.x < 0) || (tileB.x >= HORIZONTAL_SIZE) ||
                       (tileB.y < 0) || (tileB.y >= VERTICAL_SIZE)) {
                tileLocation.validPath = FALSE;
                counter = pathLength;
            }
        } else if (inputPath[counter] == '\0') {
            // just to make sure it exits the function
            counter = pathLength;
        } else {
            //printf ("Invalid path %s\n", inputPath);
            //printf ("inputPath[%d] == %c\n", counter, inputPath[counter]);
            //printf ("At (%d,%d)\n", tileLocation.x, tileLocation.y);
            tileLocation.validPath = FALSE;
            counter = pathLength;
        }
        counter++;
    }
     
    if ((isARC == TRUE) && (tileLocation.validPath == TRUE)) {
        tileLocation.x+= tileLocation.x - tileVector.x;
        tileLocation.y+= tileLocation.y - tileVector.y;
    }
     
    return tileLocation;
}
 
gameInfo storeCampus (Game g, gameInfo i) {

    // outer ring towards left side
    coordinates vertexTile;
 
    vertexTile = getTile (i, "", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (7, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "");
 
    vertexTile = getTile (i, "R", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "R");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (3, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "R");
 
    vertexTile = getTile (i, "RR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (3, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RR");
 
    vertexTile = getTile (i, "RRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (0, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRL");
 
    vertexTile = getTile (i, "RRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (0, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLR");
 
    vertexTile = getTile (i, "RRLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (0, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRL");
 
    vertexTile = getTile (i, "RRLRLL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (1, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLL");
 
    vertexTile = getTile (i, "RRLRLLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (1, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLR");
 
    vertexTile = getTile (i, "RRLRLLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (2, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRL");
 
    vertexTile = getTile (i, "RRLRLLRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (2, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLR");
 
    vertexTile = getTile (i, "RRLRLLRLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (2, "RLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRL");
 
    vertexTile = getTile (i, "RRLRLLRLRLL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRLL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (2, "RLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLL");
 
    vertexTile = getTile (i, "RRLRLLRLRLLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRLLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (6, "RLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLR");
 
    vertexTile = getTile (i, "RRLRLLRLRLLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRLLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (11, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLRL");
 
    vertexTile = getTile (i, "RRLRLLRLRLLRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRLLRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (11, "RLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLRLR");
 
    //outer side right side
 
    vertexTile = getTile (i, "L", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "L");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (7, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "L");
 
    vertexTile = getTile (i, "LR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (12, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LR");
 
    vertexTile = getTile (i, "LRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (12, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRL");
 
    vertexTile = getTile (i, "LRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (16, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLR");
 
    vertexTile = getTile (i, "LRLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (16, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRL");
 
    vertexTile = getTile (i, "LRLRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (16, "RLLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLR");
 
    vertexTile = getTile (i, "LRLRLRR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (17, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRR");
 
    vertexTile = getTile (i, "LRLRLRRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (17, "RLLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRL");
 
    vertexTile = getTile (i, "LRLRLRRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLR");
 
    vertexTile = getTile (i, "LRLRLRRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLR");
 
    vertexTile = getTile (i, "LRLRLRRLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RLLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRL");
 
    vertexTile = getTile (i, "LRLRLRRLRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLR");
 
    vertexTile = getTile (i, "LRLRLRRLRLRR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLRR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRR");
 
    vertexTile = getTile (i, "LRLRLRRLRLRRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLRRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (15, "RLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRL");
 
    vertexTile = getTile (i, "LRLRLRRLRLRRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLRRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (15, "RLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRLR");
 
    vertexTile = getTile (i, "LRLRLRRLRLRRLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLRRLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (11, "RLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRLRL");
 
    //middle ring towards left
 
    vertexTile = getTile (i, "RL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (8, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RL");
 
    vertexTile = getTile (i, "RLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (8, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLR");
 
    vertexTile = getTile (i, "RLRR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (4, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRR");
 
    vertexTile = getTile (i, "RLRRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (4, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRL");
 
    vertexTile = getTile (i, "RLRRLL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (5, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLL");
 
    vertexTile = getTile (i, "RLRRLLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (5, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLR");
 
    vertexTile = getTile (i, "RLRRLLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (6, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRL");
 
    vertexTile = getTile (i, "RLRRLLRLL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLLRLL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (10, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRLL");
 
    vertexTile = getTile (i, "RLRRLLRLLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLLRLLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (10, "RLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRLLR");
 
    // middle ring towards right side
 
    vertexTile = getTile (i, "RLL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (8, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLL");
 
    vertexTile = getTile (i, "RLLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (13, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLR");
 
    vertexTile = getTile (i, "RLLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (13, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRL");
 
    vertexTile = getTile (i, "RLLRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (17, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLR");
 
    vertexTile = getTile (i, "RLLRLRR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (14, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRR");
 
    vertexTile = getTile (i, "RLLRLRRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRL");
 
    vertexTile = getTile (i, "RLLRLRRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLR");
 
    vertexTile = getTile (i, "RLLRLRRLRR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRRLRR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (15, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLRR");
 
    vertexTile = getTile (i, "RLLRLRRLRRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRRLRRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (11, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLRRL");
 
    //inner ring towards the left
 
    vertexTile = getTile (i, "RLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRL");
 
    vertexTile = getTile (i, "RLRLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLR");
 
    vertexTile = getTile (i, "RLRLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLRL");
 
    //inner ring towards right
 
    vertexTile = getTile (i, "RLRLL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "R", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLL");
 
    vertexTile = getTile (i, "RLRLLR", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLLR");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RLLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLLR");
 
    vertexTile = getTile (i, "RLRLLRL", FALSE);
    i.board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLLRL");
    i.board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RLLLL", i);
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLLRL");

        vertexTile = getTile (i, "", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "");
 
    vertexTile = getTile (i, "R", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "R");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "R");
 
    vertexTile = getTile (i, "RR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RR");
 
    vertexTile = getTile (i, "RRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRL");
 
    vertexTile = getTile (i, "RRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLR");
 
    vertexTile = getTile (i, "RRLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRL");
 
    vertexTile = getTile (i, "RRLRLL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLL");
 
    vertexTile = getTile (i, "RRLRLLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLR");
 
    vertexTile = getTile (i, "RRLRLLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRL");
 
    vertexTile = getTile (i, "RRLRLLRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLLRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLR");
 
    vertexTile = getTile (i, "RRLRLLRLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLLRLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRL");
 
    vertexTile = getTile (i, "RRLRLLRLRLL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLLRLRLL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLL");
 
    vertexTile = getTile (i, "RRLRLLRLRLLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLLRLRLLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLR");
 
    vertexTile = getTile (i, "RRLRLLRLRLLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLLRLRLLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLRL");
 
    vertexTile = getTile (i, "RRLRLLRLRLLRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RRLRLLRLRLLRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLRLR");
 
    //outer side right side
 
    vertexTile = getTile (i, "L", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "L");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "L");
 
    vertexTile = getTile (i, "LR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LR");
 
    vertexTile = getTile (i, "LRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRL");
 
    vertexTile = getTile (i, "LRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLR");
 
    vertexTile = getTile (i, "LRLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRL");
 
    vertexTile = getTile (i, "LRLRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLR");
 
    vertexTile = getTile (i, "LRLRLRR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRR");
 
    vertexTile = getTile (i, "LRLRLRRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRL");
 
    vertexTile = getTile (i, "LRLRLRRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLR");
 
    vertexTile = getTile (i, "LRLRLRRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLR");
 
    vertexTile = getTile (i, "LRLRLRRLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRL");
 
    vertexTile = getTile (i, "LRLRLRRLRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRLRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLR");
 
    vertexTile = getTile (i, "LRLRLRRLRLRR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRLRLRR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRR");
 
    vertexTile = getTile (i, "LRLRLRRLRLRRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRLRLRRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRL");
 
    vertexTile = getTile (i, "LRLRLRRLRLRRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRLRLRRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRLR");
 
    vertexTile = getTile (i, "LRLRLRRLRLRRLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "LRLRLRRLRLRRLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRLRL");
 
    //middle ring towards left
 
    vertexTile = getTile (i, "RL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RL");
 
    vertexTile = getTile (i, "RLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLR");
 
    vertexTile = getTile (i, "RLRR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRR");
 
    vertexTile = getTile (i, "RLRRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRL");
 
    vertexTile = getTile (i, "RLRRLL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRRLL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLL");
 
    vertexTile = getTile (i, "RLRRLLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRRLLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLR");
 
    vertexTile = getTile (i, "RLRRLLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRRLLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRL");
 
    vertexTile = getTile (i, "RLRRLLRLL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRRLLRLL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRLL");
 
    vertexTile = getTile (i, "RLRRLLRLLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRRLLRLLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRLLR");
 
    // middle ring towards right side
 
    vertexTile = getTile (i, "RLL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLL");
 
    vertexTile = getTile (i, "RLLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLR");
 
    vertexTile = getTile (i, "RLLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRL");
 
    vertexTile = getTile (i, "RLLRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLLRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLR");
 
    vertexTile = getTile (i, "RLLRLRR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLLRLRR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRR");
 
    vertexTile = getTile (i, "RLLRLRRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLLRLRRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRL");
 
    vertexTile = getTile (i, "RLLRLRRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLLRLRRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLR");
 
    vertexTile = getTile (i, "RLLRLRRLRR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLLRLRRLRR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLRR");
 
    vertexTile = getTile (i, "RLLRLRRLRRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLLRLRRLRRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLRRL");
 
    //inner ring towards the left
 
    vertexTile = getTile (i, "RLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRL");
 
    vertexTile = getTile (i, "RLRLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLR");
 
    vertexTile = getTile (i, "RLRLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLRL");
 
    //inner ring towards right
 
    vertexTile = getTile (i, "RLRLL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRLL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLL");
 
    vertexTile = getTile (i, "RLRLLR", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRLLR");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLLR");
 
    vertexTile = getTile (i, "RLRLLRL", TRUE);
    i.board[vertexTile.x][vertexTile.y].id = getARC (g, "RLRLLRL");
    strcpy (i.board[vertexTile.x][vertexTile.y].vertexPath, "RLRLLRL");
    
    return i;
}
 
buildLoc getCampusTiles (Game g, gameInfo i) {
 
    int row = 0;
    int column = 0;
    int currentPlayer;
    int counter = 0;
    int counter1 = 0;
    buildLoc campusCoordinates;
 
    while (counter1 < 50) {
        campusCoordinates.buildDirection[counter1].x = 0;
        campusCoordinates.buildDirection[counter1].y = 0;
        counter1++;
    }
 
    currentPlayer = getWhoseTurn (g);
 
    while (column < HORIZONTAL_SIZE ) {
        while (row < VERTICAL_SIZE) {
            if (i.board[column][row].id == currentPlayer) {
                campusCoordinates.buildDirection[counter].x = column;
                campusCoordinates.buildDirection[counter].y = row;
                //printf("column = %d\n", column);
                //printf("row = %d\n", row);
                counter++;
            }
            row++;
        }
        row = 0;
        column++;
    }
    return campusCoordinates;
}
 
action getBuildLocations (Game g, buildLoc campusCoordinates, gameInfo i) {
 
    int counter = 0;
    int counter1 = 0;
    int counter2 = 0;
    //int counter3 = 0;
    int locX = 0;
    int locY = 0;
    int maximum;
     
     
    int pathlength;
    int pathlength1;
 
    int campusValue[20][20] = {{0}};
 
    maximum = campusValue[0][0];
 
    coordinates CoordN, CoordNE, CoordSE, CoordS, CoordSW, CoordNW;
 
    action actionN, actionNE, actionSE, actionS, actionSW, actionNW;
    action checkLoc;
    action nextAction;
 
    while (campusCoordinates.buildDirection[counter].x != 0) {
     
        strcpy (checkLoc.destination, 
                i.board[campusCoordinates.buildDirection[counter].x]
                [campusCoordinates.buildDirection[counter].y].vertexPath);
 
        pathlength = strlen (checkLoc.destination);
 
        //printf("this is counter %d\n", counter);
 
        //printf("this is pathlength %d\n", pathlength);
 
        strcpy (actionN.destination, checkLoc.destination);
        strcpy (actionNE.destination, checkLoc.destination);
        strcpy (actionSE.destination, checkLoc.destination);
        strcpy (actionS.destination, checkLoc.destination);
        strcpy (actionSW.destination, checkLoc.destination);
        strcpy (actionNW.destination, checkLoc.destination);
 
        //printf("this is acctionSW %s\n", actionSW.destination);
 
        actionN.destination[pathlength] = 'R';
        actionN.destination[pathlength + 1] = 'L';
        actionN.destination[pathlength + 2] = '\0';
 
        //printf("this is acctionN %s\n", actionN.destination);
 
        actionNE.destination[pathlength] = 'R';
        actionNE.destination[pathlength + 1] = 'R';
        actionNE.destination[pathlength + 2] = '\0';
 
        //printf("this is acctionNE %s\n", actionNE.destination);
 
        actionSE.destination[pathlength] = 'B';
        actionSE.destination[pathlength + 1] = 'R';
        actionSE.destination[pathlength + 2] = '\0';
 
        //printf("this is acctionSE %s\n", actionSE.destination);
 
        actionS.destination[pathlength] = 'B';
        actionS.destination[pathlength + 1] = 'L';
        actionS.destination[pathlength + 2] = '\0';
 
        //printf("this is acctionS %s\n", actionS.destination);
 
        actionSW.destination[pathlength] = 'L';
        actionSW.destination[pathlength + 1] = 'L';
        actionSW.destination[pathlength + 2] = '\0';
        //printf("this is acctionSW %s\n", actionSW.destination);
 
        actionNW.destination[pathlength] = 'L';
        actionNW.destination[pathlength + 1] = 'R';
        actionNW.destination[pathlength + 2] = '\0';
        //printf("this is acctionNW %s\n", actionNW.destination);
 
        CoordN = getTile (i, actionN.destination, FALSE);
        //printf("CoordN %d %d", CoordN.x, CoordN.y);
        CoordNE = getTile (i, actionNE.destination, FALSE);
        CoordSE = getTile (i, actionSE.destination, FALSE);
        CoordS = getTile (i, actionS.destination, FALSE);
        CoordSW = getTile (i, actionSW.destination, FALSE);
        CoordNW = getTile (i, actionNW.destination, FALSE);
 
        action tempN, tempNE, tempSE, tempS, tempSW, tempNW;
        int actionLength;
 
        strcpy (tempN.destination, actionN.destination);
        strcpy (tempNE.destination, actionNE.destination);
        strcpy (tempSE.destination, actionSE.destination);
        strcpy (tempS.destination, actionS.destination);
        strcpy (tempSW.destination, actionSW.destination);
        strcpy (tempNW.destination, actionNW.destination);
 
        actionLength = strlen (tempN.destination);
 
        tempN.destination[actionLength - 1] = '\0';
        tempNE.destination[actionLength - 1] = '\0';
        tempSE.destination[actionLength - 1] = '\0';
        tempS.destination[actionLength - 1] = '\0';
        tempSW.destination[actionLength - 1] = '\0';
        tempNW.destination[actionLength - 1] = '\0';
 
        action tempN1, tempNE1, tempSE1, tempS1, tempSW1, tempNW1;
        action tempN2, tempNE2, tempSE2, tempS2, tempSW2, tempNW2;
        action tempN3, tempNE3, tempSE3, tempS3, tempSW3, tempNW3;
 
        strcpy (tempN1.destination, actionN.destination);
        strcpy (tempNE1.destination, actionNE.destination);
        strcpy (tempSE1.destination, actionSE.destination);
        strcpy (tempS1.destination, actionS.destination);
        strcpy (tempSW1.destination, actionSW.destination);
        strcpy (tempNW1.destination, actionNW.destination);
 
        strcpy (tempN2.destination, actionN.destination);
        strcpy (tempNE2.destination, actionNE.destination);
        strcpy (tempSE2.destination, actionSE.destination);
        strcpy (tempS2.destination, actionS.destination);
        strcpy (tempSW2.destination, actionSW.destination);
        strcpy (tempNW2.destination, actionNW.destination);
 
        strcpy (tempN3.destination, actionN.destination);
        strcpy (tempNE3.destination, actionNE.destination);
        strcpy (tempSE3.destination, actionSE.destination);
        strcpy (tempS3.destination, actionS.destination);
        strcpy (tempSW3.destination, actionSW.destination);
        strcpy (tempNW3.destination, actionNW.destination);
 
        tempN1.destination[actionLength] = 'R';
        tempNE1.destination[actionLength] = 'R';
        tempSE1.destination[actionLength] = 'R';
        tempS1.destination[actionLength] = 'R';
        tempSW1.destination[actionLength] = 'R';
        tempNW1.destination[actionLength] = 'R';
 
        tempN1.destination[actionLength + 1] = '\0';
        tempNE1.destination[actionLength + 1] = '\0';
        tempSE1.destination[actionLength + 1] = '\0';
        tempS1.destination[actionLength + 1] = '\0';
        tempSW1.destination[actionLength + 1] = '\0';
        tempNW1.destination[actionLength + 1] = '\0';
 
        tempN2.destination[actionLength] = 'L';
        tempNE2.destination[actionLength] = 'L';
        tempSE2.destination[actionLength] = 'L';
        tempS2.destination[actionLength] = 'L';
        tempSW2.destination[actionLength] = 'L';
        tempNW2.destination[actionLength] = 'L';
 
        tempN2.destination[actionLength + 1] = '\0';
        tempNE2.destination[actionLength + 1] = '\0';
        tempSE2.destination[actionLength + 1] = '\0';
        tempS2.destination[actionLength + 1] = '\0';
        tempSW2.destination[actionLength + 1] = '\0';
        tempNW2.destination[actionLength + 1] = '\0';
 
        tempN3.destination[actionLength] = 'B';
        tempNE3.destination[actionLength] = 'B';
        tempSE3.destination[actionLength] = 'B';
        tempS3.destination[actionLength] = 'B';
        tempSW3.destination[actionLength] = 'B';
        tempNW3.destination[actionLength] = 'B';
 
        tempN3.destination[actionLength + 1] = '\0';
        tempNE3.destination[actionLength + 1] = '\0';
        tempSE3.destination[actionLength + 1] = '\0';
        tempS3.destination[actionLength + 1] = '\0';
        tempSW3.destination[actionLength + 1] = '\0';
        tempNW3.destination[actionLength + 1] = '\0';
 
        coordinates coord1, coord2, coord3, coord4, coord5, coord6;
        coordinates coord1a, coord2a, coord3a, coord4a, coord5a, coord6a;
        coordinates coord1b, coord2b, coord3b, coord4b, coord5b, coord6b;
        coordinates coord1c, coord2c, coord3c, coord4c, coord5c, coord6c;
        coordinates coord1d, coord2d, coord3d, coord4d, coord5d, coord6d;
        coordinates coord1e, coord2e, coord3e, coord4e, coord5e, coord6e;
        
        coord1 = getTile (i, actionN.destination, FALSE);
        coord1a = getTile (i, actionN.destination, TRUE);
        coord1b = getTile (i, tempN.destination, TRUE);
        coord1c = getTile (i, tempN1.destination, FALSE);
        coord1d = getTile (i, tempN2.destination, FALSE);
        coord1e = getTile (i, tempN3.destination, FALSE);
        if (coord1.validPath == TRUE) {
        if ( getCampus (g, actionN.destination) == VACANT_VERTEX ) {
            if ( coord1b.validPath == TRUE || coord1b.validPath == TRUE) {
            if ( getARC (g, tempN.destination) == VACANT_ARC ||
                 getARC (g, tempN.destination) == i.currentPlayer )
                if ( coord1a.validPath == TRUE || coord1a.validPath == TRUE) {
                if ( getARC (g, actionN.destination) == VACANT_ARC ||
                     getARC (g, actionN.destination) == i.currentPlayer )
                    if (coord1c.validPath == TRUE && coord1d.validPath == TRUE &&
                        coord1e.validPath == TRUE) {
                    if (getCampus (g, tempN1.destination) == VACANT_VERTEX &&
                        getCampus (g, tempN2.destination) == VACANT_VERTEX &&
                        getCampus (g, tempN3.destination) == VACANT_VERTEX) {
                        campusValue[0][counter] = i.board[CoordN.x][CoordN.y].rollWeighting;
                        }    
                    }
                }
            }
            //printf("this is N rollweighting %d\n", i.board[CoordN.x][CoordN.y].rollWeighting);
        }
        coord2 = getTile (i, actionNE.destination, FALSE);
        coord2a = getTile (i, actionNE.destination, TRUE);
        coord2b = getTile (i, tempNE.destination, TRUE);
        coord2c = getTile (i, tempNE1.destination, FALSE);
        coord2d = getTile (i, tempNE2.destination, FALSE);
        coord2e = getTile (i, tempNE3.destination, FALSE);
        if (coord2.validPath == TRUE) {
        if ( getCampus (g, actionNE.destination) == VACANT_VERTEX ) {
            if ( coord2b.validPath == TRUE || coord2b.validPath == TRUE) {
            if (getARC (g, tempNE.destination) == VACANT_ARC ||
                getARC (g, tempNE.destination) == i.currentPlayer) {
                if ( coord2a.validPath == TRUE || coord2a.validPath == TRUE) {
                if (getARC (g, actionNE.destination) == VACANT_ARC ||
                    getARC (g, actionNE.destination) == i.currentPlayer) {
                    if (coord2c.validPath == TRUE && coord2d.validPath == TRUE &&
                        coord2e.validPath == TRUE) {
                    if (getCampus (g, tempNE1.destination) == VACANT_VERTEX &&
                        getCampus (g, tempNE2.destination) == VACANT_VERTEX &&
                        getCampus (g, tempNE3.destination) == VACANT_VERTEX) {
                        campusValue[1][counter] = i.board[CoordNE.x][CoordNE.y].rollWeighting;
                        }
                    }
                }
            }
            //printf("this is NE rollweighting %d\n", i.board[CoordNE.x][CoordNE.y].rollWeighting);
        }
        coord3 = getTile (i, actionSE.destination, FALSE);
        coord3a = getTile (i, actionSE.destination, TRUE);
        coord3b = getTile (i, tempSE.destination, TRUE);
        coord3c = getTile (i, tempSE1.destination, FALSE);
        coord3d = getTile (i, tempSE2.destination, FALSE);
        coord3e = getTile (i, tempSE3.destination, FALSE);
        if (coord3.validPath == TRUE) {   
        if ( getCampus (g, actionSE.destination) == VACANT_VERTEX ) {
            if ( coord3b.validPath == TRUE || coord3b.validPath == TRUE) {
            if (getARC (g, tempSE.destination) == VACANT_ARC ||
                getARC (g, tempSE.destination) == i.currentPlayer) {
                if ( coord3a.validPath == TRUE || coord3a.validPath == TRUE) {
                if (getARC (g, actionSE.destination) == VACANT_ARC ||
                    getARC (g, actionSE.destination) == i.currentPlayer) {
                    if (coord3c.validPath == TRUE && coord3d.validPath == TRUE &&
                        coord3e.validPath == TRUE) {
                    if (getCampus (g, tempSE1.destination) == VACANT_VERTEX &&
                        getCampus (g, tempSE2.destination) == VACANT_VERTEX &&
                        getCampus (g, tempSE3.destination) == VACANT_VERTEX) {
                        campusValue[2][counter] = i.board[CoordSE.x][CoordSE.y].rollWeighting;
                        }    
                    }
                }    
            }
            //printf("this is SE rollweighting %d\n", i.board[CoordSE.x][CoordSE.y].rollWeighting);
        }
        coord4 = getTile (i, actionS.destination, FALSE);
        coord4a = getTile (i, actionS.destination, TRUE);
        coord4b = getTile (i, tempS.destination, TRUE);
        coord4c = getTile (i, tempS1.destination, FALSE);
        coord4d = getTile (i, tempS2.destination, FALSE);
        coord4e = getTile (i, tempS3.destination, FALSE);
        if (coord4.validPath == TRUE) {
        if ( (getCampus (g, actionS.destination)) == VACANT_VERTEX ) {
            if ( coord4b.validPath == TRUE || coord4b.validPath == TRUE) {
            if (getARC (g, tempS.destination) == VACANT_ARC ||
                getARC (g, tempS.destination) == i.currentPlayer) {
                if ( coord4a.validPath == TRUE || coord4a.validPath == TRUE) {
                if (getARC (g, actionS.destination) == VACANT_ARC ||
                    getARC (g, actionS.destination) == i.currentPlayer) {
                    if (getCampus (g, tempS1.destination) == VACANT_VERTEX &&
                        getCampus (g, tempS2.destination) == VACANT_VERTEX &&
                        getCampus (g, tempS3.destination) == VACANT_VERTEX) {
                         
                            campusValue[3][counter] = i.board[CoordS.x][CoordS.y].rollWeighting;
                        }    
                    }
                }
            }
            //printf("this is S rollweighting %d\n", i.board[CoordS.x][CoordS.y].rollWeighting);
        }
        coord5 = getTile (i, actionSW.destination, FALSE);
                        if (coord5.validPath == TRUE) {
        if ( (getCampus (g, actionSW.destination)) == VACANT_VERTEX ) {
            if (getARC (g, tempSW.destination) == VACANT_ARC ||
                getARC (g, tempSW.destination) == i.currentPlayer) {
                if (getARC (g, actionSW.destination) == VACANT_ARC ||
                    getARC (g, actionSW.destination) == i.currentPlayer) {
                    if (getCampus (g, tempSW1.destination) == VACANT_VERTEX &&
                        getCampus (g, tempSW2.destination) == VACANT_VERTEX &&
                        getCampus (g, tempSW3.destination) == VACANT_VERTEX) {
                         
                            campusValue[4][counter] = i.board[CoordSW.x][CoordSW.y].rollWeighting;
                        }
                    }
                }
            }
            //printf("this is SW rollweighting %d\n", i.board[CoordSW.x][CoordSW.y].rollWeighting);
        }
        coord6 = getTile (i, actionNW.destination, FALSE);
                        if (coord6.validPath == TRUE) {
        if ( (getCampus (g, actionNW.destination)) == VACANT_VERTEX ) {
            if (getARC (g, tempNW.destination) == VACANT_ARC ||
                getARC (g, tempNW.destination) == i.currentPlayer) {
                if (getARC (g, actionNW.destination) == VACANT_ARC ||
                    getARC (g, actionNW.destination) == i.currentPlayer) {
                    if (getCampus (g, tempNW1.destination) == VACANT_VERTEX &&
                        getCampus (g, tempNW2.destination) == VACANT_VERTEX &&
                        getCampus (g, tempNW3.destination) == VACANT_VERTEX) {
                         
                            campusValue[5][counter] = i.board[CoordNW.x][CoordNW.y].rollWeighting;
                        }
                    }
                }
            }
            //printf("this is NW rollweighting %d\n", i.board[CoordNW.x][CoordNW.y].rollWeighting);
        }
        counter++;
    }
 
    maximum = campusValue[0][0];
 
    while (counter1 < 20) {
        while (counter2 < 6) {
            if (campusValue[counter2][counter1] > maximum) {
                maximum = campusValue[counter2][counter1];
                locX = counter2;
                locY = counter1;
            }
            counter2++;
        }
        counter2 = 0;
        counter1++;
    }
    //printf("LOCY IS %d\n", locY);
    //printf("LOCX IS %d\n", locX);
 
    strcpy (nextAction.destination,
            i.board[campusCoordinates.buildDirection[locY].x]
            [campusCoordinates.buildDirection[locY].y].vertexPath);
             
    pathlength1 = strlen (nextAction.destination);
 
    if (locX == 0) {
        nextAction.destination[pathlength1] = 'R';
        nextAction.destination[pathlength1 + 1] = 'L';
        //printf ("vertex 1 owner = %d\n", getCampus (g, tempN1.destination));
        //printf ("vertex 2 owner = %d\n", getCampus (g, tempN2.destination));
        //printf ("vertex 3 owner = %d\n", getCampus (g, tempN3.destination));
    } else if (locX == 1) {
        nextAction.destination[pathlength1] = 'R';
        nextAction.destination[pathlength1 + 1] = 'R';
        //printf ("vertex 1 owner = %d\n", getCampus (g, tempNE1.destination));
        //printf ("vertex 2 owner = %d\n", getCampus (g, tempNE2.destination));
        //printf ("vertex 3 owner = %d\n", getCampus (g, tempNE3.destination));
    } else if (locX == 2) {
        nextAction.destination[pathlength1] = 'B';
        nextAction.destination[pathlength1 + 1] = 'R';
        //printf ("vertex 1 owner = %d\n", getCampus (g, tempSE1.destination));
        //printf ("vertex 2 owner = %d\n", getCampus (g, tempSE2.destination));
        //printf ("vertex 3 owner = %d\n", getCampus (g, tempSE3.destination));
    } else if (locX == 3) {
        nextAction.destination[pathlength1] = 'B';
        nextAction.destination[pathlength1 + 1] = 'L';
        //printf ("vertex 1 owner = %d\n", getCampus (g, tempS1.destination));
        //printf ("vertex 2 owner = %d\n", getCampus (g, tempS2.destination));
        //printf ("vertex 3 owner = %d\n", getCampus (g, tempS3.destination));
    } else if (locX == 4) {
        nextAction.destination[pathlength1] = 'L';
        nextAction.destination[pathlength1 + 1] = 'L';
        //printf ("vertex 1 owner = %d\n", getCampus (g, tempSW1.destination));
        //printf ("vertex 2 owner = %d\n", getCampus (g, tempSW2.destination));
        //printf ("vertex 3 owner = %d\n", getCampus (g, tempSW3.destination));
    } else if (locX == 5) {
        nextAction.destination[pathlength1] = 'L';
        nextAction.destination[pathlength1 + 1] = 'R';
        //printf ("vertex 1 owner = %d\n", getCampus (g, tempNW1.destination));
        //printf ("vertex 2 owner = %d\n", getCampus (g, tempNW2.destination));
        //printf ("vertex 3 owner = %d\n", getCampus (g, tempNW3.destination));
    }
 
    action buildposition;
    int stringlength;
 
    action temp1, temp2, temp3;
    int stringlength1 = strlen (nextAction.destination);
 
    strcpy (temp1.destination, nextAction.destination);
    strcpy (temp2.destination, nextAction.destination);
    strcpy (temp3.destination, nextAction.destination);
 
    temp1.destination[stringlength1] = 'R';
    temp1.destination[stringlength1 + 1] = '\0';
 
    temp2.destination[stringlength1] = 'L';
    temp2.destination[stringlength1 + 1] = '\0';
 
    temp3.destination[stringlength1] = 'B';
    temp3.destination[stringlength1 + 1] = '\0';
    /*
    printf ("temp1 owner = %d\n", getCampus (g, temp1.destination));
    printf ("temp1 string = %s\n", temp1.destination);
    printf ("temp2 owner = %d\n", getCampus (g, temp2.destination));
    printf ("temp2 string = %s\n", temp2.destination);
    printf ("temp3 owner = %d\n", getCampus (g, temp3.destination));
    printf ("temp3 string = %s\n", temp3.destination);
    */
    if (getCampus (g, nextAction.destination) == VACANT_VERTEX) {
        strcpy (buildposition.destination, nextAction.destination);
        stringlength = strlen (buildposition.destination);
        printf("this is buildposition %s\n", buildposition.destination);
        buildposition.destination[stringlength - 1] = 0;
        printf("this is arc buildposition %s\n", buildposition.destination);
        if (getARC (g, buildposition.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, buildposition.destination);
            nextAction.actionCode = OBTAIN_ARC;
        } else if (getARC (g, nextAction.destination) == VACANT_ARC) {
            nextAction.actionCode = OBTAIN_ARC;
        } else if (getARC (g, nextAction.destination) == i.currentPlayer) {
            nextAction.actionCode = BUILD_CAMPUS;
        }
    }  
 
    printf("currentPlayer is %d\n", i.currentPlayer);
    printf("arcOwner1 is %d\n", getARC (g, buildposition.destination));
    printf("arcOwner2 is %d\n", getARC (g, nextAction.destination));
    printf("vertexowner is %d\n", getCampus (g, nextAction.destination));
        
    return nextAction;
}
 
int getRollWeighting (int region, path vertexLoc, gameInfo i) {
 
    int map[7][11] = {{0}};
    int x = 0;
    int y = 0;
    int vertexValue = 0;
 
    map[1][3] = i.board1[0].rollWeighting;
    map[1][5] = i.board1[1].rollWeighting;
    map[1][7] = i.board1[2].rollWeighting;
    map[2][2] = i.board1[3].rollWeighting;
    map[2][4] = i.board1[4].rollWeighting;
    map[2][6] = i.board1[5].rollWeighting;
    map[2][8] = i.board1[6].rollWeighting;
    map[3][1] = i.board1[7].rollWeighting;
    map[3][3] = i.board1[8].rollWeighting;
    map[3][5] = i.board1[9].rollWeighting;
    map[3][7] = i.board1[10].rollWeighting;
    map[3][9] = i.board1[11].rollWeighting;
    map[4][2] = i.board1[12].rollWeighting;
    map[4][4] = i.board1[13].rollWeighting;
    map[4][6] = i.board1[14].rollWeighting;
    map[4][8] = i.board1[15].rollWeighting;
    map[5][3] = i.board1[16].rollWeighting;
    map[5][5] = i.board1[17].rollWeighting;
    map[5][7] = i.board1[18].rollWeighting;
 
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
