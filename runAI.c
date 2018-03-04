/*
    runGame.c
    Wei Hong Chen Z5114114 && Daniel Lau Z3376221
    Zerun Randal Li Z5057337 && Jiatong Lisa Yu Z5036563
    Thu-12Kora William Archinal
    Date: 14/05/2016
    About: Knowledge Island's runGame.c
    Steps through a game of KI
    Thinking about making all the turns entirely automated for faster testing.
    Can switch in different AIs instead of mrPass
    gcc -Wall -std=gnu99 -g -O -fno-omit-frame-pointer -lm -o runAI runAI.c mechanicalTurk.c Game.c
    This sample uses AI for uni b and c and human for player a
    
    gcc -Wall -std=gnu99 -g -O -lm -o runAI runAI.c mechanicalTurk.c Game.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Game.h"
#include "mechanicalTurk.h"
 
#include <time.h>
#include <assert.h>
 
#define WINNING_KPI 150
#define FALSE 0
#define TRUE 1
#define INFINITE_LOOP 9000
#define MAX_DICE (12 + 1)
#define MAX_STUDENTS (STUDENT_MMONEY + 1)

#define DEFAULT_DISCIPLINES {STUDENT_BQN, STUDENT_MMONEY, STUDENT_MJ, \
                STUDENT_MMONEY, STUDENT_MJ, STUDENT_BPS, STUDENT_MTV, \
                STUDENT_MTV, STUDENT_BPS,STUDENT_MTV, STUDENT_BQN, \
                STUDENT_MJ, STUDENT_BQN, STUDENT_THD, STUDENT_MJ, \
                STUDENT_MMONEY, STUDENT_MTV, STUDENT_BQN, STUDENT_BPS}
#define DEFAULT_DICE {9,10,8,12,6,5,3,11,3,11,4,6,4,7,9,2,8,10,5}
 
int checkForWinner(Game g);
int rollDice(int numRolls, int diceSides);
action getMove(Game g);
action getUserMove(Game g);
static void printStatistics (Game g);
 
 
int main(int argc, char * argv[]){
    int disciplines[] = DEFAULT_DISCIPLINES;
    int dice[] = DEFAULT_DICE;
    int turnOver = FALSE;
    int winner = NO_ONE;    
    int whoseTurn = NO_ONE;
    int diceValue = 0;
    int studentValue = 0;
    int debugValue = 0;
    int step = FALSE;
    int numTHD = 0;
    int boardState = 0;
    int counter = 0;
    int diceCount[MAX_DICE] = {0};
    int studentCount[MAX_STUDENTS] = {0};
    int modifier = 0;
    action move;
 
    srand(time(NULL)); //initialises a seed 
    
    printf ("Default board 1, random board 0\n");
    scanf ("%d", &boardState);
    if (boardState == 0) {
        while (counter < NUM_REGIONS) {
            diceValue = rollDice (2,6) + modifier;
            diceCount[diceValue]++;
            if ((diceValue == 2 || diceValue == 12) && 
                 diceCount[diceValue] < 2) {
                dice[counter] = diceValue;
                modifier = 0;
            } else if ((diceValue == 3 || diceValue == 11 || 
                        diceValue == 4 || diceValue == 10) && 
                        diceCount[diceValue] < 3) {
                dice[counter] = diceValue;
                modifier = 0;
            } else if ((diceValue == 5 || diceValue == 6 || 
                        diceValue == 7 || diceValue == 8 || 
                        diceValue == 9) && 
                        diceCount[diceValue] < 4) {
                dice[counter] = diceValue;
                modifier = 0;
            } else {
                diceCount[diceValue]--;
                counter--;
                if (modifier > 11) {
                    modifier = 0;
                } else {
                    modifier++;
                }
            }
            counter++;
        }
        modifier = 0;
        counter = 0;
        while (counter < NUM_REGIONS) {
            studentValue = rollDice (1,5);            
            studentCount[studentValue]++;
            if (studentValue == STUDENT_MTV && 
                       studentCount[studentValue] < 7) {
                disciplines[counter] = studentValue;
                modifier = 0;
            } else if ((studentValue == STUDENT_MMONEY || 
                        studentValue == STUDENT_BQN || 
                        studentValue == STUDENT_MJ) && 
                        studentCount[studentValue] < 5) {
                disciplines[counter] = studentValue;
                modifier = 0;
            } else if (studentValue == STUDENT_BPS && 
                       studentCount[studentValue] < 4) {
                disciplines[counter] = studentValue;
                modifier = 0;
            } else {
                diceCount[diceValue]--;
                counter--;
                counter++;
            }
                    
            if (modifier > 11) {
                modifier = 0;
            } else {
                modifier++;
            }
            counter++;
        }
        
        if (disciplines[0] != STUDENT_BPS && 
            disciplines[18] != STUDENT_BPS) {
            if (rollDice (1, 2) == 1) {
                disciplines[0] = STUDENT_BPS;
            } else {
                disciplines[18] = STUDENT_BPS;
            }
        } 
        
        if (disciplines[7] != STUDENT_BPS && 
            disciplines[11] != STUDENT_BPS) {
            if (rollDice (1, 2) == 1) {
                disciplines[7] = STUDENT_BPS;
            } else {
                disciplines[11] = STUDENT_BPS;
            }
        }
        
        if (disciplines[2] != STUDENT_BPS && 
            disciplines[16] != STUDENT_BPS) {
            if (rollDice (1, 2) == 1) {
                disciplines[2] = STUDENT_BPS;
            } else {
                disciplines[16] = STUDENT_BPS;
            }
        }
        
        studentValue = 0;
        while (studentValue == 0 || studentValue == 18 ||
               studentValue == 7 || studentValue == 11 ||
               studentValue == 2 || studentValue == 16) {
            studentValue = rollDice (1, 19) - 1;
        }
        disciplines[studentValue] = STUDENT_THD;
        
    }
    
    Game g = newGame (disciplines, dice);
    
    printf ("Disciplines:");
    counter = 0;
    while (counter < NUM_REGIONS) {
        printf ("%d ", disciplines[counter]);
        counter++;
    }    
    printf ("\nDice:");
    counter = 0;
    while (counter < NUM_REGIONS) {
        printf ("%d ", dice[counter]);
        counter++;
    }
    printf ("\n");
    printf ("Step by step? 1 to control dice, 0 to random\n");
    scanf ("%d", &step);
    
    while (winner == NO_ONE) {        
    
        diceValue = rollDice(2,6);
        
        
        printf ("Dice: %d\n", diceValue);
        if (step == TRUE) {
            scanf ("%d", &diceValue);
        }
        
        if (diceValue == 7) {
            numTHD++;
        }
        
        throwDice (g, diceValue); 
        whoseTurn = getWhoseTurn(g);
        printf ("Turn: %d Player: %d \n", getTurnNumber (g), whoseTurn);
        //loop until player action is PASS
        turnOver = FALSE;
        while (!turnOver) {
            debugValue = 0;            
            move = getMove(g);     
            
            
            //print info about move
            if (isLegalAction(g, move) == TRUE) { 
                
                if (move.actionCode == START_SPINOFF) {
     
                    //decide if outcome is patent or publication
                    if (rand() % 3 <= 1) {   
                        move.actionCode = OBTAIN_PUBLICATION;
                    } else {
                        move.actionCode = OBTAIN_IP_PATENT;                
                    }
                }
        
                if(move.actionCode == PASS){
                     turnOver = TRUE;
                }
                
                makeAction(g, move);
                 
                if(getKPIpoints(g, whoseTurn) >= WINNING_KPI){
                    winner = whoseTurn;
                    turnOver = TRUE;
                    printf ("Winner: %d\n", whoseTurn);  
                    printf ("Current Year: %d\n", getTurnNumber (g));
                    printf ("Number of 7s rolled: %d\n", numTHD);
                    //printStatistics (g);
                }
                
            } else {
                printf ("Invalid move.\n\
                        move.actionCode = %d\n\
                        move.destination = %s\n\
                        move.disciplineFrom = %d\n\
                        move.disciplineTo = %d\n", 
                        move.actionCode, move.destination, move.disciplineFrom, move.disciplineTo);
                debugValue++;
            }
            
            if (debugValue > INFINITE_LOOP) {
                // arbitrary representation of 5 seconds.
                printf ("The AI has been killed for failing to make a valid move %d times", 
                        INFINITE_LOOP);
                assert (FALSE);
            }
            
            printStatistics (g);
        }
    }       
 
    // TODO free memory    
    
    free (g);
    g = NULL;
    
    return EXIT_SUCCESS;
}
 
 
int checkForWinner(Game g) {
   int winner = NO_ONE;
   int playerIndex;
    
   playerIndex = UNI_A;
   while (playerIndex <= UNI_C) {
      // check if the player is over or equal the WINNING_KPI
      if (getKPIpoints(g, playerIndex) >= WINNING_KPI) {
         winner = playerIndex;
      }
      playerIndex++;
   }
    
   return winner;
}
 
int rollDice(int numRolls, int diceSides) {
 
   int diceRollCount = 0;
   int diceValue = 0;
 
   //"rolls the die" 
   while (diceRollCount < numRolls) {
      diceValue += 1 + rand() % diceSides;
      diceRollCount++;
   }
 
   return diceValue;
}
 
//Just one possibility for mixing in AI and human
//Students can modify however they like.
action getMove(Game g){
    action move;
    
    move = decideAction (g);

    return move;
}
 
static void printStatistics (Game g) {
    int playerIndex = UNI_A;
    int studentIndex = STUDENT_THD;
    
    printf ("MostARCs is %d\n", getMostARCs (g));
    printf ("MostPublications is %d\n", getMostPublications (g));
    
    while (playerIndex <= UNI_C) {
        if (playerIndex == 1) {
            printf ("UNI_A Statistics:\n");
        } else if (playerIndex == 2) {
            printf ("UNI_B Statistics:\n");
        } else {
            printf ("UNI_C Statistics:\n");
        }
        printf ("KPI: %d ", getKPIpoints (g, playerIndex));
        printf ("ARCs: %d ", getARCs (g, playerIndex));
        printf ("Campuses: %d ", getCampuses (g, playerIndex));
        printf ("GO8s: %d ", getGO8s (g, playerIndex));
        printf ("IPs: %d ", getIPs (g, playerIndex));
        printf ("Publications: %d\n", getPublications (g, playerIndex));
        studentIndex = 0;
        while (studentIndex <= STUDENT_MMONEY) {
            printf ("Number of STUDENT_%d is %d\n", studentIndex, 
                    getStudents (g, playerIndex, studentIndex));
            studentIndex++;
        }
        printf ("--------------------------------\n");
        playerIndex++;
    }    
    printf ("*****************************\n\n");
}