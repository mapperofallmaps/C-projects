/*
    game.c
    Wei Hong Chen Z5114114 && Daniel Lau Z3376221
    Zerun Randal Li Z5057337 && Jiatong Lisa Yu Z5036563
    Thu-12Kora William Archinal
    Date: 21/04/2016
    Super Update: 9/05/2016
    About: Knowledge Island's Game.c
    Contains the functions to run a game of KI

*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "Game.h"

// helper functions, make them static (only accessed in one file)
// prototype at the top of .c
// runGame.c that uses ADT
// campuses always in the same starting locations
// same `ing centres in the same locations each time
// start at turn 0
// player A's turn first, starts at the top-est left and bottom-est right
// player B's turn 2nd, left-est top, right-est bottom
// player C's turn 3rd, right-est bottom, left-est top
// regionID reads down from 0 and then reads left to right

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


// struct for players
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
    
    // exchangeRate[STUDENT_THD] is invalid 
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

static void setInitialPlayers (Game g);
static void setInitialRegions (Game g, int discipline[], int dice[]);
static void clearBoard (Game g);
static void setInitialCampusesAndARCs (Game g);
static void setCampusStudents (Game g);
static void setCampusRetrainingTypes (Game g);
static void fillCampusTiles (Game g, int row, int column,
                      int discipline, int dice);                      
static void addStudents (Game g, int row, int column, int counter, 
                         int diceScore);
static coordinates getTile (Game g, path inputPath, int isARC);                         
static void setMostPublications (Game g);                         
static void setMostARCs (Game g);                         

/*
p->house.numBeds;
struct within a struct isn't a pointer to another struct
hence don't need another ->
*/

// typedef struct _game * Game;

// your team designs this type not us
// store in this struct all the things you might want to know about
// the game so you can write the interface functions in this header
// eg you might want to store the current turn number (so i've put
// it in for you as an example but take it out if you don't want it)

// in your Game.c include the details of the data
// you want to store in the _game struct eg

// typedef struct _game { 
//   int currentTurn;
//   ... more stuff in here
// } game;

// a path is a sequence of L=left R=right B=back steps
// starting from the initial campus of player 1 / A at the top 
// of the map facing inwards represented as a string of length 
// PATH_LIMIT or less (including the terminating 0).  
// a path can specify a vertex (the vertex at the end of the path)
// and a path can specify an ARC (the last ARC in the path)
// it is fine and legal for a path to not be the shortest path
// to the destination, it can even contain loops if you like.
// The length of a path (including the terminating 0) must never 
// exceed PATH_LIMIT.  Every edge in the path must lie on the 
// island, paths cannot include edges which are in the sea.

// always start from UNI_A at the top of the map
// going back makes you change direction`

// typedef char path[PATH_LIMIT];

// actions are what the player AI returns.  They say the one thing
// the AI wants to do next.  In the playGame logic you'll ask
// a player for their next action, then you'll check it is a legal
// action (using isLegalAction()), then you'll perform that action
// for them (using makeAction()), then you'll ask the same player
// for another action and repeat this over and over again until they 
// return PASS at which time you'll throw the dice (using 
// throwDice()) and advance the game to the next player.  And repeat.

/*
typedef struct _action {
    int actionCode;  // see #defines above
    path destination; // if the action operates on a vertex or ARC this
                     // specifies *which* vertex or path.  unused 
                     // otherwise
    int disciplineFrom;  // used for the retrain students action
    int disciplineTo;    // used for the retrain students action
} action;
*/

/* **** Functions which change the game aka SETTERS **** */
// make a new game, given the disciplines produced by each
// region, and the value on the dice discs in each region.
// note: each array must be NUM_REGIONS long
// eg if you are using my sample game struct above this function
// would need to set the field currentTurn to -1.  (because the turn
// number is -1 at the start of the game)
// the ordering of the regions is column by column left to right,
// going from the top of each column to the bottom before moving 
// to the next column to the right.
//
// so to create the default game as shown on the badly drawn map:
//
/*
#define DEFAULT_DISCIPLINES {STUDENT_BQN, STUDENT_MMONEY, STUDENT_MJ, \ 
                STUDENT_MMONEY, STUDENT_MJ, STUDENT_BPS, STUDENT_MTV, \
                STUDENT_MTV, STUDENT_BPS,STUDENT_MTV, STUDENT_BQN, \
                STUDENT_MJ, STUDENT_BQN, STUDENT_THD, STUDENT_MJ, \
                STUDENT_MMONEY, STUDENT_MTV, STUDENT_BQN, STUDENT_BPS}
#define DEFAULT_DICE {9,10,8,12,6,5,3,11,3,11,4,6,4,7,9,2,8,10,5}
*/
//
//   int disciplines[] = DEFAULT_DISCIPLINES;
//   int dice[] = DEFAULT_DICE;
//   Game g = newGame (disciplines, dice);
Game newGame (int discipline[], int dice[]) {
    Game g = malloc (sizeof(struct _game));
    
    // initial conditions
    g->currentTurn = TERRA_NULLIUS_TURN;
    g->whoseTurn = NO_ONE;
    g->mostPublications = NO_ONE;
    g->mostARCs = NO_ONE;
    g->numGO8s = 0;
    
    // initial player conditions
    setInitialPlayers (g);
    
    // technically not on the "board" tiles yet
    // just for ease in passing the asserts right now
    setInitialRegions (g, discipline, dice);
        
    // fill the board emptily right now
    clearBoard (g);
    
    // fill the board with correct ARC and Campus tiles
    setInitialCampusesAndARCs (g);
    
    // set campus tiles to have correct disciplines and dice values
    // fill the students and dice values of the tiles
    setCampusStudents (g);
    
    // correctly allocate retraining centres to campus tiles
    setCampusRetrainingTypes (g);
    
    return g;
}

