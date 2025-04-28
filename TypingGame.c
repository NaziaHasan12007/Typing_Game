#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/*
    Typing Test Program using Ncurses

    Features:
    1. Input redirection for word list.
    2. Difficulty selection (Easy, Medium, Hard) that generates words of varying lengths.
    3. Displays reference text.
    4. Typing input with real-time feedback:
        - Green: Correct character
        - Red: Incorrect character
        - Supports Backspace
    5. Timer: 60 seconds duration
    6. Real-time and final statistics:
        - WPM (Words per Minute)
        - Keystrokes
        - Mistakes
        - CPS (Characters per Second)
        - Accuracy
    7. Can quit anytime using ESC key
    8. Can restart anytime using key '4'
*/

#define max_word 70
#define max_length 20

int word_count, total_char;
char word_pool[1000][max_length];
char reference_text[max_word][max_length];
char printed_text[max_word][max_length];
char reference_line[1210]; // Stores the full reference text
int total_words, max_character;
int start_y, start_x; // Starting coordinates of the reference text

void load_text();
void generate_text(int total_word, int min_char, int max_char);
int wpm(char reference_text[max_word][max_length], char input[1210], double total_time);

int main(void) {
    char input[1210];
    int difficulty;
    int restart = 1;
    int mistakes[1210] = {0}; // Mistake tracking per character

    load_text(); // Load word list from stdin

    while (restart) {
        restart = 0;
        freopen("/dev/tty", "r", stdin); // Reset input to terminal

        initscr(); // Initialize ncurses
        noecho();
        cbreak();
        keypad(stdscr, TRUE);

        // Initialize color pairs
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK); // Correct input
        init_pair(2, COLOR_RED, COLOR_BLACK);   // Incorrect input
        init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Prompt messages
        init_pair(4, COLOR_CYAN, COLOR_BLACK);   // Difficulty options
        init_pair(5, COLOR_WHITE, COLOR_BLACK);  // Other info

        // Prompt for difficulty
        while (1) {
            attron(COLOR_PAIR(3));
            printw("Select Difficulty:\n");
            attroff(COLOR_PAIR(3));

            attron(COLOR_PAIR(4));
            printw("1.Easy\n2.Medium\n3.Hard\n");
            attroff(COLOR_PAIR(4));

            attron(COLOR_PAIR(5));
            printw("Press 4 to restart anytime\nPress enter to start\nPress ESC to quit anytime\n");
            attroff(COLOR_PAIR(5));

            refresh();
            scanw("%d", &difficulty);
            if (difficulty >= 1 && difficulty <= 3) break;
            else {
                attron(COLOR_PAIR(5));
                printw("Invalid Input\nEnter between 1 and 3\n");
                attroff(COLOR_PAIR(5));
            }
        }

        // Generate text based on difficulty
        if (difficulty == 1) {
            generate_text(20, 1, 4);
            total_words = 20;
        } else if (difficulty == 2) {
            generate_text(40, 5, 7);
            total_words = 40;
        } else {
            generate_text(60, 8, 19);
            total_words = 60;
        }

        int y_ref = getcury(stdscr) + 2; // Place reference text 2 lines below
        start_y = y_ref;
        start_x = 0;
        move(y_ref, 0);
        refresh();

        time_t start_time = time(NULL);
        int st = getch();

        if (st == 10) { // If Enter pressed to start
            int x = 0, total_keystroke = 0, mistake = 0;
            input[0] = '\0';
            int ref_len = strlen(reference_line);

            while (time(NULL) - start_time < 60) {
                if (x >= ref_len) break;

                int ch = getch();
                if (ch == 27) break; // ESC to quit
                if (ch == '4') { restart = 1; break; } // '4' to restart

                if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
                    if (x > 0) {
                        x--;
                        total_keystroke--;
                        mistake -= mistakes[x];
                        input[x] = '\0';

                        // Redraw characters
                        for (int k = 0; k < x; k++) {
                            int row = y_ref + k / COLS;
                            int col = k % COLS;
                            if (input[k] == reference_line[k]) {
                                attron(COLOR_PAIR(1));
                                mvaddch(row, col, input[k]);
                                attroff(COLOR_PAIR(1));
                            } else {
                                attron(COLOR_PAIR(2));
                                mvaddch(row, col, input[k]);
                                attroff(COLOR_PAIR(2));
                            }
                        }
                        move(y_ref + x / COLS, x % COLS);
                        refresh();
                    }
                    continue;
                }

                // Add input character and check
                if (x < ref_len) {
                    input[x] = ch;
                    input[x + 1] = '\0';
                    total_keystroke++;

                    int row = y_ref + x / COLS;
                    int col = x % COLS;

                    if (ch == reference_line[x]) {
                        attron(COLOR_PAIR(1));
                        mvaddch(row, col, ch);
                        attroff(COLOR_PAIR(1));
                        mistakes[x] = 0;
                    } else {
                        attron(COLOR_PAIR(2));
                        mvaddch(row, col, ch);
                        attroff(COLOR_PAIR(2));
                        mistakes[x] = 1;
                        mistake++;
                    }
                    x++;
                    move(y_ref + x / COLS, x % COLS);
                    refresh();
                }

                // Show real-time stats
                move(50, 0);
                clrtoeol();
                double present_time = difftime(time(NULL), start_time);
                printw("WPM: %d\nKeystrokes: %d\nMistakes: %d\nCPS: %.2f\nAccuracy: %.2f%%",
                       wpm(printed_text, input, present_time),
                       total_keystroke,
                       mistake,
                       total_keystroke / present_time,
                       100 - (mistake * 100.0 / total_keystroke));
                refresh();
            }

            if (!restart) {
                endwin();
                double total_time = difftime(time(NULL), start_time);
                printf("Final Statistics\nWPM: %d\nKeystrokes: %d\nMistakes: %d\nCPS: %.2f\nAccuracy: %.2f%%\n",
                       wpm(printed_text, input, total_time),
                       total_keystroke,
                       mistake,
                       total_keystroke / total_time,
                       100 - (mistake * 100.0 / total_keystroke));
            } else endwin();
        } else endwin();
    }
    return 0;
}

