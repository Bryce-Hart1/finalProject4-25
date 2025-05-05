#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

/*
we were having issues with how unicode prints on windows compared to windows.
This program has only been tested for the following terminals: zsh, powershell
*/
// Windows-specific headers
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif
#ifdef __APPLE__
#include<locale.h>
#endif
// Game symbols (wide characters)
const wchar_t block = L'█';
const wchar_t characterUp = L'˅';
const wchar_t characterDown = L'^';
const wchar_t characterLeft = L'>';
const wchar_t characterRight = L'<';
const wchar_t ghost = L'ᗣ';
const wchar_t pellet = L'•';
const wchar_t powerPellet = L'⬤';
const wchar_t es = L' '; // empty space
bool powerPelletIsOn = false;
int powerPelletActiveFor = 0; // how many more turns powerPellet is active for
#define ROWS 21
#define COLS 19
int totalPlayerScore = 0;

/*
we then made true false arrays for all items. this makes everything very simple to keep up with
needed one for walls pellets powerpellets player and ghost (so 5). made walls const so that way the wall cannot be replaced.
*/
const bool wallExists[ROWS][COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},//0
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,0,1},//4
    {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1},
    {0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0},
    {1,1,1,1,0,1,0,1,1,0,1,1,0,1,0,1,1,1,1},
    {0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0},//9
    {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
    {0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0},
    {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} };