static void setInitialPlayers (Game g) {
    int counter = 1;
    
    // most of these are just to past the initial asserts
    // will need a ~setPlayersResources function later on
    // e.g. setPlayerCampuses that changes .numCampuses
    // that will do the same thing but dynamically
    
    g->uni[NO_ONE].discipline[STUDENT_THD] = 0;
    g->uni[NO_ONE].discipline[STUDENT_BPS] = 0;
    g->uni[NO_ONE].discipline[STUDENT_BQN] = 0;
    g->uni[NO_ONE].discipline[STUDENT_MJ] = 0;
    g->uni[NO_ONE].discipline[STUDENT_MTV] = 0;
    g->uni[NO_ONE].discipline[STUDENT_MMONEY] = 0;
    g->uni[NO_ONE].kpi = 0;
    g->uni[NO_ONE].numCampuses = 0;
    g->uni[NO_ONE].numGO8s = 0;
    g->uni[NO_ONE].numARCs = 0;
    g->uni[NO_ONE].numPublications = 0;
    g->uni[NO_ONE].numPatents = 0;
    
    while (counter <= UNI_C) {
        g->uni[counter].discipline[STUDENT_THD] = 0;
        g->uni[counter].discipline[STUDENT_BPS] = 3;
        g->uni[counter].discipline[STUDENT_BQN] = 3;
        g->uni[counter].discipline[STUDENT_MJ] = 1;
        g->uni[counter].discipline[STUDENT_MTV] = 1;
        g->uni[counter].discipline[STUDENT_MMONEY] = 1; 
        g->uni[counter].kpi = 20;
        g->uni[counter].numCampuses = 2;
        g->uni[counter].numGO8s = 0;
        g->uni[counter].numARCs = 0;
        g->uni[counter].numPublications = 0;
        g->uni[counter].numPatents = 0;
        g->uni[counter].exchangeRate[STUDENT_THD] = 0;
        g->uni[counter].exchangeRate[STUDENT_BPS] = 3;
        g->uni[counter].exchangeRate[STUDENT_BQN] = 3;
        g->uni[counter].exchangeRate[STUDENT_MJ] = 3;
        g->uni[counter].exchangeRate[STUDENT_MTV] = 3;
        g->uni[counter].exchangeRate[STUDENT_MMONEY] = 3;
        counter++; 
    }
}

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
    
    /*
    // remove this later on, just for visual purposes
    printf ("Printing the game board\n");
    row = 0;
    column = 0;
    while (row < VERTICAL_SIZE) {
        while (column < HORIZONTAL_SIZE) {
            if ((column != 0) && (column != 24)) {
                if (g->board[column][row].tileType == NULL_TILE) {
                    printf ("*");
                } else if (g->board[column][row].tileType == CAMPUS_TILE) {
                    if (g->board[column][row].id != VACANT_VERTEX) {
                        printf ("%d", g->board[column][row].id);
                    } else {
                        printf ("C");                
                    }
                } else if (g->board[column][row].tileType == ARC_TILE) {
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
    }
    */
}

