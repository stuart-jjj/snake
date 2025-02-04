//
//  main.c
//  snakegame
//
//  Created by Beatrix Jones on 12/1/2025.
//  Updated by Stuart on 12/1/2025 - basic movement
//  Updated by Stuart on 16/1/2025 - add user input, length definable, sceen wrap, border
//

/*  Explanation of Key Components:
 *
 *  get_new_direction:
 *  Function checks for user input using getch() and updates the snake's direction based on the arrow keys.
 *  If no valid key is pressed, it returns the current direction.
 *
 *  update_snake:
 *  Function updates the snake's body positions by shifting each segment to the position of the previous segment.
 *  The head position is updated based on the current direction.
 *
 *  draw_border:
 *  This function draws a border around the play area using the # character.
 *  It uses mvprintw() to place characters at specific coordinates.
 *
 *  check_collision:
 *  Function to detect if the snake crosses over its body. If it does, the game ends.
 *
 *  main:
 *  Initializes the snake's body positions and the ncurses environment.
 *  Runs the game loop, which handles input, updates the snake's position, and redraws the screen.
 *  The game loop checks if it's time to extend the snake and calls extend_snake() if necessary.
 *  Cleans up resources and exits when the user presses 'q'.
 */
 
//Timer Mechanism:
//The last_extend_time variable tracks the last time the snake was extended.
//The time() function is used to check if the specified interval (EXTEND_INTERVAL) has passed.
//
//Snake Extension:
//The extend_snake() function adds a new segment at the tail of the snake and increases its length.
//The Snake structure uses dynamic arrays (int *x and int *y) to store the snake's body segments.
//The extend_snake() function uses realloc() to increase the size of these arrays when the snake grows.
//

#include <ncurses.h>   // Include the ncurses library for terminal handling
#include <unistd.h>    // Include for usleep() to introduce delays
#include <stdbool.h>   // Include for boolean type
#include <stdlib.h>    // Include for dynamic memory allocation
#include <time.h>      // Include for time() function

// Define directions using an enum for better readability
typedef enum {
    DIR_UP,    // Represents the upward direction
    DIR_DOWN,  // Represents the downward direction
    DIR_LEFT,  // Represents the leftward direction
    DIR_RIGHT  // Represents the rightward direction
} Direction;

// Define the initial snake length
#define INITIAL_SNAKE_LENGTH 5

// Define the interval (in seconds) for extending the snake
#define EXTEND_INTERVAL 5

// Define a structure to represent the snake
typedef struct {
    int *x;    // Dynamic array to store the X coordinates of the snake's body segments
    int *y;    // Dynamic array to store the Y coordinates of the snake's body segments
    int length; // Current length of the snake
} Snake;

/**
 * Function: get_new_direction
 * --------------------------
 * Handles user input and updates the snake's direction.
 *
 * Parameters:
 *   - current_direction: The current direction of the snake.
 *
 * Returns:
 *   - The new direction based on user input, or the current direction if no valid key is pressed.
 */
Direction get_new_direction(Direction current_direction) {
    int ch = getch(); // Get a character from the keyboard input (non-blocking due to nodelay)
    switch (ch) {     // Check which key was pressed
        case KEY_UP:
            return DIR_UP;    // Return the upward direction
        case KEY_DOWN:
            return DIR_DOWN;  // Return the downward direction
        case KEY_LEFT:
            return DIR_LEFT;  // Return the leftward direction
        case KEY_RIGHT:
            return DIR_RIGHT; // Return the rightward direction
        default:
            return current_direction; // If no valid key is pressed, keep the current direction
    }
}

/**
 * Function: update_snake
 * ----------------------
 * Updates the snake's position based on the current direction.
 *
 * Parameters:
 *   - snake: A pointer to the Snake structure representing the snake.
 *   - direction: The current direction of the snake.
 */
void update_snake(Snake *snake, Direction direction) {
    // Shift the snake's body segments to make room for the new head
    for (int i = snake->length - 1; i > 0; i--) {
        snake->x[i] = snake->x[i - 1]; // Move each segment to the position of the previous segment
        snake->y[i] = snake->y[i - 1];
    }

    // Update the head position based on the direction
    switch (direction) {
        case DIR_UP:
            snake->y[0]--; // Move the head up (decrease Y coordinate)
            break;
        case DIR_DOWN:
            snake->y[0]++; // Move the head down (increase Y coordinate)
            break;
        case DIR_LEFT:
            snake->x[0]--; // Move the head left (decrease X coordinate)
            break;
        case DIR_RIGHT:
            snake->x[0]++; // Move the head right (increase X coordinate)
            break;
    }
}

/**
 * Function: check_collision
 * -------------------------
 * Checks if the snake's head collides with its body.
 *
 * Parameters:
 *   - snake: A pointer to the Snake structure representing the snake.
 *
 * Returns:
 *   - true if the head collides with the body, false otherwise.
 */
bool check_collision(Snake *snake) {
    // Check if the head's position matches any of the body segments' positions
    for (int i = 1; i < snake->length; i++) {
        if (snake->x[0] == snake->x[i] && snake->y[0] == snake->y[i]) {
            return true; // Collision detected
        }
    }
    return false; // No collision
}

/**
 * Function: extend_snake
 * ----------------------
 * Extends the snake by 1 character by adding a new segment at the tail.
 *
 * Parameters:
 *   - snake: A pointer to the Snake structure representing the snake.
 */
