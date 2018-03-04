
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "Game.h"
#include "mechanicalTurk.h"

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

typedef struct _buildLoc {
    coordinates buildDirection[50];
} buildLoc;

typedef struct _player {   
    // e.g. disciplines[STUDENT_THD], stores number of THDs for each player
    int discipline[NUMBER_OF_DISCIPLINES];
     
    // how many kpi poitns
    int kpi;
     
    // how many campuses owned
    int numCampuses;
     
    // number of GO8s
    int numGO8s;
     
    // number of ARCs
    int numARCs;
     
    // number of business-spinoffs rewards
    int numPublications;
    int numPatents;
     
    // exchangeRate[STUDENT_THD] is invalid i.e. 2^32 or something
    // exchangeRate[STUDENT_BPS] == 3 (at minimum)
    int exchangeRate[NUMBER_OF_DISCIPLINES];
     
} player;
 
// struct for the edges or vertices or null places
typedef struct _tile {
    // what students are produced
    // e.g. for a Campus that connects only to 1 region then
    // g->board[][].students[0] = STUDENT_BPS
    // g->board[][].students[1] = -1 (null or something else)
    // g->board[][].students[2] = -1 (null or something else)
    int students[MAX_REGION_CONNECTIONS];
     
    // what dice needed to produce students
    // e.g. what dice values needed to make the campus produce students
    int dice[MAX_REGION_CONNECTIONS];
     
    // what type of tile it is
    // #define NULL_TILE 0
    // #define CAMPUS_TILE 1
    // #define ARC_TILE 2
    int tileType;
     
    // what is on the tile
    // e.g. CAMPUS_A, CAMPUS_B
    // e.g. ARC_A, ARC_B
    // e.g. NULL_ID, VACANT_ARC, VACANT_VERTEX
    int id;
     
    // for the retraining centres
    int retrainingType;

    // weighting of the vertex tile
    int rollWeighting;

    // path to the vertex in string form
    path vertexPath;
     
} tile;
 
// hexagonal regions from 0 to 18
typedef struct _region {
    // stores the disciplines produced by each region
    int discipline;
    // stores the dice value needed to produce the discipline
    int dice;
} region;
 
// struct for all values stored by the game
typedef struct _game { 
    // what turn it is
    int currentTurn;
     
    // which player's turn
    // NO_ONE UNI_A UNI_B UNI_C
    int whoseTurn;
     
    // who has the most numPublications
    int mostPublications;
     
    // who has the most ARCs
    int mostARCs;
     
    // make sure there are only 8 GO8s in the game
    int numGO8s;
     
    // each player within the game has their own struct
    // stores the information for each player
    player uni[NUM_UNIS + 1]; // uni[0],[1],[2],[3] == NO_ONE,A,B,C
    // need +1 because NUM_UNIS is only 3, and we want to make sure
    // will create errors, seg faults, accessing memory you're not
    // supposed to if it is only of size 3 instead of 4 
     
    // for ease of programming getDiscipline and getDiceValue
    region hexagon[NUM_REGIONS];    
     
    // this stores the entire board
    tile board[HORIZONTAL_SIZE][VERTICAL_SIZE];
     
} game;
 
// to return more than one variable at a time
// needed for the pathing function
typedef struct _coordinates {
    // x-coordinates of a tile on the board
    int x;
    // y-coordinates of a tile on the board
    int y;
    // for use by isLegal 
    int validPath;
} coordinates;

static void setInitialRegions (Game g, int discipline[], int dice[]) {
    int counter = 0;
     
    // fill in each region of hexagon with their respective discipline/dice
    while (counter < NUM_REGIONS) {
        g->hexagon[counter].discipline = discipline[counter];
        g->hexagon[counter].dice = dice[counter];
        counter++;
    }
}