bool pelletsExists[ROWS][COLS] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,1,0},
    {0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,1,0},
    {0,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0},
    {0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0},
    {0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
bool powerPelletExist[ROWS][COLS] = {       //[2][1]   [2][17]   [15][1]   [15][17]
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

bool playerExist[ROWS][COLS] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

bool ghostExist[ROWS][COLS] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

// Initialize console for Unicode (cross-platform)
void init_console() {
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT); // Windows
    #else
        setlocale(LC_ALL, "en_US.UTF-8"); // Mac/Linux
    #endif
}

int getRandomNumber() {
    return rand() % 100 + 1; // returns a number between 1 and 100
}

void updateGhost(int oldRow, int oldCol, int newRow, int newCol) {
    ghostExist[oldRow][oldCol] = false;
    ghostExist[newRow][newCol] = true;
}


void MoveRandDir(int *ghostrow, int *ghostcol) {
    int oldRow = *ghostrow;
    int oldCol = *ghostcol;
    int randomNumber = getRandomNumber() % 4;
    int attempts = 0; // Track how many times we attempt to find a valid move

    while (attempts < 10) { // Limit number of attempts to prevent infinite loop
        if (randomNumber == 0 && *ghostrow > 0 && !wallExists[*ghostrow - 1][*ghostcol] && !ghostExist[*ghostrow - 1][*ghostcol]) {
            (*ghostrow) -= 1; // move up
            break;
        } else if (randomNumber == 1 && *ghostrow < ROWS - 1 && !wallExists[*ghostrow + 1][*ghostcol] && !ghostExist[*ghostrow + 1][*ghostcol]) {
            (*ghostrow) += 1; // move down
            break;
        } else if (randomNumber == 2 && *ghostcol > 0 && !wallExists[*ghostrow][*ghostcol - 1] && !ghostExist[*ghostrow][*ghostcol - 1]) {
            (*ghostcol) -= 1; // move left
            break;
        } else if (randomNumber == 3 && *ghostcol < COLS - 1 && !wallExists[*ghostrow][*ghostcol + 1] && !ghostExist[*ghostrow][*ghostcol + 1]) {
            (*ghostcol) += 1; // move right
            break;
        } else {
            randomNumber = getRandomNumber() % 4; // Try another direction
        }

        attempts++; // Increment attempts counter
    }

    // Update ghost positions in ghostExist array, if the ghost moved
    if (attempts < 10) {
        ghostExist[oldRow][oldCol] = false;  // Clear old position
        ghostExist[*ghostrow][*ghostcol] = true;  // Set new position
    }
}





bool lineOfSight(int currentRow, int currentCol, char *moving) {
    int i = currentRow, j = currentCol;
    // Look down
    while (i >= 0 && i < ROWS && j >= 0 && j < COLS && !wallExists[i][j] && !ghostExist[currentRow+1][currentCol]){
        if (playerExist[i][j]) { *moving = 's'; return true; }
        i++;
    }

    i = currentRow; //reset I look up
    while (i >= 0 && i < ROWS && j >= 0 && j < COLS && !wallExists[i][j] && !ghostExist[currentRow-1][currentCol]){
        if (playerExist[i][j]) { *moving = 'w'; return true; }
        i--;
    }

    i = currentRow; // reset I look right
    while (i >= 0 && i < ROWS && j >= 0 && j < COLS && !wallExists[i][j] && !ghostExist[currentRow][currentCol+1]){
        if (playerExist[i][j]) { *moving = 'd'; return true; }
        j++;
    }

    j = currentCol; //reset J look left
    while (i >= 0 && i < ROWS && j >= 0 && j < COLS && !wallExists[i][j] && !ghostExist[currentRow][currentCol-1]){
        if (playerExist[i][j]) { *moving = 'a'; return true; }
        j--;
    }

    return false;
}

void updateChance(int chanceArray[4][4], int numberToChange, int colNum) {
    for (int i = 0; i < 4; i++) {
        if (chanceArray[i][colNum] == numberToChange) {
            chanceArray[i][colNum] += 25;
        } else {
            chanceArray[i][colNum] = 0;
        }
    }
}

void ghostAI(int ghostrow[], int ghostcol[]) {
    int currentRow, currentCol, precent;
    int movementChance[4][4] = {{0, 0, 0, 0}, // Up
                                {0, 0, 0, 0}, // Down
                                {0, 0, 0, 0}, // Left
                                {0, 0, 0, 0}}; // Right
    char moving = 'a'; // Direction ghost has been moving

    for (int i = 0; i < 4; i++) { // Play for all 4 ghosts
        precent = getRandomNumber();
        currentRow = ghostrow[i];
        currentCol = ghostcol[i];

        // Check line of sight and move toward player if possible
        if (lineOfSight(currentRow, currentCol, &moving)) { // If line of sight, move toward player
            switch (moving) {
                case 'a': // Left
                    if (currentCol > 0 && !wallExists[currentRow][currentCol - 1] || !ghostExist[currentRow][currentCol - 1]) {
                        updateGhost(currentRow, currentCol, currentRow, currentCol - 1);
                        ghostcol[i] -= 1;
                    }
                    break;
                case 'w': // Up
                    if (currentRow > 0 && !wallExists[currentRow - 1][currentCol] || !ghostExist[currentRow][currentCol - 1]) {
                        updateGhost(currentRow, currentCol, currentRow - 1, currentCol);
                        ghostrow[i] -= 1;
                    }
                    break;
                case 's': // Down
                    if (currentRow < ROWS - 1 && !wallExists[currentRow + 1][currentCol] || !ghostExist[currentRow][currentCol - 1]) {
                        updateGhost(currentRow, currentCol, currentRow + 1, currentCol);
                        ghostrow[i] += 1;
                    }
                    break;
                case 'd': // Right
                    if (currentCol < COLS - 1 && !wallExists[currentRow][currentCol + 1] || !ghostExist[currentRow][currentCol - 1]) {
                        updateGhost(currentRow, currentCol, currentRow, currentCol + 1);
                        ghostcol[i] += 1;
                    }
                    break;
                default:
                    return;
            }
        } else { // If no line of sight, move randomly
            srand(time(NULL)); // Seed the random generator
            precent = getRandomNumber();

            // Move up
            if (precent + movementChance[0][i] > 75 && currentRow > 0 && !wallExists[currentRow - 1][currentCol] && !ghostExist[currentRow][currentCol]) {
                updateGhost(currentRow, currentCol, currentRow - 1, currentCol);
                updateChance(movementChance, movementChance[0][i], i);
                ghostrow[i] -= 1;
            }
            // Move down
            else if (precent + movementChance[1][i] > 75 && currentRow < ROWS - 1 && !wallExists[currentRow + 1][currentCol]&& !ghostExist[currentRow][currentCol]) {
                updateGhost(currentRow, currentCol, currentRow + 1, currentCol);
                updateChance(movementChance, movementChance[1][i], i);
                ghostrow[i] += 1;
            }
            // Move left
            else if (precent + movementChance[2][i] > 75 && currentCol > 0 && !wallExists[currentRow][currentCol - 1] && !ghostExist[currentRow][currentCol]) {
                updateGhost(currentRow, currentCol, currentRow, currentCol - 1);
                updateChance(movementChance, movementChance[2][i], i);
                ghostcol[i] -= 1;
            }
            // Move right
            else if (precent + movementChance[3][i] > 75 && currentCol < COLS - 1 && !wallExists[currentRow][currentCol + 1] && !ghostExist[currentRow][currentCol]){
                updateGhost(currentRow, currentCol, currentRow, currentCol + 1);
                updateChance(movementChance, movementChance[3][i], i);
                ghostcol[i] += 1;
            } else {
                // Fallback to random direction if no valid move found
                MoveRandDir(&ghostrow[i], &ghostcol[i]);
            }
        }
    }
}

// for the first couple turns, move the ghosts out individually.
void Parentfunct_moveGhosts(int ghostRow[], int ghostCol[], int runTime){
if(runTime < 5){
    MoveRandDir(&ghostRow[0], &ghostCol[0]);
}else if(runTime < 10 && runTime > 5){
    MoveRandDir(&ghostRow[0], &ghostCol[0]);
    MoveRandDir(&ghostRow[1], &ghostCol[1]); //move first 2
}else if(runTime > 10 && runTime < 15){
    MoveRandDir(&ghostRow[0], &ghostCol[0]);
    MoveRandDir(&ghostRow[1], &ghostCol[1]);
    MoveRandDir(&ghostRow[2], &ghostCol[2]); //move first 3
}else{
    ghostAI(ghostRow, ghostCol); //AI takes over
}     
}

void movePlayer(int userCord[], char moveInput){
    int cr = userCord[0];
    int cc = userCord[1];
    int nr = cr, nc = cc;

    switch(moveInput){
        case 'a': nc -= 1; break;
        case 'w': nr -= 1; break;
        case 's': nr += 1; break;
        case 'd': nc += 1; break;
        default: return; // Invalid input
    }

    // Check bounds and wall
    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && !wallExists[nr][nc]) {
        playerExist[cr][cc] = false; // Clear old position
        playerExist[nr][nc] = true;// Set new position
        if(pelletsExists[nr][nc]){
            totalPlayerScore++;
        }
        pelletsExists[nr][nc] = false;
        userCord[0] = nr;
        userCord[1] = nc;
    }// player will not move
}

