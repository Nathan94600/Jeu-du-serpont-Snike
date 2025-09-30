// Coded by DosX
// GitHub: https://github.com/DosX-dev

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <ctype.h>

#include "colors.h"

#define SIZE 25

#define WIDTH SIZE
#define HEIGHT SIZE

#define true 0b1          //
#define false 0b0         // I know there are predefined values ​​like TRUE, FALSE, NULL etc
#define short 32768       // But I prefer using these values ​​in small case.
#define null (void *)0b0  //

int gameover = false, stop = false,
    score = 0,
    snakeX[short], snakeY[short],
    snake_length = 1,
    apple1X = -1, apple1Y = -1,
    apple2X = -1, apple2Y = -1,
    apple3X = -1, apple3Y = -1,
    dirX = 0, dirY = 0,
    eated = 0,
    speedDelay = 100;

HANDLE hConsole = null;
CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

void generateApples() {
    do {
        apple1X = 1 + rand() % (WIDTH - 2);
        apple1Y = 1 + rand() % (HEIGHT - 2);
    } while (snakeX[0] == apple1X && snakeY[0] == apple1Y);

    do {
        apple2X = 1 + rand() % (WIDTH - 2);
        apple2Y = 1 + rand() % (HEIGHT - 2);
    } while ((snakeX[0] == apple2X && snakeY[0] == apple2Y) || (apple1X == apple2X && apple1Y == apple2Y));

    do {
        apple3X = 1 + rand() % (WIDTH - 2);
        apple3Y = 1 + rand() % (HEIGHT - 2);
    } while ((snakeX[0] == apple3X && snakeY[0] == apple3Y) || (apple1X == apple3X && apple1Y == apple3Y) || (apple2X == apple3X && apple2Y == apple3Y));
}

void setup() {
    snakeX[0] = WIDTH / 2;
    snakeY[0] = HEIGHT / 2;  // Set snake head at center of game zone

    srand(time(0x00));  // Initialize the random number generator with the current time
    generateApples();

    dirX = 0;
    dirY = 1;  // Move down
}

void refillRenderZone() {
    SetConsoleCursorPosition(hConsole, (COORD){0, 2});  // Skip 2 lines
}

void draw(char *text, int color) {
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, color);
    printf(text);
    SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
}

// Function to render the game screen
void render() {
    refillRenderZone();

    for (int i = 0; i < HEIGHT; i++)  // Loop through the game board rows
    {
        for (int j = 0; j < WIDTH; j++)  // Loop through the game board columns
        {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)               // Check if it's a border cell
                draw("  ", !gameover ? BACKGROUND_CYAN : BACKGROUND_RED);            // Draw a cyan-colored border
            else if (i == snakeY[0] && j == snakeX[0])                               // Check if it's the snake's head cell
                draw(!gameover ? "^^" : "oO", BACKGROUND_WHITE | FOREGROUND_BLACK);  // Draw the snake's head with eyes
            else if (i == apple1Y && j == apple1X)                                       // Check if it's the food cell
                draw("  ", BACKGROUND_LIGHT_RED);                                    // Draw an apple (food)
            else if (i == apple2Y && j == apple2X)                                       // Check if it's the food cell
                draw("  ", BACKGROUND_LIGHT_RED);                                    // Draw an apple (food)
            else if (i == apple3Y && j == apple3X)                                       // Check if it's the food cell
                draw("  ", BACKGROUND_LIGHT_RED);                                    // Draw an apple (food)
            else {
                int isBody = false;
                for (int k = 1; k < snake_length; k++)  // Loop through the snake's body cells
                {
                    if (snakeX[k] == j && snakeY[k] == i)  // Check if it's a cell in the snake's body
                    {
                        draw("##", BACKGROUND_GRAY | FOREGROUND_GRAY);  // Draw the snake's
                                                                        // body segment
                        isBody = true;
                        break;
                    }
                }
                if (!isBody) {
                    printf("  ");  // Draw two empty spaces for an empty cell
                }
            }
        }
        printf("\n");  // Move to the next row
    }

    draw("SCORE", BACKGROUND_YELLOW);  // Draw the "SCORE" label in yellow
    printf(": %d\n", score);           // Display the current score value
}