static void setCampusStudents (Game g) {
    int row = X_REGION_0_1_2;
    int column = Y_REGION_0_8_16;
    int counter = 0;
    
    // move down the map taking each region's eastern-most tile to be passed
    // into fillCampusTiles
    while (counter < NUM_REGIONS) {
        fillCampusTiles (g, row, column,
                         g->hexagon[counter].discipline, g->hexagon[counter].dice);
        row = row + 4;
        if ((row > Y_REGION_2_10_18) && (column == X_REGION_0_1_2)) {
            column = X_REGION_3_4_5_6;
            row = Y_REGION_3_12;
        } else if ((row > Y_REGION_6_15) && (column == X_REGION_3_4_5_6)) {
            column = X_REGION_7_8_9_10_11;
            row = Y_REGION_7;
        } else if ((row > Y_REGION_11) && (column == X_REGION_7_8_9_10_11)) {
            column = X_REGION_12_13_14_15;
            row = Y_REGION_3_12;
        } else if ((row > Y_REGION_6_15) && (column == X_REGION_12_13_14_15)) {
            column = X_REGION_16_17_18;
            row = Y_REGION_0_8_16;
        }
        counter++;
    }
        
}

static void fillCampusTiles (Game g, int row, int column,
                      int discipline, int dice) {
    int counter = 0;
    int vertexPosition = EAST;
    
    // from the eastern-most tile this function will in a clock-wise
    // manner, loop around all 6 campus tiles and fill them with data
    // on each region's disciplines and students
    while (vertexPosition < END_OF_VERTEX_POSITIONS) {
        while (counter < MAX_REGION_CONNECTIONS) {
            if (g->board[column][row].students[counter] == NO_STUDENT) {
                g->board[column][row].students[counter] = discipline;
                g->board[column][row].dice[counter] = dice;
                counter = MAX_REGION_CONNECTIONS;
            }
            counter++;
        }
        counter = 0;
        // cycle around the hexagon (region) assigning the positions of vertices
        if ((vertexPosition == EAST) || (vertexPosition == SOUTH_EAST) ||
            (vertexPosition == SOUTH_WEST)) {
            column-= VERTEX_DISTANCE;
        }
        if ((vertexPosition == EAST) || (vertexPosition == NORTH_EAST)) {
            row+= VERTEX_DISTANCE;
        }
        if ((vertexPosition == SOUTH_WEST) || (vertexPosition == WEST)) {
            row-= VERTEX_DISTANCE;
        }
        if ((vertexPosition == WEST) || (vertexPosition == NORTH_WEST) ||
                   (vertexPosition == NORTH_EAST)) {
            column+= VERTEX_DISTANCE;
        }
        
        vertexPosition++;
    }
    
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

// free all the memory malloced for the game
void disposeGame (Game g) {
    free(g);
}

// make the specified action for the current player and update the 
// game state accordingly.  
// The function may assume that the action requested is legal.
// START_SPINOFF is not a legal action here
void makeAction (Game g, action a) {
    int currentPlayer = getWhoseTurn (g);
    int previousMostARCs = NO_ONE;
    int previousMostPublications = NO_ONE;
    
    coordinates buildLoc;
    buildLoc.x = X_PATH_START;
    buildLoc.y = Y_PATH_START;
    buildLoc.validPath = TRUE;
    
    // this prevents illegal actions being made on turn -1
    if (getWhoseTurn (g) == NO_ONE) {
        printf ("Turn number: %d, a.k.a Terra Nullius\n", getTurnNumber (g));
        printf ("NO_ONE is not allowed to make any moves\n");
        printf ("Please throw the dice first to reach turn 0\n");
    }
    
    if (a.actionCode == BUILD_CAMPUS) {
        buildLoc = getTile (g, a.destination, FALSE);
        // change id of the tile to the correct value e.g. CAMPUS_A
        g->board[buildLoc.x][buildLoc.y].id = currentPlayer;
        // pay student cost
        g->uni[currentPlayer].discipline[STUDENT_BPS]--;
        g->uni[currentPlayer].discipline[STUDENT_BQN]--;
        g->uni[currentPlayer].discipline[STUDENT_MJ]--;
        g->uni[currentPlayer].discipline[STUDENT_MTV]--; 
        // increase KPI & numCampuses
        g->uni[currentPlayer].kpi+= 10;
        g->uni[currentPlayer].numCampuses++;
        // increment a player's exchangeRate for students if necessary
        if (g->board[buildLoc.x][buildLoc.y].retrainingType == RETRAINING_MTV) {
            g->uni[currentPlayer].exchangeRate[STUDENT_MTV] = 2;
        } else if (g->board[buildLoc.x][buildLoc.y].retrainingType == RETRAINING_MMONEY) {
            g->uni[currentPlayer].exchangeRate[STUDENT_MMONEY] = 2;
        } else if (g->board[buildLoc.x][buildLoc.y].retrainingType == RETRAINING_BQN) {
            g->uni[currentPlayer].exchangeRate[STUDENT_BQN] = 2;
        } else if (g->board[buildLoc.x][buildLoc.y].retrainingType == RETRAINING_MJ) {
            g->uni[currentPlayer].exchangeRate[STUDENT_MJ] = 2;
        } else if (g->board[buildLoc.x][buildLoc.y].retrainingType == RETRAINING_BPS) {
            g->uni[currentPlayer].exchangeRate[STUDENT_BPS] = 2;
        }
    } else if (a.actionCode == BUILD_GO8) {
        buildLoc = getTile (g, a.destination, FALSE);
        g->board[buildLoc.x][buildLoc.y].id = (currentPlayer + 3);
        g->uni[currentPlayer].discipline[STUDENT_MJ]-= 2;  
        g->uni[currentPlayer].discipline[STUDENT_MMONEY]-= 3;
        // net KPI increase of 10
        g->uni[currentPlayer].kpi+= 10;
        // need to decrease numCampuses but increment GO8
        g->uni[currentPlayer].numCampuses--;  
        g->uni[currentPlayer].numGO8s++;  
        g->numGO8s++;
    } else if (a.actionCode == OBTAIN_ARC) {
        buildLoc = getTile (g, a.destination, TRUE);
        g->board[buildLoc.x][buildLoc.y].id = currentPlayer;
        g->uni[currentPlayer].discipline[STUDENT_BPS]--; 
        g->uni[currentPlayer].discipline[STUDENT_BQN]--; 
        g->uni[currentPlayer].kpi+= 2;
        g->uni[currentPlayer].numARCs++;
        // figure out who has mostARCs
        previousMostARCs = g->mostARCs;
        setMostARCs (g);        
        // see if there needs to be an exchange of KPI
        if (previousMostARCs != g->mostARCs) {
            g->uni[previousMostARCs].kpi-= 10;
            g->uni[g->mostARCs].kpi+= 10;
        }
        
        if (previousMostARCs == NO_ONE) {
            g->uni[NO_ONE].kpi = 0;
        }
    } else if ((a.actionCode == OBTAIN_PUBLICATION) ||
               (a.actionCode == OBTAIN_IP_PATENT)) {
        // charge resource requirements
        g->uni[currentPlayer].discipline[STUDENT_MJ]--; 
        g->uni[currentPlayer].discipline[STUDENT_MTV]--; 
        g->uni[currentPlayer].discipline[STUDENT_MMONEY]--; 
        if (a.actionCode == OBTAIN_PUBLICATION) {
            previousMostPublications = g->mostPublications;
            g->uni[currentPlayer].numPublications++;
            setMostPublications (g);
            if (previousMostPublications != g->mostPublications) {
                g->uni[previousMostPublications].kpi-= 10;
                g->uni[g->mostPublications].kpi+= 10;
            }
            if (previousMostPublications == NO_ONE) {
                g->uni[NO_ONE].kpi = 0;
            }
        } else if (a.actionCode == OBTAIN_IP_PATENT) {
            g->uni[currentPlayer].numPatents++;
            g->uni[currentPlayer].kpi+= 10;
        }
    } else if (a.actionCode == RETRAIN_STUDENTS) {
        g->uni[currentPlayer].discipline[a.disciplineFrom] =
        g->uni[currentPlayer].discipline[a.disciplineFrom] -
        g->uni[currentPlayer].exchangeRate[a.disciplineFrom];
        g->uni[currentPlayer].discipline[a.disciplineTo]++;
    }
    
}

// advance the game to the next turn, 
// assuming that the dice has just been rolled and produced diceScore
// the game starts in turn -1 (we call this state "Terra Nullius") and 
// moves to turn 0 as soon as the first dice is thrown. 
void throwDice (Game g, int diceScore) {
    
    int row = 0;
    int column = 0;
    int counter = 0;
    int playerCounter = 1;
    // always increment the turn when throwin the dice
    g->currentTurn++;
    
    while (row < VERTICAL_SIZE) {
        while (column < HORIZONTAL_SIZE) {
            while (counter < MAX_REGION_CONNECTIONS) {
                // increment students for regions with diceScore
                addStudents (g, row, column, counter, diceScore);
                counter++;
            }
            counter = 0;
            column++;
        }
        column = 0;
        row++;
    }
    
        playerCounter = 0;
    
    // convert MTV and MMONEY into THDs for all players
    if (diceScore == 7) {
        while (playerCounter <= NUM_UNIS) {
            g->uni[playerCounter].discipline[STUDENT_THD]+= g->uni[playerCounter].discipline[STUDENT_MTV];
            g->uni[playerCounter].discipline[STUDENT_THD]+= g->uni[playerCounter].discipline[STUDENT_MMONEY];
            g->uni[playerCounter].discipline[STUDENT_MTV] = 0;
            g->uni[playerCounter].discipline[STUDENT_MMONEY] = 0;
            playerCounter++;
        }
    }
    
    
}

static void addStudents (Game g, int row, int column, int counter, 
                         int diceScore) {
    int player = NO_ONE;
    int isGO8 = FALSE;
    int increasedDiscipline = NO_STUDENT;
    int secondCounter = 0;
    // check if the specified row/column contains a campus
    if (g->board[column][row].dice[counter] == diceScore) {
        if (g->board[column][row].id == CAMPUS_A) {
            player = UNI_A;
        } else if (g->board[column][row].id == CAMPUS_B) {
            player = UNI_B;
        } else if (g->board[column][row].id == CAMPUS_C) {
            player = UNI_C;
        } else if (g->board[column][row].id == GO8_A) {
            player = UNI_A;
            isGO8 = TRUE;
        } else if (g->board[column][row].id == GO8_B) {
            player = UNI_B;
            isGO8 = TRUE;
        } else if (g->board[column][row].id == GO8_C) {
            player = UNI_C;
            isGO8 = TRUE;
        }
        // if there exists a valid player to increment resources
        if (player != NO_ONE) {
            while (secondCounter < MAX_REGION_CONNECTIONS) {
                // loop through all resources provided by that campus
                // i.e. which regions are connected to the campus
                increasedDiscipline = g->board[column][row].students[secondCounter];
                // make sure that only valid student types are considered
                if ((increasedDiscipline != NO_STUDENT) && 
                    (g->board[column][row].dice[secondCounter] == diceScore)) {
                    if (isGO8 == FALSE) {
                        g->uni[player].discipline[increasedDiscipline]++;                       
                    } else if (isGO8 == TRUE) {
                        g->uni[player].discipline[increasedDiscipline]+= 2;                        
                    }
                }                              
                secondCounter++;
            }
        }
    }  
}

/* **** Functions which GET data about the game aka GETTERS **** */

// what type of students are produced by the specified region?
// regionID is the index of the region in the newGame arrays (above) 
// see discipline codes above
int getDiscipline (Game g, int regionID) {
    return g->hexagon[regionID].discipline;
}

// what dice value produces students in the specified region?
// 2..12
int getDiceValue (Game g, int regionID) {
    return g->hexagon[regionID].dice;
}

// which university currently has the prestige award for the most ARCs?
// this is NO_ONE until the first arc is purchased after the game 
// has started.  
int getMostARCs (Game g) {
    return g->mostARCs;
}

static void setMostARCs (Game g) {    
    if ((g->uni[UNI_A].numARCs > g->uni[UNI_B].numARCs) &&
        (g->uni[UNI_A].numARCs > g->uni[UNI_C].numARCs)) {
        g->mostARCs = UNI_A;
    } else if ((g->uni[UNI_B].numARCs > g->uni[UNI_A].numARCs) &&
               (g->uni[UNI_B].numARCs > g->uni[UNI_C].numARCs)) {
        g->mostARCs = UNI_B;
    } else if ((g->uni[UNI_C].numARCs > g->uni[UNI_A].numARCs) &&
               (g->uni[UNI_C].numARCs > g->uni[UNI_B].numARCs)) {
        g->mostARCs = UNI_C;
    }
}

// which university currently has the prestige award for the most pubs?
// this is NO_ONE until the first publication is made.
int getMostPublications (Game g) {  
    return g->mostPublications;
}

static void setMostPublications (Game g) {
    if ((g->uni[UNI_A].numPublications > g->uni[UNI_B].numPublications) &&
        (g->uni[UNI_A].numPublications > g->uni[UNI_C].numPublications)) {
        g->mostPublications = UNI_A;
    } else if ((g->uni[UNI_B].numPublications > g->uni[UNI_A].numPublications) &&
               (g->uni[UNI_B].numPublications > g->uni[UNI_C].numPublications)) {
        g->mostPublications = UNI_B;
    } else if ((g->uni[UNI_C].numPublications > g->uni[UNI_A].numPublications) &&
               (g->uni[UNI_C].numPublications > g->uni[UNI_B].numPublications)) {
        g->mostPublications = UNI_C;
    }    
}

// return the current turn number of the game -1,0,1, ..
int getTurnNumber (Game g) {
    return g->currentTurn;
}

// return the player ID of the player whose turn it is 
// the result of this function is NO_ONE during Terra Nullis
int getWhoseTurn (Game g) {
    int whoseTurn = NO_ONE;
    
    // ensures that it is NO_ONE's turn on -1
    if (getTurnNumber (g) >= 0) {
        whoseTurn = ((getTurnNumber (g)) % NUM_UNIS + 1);
    }

    return whoseTurn;
}

// return the contents of the given vertex (ie campus code or 
// VACANT_VERTEX)
int getCampus(Game g, path pathToVertex) {
    int campusValue = VACANT_VERTEX;
    int isARC = FALSE;
    coordinates finalLocation;
    
    finalLocation = getTile (g, pathToVertex, isARC);
    
    campusValue = g->board[finalLocation.x][finalLocation.y].id;
    
    return campusValue;
}

// the contents of the given edge (ie ARC code or vacent ARC)
int getARC(Game g, path pathToEdge) {
    int arcValue = VACANT_VERTEX;
    int isARC = TRUE;
    coordinates finalLocation;
    
    finalLocation = getTile (g, pathToEdge, isARC);    
    
    arcValue = g->board[finalLocation.x][finalLocation.y].id;
    
    return arcValue;
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

// returns TRUE if it is legal for the current
// player to make the specified action, FALSE otherwise.
//
// "legal" means everything is legal: 
//   * that the action code is a valid action code which is legal to 
//     be made at this time
//   * that any path is well formed and legal ie consisting only of 
//     the legal direction characters and of a legal length, 
//     and which does not leave the island into the sea at any stage.
//   * that disciplines mentioned in any retraining actions are valid 
//     discipline numbers, and that the university has sufficient
//     students of the correct type to perform the retraining
//
// eg when placing a campus consider such things as: 
//   * is the path a well formed legal path 
//   * does it lead to a vacent vertex?
//   * under the rules of the game are they allowed to place a 
//     campus at that vertex?  (eg is it adjacent to one of their ARCs?)
//   * does the player have the 4 specific students required to pay for 
//     that campus?
// It is not legal to make any action during Terra Nullis ie 
// before the game has started.
// It is not legal for a player to make the moves OBTAIN_PUBLICATION 
// or OBTAIN_IP_PATENT (they can make the move START_SPINOFF)
// you can assume that any pths passed in are NULL terminated strings.
int isLegalAction (Game g, action a) {
    int isLegal = FALSE;
    int currentPlayer = getWhoseTurn (g);
    int pathLength = 0;
    
    // stores whether surrounding ARCs or Campuses on L/R/B paths
    // are valid locations
    int validLARC = FALSE;
    int validRARC = FALSE;
    int validBARC = FALSE;
    int validBLARC = FALSE;
    int validBRARC = FALSE;
    int validLCampus = FALSE;
    int validRCampus = FALSE;
    int validBCampus = FALSE;
    int validFCampus = FALSE;
    
    // to store whether the ARC, Campus or GO8
    // belongs to the correct player
    int matchingARC = VACANT_ARC;
    int matchingCampus = VACANT_VERTEX;
    int matchingGO8 = VACANT_VERTEX;
    
    coordinates buildLoc;
    buildLoc.x = X_PATH_START;
    buildLoc.y = Y_PATH_START;
    buildLoc.validPath = TRUE;

    coordinates surroundingLoc;
    surroundingLoc.x = X_PATH_START;
    surroundingLoc.y = Y_PATH_START;
    surroundingLoc.validPath = TRUE; 
    
    // need to add;
    // check if there's actually a string
    // so we don't potentially bufferoverflow
    
   char searchPath[PATH_LIMIT + 3]; 
   
    if ((a.actionCode == BUILD_CAMPUS) || (a.actionCode == BUILD_GO8) || 
        (a.actionCode == OBTAIN_ARC)) {
        
        // fill with EOF
        memset (searchPath, '\0', PATH_LIMIT + 3);
        while (a.destination[pathLength] != '\0') {
            searchPath[pathLength] = a.destination[pathLength];
            pathLength++;
        }
        
        // dummy starting values for the non-copied over chars
        searchPath[pathLength] = 'R';
        searchPath[pathLength + 1] = '\0';
        searchPath[pathLength + 2] = '\0';
        searchPath[pathLength + 3] = '\0';
    }
    
    if (currentPlayer == UNI_A) {
        matchingARC = ARC_A;
        matchingCampus = CAMPUS_A;
        matchingGO8 = GO8_A;
    } else if (currentPlayer == UNI_B) {
        matchingARC = ARC_B;
        matchingCampus = CAMPUS_B;
        matchingGO8 = GO8_B;
    } else if (currentPlayer == UNI_C) {
        matchingARC = ARC_C;
        matchingCampus = CAMPUS_C;
        matchingGO8 = GO8_C;
    }
    
    if (g->currentTurn == -1) {
        isLegal = FALSE;
    } else if (a.actionCode == PASS) {
        isLegal = TRUE;
    } else if (a.actionCode == BUILD_CAMPUS) {
        buildLoc = getTile (g, a.destination, FALSE);
        if (buildLoc.validPath == TRUE) {
            if ((g->uni[currentPlayer].discipline[STUDENT_BPS] >= 1) && 
                (g->uni[currentPlayer].discipline[STUDENT_BQN] >= 1) &&
                (g->uni[currentPlayer].discipline[STUDENT_MJ] >= 1) &&
                (g->uni[currentPlayer].discipline[STUDENT_MTV] >= 1) &&
                (getCampus (g, a.destination) == VACANT_VERTEX)) {
                
                // look for nearby ARCs and see if they are matchingARCs          
                searchPath[pathLength] = 'L';            
                surroundingLoc = getTile (g, searchPath, TRUE);
                if (surroundingLoc.validPath == TRUE) {
                    if (getARC(g, searchPath) == matchingARC) {
                        validLARC = TRUE;
                    }
                }
                            
                searchPath[pathLength] = 'R';            
                surroundingLoc = getTile (g, searchPath, TRUE);
                if (surroundingLoc.validPath == TRUE) {
                    if (getARC(g, searchPath) == matchingARC) {
                        validRARC = TRUE;
                    }
                }
                
                searchPath[pathLength] = 'B';            
                surroundingLoc = getTile (g, searchPath, TRUE);
                if (surroundingLoc.validPath == TRUE) {
                    if (getARC(g, searchPath) == matchingARC) {
                        validBARC = TRUE;
                    }
                }
                
                // search campuses that are L, R and B
                // if any exist, then build campus is not allowed
                
                searchPath[pathLength] = 'L';
                surroundingLoc = getTile (g, searchPath, FALSE);
                if (surroundingLoc.validPath == TRUE) {
                    validLCampus = getCampus (g, searchPath);
                    if (validLCampus == VACANT_VERTEX) {
                        validLCampus = TRUE;
                    } else {
                        validLCampus = FALSE;
                    }            
                } else {
                    validLCampus = TRUE;
                }

                searchPath[pathLength] = 'R';
                surroundingLoc = getTile (g, searchPath, FALSE);
                if (surroundingLoc.validPath == TRUE) {
                    validRCampus = getCampus (g, searchPath);
                    if (validRCampus == VACANT_VERTEX) {
                        validRCampus = TRUE;
                    } else {
                        validRCampus = FALSE;
                    }            
                } else {
                    validRCampus = TRUE;
                }
                
                
                searchPath[pathLength] = 'B';     
                surroundingLoc = getTile (g, searchPath, FALSE);
                if (surroundingLoc.validPath == TRUE) {
                    validBCampus = getCampus (g, searchPath);
                    if (validBCampus == VACANT_VERTEX) {
                        validBCampus = TRUE;
                    } else {
                        validBCampus = FALSE;
                    }            
                } else {
                    validBCampus = TRUE;
                }
                
                // if any of the arcs are true and there are no nearby campuses
                // then it is legal to build a campus in such locations
                if (((validLARC == TRUE) || (validRARC == TRUE) || (validBARC == TRUE)) &&
                    (validLCampus == TRUE) && (validRCampus) && (validBCampus)) {
                    isLegal = TRUE;
                }
            }
        } else {
            isLegal = FALSE;
        }
    } else if (a.actionCode == BUILD_GO8) {
        buildLoc = getTile (g, a.destination, FALSE);
        if (buildLoc.validPath == TRUE) {
            if ((g->uni[currentPlayer].discipline[STUDENT_MJ] >= 2) &&
                (g->uni[currentPlayer].discipline[STUDENT_MMONEY] >= 3) &&
                (getCampus (g, a.destination) == matchingCampus) &&
                (g->numGO8s < 8)) {
                isLegal = TRUE;
            }
        } else {
            isLegal = FALSE;
        }
    } else if (a.actionCode == OBTAIN_ARC) {
        buildLoc = getTile (g, a.destination, TRUE);
        if (buildLoc.validPath == TRUE) {
            if ((g->uni[currentPlayer].discipline[STUDENT_BPS] >= 1) &&
                (g->uni[currentPlayer].discipline[STUDENT_BQN] >= 1) && 
                (buildLoc.validPath == TRUE) && 
                (getARC (g, a.destination) == VACANT_ARC)) {
                // might make this a function since i just copied and pasted
                searchPath[pathLength] = 'L';                     
                surroundingLoc = getTile (g, searchPath, TRUE);
                if (surroundingLoc.validPath == TRUE) {
                    if (getARC (g, searchPath) == matchingARC) {
                        validLARC = TRUE;
                    }
                }
                
                searchPath[pathLength] = 'R';
                surroundingLoc = getTile (g, searchPath, TRUE);
                if (surroundingLoc.validPath == TRUE) {
                    if (getARC (g, searchPath) == matchingARC) {
                        validRARC = TRUE;
                    }
                }
                
                searchPath[pathLength] = 'B';            
                searchPath[pathLength + 1] = 'L';           
                surroundingLoc = getTile (g, searchPath, TRUE);
                if (surroundingLoc.validPath == TRUE) {
                    if (getARC(g, searchPath) == matchingARC) {
                        validBLARC = TRUE;
                    }
                }
                
                searchPath[pathLength + 1] = 'R';
                surroundingLoc = getTile (g, searchPath, TRUE);
                if (surroundingLoc.validPath == TRUE) {
                    if (getARC(g, searchPath) == matchingARC) {
                        validBRARC = TRUE;
                    }
                }
                
                buildLoc = getTile (g, a.destination, FALSE);
                // search campus that is infront                 
                surroundingLoc = getTile (g, a.destination, FALSE);
                if (surroundingLoc.validPath == TRUE) {
                    validFCampus = getCampus (g, a.destination);
                    if ((validFCampus == matchingCampus) || (validFCampus == matchingGO8)) {
                        validFCampus = TRUE;
                    } else {
                        validFCampus = FALSE;
                    }
                }
                
                // search campus that is behind
                searchPath[pathLength] = 'B';    
                searchPath[pathLength + 1] = '\0';    
                surroundingLoc = getTile (g, searchPath, FALSE);
                if (surroundingLoc.validPath == TRUE) {
                    validBCampus = getCampus (g, searchPath);
                    if ((validBCampus == matchingCampus) || (validBCampus == matchingGO8)) {
                        validBCampus = TRUE;
                    } else {
                        validBCampus = FALSE;
                    }            
                }
                
                // if any arcs
                if ((validLARC == TRUE) || (validRARC == TRUE) || (validBARC == TRUE) ||
                    (validBLARC == TRUE) || (validBRARC == TRUE) || 
                    (validFCampus == TRUE) || (validBCampus == TRUE)) {
                    isLegal = TRUE;
                }
            }
        } else {
            isLegal = FALSE;
        }
    } else if (a.actionCode == START_SPINOFF) {
        if ((g->uni[currentPlayer].discipline[STUDENT_MJ] >= 1) &&
            (g->uni[currentPlayer].discipline[STUDENT_MTV] >= 1) &&
            (g->uni[currentPlayer].discipline[STUDENT_MMONEY] >= 1)) {
            isLegal = TRUE;
        }
    } else if (a.actionCode == RETRAIN_STUDENTS) {
        if ((a.disciplineFrom > STUDENT_THD) && (a.disciplineFrom <= STUDENT_MMONEY) &&
             (a.disciplineTo >= STUDENT_THD) && (a.disciplineTo <= STUDENT_MMONEY)) {
            if (g->uni[currentPlayer].discipline[a.disciplineFrom] >=
                g->uni[currentPlayer].exchangeRate[a.disciplineFrom]) {
                isLegal = TRUE;
            }
        }
    }
    
    if ((a.actionCode == OBTAIN_IP_PATENT) || (a.actionCode == OBTAIN_PUBLICATION)) {
        isLegal = FALSE;
    }
    
    return isLegal;
}

// --- get data about a specified player ---

// return the number of KPI points the specified player currently has
int getKPIpoints (Game g, int player) {    
    return g->uni[player].kpi;
}

// return the number of ARC grants the specified player currently has
int getARCs (Game g, int player) {    
    return g->uni[player].numARCs;
}

// return the number of GO8 campuses the specified player currently has
int getGO8s (Game g, int player) {
    return g->uni[player].numGO8s;
}

// return the number of normal Campuses the specified player currently has
int getCampuses (Game g, int player) {    
    return g->uni[player].numCampuses;
}

// return the number of IP Patents the specified player currently has
int getIPs (Game g, int player) {
    return g->uni[player].numPatents;
}

// return the number of Publications the specified player currently has
int getPublications (Game g, int player) {
    return g->uni[player].numPublications;
}

// return the number of students of the specified discipline type 
// the specified player currently has
int getStudents (Game g, int player, int discipline) {    
    return g->uni[player].discipline[discipline];
}

// return how many students of discipline type disciplineFrom
// the specified player would need to retrain in order to get one 
// student of discipline type disciplineTo.  This will depend 
// on what retraining centers, if any, they have a campus at.
int getExchangeRate (Game g, int player, 
                     int disciplineFrom, int disciplineTo) {  
    assert (disciplineFrom != STUDENT_THD);
    return g->uni[player].exchangeRate[disciplineFrom];
}