void extend_snake(Snake *snake) {
    // Reallocate memory for the snake's body segments
    snake->x = realloc(snake->x, (snake->length + 1) * sizeof(int));
    snake->y = realloc(snake->y, (snake->length + 1) * sizeof(int));

    // Add a new segment at the tail position
    snake->x[snake->length] = snake->x[snake->length - 1];
    snake->y[snake->length] = snake->y[snake->length - 1];

    // Increase the snake's length
    snake->length++;
}

/**
 * Function: draw_border
 * ---------------------
 * Draws a border around the play area using the '#' character.
 *
 * Parameters:
 *   - max_x: The maximum X coordinate of the terminal (width).
 *   - max_y: The maximum Y coordinate of the terminal (height).
 */
void draw_border(int max_x, int max_y) {
    // Draw the top and bottom borders
    for (int i = 0; i < max_x; i++) {
        mvprintw(0, i, "#");         // Draw the top border
        mvprintw(max_y - 1, i, "#"); // Draw the bottom border
    }
    // Draw the left and right borders
    for (int i = 0; i < max_y; i++) {
        mvprintw(i, 0, "#");         // Draw the left border
        mvprintw(i, max_x - 1, "#"); // Draw the right border
    }
}

/**
 * Function: show_splash_screen
 * ----------------------------
 * Displays a splash screen with the text "Snake Game!" for 2 seconds.
 *
 * Parameters:
 *   - max_x: The maximum X coordinate of the terminal (width).
 *   - max_y: The maximum Y coordinate of the terminal (height).
 */
void show_splash_screen(int max_x, int max_y) {
    clear(); // Clear the screen
    mvprintw(max_y / 3, (max_x - 11 ) / 2, "Snake Game!"); // Center the splash screen text
    refresh(); // Refresh the screen to display the text
    sleep(2);  // Display the splash screen for 2 seconds
    clear(); // Clear the screen
}

/**
 * Function: main
 * --------------
 * The entry point of the program. Initializes the game, handles the game loop, and cleans up resources.
 */
int main(void) {
    Snake snake; // Declare a Snake structure to represent the snake
    Direction current_direction = DIR_LEFT; // Initialize the snake's direction to left

    // Initialize ncurses
    initscr();             // Start curses mode (initialize the screen)
    cbreak();              // Disable line buffering (input is processed character by character)
    noecho();              // Don't echo input (keeps user input from being displayed on the screen)
    curs_set(0);           // Hide the default cursor
    keypad(stdscr, TRUE);  // Enable special keys (like arrow keys)
    nodelay(stdscr, TRUE); // Make getch() non-blocking (allows the game loop to continue without waiting for input)

    // Get the terminal's maximum dimensions
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x); // Store the width in max_x and the height in max_y

    // Show the splash screen
    show_splash_screen(max_x, max_y);

    // Initialize the snake's body positions
    snake.length = INITIAL_SNAKE_LENGTH;
    snake.x = malloc(snake.length * sizeof(int));
    snake.y = malloc(snake.length * sizeof(int));
    for (int i = 0; i < snake.length; i++) {
        snake.x[i] = (max_x / 2) + i; // Set the X positions in a horizontal line (starting from the middle)
        snake.y[i] = max_y / 2;       // Set the Y positions to the middle row
    }

    // Main game loop
    bool game_over = false; // Flag to track if the game is over
    time_t last_extend_time = time(NULL); // Track the last time the snake was extended
    while (!game_over) {
        // Check for user input (exit on 'q')
        int ch = getch(); // Get a character from the keyboard input
        if (ch == 'q') {
            break; // Exit the loop if 'q' is pressed
        }

        // Clear the previous snake positions
        for (int i = 0; i < snake.length; i++) {
            mvprintw(snake.y[i], snake.x[i], " "); // Clear each segment of the snake
        }

        // Update the snake's position
        update_snake(&snake, current_direction);

        // Wrap the snake around the screen if it hits a border
        if (snake.x[0] <= 0) {
            snake.x[0] = max_x - 2; // Wrap to the right side
        } else if (snake.x[0] >= max_x - 1) {
            snake.x[0] = 1; // Wrap to the left side
        }
        if (snake.y[0] <= 0) {
            snake.y[0] = max_y - 2; // Wrap to the bottom
        } else if (snake.y[0] >= max_y - 1) {
            snake.y[0] = 1; // Wrap to the top
        }

        // Check for collision with the snake's body
        if (check_collision(&snake)) {
            game_over = true; // End the game if the snake collides with itself
            break;
        }

        // Draw the snake
        for (int i = 0; i < snake.length; i++) {
            mvprintw(snake.y[i], snake.x[i], "O"); // Draw each segment of the snake
        }

        // Draw the border
        draw_border(max_x, max_y);

        // Refresh the screen to display changes
        refresh();

        // Check if it's time to extend the snake
        time_t current_time = time(NULL);
        if (current_time - last_extend_time >= EXTEND_INTERVAL) {
            extend_snake(&snake); // Extend the snake by 1 character
            last_extend_time = current_time; // Update the last extend time
        }

        // Slow down the movement
        usleep(100000); // Introduce a delay of 100ms (100,000 microseconds)

        // Handle user input and update direction
        current_direction = get_new_direction(current_direction);
    }

    // Display game over message
    if (game_over) {
        mvprintw(max_y / 2, (max_x - 10) / 2, "GAME OVER!"); // Center the message
        refresh(); // Refresh the screen to display the message
        usleep(2000000); // Wait for 2 seconds before exiting
    }

    // Clean up and exit
    free(snake.x); // Free the dynamically allocated memory for the snake's X coordinates
    free(snake.y); // Free the dynamically allocated memory for the snake's Y coordinates
    endwin(); // End curses mode and restore the terminal
    return 0; // Return 0 to indicate successful execution
}
