//TODO - is main the controller here? do you just have to implement the controller and view every time?
#include <ncurses.h> //documentation for learning: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define CTET_REMOVE_PREFIX
#include "ctetris.h"
#include "typedefs.h"

#define get_timespec_ms(timespec) ((u64)(timespec).tv_sec * 1000ULL + (u64)(timespec).tv_nsec / 1000000ULL)
#define diff_ms(now_ts, last_ts) get_timespec_ms(now_ts) - get_timespec_ms(last_ts)

#ifdef TEXTVIEW

void print_board(const State* state) {
    const Size size = state->size;
    board_t* board = state->board;
    char board_str[size.rows*size.cols+size.rows];
    int count = 0;
    for (int i=0; i<size.rows; i++) {
        for (int j=0; j<size.cols; j++) {
            unsigned char bval = BOARD_AT(state, i, j);
            if (bval <= '9') board_str[count] = '0' + BOARD_AT(state, i, j);
            else board_str[count] = BOARD_AT(state, i, j);
            ++count;
        }
        board_str[count++] = '\n';
    }
    board_str[count-1] = '\0';
    printf("%s\n", board_str);
    printf("\033[%dA\r", size.rows); //Go up and left to start of board
}

void setup_keypress_reading(struct termios* og_term, struct termios* ctet_term) {
    tcgetattr(STDIN_FILENO, og_term);
    ctet_term = og_term;
    ctet_term->c_lflag &= ~(ECHO | ICANON);
    ctet_term->c_cc[VMIN] = 0;
    ctet_term->c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, ctet_term);
}

int main() {
    struct termios og_term, ctet_term;
    setup_keypress_reading(&og_term, &ctet_term);

    State* state = new_state((Size){10, 10});
    char key_ch = 0;
    bool ctet_running = true;
    unsigned char bspot[2] = {0};
    BOARD_AT(state, 0, 0) = '@';
    print_board(state);

    struct timespec last_ts, now_ts;
    clock_gettime(CLOCK_MONOTONIC, &last_ts);

    while (ctet_running) {
        clock_gettime(CLOCK_MONOTONIC, &now_ts);
        u64 delta_milliseconds = diff_ms(now_ts, last_ts);

        read(STDIN_FILENO, &key_ch, 1);
        switch (key_ch) {
            case 'q':
                ctet_running = false;
                break;

            case 'j':
                if (bspot[0] < 9) {
                    BOARD_AT(state, bspot[0], bspot[1]) = 1;
                    ++bspot[0];
                    BOARD_AT(state, bspot[0], bspot[1]) = 'X';
                    print_board(state);
                }
                break;

            case 'h':
                if (bspot[1] > 0) {
                    BOARD_AT(state, bspot[0], bspot[1]) = 1;
                    --bspot[1];
                    BOARD_AT(state, bspot[0], bspot[1]) = 'X';
                    print_board(state);
                }
                break;

            case 'k':
                if (bspot[0] > 0) {
                    BOARD_AT(state, bspot[0], bspot[1]) = 1;
                    --bspot[0];
                    BOARD_AT(state, bspot[0], bspot[1]) = 'X';
                    print_board(state);
                }
                break;

            case 'l':
                if (bspot[1] < 9) {
                    BOARD_AT(state, bspot[0], bspot[1]) = 1;
                    ++bspot[1];
                    BOARD_AT(state, bspot[0], bspot[1]) = 'X';
                    print_board(state);
                }
                break;
        }
        if (delta_milliseconds >= 1000) {
            clock_gettime(CLOCK_MONOTONIC, &last_ts);
        }

        //TODO: Does this actually rest the program, are there better rests to use, is this fine to use
        //for the current use case? Like, is it fine to keep the program running like this if it's not
        //actually like an energy saving sleep?
        key_ch = 0;
        nanosleep(&(struct timespec){.tv_sec=0, .tv_nsec=25000000}, NULL); //about 34-35 frames per second
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_term);
    free_state(state);

    return 0;
}

#else //ncurses
int main() {
    initscr();
    noecho(); //don't print user input to screen when typed
    keypad(stdscr, true); //include keypad in getch as val can get

    addch(ACS_UARROW | A_UNDERLINE | A_BOLD);
     mvprintw(3, 20, "First I'm here...");
     refresh(); //draw to screen
     getch(); //get character pressed as ascii val, pause screen here
     mvprintw(7, 5, "Now I'm here!");
     refresh();
    // getch();

    WINDOW* color_example = newwin(15, 10, 3, 10); //w-15, h-10, at y-10 x-10
    //box(color_example, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h');
    wborder(color_example, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h');
    mvwprintw(color_example, 0, 1, "test"); //commands are layered too, so place text over box to show, or box over text to replace

    refresh(); //seems to layer refreshes, so last refreshed is on top
    //wrefresh(color_example);

    int y, x;
    move(3, 3);
    x = getcurx(stdscr), y = getcury(stdscr);

    size_t row, col;
    getmaxyx(stdscr, row, col);
    char cmsg[] = "Center of stdscr window";
    mvprintw(row/2, (col-strlen(cmsg))/2, cmsg);

    char a = getch();
    endwin();

    printf("getyx: %d, %d, %c:%d\n", x, y, a, (unsigned int)a);

    return 0;
}
#endif