// Print the game map this will do all chars. this will always print the map
//you do not need to do == true
void display_map(int lives){
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++){
            if(wallExists[i][j]){
                wprintf(L"%lc", block);
            }else if(playerExist[i][j]){
                wprintf(L"%lc", characterRight);
            }else if(powerPelletExist[i][j]){
                wprintf(L"%lc", powerPellet);
            }else if(ghostExist[i][j]){
                wprintf(L"%lc", ghost);
            }else if(pelletsExists[i][j]){
                wprintf(L"%lc", pellet);
            }else{
                wprintf(L"%lc", es);
            }
        }
        wprintf(L"\n");
    }
        wprintf(L"SCORE: %d   LIVES: %d\n", totalPlayerScore, lives);
        wprintf(L"Enter move (w/a/s/d) or '0' to quit: \n"); 
}

void teleportCheck(int userCord[], int ghostRow[], int ghostCol[]){

    if (userCord[0] == 9 && userCord[1] == 0)
    {
        userCord[0] = 9;
        userCord[1] = 17;
        playerExist[9][0] = false;
    }
    else if (userCord[0] == 9 && userCord[1] == 18)
    {
        userCord[0] = 9;
        userCord[1] = 1;
        playerExist[9][18] = false;
    }
}

void resetGame(int userCord[], int ghostRow[], int ghostCol[])
{
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            playerExist[i][j] = false;
            ghostExist[i][j] = false;
        }
    }

    playerExist[15][9] = true;

    ghostExist[8][9] = true;
    ghostExist[9][9] = true;
    ghostExist[9][8] = true;
    ghostExist[9][10] = true;

    userCord[0] = 15;
    userCord[1] = 9;

    ghostRow[0] = 8;
    ghostRow[1] = 9;
    ghostRow[2] = 9;
    ghostRow[3] = 9;

    ghostCol[0] = 9;
    ghostCol[1] = 9;
    ghostCol[2] = 8;
    ghostCol[3] = 10;
}

