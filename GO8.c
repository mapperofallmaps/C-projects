    path maxCampus;
    int counter = 0;

    if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 2 &&
        i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 1 &&
        i.numGO8s < 8) {
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
    }

    if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 1 &&
        i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 1 &&
        i.numGO8s < 8) {
        if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 2 ||
            i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 2 ||
            i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 2){
            nextAction = exchangeGO8 (g, i);
        }
    }
    if (i.uni[i.currentPlayer].discipline[STUDENT_MMONEY] > 2 &&
        i.uni[i.currentPlayer].discipline[STUDENT_MTV] > 0 &&
        i.numGO8s < 8) {
        if (i.uni[i.currentPlayer].discipline[STUDENT_BPS] > 2 ||
            i.uni[i.currentPlayer].discipline[STUDENT_BQN] > 2 ||
            i.uni[i.currentPlayer].discipline[STUDENT_MJ] > 2){
            nextAction = exchangeGO8 (g, i);
        }
    }