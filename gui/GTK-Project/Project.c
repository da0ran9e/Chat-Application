// The main and bigger project is located in:
// https://github.com/UMMAN2005/GTK-Project-TTT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void restart(char);


void displayBoard(char board[][10], int size) {
    printf("\n ");
    for (int i = 1; i <= size; i++) {
        printf(" %d ", i);
        if (i < size) {
            printf("|");
        }
    }
    printf("\n");
    for (int i = 0; i < size; i++) {
    
        printf("%d", i + 1);
        for (int j = 0; j < size; j++) {
            printf(" %c ", board[i][j]);
            if (j < size - 1) {
                printf("|");
            }
        }
        printf("\n");
        if (i < size - 1) {
            printf(" ");
            for (int j = 0; j < size; j++) {
                printf("---");
                if (j < size - 1) {
                    printf("+");
                }
            }
            printf("\n");
        }
    }
    printf("\n");
}

//Function to check if the player has won
int checkWin(char board[][10], int size, char player) {
    // Check rows
    int count = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (board[i][j] == player) {
                count++;
                if (count == 3) return 1;
            }
            else count = 0;
        }

    }

    // Check columns
    count = 0;
    for (int j = 0; j < size; ++j) {
        for (int i = 0; i < size; ++i) {
            if (board[i][j] == player) {
                count++;
                if(count == 3) return 1;
            }
            else count = 0;
        }
    }



    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
        if (i + 2 < size && j + 2 < size && board[i][j] == player && board[i + 1][j + 1] == player && board[i + 2][j + 2] == player) {
            return 1;
            }
        if (i + 2 < size && j - 2 >= 0 && board[i][j] == player && board[i + 1][j - 1] == player && board[i + 2][j - 2] == player) {
            return 1;
            }
        }
    }


    return 0; // No winner yet
}



// Function to make a move on the board
void makeMove(char board[][10], int row, int col, char player) {
    board[row][col] = player;
}


void initializeGame(char currentPlayer) {
    int size;

    // Get the size of the board from the user
    do {
        printf("Enter the size of the Tic-Tac-Toe board (between 3 and 10): ");
        if (scanf("%d", &size) != 1) {
            printf("Input is not valid!\n");
            while (getchar() != '\n');
        };
    } while (size < 3 || size > 10);

    // Initialize the board
    char board[10][10];
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            board[i][j] = ' ';
        }
    }

    int row, col;

    // Game loop
    int totalMoves = size * size;
    int moveCount = 0;

    do {
        // Display the current state of the board
        displayBoard(board, size);

        // Get the player's move

    do {
        printf("Player %c, enter your move (row and column): ", currentPlayer);
        scanf("%d %d", &row, &col);
    
        if (row < 1 || row > size || col < 1 || col > size) {
            printf("Invalid move. Row and column must be between 1 and %d. Try again.\n", size);
        } else if (board[row - 1][col - 1] != ' ') {
            printf("This place is not empty. Try again.\n");
        }
    } while (row < 1 || row > size || col < 1 || col > size || board[row - 1][col - 1] != ' ');

        // Make the move
        makeMove(board, row - 1, col - 1, currentPlayer);

        // Check for a win
        if (checkWin(board, size, currentPlayer)) {
            displayBoard(board, size);
            printf("Player %c wins!\n", currentPlayer);
            restart(currentPlayer);
            break;
        }

        moveCount++;

        // Check for a draw
        if (moveCount == totalMoves) {
            displayBoard(board, size);
            printf("It's a draw!\n");
            break;
        }

        // Switch to the other player
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';

    } while (1);
}

void restart(char currentPlayer) {
    char* answer;
    printf("Would you like to play again? ('yes' or 'no'): ");
    scanf("%s", answer);
    for (int i = 0; i < strlen(answer); i++) {
        answer[i] =  tolower(answer[i]);
    }

    if (strcmp(answer, "yes") == 0) {
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        initializeGame(currentPlayer);
    } else if (strcmp(answer, "no") == 0) {
        printf("Thanks for playing! Goodbye.\n");
    } else {
        printf("Invalid input. Please enter 'yes' or 'no'!\n ");
        restart(currentPlayer);
    }
}

int main() {
    char player;
    do {
        printf("Which player should start the first? `X` or `O` (Default `X`): ");
        scanf(" %c", &player); // Added a space before %c to consume any leading whitespaces

        player = toupper(player);

        if (player != 'X' && player != 'O') {
            printf("Invalid input. Please enter 'X' or 'O'!\n");
        }
} while (player != 'X' && player != 'O');

    

    
    player = toupper(player);
    
    if ((int)player == 88)
    initializeGame('X');
    else if ((int)player == 79)
    initializeGame('O');
    else
    initializeGame('X');
    return 0;
}