bool switchPowerPellet = false;

void powerPelletActive(int gameRunTime, int userCord[], int ghostRow[], int ghostCol[]){
    int i = userCord[0];
    int j = userCord[1]; //Check these, they just define userRow and userCol
    if(playerExist[i][j] && powerPelletExist[i][j]){
        switchPowerPellet = true;
        powerPelletExist[i][j] = false;
    }
    if (switchPowerPellet){ // powerPelletSwitch triggers itself false inside the statement. makes powerPellet active
        powerPelletActiveFor = 12;
        powerPelletIsOn = true;
        switchPowerPellet = false;
    }
    if(powerPelletActiveFor > 0){
        if(ghostExist[userCord[0]][userCord[1]] && playerExist[userCord[0]][userCord[1]]){
            //ghost dies
            totalPlayerScore += 400;
            ghostExist[userCord[0]][userCord[1]] = false;
            ghostExist[9][9] = true;
            for (int a = 0; a < 4; a++){ // check to see what ghost the player killed
                if(ghostRow[a] == i && ghostCol[a] == j){
                    ghostRow[a] = 9;
                    ghostCol[a] = 9;
                }
            }
        }
        wprintf(L"POWER PELLET ACTIVE! %d MOVE(S) LEFT!\n", powerPelletActiveFor);
        powerPelletActiveFor--;
    }
    if(powerPelletActiveFor == 1){
        powerPelletIsOn = false;
    }

    return;
}

bool allPelletsCollected(){
    for (int i = 0; i < ROWS; i++){
        for (int j = 0; j < COLS; j++){
            if (powerPelletExist[i][j] || pelletsExists[i][j]) {
                return false;
            }
            }
        }
    return true;
}

bool gameOvercheck(int userCord[], int *lives, int ghostRow[], int ghostCol[]){
    if(playerExist[userCord[0]][userCord[1]] && ghostExist[userCord[0]][userCord[1]] && !powerPelletIsOn){
        *lives -= 1;
        wprintf(L"You died! You have %d lives left!\n", *lives);
        resetGame(userCord, ghostRow, ghostCol);
    }

    if (allPelletsCollected())
    {
        wprintf(L"You Win! Your final score is: %d\n", totalPlayerScore);
        return true;
    }
    
    if (*lives == 0)
    {
       wprintf(L"\nGAME OVER!\n");
       return true;
    }
    
    return false;
}

int main() {
#ifdef __APPLE__
    setlocale(LC_ALL, "en_US.UTF-8");
#endif
    init_console(); // Set up Unicode output
    int ghostRow[4] = {8,9,9,9};
    int ghostCol[4] = {9,9,8,10};
    int userCord[2] = {15, 9}; // Player start pos y,x
    bool running = true;
    int gameRunTime = 0;
    int score = 0; //14,600 for pellets, 1,000 for 4 power pellets (250), 800 for each ghost killed
    int lives = 3;
    int powerPelletActiveFor = 0; //12 ticks(moves) are added when a power pellet is used

    while (running && !gameOvercheck(userCord, &lives, ghostRow, ghostCol)){
        gameRunTime++;
        char input;
        display_map(lives);
        //ghostDebug();
        scanf(" %c", &input);
        if (input == '0'){
            running = false;}
        movePlayer(userCord, input);
        Parentfunct_moveGhosts(ghostRow, ghostCol, gameRunTime);
        teleportCheck(userCord, ghostRow, ghostCol);
        powerPelletActive(gameRunTime, userCord, ghostRow, ghostCol);
}
    display_map(lives);
return 0;
}