// Loads words from standard input into word_pool
void load_text() {
    word_count = 0;
    while (fgets(word_pool[word_count], max_length, stdin) && word_count < 1000) {
        word_pool[word_count][strcspn(word_pool[word_count], "\n")] = '\0';
        word_count++;
    }
}

// Generates reference text based on difficulty
void generate_text(int total_word, int min_char, int max_char) {
    int ref_count = 0;
    srand(time(NULL));
    for (int j = 0; j < word_count && ref_count < max_word; j++) {
        if (strlen(word_pool[j]) >= min_char && strlen(word_pool[j]) <= max_char) {
            strcpy(reference_text[ref_count], word_pool[j]);
            ref_count++;
        }
    }

    reference_line[0] = '\0';
    int row = getcury(stdscr);
    int col = 0;

    for (int j = 0; j < total_word && j < 60; j++) {
        int idx = rand() % ref_count;
        char *word = reference_text[idx];
        int word_len = strlen(word);

        if (col + word_len + 1 >= COLS) {
            row++;
            col = 0;
        }

        mvprintw(row, col, "%s ", word);
        strcat(reference_line, word);
        strcat(reference_line, " ");
        strcpy(printed_text[j], word);
        col += word_len + 1;
    }
    reference_line[strlen(reference_line) - 1] = '\0'; // Remove trailing space
    refresh();
}

// Calculates Words Per Minute
int wpm(char reference_text[max_word][max_length], char input[1210], double total_time) {
    int correct_word = 0;
    int i = 0;
    char input_copy[1210];
    strcpy(input_copy, input);

    char *token = strtok(input_copy, " ");
    while (token != NULL && i < total_words) {
        if (strcmp(token, reference_text[i]) == 0) {
            correct_word++;
        }
        i++;
        token = strtok(NULL, " ");
    }

    float minutes = total_time / 60.0;
    return (minutes > 0) ? (int)(correct_word / minutes) : 0;
}