static void clearBoard (Game g) {
    int row = 0;
    int column = 0;
    int counter = 0;
     
    // checking through each row and column of the board
    // set all disciplines and dice values to a lack of production
    while (row < VERTICAL_SIZE) {
        while (column < HORIZONTAL_SIZE) {
            while (counter < MAX_REGION_CONNECTIONS) {
                g->board[column][row].students[counter] = NO_STUDENT;
                g->board[column][row].dice[counter] = NO_DICE;
                counter++;
            }
            counter = 0;
            // fill the board with NULL_TILEs that cannot be walked on
            g->board[column][row].tileType = NULL_TILE;
            g->board[column][row].id = NULL_ID;
            g->board[column][row].retrainingType = RETRAINING_NULL;
            column++;
        }
        column = 0;
        row++;
    }
}
 
static void setInitialCampusesAndARCs (Game g) {
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
                        g->board[column][row].tileType = CAMPUS_TILE;  
                        g->board[column][row].id = VACANT_VERTEX;  
                    }
                } else if ((column == 3) || (column == 5) ||
                           (column == 19) || (column == 21)) {
                    if (((row - 4) % 4 - 1 == 0) && (row < 18)) {
                        g->board[column][row].tileType = CAMPUS_TILE;
                        g->board[column][row].id = VACANT_VERTEX;
                    }
                } else if ((column == 7) || (column == 9) ||
                           (column == 15) || (column == 17)) {
                    if (((row - 2) % 4 - 1 == 0) && (row < 20)) {
                        g->board[column][row].tileType = CAMPUS_TILE;
                        g->board[column][row].id = VACANT_VERTEX;
                    }
                } else if ((column == 11) || (column == 13)) {
                    if ((row % 4 - 1 == 0) && (row < 22)) {
                        g->board[column][row].tileType = CAMPUS_TILE;
                        g->board[column][row].id = VACANT_VERTEX;
                    }
                }
            } else if ((column % 2) == 0) {
                // then set out ARC tiles which occur only on even columns
                if ((column == 2) || (column == 22)) {
                    if (((row - 5) % 2 - 1 == 0) && (row < 17)) {
                        g->board[column][row].tileType = ARC_TILE;
                        g->board[column][row].id = VACANT_ARC;
                    }
                } else if ((column == 4) || (column == 20)) {
                    if (((row - 4) % 4 - 1 == 0) && (row < 18)) {
                        g->board[column][row].tileType = ARC_TILE;
                        g->board[column][row].id = VACANT_ARC;
                    }
                } else if ((column == 6) || (column == 18)) {
                    if (((row - 3) % 2 - 1 == 0) && (row < 19)) {
                        g->board[column][row].tileType = ARC_TILE;
                        g->board[column][row].id = VACANT_ARC;
                    }
                } else if ((column == 8) || (column == 16)) {
                    if (((row - 2) % 4 - 1 == 0) && (row < 20)) {
                        g->board[column][row].tileType = ARC_TILE;
                        g->board[column][row].id = VACANT_ARC;
                    }
                } else if ((column == 10) || (column == 14)) {
                    if (((row - 1) % 2 - 1 == 0) && (row < 21)) {
                        g->board[column][row].tileType = ARC_TILE;
                        g->board[column][row].id = VACANT_ARC;
                    }
                } else if (column == 12) {
                    if ((row % 4 - 1 == 0) && (row < 22)) {
                        g->board[column][row].tileType = ARC_TILE;
                        g->board[column][row].id = VACANT_ARC;
                    }
                }
            }
            column++;
        }
        column = 0;
        row++;
    }
     
    // starting positions are always the same
    g->board[11][1].id = CAMPUS_A;
    g->board[13][21].id = CAMPUS_A;
    g->board[1][7].id = CAMPUS_B;
    g->board[23][15].id = CAMPUS_B;
    g->board[21][5].id = CAMPUS_C;
    g->board[3][17].id = CAMPUS_C;
}

