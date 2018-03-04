/*
 *  Mr Thd.  Completely useless
 *  This AI makes really bad moves!
 *  When they have enough BPS or BQN students they convert them to THDs.
 *  Otherwise they pass
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Game.h"
#include "mechanicalTurk.h"

action decideAction (Game g) {

   
    action nextAction;
   
    nextAction.actionCode = PASS;
   

    int currentPlayer = getWhoseTurn(g); 
    
    if( currentPlayer != NO_ONE ) {
        
        int numBPS = getStudents(g, currentPlayer, STUDENT_BPS);
        int numBQN = getStudents(g, currentPlayer, STUDENT_BQN);

        //This checks you have all the necessary conditions
        //then creates the action
        //Alternately you could create the action and then just check it is legal if that is easier    
        if( numBPS >= getExchangeRate(g,currentPlayer, STUDENT_BPS,STUDENT_THD )) {
            nextAction.actionCode = RETRAIN_STUDENTS;
            nextAction.disciplineFrom = STUDENT_BPS;
            nextAction.disciplineTo = STUDENT_THD;
 
        }else if( numBQN >= getExchangeRate(g,currentPlayer, STUDENT_BQN,STUDENT_THD )) {
            nextAction.actionCode = RETRAIN_STUDENTS;
            nextAction.disciplineFrom = STUDENT_BQN;
            nextAction.disciplineTo = STUDENT_THD;
        } 
    }

    return nextAction;
}
