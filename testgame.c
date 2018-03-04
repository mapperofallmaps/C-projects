#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_DISCIPLINES {STUDENT_BQN, STUDENT_MMONEY, STUDENT_MJ, \
                STUDENT_MMONEY, STUDENT_MJ, STUDENT_BPS, STUDENT_MTV, \
                STUDENT_MTV, STUDENT_BPS,STUDENT_MTV, STUDENT_BQN, \
                STUDENT_MJ, STUDENT_BQN, STUDENT_THD, STUDENT_MJ, \
                STUDENT_MMONEY, STUDENT_MTV, STUDENT_BQN, STUDENT_BPS}

#define DEFAULT_DICE {9,10,8,12,6,5,3,11,3,11,4,6,4,7,9,2,8,10,5}


int test (void);

int main (int argc, char *argv[]) {
	int disciplines[] = DEFAULT_DISCIPLINES;
	int dice[] = DEFAULT_DICE

	Game g = newGame (disciplines, dice);
	test ();
	disposeGame (Game g);;
	return EXIT_SUCCESS
}

int test (void) {

	throwDice (Game g, 7);
	assert ( (getWhoseTurn (Game g)) == UNI_A);
	assert ( (getARC (Game g, "R")) == VACANT_ARC);
	action A1action;
	A1action.actionCode = OBTAIN_ARC;
	A1action.destination = "R";
	makeAction (Game g, A1action);
	assert ( (getARC (Game g, "R")) != VACANT_ARC);
	assert ( (getStudents (Game g, UNI_A, STUDENT_BPS)) == 2);
	assert ( (getStudents (Game g, UNI_A, STUDENT_BQN)) == 2);
	assert ( (getARCs (Game g, UNI_A)) == 1);
	assert ( (getKPIpoints (Game g, UNI_A)) == 22);
	action A2action;
	A2action.actionCode = OBTAIN_ARC;
	A2action.destination = "RL";
	makeAction (Game g, A2action);
	assert ( (getARC (Game g, "RL")) != VACANT_ARC);
	assert ( (getStudents (Game g, UNI_A, STUDENT_BPS)) == 1);
	assert ( (getStudents (Game g, UNI_A, STUDENT_BQN)) == 1);
	assert ( (getARCs (Game g, UNI_A)) == 2);
	assert ( (getKPIpoints (Game g, UNI_A)) == 24);
	assert ( (getCampus (Game g, "RL")) == VACANT_VERTEX);
	action A3action;
	A3action.actionCode = BUILD_CAMPUS;
	A3action.destination = "RL";
	assert ( (getCampus (Game g, "RL")) != VACANT_VERTEX);
	assert ( (getStudents (Game g, UNI_A, STUDENT_BPS)) == 0);
	assert ( (getStudents (Game g, UNI_A, STUDENT_BQN)) == 0);
	assert ( (getStudents (Game g, UNI_A, STUDENT_MJ)) == 0);
	assert ( (getStudents (Game g, UNI_A, STUDENT_MTV)) == 0);
	assert ( (getCampuses (Game g, UNI_A)) == 1);
	assert ( (getKPIpoints (Game g, UNI_A)) == 34);

	throwDice (Game g, 7);
	assert ( (getWhoseTurn (Game g)) == UNI_B;
	action B1actoin;
	B1action.actionCode = START_SPINOFF;
	makeAction (Game g, B1action);
	assert ( (getStudents (Game g, UNI_B, STUDENT_M$)) == 0);
	assert ( (getStudents (Game g, UNI_B, STUDENT_MJ)) == 0);
	assert ( (getStudents (Game g, UNI_B, STUDENT_MTV)) == 0);
	assert ( (getARC (Game g, "RLRLL")) == VACANT_ARC);
	action B2action;
	B2action.actionCode = OBTAIN_ARC;
	B2action.destination = "RLRLL";
	makeAction (Game g, B2action);
	assert ( (getARC (Game g, "RLRLL")) != VACANT_ARC);
	assert ( (getStudents (Game g, UNI_B, STUDENT_BPS)) == 2);
	assert ( (getStudents (Game g, UNI_B, STUDENT_BQN)) == 2);

	throwDice (Game g, 12);
	assert ( (getStudents (Game g, UNI_A, STUDENT_M$)) == 1);
	assert ( (getWhoseTurn (Game g)) == UNI_C;
	action C1action;
	C1action.actionCode = PASS;
	makeAction (Game g, C1action);

	throwDice (Game g, 12);
	assert ( (getStudents (Game g, UNI_A, STUDENT_M$)) == 2);
	assert ( (getWhoseTurn (Game g)) == UNI_A;
	makeAction (Game g, C1action);

	throwDice (Game g, 12);
	assert ( (getStudents (Game g, UNI_A, STUDENT_M$)) == 3);
	assert ( (getWhoseTurn (Game g)) == UNI_B;
	makeAction (Game g, C1action);

	throwDice (Game g, 6);
	assert ( (getStudents (Game g, UNI_A, STUDENT_MJ)) == 1);
	assert ( (getWhoseTurn (Game g)) == UNI_C;
	makeAction (Game g, C1action);

	throwDice (Game g, 6);
	assert ( (getStudents (Game g, UNI_A, STUDENT_MJ)) == 2);
	assert ( (getWhoseTurn (Game g)) == UNI_A;
	action A4action;
	A4action.actionCode = BUILD_GO8;
	A4action.destination = "RL";
	makeAction (Game g, A4action);
	assert ( (getKPIpoints (Game g, UNI_A)) == 44);
	assert ( (getG08s (Game g, UNI_A)) == 1);
	assert ( (getStudents (Game g, UNI_A, STUDENT_MJ)) == 0);
	assert ( (getStudents (Game g, UNI_A, STUDENT_M$)) == 0);

	throwDice (Game g, 12);
	assert ( (getStudents (Game g, UNI_A, STUDENT_M$)) == 2);

