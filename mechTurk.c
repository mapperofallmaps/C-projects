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

#define NUMBER_OF_DISCIPLINES 6
 
#define NO_STUDENT -1
#define NO_DICE 0
#define TERRA_NULLIUS_TURN -1
 
// fallback to stop THDs being retrained
#define UNRETRAINABLE 2147483646
 
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
action simpleExchange (Game g, gameInfo i);
action simpleSpinoff (Game g, gameInfo i);
int getRollWeighting (int region, path vertexLoc, gameInfo i);
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
     
    gameInfo i;
     
    // store game information
    i.currentTurn = getTurnNumber (g);
    printf ("Turn: %d\n", i.currentTurn);
    i.currentPlayer = getWhoseTurn (g);
    printf ("Player: %d\n", i.currentPlayer);
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
     
     
    //assigning rollweighting to every region
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
            i.board1[counter].rollWeighting += 3;
        } else if (i.board1[counter].discipline == STUDENT_BQN) {
            i.board1[counter].rollWeighting += 3;
        } else if (i.board1[counter].discipline == STUDENT_MJ) {
            i.board1[counter].rollWeighting += 2;
        } else if (i.board1[counter].discipline == STUDENT_MTV) {
            i.board1[counter].rollWeighting += 1;
        } else if (i.board1[counter].discipline == STUDENT_MMONEY) {
            i.board1[counter].rollWeighting += 1;
        } else if (i.board1[counter].discipline == STUDENT_THD) {
            i.board1[counter].rollWeighting -= 7;
        }

        if ((i.board1[counter].discipline == STUDENT_MMONEY ||
             i.board1[counter].discipline == STUDENT_MTV)&& 
             i.board1[counter].diceValue == 7) {
             i.board1[counter].rollWeighting -= 8;
        } 
        counter++;
    }

    i = clearBoard (i); 
    i = setInitialCampusesAndARCs (i);
    i = storeCampus (g, i);

    buildLoc campusLocations;
    campusLocations = getCampusTiles (g, i);

    nextAction = getBuildLocations (g, campusLocations, i);

    int stringlength = 0;
    nextAction.actionCode = OBTAIN_ARC;

    action buildposition;

    if (getCampus (g, nextAction.destination) == VACANT_VERTEX) {
        strcpy (buildposition.destination, nextAction.destination);
        stringlength = strlen (buildposition.destination);
        printf("thisis buildposition %s\n", buildposition.destination);
        buildposition.destination[stringlength - 1] = 0;
        printf("this is buildposition %s\n", buildposition.destination);
        if (getARC (g, buildposition.destination) == VACANT_ARC) {
            strcpy (nextAction.destination, buildposition.destination);
        } else if (getARC (g, nextAction.destination) == VACANT_ARC) {
            nextAction.actionCode = OBTAIN_ARC;
        } 
    }  
    
       
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
                nextAction.actionCode = RETRAIN_STUDENTS;
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
                        }  else if (i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 
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
                        }  else if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 
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
                    } 
                } 
            // apply special cases, i.e. when no campus building resources are
            // ready to be exchanged
            } else if ((i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 
                        i.uni[i.currentPlayer].exchangeRate[STUDENT_MMONEY]) &&
                        i.uni[i.currentPlayer].discipline[STUDENT_BPS] >= 1 &&
                        i.uni[i.currentPlayer].discipline[STUDENT_BQN] >= 1 && 
                        i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 1) {
                // with excess MMONEY and sufficient BPS/BQN/MJ, then make an MTV
                nextAction.actionCode = RETRAIN_STUDENTS;
                nextAction.disciplineFrom = STUDENT_MMONEY;
                nextAction.disciplineTo = STUDENT_MTV;
            } else if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 
                       i.uni[i.currentPlayer].exchangeRate[STUDENT_MMONEY]) {
                // else, figure out whether BPS/BQN/MJ needs the extra student more
                nextAction = simpleExchange (g, i);
            }
        }
    } else {
        // make a spinoff if it's a good idea with current resources
        nextAction = simpleSpinoff (g, i);
        if (nextAction.actionCode != START_SPINOFF && 
            nextAction.actionCode != RETRAIN_STUDENTS) {
            // a.k.a smart saving, take stuff away from MTV and MMONEY
            // put it into the un-7-able students
            // only do it when not trying to get a spinoff
            nextAction = simpleExchange (g, i);
        }
    }
    
    if (i.uni[i.currentPlayer].kpi == 140) {
        nextAction = simpleSpinoff (g, i);
    }

    // use variables randomly
    if (ourGO8 == UNI_A + 999) {
        i.currentTurn++;
        ourGO8 = enemy2 + enemy1 + ourCampus;
    }

    if (i.uni[i.currentPlayer].discipline[STUDENT_MJ] >= 2 &&
        i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] >= 3 && 
        (getGO8s (g, UNI_A) + getGO8s (g, UNI_B) + getGO8s (g, UNI_C) < MAX_GO8)) {
        nextAction.actionCode = BUILD_GO8;
        while (getCampus (g, i.board[campusLocations.buildDirection[count].x]
                       [campusLocations.buildDirection[count].y].vertexPath) !=
                        getWhoseTurn (g)) {
            nextAction.destination = i.board[campusLocations.buildDirection[count].x]
                                     [campusLocations.buildDirection[count].y].vertexPath;
            count++;
        }
    }
    
     
    printf ("End Move;\n\
            actionCode = %d\n\
            destination = %s\n\
            disciplineFrom = %d\n\
            disciplineTo = %d\n\
            playerKPI == %d\n", 
            nextAction.actionCode, 
            nextAction.destination, 
            nextAction.disciplineFrom, 
            nextAction.disciplineTo,
            i.uni[UNI_A].kpi);
    printf ("Is legal? %d\n", isLegalAction (g, nextAction));
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
    int currentPlayer = getTurnNumber (g) % 3 + 1;
    int studentMMONEY = getStudents (g, currentPlayer, STUDENT_MMONEY);
    // i.uni[currentPlayer].discipline[STUDENT_MMONEY] broken???
     
    if ((i.uni[currentPlayer].discipline[STUDENT_MJ] > 0) && 
        (i.uni[currentPlayer].discipline[STUDENT_MTV] > 0) && 
        (studentMMONEY > 0)) {
        // with sufficient resources, start a spinoff
        spinoffAction.actionCode = START_SPINOFF;
    } else {  
        if (((int) ((double)(i.uni[currentPlayer].discipline[STUDENT_BPS] / 
                             i.uni[currentPlayer].exchangeRate[STUDENT_BPS])) + 
                   (double)((i.uni[currentPlayer].discipline[STUDENT_BQN] /
                             i.uni[currentPlayer].exchangeRate[STUDENT_BQN])) >= 
                        (2 - i.uni[currentPlayer].discipline[STUDENT_MTV] - 
                             i.uni[currentPlayer].discipline[STUDENT_MMONEY])) && 
                            (i.uni[currentPlayer].discipline[STUDENT_MJ] > 0)) {
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
            } else if (studentMMONEY < 1) {
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
        } else if (i.uni[currentPlayer].discipline[STUDENT_MJ] >= 
                   i.uni[currentPlayer].exchangeRate[STUDENT_MJ] * 2 + 1) {
            // if we have MJs such that we conver them into 1 MTV and 1 MMONEY
            // such that in total we have enough to make a spinoff, then start the exchange
            // repeat this concept for BPS, BQN, MTV, MMONEY
            // this ensures if we have excess of one of the resources, we can quickly
            // conver it into spinoff-able resources then create the spin-off
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MJ;
            spinoffAction.disciplineTo = STUDENT_MTV;
        } else if ((i.uni[currentPlayer].discipline[STUDENT_MJ] >= 
                    i.uni[currentPlayer].exchangeRate[STUDENT_MJ] + 1) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0 ||
                    studentMMONEY == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MJ;
             
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (studentMMONEY == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_BPS] >= 
                   i.uni[currentPlayer].exchangeRate[STUDENT_BPS] * 2 + 1) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_BPS;
            spinoffAction.disciplineTo = STUDENT_MTV;
        } else if ((i.uni[currentPlayer].discipline[STUDENT_BPS] >= 
                    i.uni[currentPlayer].exchangeRate[STUDENT_BPS] + 1) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0 ||
                    studentMMONEY == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_BPS;
             
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (studentMMONEY == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_BQN] >= 
                   i.uni[currentPlayer].exchangeRate[STUDENT_BQN] * 2 + 1) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_BQN;
             
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (studentMMONEY == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if ((i.uni[currentPlayer].discipline[STUDENT_BQN] >= 
                    i.uni[currentPlayer].exchangeRate[STUDENT_BQN] + 1) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0 ||
                    studentMMONEY == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_BQN;
             
            if (i.uni[currentPlayer].discipline[STUDENT_MTV] == 0) {
                spinoffAction.disciplineTo = STUDENT_MTV;
            } else if (studentMMONEY == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_MTV] >= 
                   i.uni[currentPlayer].exchangeRate[STUDENT_MTV] * 2 + 1) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MTV;
            spinoffAction.disciplineTo = STUDENT_MJ;
        } else if ((i.uni[currentPlayer].discipline[STUDENT_MTV] >= 
                    i.uni[currentPlayer].exchangeRate[STUDENT_MTV] + 1) && 
                   (i.uni[currentPlayer].discipline[STUDENT_MJ] == 0 ||
                    studentMMONEY == 0)) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MTV;
             
            if (i.uni[currentPlayer].discipline[STUDENT_MJ] == 0) {
                spinoffAction.disciplineTo = STUDENT_MJ;
            } else if (studentMMONEY == 0) {
                spinoffAction.disciplineTo = STUDENT_MMONEY;
            }
        } else if (i.uni[currentPlayer].discipline[STUDENT_MMONEY] >= 
                   i.uni[currentPlayer].exchangeRate[STUDENT_MMONEY] * 2 + 1) {
            spinoffAction.actionCode = RETRAIN_STUDENTS;
            spinoffAction.disciplineFrom = STUDENT_MMONEY;
            spinoffAction.disciplineTo = STUDENT_MJ;
        } else if ((i.uni[currentPlayer].discipline[STUDENT_MMONEY] >= 
                    i.uni[currentPlayer].exchangeRate[STUDENT_MMONEY] + 1) && 
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
    int currentPlayer = getTurnNumber (g) % 3 + 1;
    int studentMMONEY = getStudents (g, currentPlayer, STUDENT_MMONEY);
    // i.uni[currentPlayer].discipline[STUDENT_MMONEY] broken???
     
    if (studentMMONEY > i.uni[currentPlayer].exchangeRate[STUDENT_MMONEY]) {
        exchangeAction.disciplineFrom = STUDENT_MMONEY;
    } else if (i.uni[currentPlayer].discipline[STUDENT_MTV] > 
        i.uni[currentPlayer].exchangeRate[STUDENT_MTV]) {
        exchangeAction.disciplineFrom = STUDENT_MTV;
    }
     
    if ((i.uni[currentPlayer].discipline[STUDENT_BPS] >= 
         i.uni[currentPlayer].discipline[STUDENT_BQN]) && 
        (i.uni[currentPlayer].discipline[STUDENT_MJ] >= 
         i.uni[currentPlayer].discipline[STUDENT_BQN])) {
        exchangeAction.disciplineTo = STUDENT_BQN;
    } else if ((i.uni[currentPlayer].discipline[STUDENT_BQN] >= 
                i.uni[currentPlayer].discipline[STUDENT_BPS]) && 
               (i.uni[currentPlayer].discipline[STUDENT_MJ] >= 
                i.uni[currentPlayer].discipline[STUDENT_BPS])) {
        exchangeAction.disciplineTo = STUDENT_BPS;
    } else if ((i.uni[currentPlayer].discipline[STUDENT_BQN] >= 
                i.uni[currentPlayer].discipline[STUDENT_MJ]) && 
               (i.uni[currentPlayer].discipline[STUDENT_BPS] >= 
                i.uni[currentPlayer].discipline[STUDENT_MJ])) {
        exchangeAction.disciplineTo = STUDENT_MJ;
    }
     
    return exchangeAction;
}

