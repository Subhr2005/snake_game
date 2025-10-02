

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Platform-specific headers and functions
#ifdef _WIN32
#include <conio.h>  // For _kbhit() and _getch() on Windows
#include <windows.h> // For Sleep()
void clear_screen() {
    system("cls");
}
void sleep_ms(int milliseconds) {
    Sleep(milliseconds);
}
#else
#include <unistd.h> // For usleep() on Linux/macOS
#include <termios.h> // For terminal manipulation

int _kbhit() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}
int _getch() {
    int ch;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
void clear_screen() {
    system("clear");
}
void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}
#endif


#define WIDTH 20
#define HEIGHT 20

int game_over;
int score;
int head_x, head_y; 
int fruit_x, fruit_y; 
int tail_x[100], tail_y[100]; 
int tail_len;


enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum Direction dir;

/**
 * @brief Initializes the game state.
 * Sets up the initial positions of the snake and fruit, score, and direction.
 */
void setup() {
    game_over = 0;
    dir = STOP;
    head_x = WIDTH / 2;
    head_y = HEIGHT / 2;
    
    // Generate initial fruit location
    srand(time(0));
    fruit_x = rand() % WIDTH;
    fruit_y = rand() % HEIGHT;
    
    score = 0;
    tail_len = 0;
}

/**
 * @brief Draws the game board, snake, fruit, and score to the console.
 */
void draw() {
    clear_screen();
    
    // Top border
    for (int i = 0; i < WIDTH + 2; i++)
        printf("#");
    printf("\n");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            // Left border
            if (j == 0)
                printf("#");

            // Draw snake head
            if (i == head_y && j == head_x)
                printf("O");
            // Draw fruit
            else if (i == fruit_y && j == fruit_x)
                printf("F");
            // Draw snake tail
            else {
                int is_tail_segment = 0;
                for (int k = 0; k < tail_len; k++) {
                    if (tail_x[k] == j && tail_y[k] == i) {
                        printf("o");
                        is_tail_segment = 1;
                        break;
                    }
                }
                if (!is_tail_segment)
                    printf(" ");
            }
            
            // Right border
            if (j == WIDTH - 1)
                printf("#");
        }
        printf("\n");
    }

    // Bottom border
    for (int i = 0; i < WIDTH + 2; i++)
        printf("#");
    printf("\n");
    
    printf("Score: %d\n", score);
}

/**
 * @brief Handles user input to control the snake's direction.
 */
void input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a':
                if (dir != RIGHT) dir = LEFT;
                break;
            case 'd':
                if (dir != LEFT) dir = RIGHT;
                break;
            case 'w':
                if (dir != DOWN) dir = UP;
                break;
            case 's':
                if (dir != UP) dir = DOWN;
                break;
            case 'x':
                game_over = 1;
                break;
        }
    }
}

/**
 * @brief Updates the game state based on the current logic.
 * Moves the snake, checks for collisions, and handles fruit consumption.
 */
void logic() {
    // --- Tail Logic ---
    // The tail segments follow the segment in front of them.
    int prev_x = tail_x[0];
    int prev_y = tail_y[0];
    int prev2_x, prev2_y;
    tail_x[0] = head_x;
    tail_y[0] = head_y;
    for (int i = 1; i < tail_len; i++) {
        prev2_x = tail_x[i];
        prev2_y = tail_y[i];
        tail_x[i] = prev_x;
        tail_y[i] = prev_y;
        prev_x = prev2_x;
        prev_y = prev2_y;
    }

    // --- Head Movement ---
    switch (dir) {
        case LEFT:
            head_x--;
            break;
        case RIGHT:
            head_x++;
            break;
        case UP:
            head_y--;
            break;
        case DOWN:
            head_y++;
            break;
        default:
            break;
    }

    // --- Collision Detection ---
    // Wall collision
    if (head_x >= WIDTH || head_x < 0 || head_y >= HEIGHT || head_y < 0)
        game_over = 1;
    
    // Tail collision
    for (int i = 0; i < tail_len; i++) {
        if (tail_x[i] == head_x && tail_y[i] == head_y)
            game_over = 1;
    }

    // --- Fruit Consumption ---
    if (head_x == fruit_x && head_y == fruit_y) {
        score += 10;
        fruit_x = rand() % WIDTH;
        fruit_y = rand() % HEIGHT;
        tail_len++;
    }
}

/**
 * @brief Main function to run the game loop.
 */
int main() {
    setup();
    
    // Main game loop
    while (!game_over) {
        draw();
        input();
        logic();
        sleep_ms(100); // Adjust to control game speed
    }
    
    printf("\nGame Over!\nFinal Score: %d\n", score);
    
    return 0;
}
