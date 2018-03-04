#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Game.h"
#include "mechanicalTurk.h"

action decideAction (Game g) {

	action nextAction;
    nextAction.actionCode = PASS;

	int currentPlayer = getWhoseTurn (g);
	int numMJ;
	int numMTV;
	int numMMONEY;

	if (currentPlayer != NO_ONE) {

		numMJ = getStudents(g, currentPlayer, STUDENT_MJ);
		numMTV = getStudents(g, currentPlayer, STUDENT_MTV);
		numMMONEY = getStudents(g, currentPlayer, STUDENT_MMONEY);

		if (numMJ > 0 && numMTV > 0 && numMMONEY > 0) {
			nextAction.actionCode = START_SPINOFF;
		}
    }
    return nextAction;
}