/*action buildLoc (Game g, gameInfo i) {


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
*/

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
    
    /*printf ("Printing the game board\n");
    row = 0;
    column = 0;
    while (row < VERTICAL_SIZE) {
        while (column < HORIZONTAL_SIZE) {
            if ((column != 0) && (column != 24)) {
                if (i.board[column][row].tileType == NULL_TILE) {
                    printf ("*");
                } else if (i.board[column][row].tileType == CAMPUS_TILE) {
                    if (i.board[column][row].id != VACANT_VERTEX) {
                        printf ("%d", i.board[column][row].id);
                    } else {
                        printf ("C");                
                    }
                } else if (i.board[column][row].tileType == ARC_TILE) {
                    printf ("A");
                }
            } else if ((column == 0) || (column == 24)) {
                printf("*");
            }
            column++;
        }
        column = 0;
        printf ("\n");
        row++;
    }*/
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
        printf ("Invalid path %s\n", inputPath);
        printf ("inputPath[%d] == %c\n", counter, inputPath[counter]);
        printf ("At (%d,%d)\n", tileLocation.x, tileLocation.y);
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
                printf ("Pathing from an invalid tile.\n");
                printf ("Invalid path %s\n", inputPath);
                printf ("inputPath[%d] == %c\n", counter, inputPath[counter]);
                printf ("At (%d,%d)\n", tileLocation.x, tileLocation.y);
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
            printf ("Invalid path %s\n", inputPath);
            printf ("inputPath[%d] == %c\n", counter, inputPath[counter]);
            printf ("At (%d,%d)\n", tileLocation.x, tileLocation.y);
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
                printf("column = %d\n", column);
                printf("row = %d\n", row);
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
    int counter3 = 0;
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

        printf("this is counter %d\n", counter);

        printf("this is pathlength %d\n", pathlength);

        strcpy (actionN.destination, checkLoc.destination);
        strcpy (actionNE.destination, checkLoc.destination);
        strcpy (actionSE.destination, checkLoc.destination);
        strcpy (actionS.destination, checkLoc.destination);
        strcpy (actionSW.destination, checkLoc.destination);
        strcpy (actionNW.destination, checkLoc.destination);

        printf("this is acctionSW %s\n", actionSW.destination);

        actionN.destination[pathlength] = 'R';
        actionN.destination[pathlength + 1] = 'L';

        printf("this is acctionN %s\n", actionN.destination);

        actionNE.destination[pathlength] = 'R';
        actionNE.destination[pathlength + 1] = 'R';

        printf("this is acctionNE %s\n", actionNE.destination);

        actionSE.destination[pathlength] = 'B';
        actionSE.destination[pathlength + 1] = 'R';

        printf("this is acctionSE %s\n", actionSE.destination);

        actionS.destination[pathlength] = 'B';
        actionS.destination[pathlength + 1] = 'L';

        printf("this is acctionS %s\n", actionS.destination);

        actionSW.destination[pathlength] = 'L';
        actionSW.destination[pathlength + 1] = 'L';
        actionSW.destination[pathlength + 2] = '\0';
        printf("this is acctionSW %s\n", actionSW.destination);

        actionNW.destination[pathlength] = 'L';
        actionNW.destination[pathlength + 1] = 'R';
        actionNW.destination[pathlength + 2] = '\0';
        printf("this is acctionNW %s\n", actionNW.destination);

        CoordN = getTile (i, actionN.destination, FALSE);
        printf("CoordN %d %d", CoordN.x, CoordN.y);
        CoordNE = getTile (i, actionNE.destination, FALSE);
        CoordSE = getTile (i, actionSE.destination, FALSE);
        CoordS = getTile (i, actionS.destination, FALSE);
        CoordSW = getTile (i, actionSW.destination, FALSE);
        CoordNW = getTile (i, actionNW.destination, FALSE);

        if ( (getCampus (g, actionN.destination)) == VACANT_VERTEX ) {
            campusValue[0][counter] = i.board[CoordN.x][CoordN.y].rollWeighting;
            printf("this is N rollweighting %d\n", i.board[CoordN.x][CoordN.y].rollWeighting);
        }
        if ( (getCampus (g, actionNE.destination)) == VACANT_VERTEX ) {
            campusValue[1][counter] = i.board[CoordNE.x][CoordNE.y].rollWeighting;
            printf("this is NE rollweighting %d\n", i.board[CoordNE.x][CoordNE.y].rollWeighting);
        }   
        if ( (getCampus (g, actionSE.destination)) == VACANT_VERTEX ) {
            campusValue[2][counter] = i.board[CoordSE.x][CoordSE.y].rollWeighting;
            printf("this is SE rollweighting %d\n", i.board[CoordSE.x][CoordSE.y].rollWeighting);
        }
        if ( (getCampus (g, actionS.destination)) == VACANT_VERTEX ) {
            campusValue[3][counter] = i.board[CoordS.x][CoordS.y].rollWeighting;
            printf("this is S rollweighting %d\n", i.board[CoordS.x][CoordS.y].rollWeighting);
        }
        if ( (getCampus (g, actionSW.destination)) == VACANT_VERTEX ) {
            campusValue[4][counter] = i.board[CoordSW.x][CoordSW.y].rollWeighting;
            printf("this is SW rollweighting %d\n", i.board[CoordSW.x][CoordSW.y].rollWeighting);
        }
        if ( (getCampus (g, actionNW.destination)) == VACANT_VERTEX ) {
            campusValue[5][counter] = i.board[CoordNW.x][CoordNW.y].rollWeighting;
            printf("this is NW rollweighting %d\n", i.board[CoordNW.x][CoordNW.y].rollWeighting);
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
    printf("LOCY IS %d\n", locY);
    printf("LOCX IS %d\n", locX);

    while (counter3 < 20) {
        if (locY == counter3) {
            strcpy (nextAction.destination,
                    i.board[campusCoordinates.buildDirection[locY].x]
                    [campusCoordinates.buildDirection[locY].y].vertexPath);
            printf("PATH %s\n", nextAction.destination);
            pathlength1 = strlen (nextAction.destination);
            if (locX == 0) {
                nextAction.destination[pathlength1] = 'R';
                nextAction.destination[pathlength1 + 1] = 'L';
            } else if (locX == 1) {
                nextAction.destination[pathlength1] = 'R';
                nextAction.destination[pathlength1 + 1] = 'R';
            } else if (locX == 2) {
                nextAction.destination[pathlength1] = 'B';
                nextAction.destination[pathlength1 + 1] = 'R';
            } else if (locX == 3) {
                nextAction.destination[pathlength1] = 'B';
                nextAction.destination[pathlength1 + 1] = 'L';
            } else if (locX == 4) {
                nextAction.destination[pathlength1] = 'L';
                nextAction.destination[pathlength1 + 1] = 'L';
            } else if (locX == 5) {
                nextAction.destination[pathlength1] = 'L';
                nextAction.destination[pathlength1 + 1] = 'R';
            }
        }
        counter3++;
    }

    action buildLocation;
    strcpy (buildLocation.destination, nextAction.destination);
    int stringlength1;
    stringlength1 = strlen (buildLocation.destination);
    buildLocation.destination[stringlength1 - 1] = "\0";
    if (getARC (g, buildLocation.destination) == VACANT_ARC) {
        strcpy (nextAction.destination, buildLocation.destination);
    }
    
    printf("THE NEXTACTION PATH IS %s\n", nextAction.destination);


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
 