// Function to handle user input
void input() {
    if (_kbhit()) {
        switch (tolower(_getch())) {
            case 'a':
                // Ignore if the current direction is right (moving to the
                // right) to avoid going backward
                if (dirX != 1) {
                    dirX = -1;  // Move left
                    dirY = 0;
                }
                break;
            case 'd':
                // Ignore if the current direction is left (moving to the left)
                // to avoid going backward
                if (dirX != -1) {
                    dirX = 1;  // Move right
                    dirY = 0;
                }
                break;
            case 'w':
                // Ignore if the current direction is down (moving downwards) to
                // avoid going backward
                if (dirY != 1) {
                    dirX = 0;
                    dirY = -1;  // Move up
                }
                break;
            case 's':
                // Ignore if the current direction is up (moving upwards) to
                // avoid going backward
                if (dirY != -1) {
                    dirX = 0;
                    dirY = 1;  // Move down
                }
                break;
            case 'c':
                stop = true;
                break;
        }
    }
}

// Function to update the game logic
void logic() {
    int prevX = snakeX[0],
        prevY = snakeY[0],
        prev2X, prev2Y;
    snakeX[0] += dirX;
    snakeY[0] += dirY;

    // Met fin à la partie si il y a une collision avec un mur
    if (snakeX[0] == 0 || snakeX[0] == WIDTH - 1 || snakeY[0] == 0 || snakeY[0] == HEIGHT - 1) {
        if (snakeX[0] == 0)
            snakeX[0] = 1;
        else if (snakeX[0] == WIDTH - 1)
            snakeX[0] = WIDTH - 2;
        if (snakeY[0] == 0)
            snakeY[0] = 1;
        else if (snakeY[0] == HEIGHT - 1)
            snakeY[0] = HEIGHT - 2;

        gameover = true;
    };

    for (int i = 1; i < snake_length; i++) {
        prev2X = snakeX[i];
        prev2Y = snakeY[i];
        snakeX[i] = prevX;
        snakeY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Check if the snake eats the food
    if ((snakeX[0] == apple1X && snakeY[0] == apple1Y) || (snakeX[0] == apple2X && snakeY[0] == apple2Y) || (snakeX[0] == apple3X && snakeY[0] == apple3Y)) {
        // Generate new random coordinates for food within the game field (excluding the borders)
        eated++;

        if (snakeX[0] == apple1X && snakeY[0] == apple1Y) {
            apple1Y = -1;
            apple1X = -1;
        }

        if (snakeX[0] == apple2X && snakeY[0] == apple2Y) {
            apple2Y = -1;
            apple2X = -1;
        }

        if (snakeX[0] == apple3X && snakeY[0] == apple3Y) {
            apple3Y = -1;
            apple3X = -1;
        }

        score += 10;     // Increase the score
        
        // Increase the length of the snake
        if (eated == 3) {
            snake_length += 3;

            eated = 0;

            generateApples();
        };
    }

    // Check for collision with itself
    for (int i = 1; i < snake_length; i++) {
        if (snakeX[i] == snakeX[0] && snakeY[i] == snakeY[0]) {
            gameover = true;  // Game over if the snake collides with itself
            break;
        }
    }
}

// Oh ok, legacy code. Really bad code
// int getBitness() { return sizeof(void *) == 0x4 ? 32 : 64; }

#if defined(__i386__)  // i386 (32-bit)
#define bitness 32
#elif defined(__x86_64__)  // x86-64 (AMD64)
#define bitness 64
#else
#define bitness 32
#endif

int fileExists(const char *path) {
    FILE *f = fopen(path, "r");

    if (f) {
        fclose(f);

        return 1; // existe
    }

    return 0; // n'existe pas
}

void main() {
    int vitesse = 1;
    char ch[1], loadSaveChar, filePath[1000], saveName[1000];
    FILE* f;

	do {
		printf("Voulez vous charger une sauvegarde (O/N): ");
		scanf("%s", &loadSaveChar);

        loadSaveChar = tolower((unsigned char)loadSaveChar);
    } while (loadSaveChar != 'o' && loadSaveChar != 'n');

    if (loadSaveChar == 'o') {
        printf("\nChemin de la sauvegarde: ");
        scanf("%s", filePath);

    	do {
	        printf("\nFichier introuable, redonnez le chemin de la sauvegarde: ");
            scanf("%s", filePath);
        } while (!fileExists(filePath));

        // Charger la sauvegarde

        
    } else {
    	do {
	    	printf("Choisissez une vitesse (1 ou 2): ");
	
    		scanf("%s", ch);
	
	    	vitesse = strtod(ch, NULL);
        } while (vitesse != 1 && vitesse != 2);

        speedDelay /= vitesse * vitesse;

        char title[short];

        sprintf(title, "Tiny Snake (x%d)", bitness);

        SetConsoleTitleA(title);

        int buffer[] = {(WIDTH * 2) + 1, HEIGHT + 8};

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SMALL_RECT windowSize = {0, 0, buffer[0] - 1, buffer[1] - 1};
        SetConsoleWindowInfo(hConsole, true, &windowSize);

        SetConsoleScreenBufferSize(hConsole, (COORD){buffer[0], buffer[1]});

        CONSOLE_CURSOR_INFO cursorinfo = {0};
        cursorinfo.dwSize = 1;
        cursorinfo.bVisible = FALSE;  // Turns off the blinking cursor (Thank to https://github.com/R32GTT)
        SetConsoleCursorInfo(hConsole, &cursorinfo);

        printf("Coded by DosX-dev (GitHub)\nUSE ONLY ENGLISH KEYBOARD LAYOUT! (WASD)\n");

        setup();  // Initialize the game

        while (!gameover && !stop) {
            render();  // Draw the current state of the game
            input();   // Handle user input
            logic();   // Update the game logic

            Sleep(speedDelay);  // Add a delay to control the snake's speed
        }

        if (stop) {
            // Système de sauvegarde
            printf("\nNom de la sauvegarde: ");
            scanf("%s", &saveName);

            strcat(saveName, ".save");

            while (fileExists(saveName))
            {
                printf("\nLe fichier %s existe deja, choisissez un nouveau nom pour la sauvegarde: ", saveName);
                scanf("%s", &saveName);

                strcat(saveName, ".save");
            }

            f = fopen(saveName, "a");

            fprintf(
                f,
                "%d %d %d %d %d %d %d %d %d %d %d %d %d",
                vitesse, score, gameover, apple1X, apple1Y, apple2X, apple2Y, apple3X, apple3Y, eated, dirX, dirY, snake_length
            );

            for (int i = 0; i < short; i++) {
                fprintf(f, " %d %d", snakeX[i], snakeY[i]);
            }

            fclose(f);

            printf("\nPartie sauvegarder dans le fichier %s", saveName);

            return;
        } else {
            dirX = 0;
            dirY = 0;

            render();  // Run the renderer for the last time to draw the desired colors (lose)

            printf("\n");

            draw("==============\n=", BACKGROUND_RED | FOREGROUND_RED);
            draw(" GAME OVER! ", BACKGROUND_WHITE | FOREGROUND_RED);
            draw("=\n==============", BACKGROUND_RED | FOREGROUND_RED);

            printf("\nPress X to exit");

            while (true) {
                if (tolower(_getch()) == 'x') {
                    return;
                }
            }
        }
    }
}

// I like coffee. Preferably with milk
