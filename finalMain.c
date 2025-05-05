#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
typedef int bool;
#define false 0
#define true 1
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
int playerIconDirection = 3; // 3 for <, refer to above order
const wchar_t playerIconArr[4] = {characterUp, characterDown, characterLeft, characterRight};
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
    return rand() % 100 + 1;
}

void updateGhost(int oldRow, int oldCol, int newRow, int newCol) {
    ghostExist[oldRow][oldCol] = false;
    ghostExist[newRow][newCol] = true;
}

void MoveRandDir(int *ghostrow, int *ghostcol) {
    int oldRow = *ghostrow, oldCol = *ghostcol;
    int directions[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}}; // Up, Down, Left, Right
    int tries[4] = {0, 1, 2, 3};

    // Shuffle directions
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = tries[i];
        tries[i] = tries[j];
        tries[j] = temp;
    }

    for (int k = 0; k < 4; k++) {
        int d = tries[k];
        int newRow = *ghostrow + directions[d][0];
        int newCol = *ghostcol + directions[d][1];

        if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS &&
            !wallExists[newRow][newCol] && !ghostExist[newRow][newCol]) {
            *ghostrow = newRow;
            *ghostcol = newCol;
            updateGhost(oldRow, oldCol, newRow, newCol);
            return;
        }
    }
}

bool lineOfSight(int r, int c, char *moving) {
    int dr[4] = {1, -1, 0, 0}, dc[4] = {0, 0, 1, -1};
    char dirChar[4] = {'s', 'w', 'd', 'a'};

    for (int d = 0; d < 4; d++) {
        int i = r + dr[d], j = c + dc[d];
        while (i >= 0 && i < ROWS && j >= 0 && j < COLS && !wallExists[i][j]) {
            if (playerExist[i][j]) {
                *moving = dirChar[d];
                return true;
            }
            i += dr[d];
            j += dc[d];
        }
    }
    return false;
}

void updateChance(int chanceArray[4][4], int numberToChange, int colNum) {
    for (int i = 0; i < 4; i++) {
        chanceArray[i][colNum] = (chanceArray[i][colNum] == numberToChange)
                                 ? chanceArray[i][colNum] + 25 : 0;
    }
}

void ghostAI(int ghostrow[], int ghostcol[]) {
    int movementChance[4][4] = {{0}}; // [direction][ghostIndex]
    int dr[4] = {-1, 1, 0, 0}; // U, D, L, R
    int dc[4] = {0, 0, -1, 1};
    char dirChar[4] = {'w', 's', 'a', 'd'};

    for (int i = 0; i < 4; i++) {
        int r = ghostrow[i], c = ghostcol[i];
        char moveDir;
        int chance = getRandomNumber();

        if (lineOfSight(r, c, &moveDir)) {
            for (int d = 0; d < 4; d++) {
                if (dirChar[d] == moveDir) {
                    int nr = r + dr[d], nc = c + dc[d];
                    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS &&
                        !wallExists[nr][nc] && !ghostExist[nr][nc]) {
                        updateGhost(r, c, nr, nc);
                        ghostrow[i] = nr;
                        ghostcol[i] = nc;
                    }
                    break;
                }
            }
        } else {
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d], nc = c + dc[d];
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS &&
                    !wallExists[nr][nc] && !ghostExist[nr][nc] &&
                    chance + movementChance[d][i] > 75) {
                    updateGhost(r, c, nr, nc);
                    ghostrow[i] = nr;
                    ghostcol[i] = nc;
                    updateChance(movementChance, movementChance[d][i], i);
                    goto next_ghost;
                }
            }
            MoveRandDir(&ghostrow[i], &ghostcol[i]);
        }
        next_ghost:;
    }
}