static void setCampusRetrainingTypes (Game g) {   
    // starting campuses retraining types will never change
    g->board[7][3].retrainingType = RETRAINING_MTV;
    g->board[9][3].retrainingType = RETRAINING_MTV;
    
    g->board[15][3].retrainingType = RETRAINING_MMONEY;
    g->board[17][3].retrainingType = RETRAINING_MMONEY;
    
    g->board[21][5].retrainingType = RETRAINING_ANY;
    g->board[23][7].retrainingType = RETRAINING_ANY;
    
    g->board[21][13].retrainingType = RETRAINING_BQN;
    g->board[23][11].retrainingType = RETRAINING_BQN;
    
    g->board[17][19].retrainingType = RETRAINING_MJ;
    g->board[19][17].retrainingType = RETRAINING_MJ;
    
    g->board[11][21].retrainingType = RETRAINING_ANY;
    g->board[13][21].retrainingType = RETRAINING_ANY;
    
    g->board[5][17].retrainingType = RETRAINING_BPS;
    g->board[7][19].retrainingType = RETRAINING_BPS; 
    
    g->board[1][11].retrainingType = RETRAINING_ANY;
    g->board[3][13].retrainingType = RETRAINING_ANY;
    
    g->board[1][7].retrainingType = RETRAINING_ANY;
    g->board[3][5].retrainingType = RETRAINING_ANY;
}