void Parentfunct_moveGhosts(int ghostRow[], int ghostCol[], int runTime) {
    int maxMove = (runTime < 5) ? 1 : (runTime < 10) ? 2 : (runTime < 15) ? 3 : 4;

    if (maxMove < 4) {
        for (int i = 0; i < maxMove; i++) {
            MoveRandDir(&ghostRow[i], &ghostCol[i]);
        }
    } else {
        ghostAI(ghostRow, ghostCol);
    }
}

void movePlayer(int userCord[], char moveInput){
    int cr = userCord[0];
    int cc = userCord[1];
    int nr = cr, nc = cc;

    switch(moveInput){
        case 'a': 
        nc -= 1; 
            playerIconDirection = 2;
            break;
        case 'w': 
            nr -= 1; 
            playerIconDirection = 0;
            break;
        case 's': 
            nr += 1;
            playerIconDirection = 1;
            break;
        case 'd': nc += 1;
            playerIconDirection = 3;
            break;
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
    }
}
void bottomText(int gameRunTime){
    int messageLinesToAdd = ((22 - powerPelletActiveFor) / 2);
    char messageToDisplay[ROWS] = " FREEZE ACTIVE "; // message displayed at the bottom
    char messagePowerPelletOn[ROWS];
    if(!powerPelletIsOn && (gameRunTime % 10) > 5){
        printf("|A/W/S/D, 0 to quit|");
        return;
    }else if(powerPelletIsOn && powerPelletActiveFor > 0){
        for(int i = 0; i < ROWS; i++){
            if(messageLinesToAdd > 0){
                messageToDisplay[i] = '|';
                messageToDisplay[ROWS-i] = '|';
                messageLinesToAdd--;
            }
        }
        for(int j = 0; j < ROWS; j++){
            printf("%c", messageToDisplay[j]);
        }
    }else{
        printf("| Current turn: %d |", gameRunTime);
    }

    
}

void display_map(int lives, int gameRunTime){ // prints map
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++){
            if(wallExists[i][j]){
                wprintf(L"%lc", block);
            }else if(playerExist[i][j]){
                wprintf(L"%lc", playerIconArr[playerIconDirection]);
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
        bottomText(gameRunTime);
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

bool switchingPowerPelletOn = false;

void powerPelletFunction(int gameRunTime, int userCord[], int ghostRow[], int ghostCol[]){
    int r = userCord[0];
    int c = userCord[1]; //Check these, they just define userRow and userCol
    if(playerExist[r][c] && powerPelletExist[r][c]){
        switchingPowerPelletOn = true;
        powerPelletExist[r][c] = false;
        totalPlayerScore += 200; //I looked this up this is how much you get if the screen freezes
    }
    if (switchingPowerPelletOn){ // powerPelletSwitch triggers itself false inside the statement. makes powerPellet active
        powerPelletActiveFor = 20; // change how long powerPellet lasts
        powerPelletIsOn = true;
        switchingPowerPelletOn = false;
    }
    if(powerPelletIsOn){
        if(ghostExist[r][userCord[1]] && playerExist[userCord[0]][userCord[1]]){
            //ghost dies
            totalPlayerScore += 400;
            ghostExist[r][c] = false;
            ghostExist[9][9] = true;
            for (int a = 0; a < 4; a++){ // check to see what ghost the player killed
                if(ghostRow[a] == r && ghostCol[a] == c){
                    ghostRow[a] = 9;
                    ghostCol[a] = 9;
                }
            }
        }
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
    while (running && !gameOvercheck(userCord, &lives, ghostRow, ghostCol)){
        gameRunTime++;
        char input;
        display_map(lives, gameRunTime);
        //ghostDebug();
        scanf(" %c", &input);
        if (input == '0'){
            running = false;}
        movePlayer(userCord, input);
        Parentfunct_moveGhosts(ghostRow, ghostCol, gameRunTime);
        teleportCheck(userCord, ghostRow, ghostCol);
        powerPelletFunction(gameRunTime, userCord, ghostRow, ghostCol);
}
    display_map(lives, gameRunTime); //Display Map one more time
return 0;
}