static coordinates getTile (Game g, path inputPath, int isARC) {
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
            if ((g->board[tileLocation.x + 1][tileLocation.y + 1].tileType == ARC_TILE) ||
                (g->board[tileLocation.x + 1][tileLocation.y - 1].tileType == ARC_TILE) ||
                (g->board[tileLocation.x - 1][tileLocation.y].tileType == ARC_TILE)) {
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
            } else if ((g->board[tileLocation.x - 1][tileLocation.y - 1].tileType == ARC_TILE) ||
                       (g->board[tileLocation.x - 1][tileLocation.y + 1].tileType == ARC_TILE) ||
                       (g->board[tileLocation.x + 1][tileLocation.y].tileType == ARC_TILE)) {
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
                if (g->board[tileL.x][tileL.y].tileType == CAMPUS_TILE) {
                    tileLocation.x = tileL.x;
                    tileLocation.y = tileL.y;
                } else if (g->board[tileL.x][tileL.y].tileType == NULL_TILE) {
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
                if (g->board[tileR.x][tileR.y].tileType == CAMPUS_TILE) {
                    tileLocation.x = tileR.x;
                    tileLocation.y = tileR.y;
                } else if (g->board[tileR.x][tileR.y].tileType == NULL_TILE) {
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
                if (g->board[tileB.x][tileB.y].tileType == CAMPUS_TILE) {
                    tileLocation.x = tileB.x;
                    tileLocation.y = tileB.y;
                } else if (g->board[tileB.x][tileB.y].tileType == NULL_TILE) {
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

void storeCampus (Game g, gameInfo i) {

    // outer ring towards left side
    coordinates vertexTile;

    vertexTile = getTile (g, "RB", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RB");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (7, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RB");

    vertexTile = getTile (g, "R", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "R");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (3, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "R");

    vertexTile = getTile (g, "RR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (3, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RR");

    vertexTile = getTile (g, "RRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (0, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRL");

    vertexTile = getTile (g, "RRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (0, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLR");

    vertexTile = getTile (g, "RRLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (0, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRL");

    vertexTile = getTile (g, "RRLRLL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (1, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLL");

    vertexTile = getTile (g, "RRLRLLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (1, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLR");

    vertexTile = getTile (g, "RRLRLLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (2, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRL");

    vertexTile = getTile (g, "RRLRLLRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (2, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLR");

    vertexTile = getTile (g, "RRLRLLRLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (2, "RLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRL");

    vertexTile = getTile (g, "RRLRLLRLRLL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRLL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (2, "RLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLL");

    vertexTile = getTile (g, "RRLRLLRLRLLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRLLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (6, "RLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLR");

    vertexTile = getTile (g, "RRLRLLRLRLLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRLLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (11, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLRL");

    vertexTile = getTile (g, "RRLRLLRLRLLRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RRLRLLRLRLLRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (11, "RLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RRLRLLRLRLLRLR");

    //outer side right side

    vertexTile = getTile (g, "L", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "L");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (7, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "L");

    vertexTile = getTile (g, "LR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (12, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LR");

    vertexTile = getTile (g, "LRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (12, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRL");

    vertexTile = getTile (g, "LRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (16, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLR");

    vertexTile = getTile (g, "LRLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (16, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRL");

    vertexTile = getTile (g, "LRLRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (16, "RLLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLR");

    vertexTile = getTile (g, "LRLRLRR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (17, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRR");

    vertexTile = getTile (g, "LRLRLRRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (17, "RLLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRL");

    vertexTile = getTile (g, "LRLRLRRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLR");

    vertexTile = getTile (g, "LRLRLRRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLR");

    vertexTile = getTile (g, "LRLRLRRLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RLLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRL");

    vertexTile = getTile (g, "LRLRLRRLRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLR");

    vertexTile = getTile (g, "LRLRLRRLRLRR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLRR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRR");

    vertexTile = getTile (g, "LRLRLRRLRLRRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLRRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (15, "RLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRL");

    vertexTile = getTile (g, "LRLRLRRLRLRRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLRRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (15, "RLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRLR");

    vertexTile = getTile (g, "LRLRLRRLRLRRLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "LRLRLRRLRLRRLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (11, "RLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "LRLRLRRLRLRRLRL");

    //middle ring towards left

    vertexTile = getTile (g, "RL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (8, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RL");

    vertexTile = getTile (g, "RLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (8, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLR");

    vertexTile = getTile (g, "RLRR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (4, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRR");

    vertexTile = getTile (g, "RLRRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (4, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRRL");

    vertexTile = getTile (g, "RLRRLL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (5, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLL");

    vertexTile = getTile (g, "RLRRLLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (5, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLR");

    vertexTile = getTile (g, "RLRRLLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (6, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRL");

    vertexTile = getTile (g, "RLRRLLRLL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLLRLL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (10, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRLL");

    vertexTile = getTile (g, "RLRRLLRLLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRRLLRLLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (10, "RLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRRLLRLLR");

    // middle ring towards right side

    vertexTile = getTile (g, "RLL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (8, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLL");

    vertexTile = getTile (g, "RLLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (13, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLLR");

    vertexTile = getTile (g, "RLLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (13, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLLRL");

    vertexTile = getTile (g, "RLLRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (17, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLR");

    vertexTile = getTile (g, "RLLRLRR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (14, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRR");

    vertexTile = getTile (g, "RLLRLRRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRL");

    vertexTile = getTile (g, "RLLRLRRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (18, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLR");

    vertexTile = getTile (g, "RLLRLRRLRR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRRLRR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (15, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLRR");

    vertexTile = getTile (g, "RLLRLRRLRRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLLRLRRLRRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (11, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLLRLRRLRRL");

    //inner ring towards the left

    vertexTile = getTile (g, "RLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRL");

    vertexTile = getTile (g, "RLRLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRLR");

    vertexTile = getTile (g, "RLRLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRLRL");

    //inner ring towards right

    vertexTile = getTile (g, "RLRLL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "R", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRLL");

    vertexTile = getTile (g, "RLRLLR", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLLR");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RLLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRLLR");

    vertexTile = getTile (g, "RLRLLRL", FALSE);
    g->board[vertexTile.x][vertexTile.y].id = getCampus (g, "RLRLLRL");
    g->board[vertexTile.x][vertexTile.y].rollWeighting = getRollWeighting (9, "RLLLL", i);
    strcpy (g->board[vertexTile.x][vertexTile.y].vertexPath, "RLRLLRL");
}

buildLoc getCampusTiles (Game g, gameInfo i) {

    int row = 0;
    int column = 0;
    int currentPlayer == getWhoseTurn (g);
    int counter;
    buildLoc campusCoordinates;

    while (row < VERTICAL_SIZE) {
        while (column < HORIZONTAL_SIZE) {
            if (g->board[row][column].id == currentPlayer) {
                campusCoordinates.buildDirection[counter].x = column;
                campusCoordinates.buildDirection[counter].y = row;
                counter++;
            }
            column++;
        }
        row++;
    }
    return campusCoordinates;
    //takes in gameboard and returns locations of campuses owned in the struct form
    // buildDirection is a array of coordinates
}

action getBuildLocations (buildLoc campusCoordinates) {

    int counter = 0;
    int counter1 = 0;
    int counter2 = 0;
    int counter3 = 0;

    int campusValue[20][20] = {{0}};

    coordinates CoordN, CoordNE, CoordSE, CoordS, CoordSW, CoordNW;

    action actionN, actionNE, actionSE, actionS, actionSW, actionNW;
    action checkLoc;

    while (counter < 20) {

    strcpy (g->board[campusCoordinates.buildDirection[counter].x]
            [campusCoordinates.buildDirection[counter].y].vertexPath,
            checkLoc.destination);

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

    actionSE.destination[pathlength + 1] = 'B';
    actionSE.destination[pathlength + 2] = 'R';

    actionS.destination[pathlength + 1] = 'B';
    actionS.destination[pathlength + 2] = 'L';

    actionSW.destination[pathlength + 1] = 'L';
    actionSW.destination[pathlength + 2] = 'L';

    actionNW.destination[pathlength + 1] = 'L';
    actionNW.destination[pathlength + 2] = 'R';

    CoordN = getTile (g, actionN.destination, FALSE);
    CoordNE = getTile (g, actionNE.destination, FALSE);
    CoordSE = getTile (g, actionSE.destination, FALSE);
    CoordS = getTile (g, actionS.destination, FALSE);
    CoordSW = getTile (g, actionSW.destination, FALSE);
    CoordNW = getTile (g, actionNW.destination, FALSE);
    //run getTile on each of the locations to find the rollweighting on each tile.
    
    if ( (getCampus (g, actionN.destination)) == VACANT_VERTEX) ) {
        campusValue[0][counter] = g->board[CoordN.x][CoordN.y].rollWeighting;
    }
    if ( (getCampus (g, actionNE.destination)) == VACANT_VERTEX) ) {
    campusValue[1][counter] = g->board[CoordNE.x][CoordNE.y].rollWeighting;
    }   
    if ( (getCampus (g, actionSE.destination)) == VACANT_VERTEX) ) {
        campusValue[2][counter] = g->board[CoordSE.x][CoordSE.y].rollWeighting;
    }
    if ( (getCampus (g, actionS.destination)) == VACANT_VERTEX) ) {
        campusValue[3][counter] = g->board[CoordS.x][CoordS.y].rollWeighting;
    }
    if ( (getCampus (g, actionSW.destination)) == VACANT_VERTEX) ) {
    campusValue[4][counter] = g->board[CoordSW.x][CoordSW.y].rollWeighting;
    }
    if ( (getCampus (g, actionNW.destination)) == VACANT_VERTEX) ) {
    campusValue[5][counter] = g->board[CoordNW.x][CoordNW.y].rollWeighting;
    }
    counter++;
    }

    int maximum = campusValue[0][0];
    int locX = 0;
    int locY = 0;
    action nextAction;
    int pathlength1;

    while (counter1 < 20) {
        while (counter2 < 6) {
            if (campusValue[counter2][counter1] > maximum) {
                maximum = campusValue[counter2][counter1];
                locX = counter2;
                locY = counter1;
            }
            counter2++;
        }
        counter1++;
    }

    while (counter3 < 20) {
        if (locY == counter3) {
            strcpy (g->board[campusCoordinates.buildDirection[counter].x]
            [campusCoordinates.buildDirection[counter].y].vertexPath,
            nextAction.destination);
            pathlength1 = strlen (nextAction.destination);
            if (locX == 0) {
                nextAction.destination[pathlength1 + 1] = 'R';
                nextAction.destination[pathlength1 + 2] = 'L';
            } else if (locX == 1) {
                nextAction.destination[pathlength1 + 1] = 'R';
                nextAction.destination[pathlength1 + 2] = 'R';
            } else if (locX == 2) {
                nextAction.destination[pathlength1 + 1] = 'B';
                nextAction.destination[pathlength1 + 2] = 'R';
            } else if (locX == 3) {
                nextAction.destination[pathlength1 + 1] = 'B';
                nextAction.destination[pathlength1 + 2] = 'L';
            } else if (locX == 4) {
                nextAction.destination[pathlength1 + 1] = 'L';
                nextAction.destination[pathlength1 + 2] = 'L';
            } else if (locX == 5) {
                nextAction.destination[pathlength1 + 1] = 'L';
                nextAction.destination[pathlength1 + 2] = 'R';
            }
        }
    }
    return nextAction;
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
