#include "controller.h"
#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#ifdef TEXTVIEW
#   include <termios.h>
#   include <unistd.h>
#else
#   include <ncurses.h>
#endif

#define CTET_REMOVE_PREFIX
#include "ctetris.h"
#include "typedefs.h"

#define get_timespec_ms(timespec) ((u64)(timespec).tv_sec * 1000ULL + (u64)(timespec).tv_nsec / 1000000ULL)
#define diff_ms(now_ts, last_ts) get_timespec_ms(now_ts) - get_timespec_ms(last_ts)

#ifdef TEXTVIEW
void print_board(const board_t* board, const Size size) {
    for (int i=0; i<size.rows; i++) {
        for (int j=0; j<size.cols; j++) {
            unsigned char val = CTET_GRID_AT(board, size, i, j);
            switch (val) {
                case 0: printf("\033[38;2;0;0;0m");       break;
                case 1: printf("\033[48;2;255;128;0m");   break;
                case 2: printf("\033[48;2;0;0;255m");     break;
                case 3: printf("\033[48;2;255;255;0m");   break;
                case 4: printf("\033[48;2;0;255;255m");   break;
                case 5: printf("\033[48;2;153;0;204m");   break;
                case 6: printf("\033[48;2;0;255;0m");     break;
                case 7: printf("\033[48;2;255;0;0m");     break;
            }
            fputs("▓▓\033[0m", stdout);
        }
        printf("\033[1B\033[%dD", size.cols*2);
    }
}

void print_state(State* s) {
    //border, if not already printed
    static bool outline = true;
    fputs("\033[38;2;200;200;200m", stdout);
    for (int i=0; i<s->size.rows+1 && outline; i++) {
        for (int j=0; j<s->size.cols+2; j++) {
            fputs("▓▓", stdout);
        }
        putchar('\n');
    }
    if (outline) {
        outline = false;
        printf("\033[%dA\033[2C", s->size.rows+1);
    }

    //board itself
    print_board(s->board, s->size);
    printf("\033[%dA", s->size.rows); //Go up to start of print board

    //additional state info: level, score, next 3 tets, and store box
    //TODO - complete store and unstore functionality
    char buf[32] = {0};
    fputs("\0337", stdout); //save current cursor pos
    printf("\033[%dC", (s->size.cols+2)*2);

    sprintf(buf, "level: %d", s->level);
    printf("%s", buf);
    printf("\033[%zuD\033[1B", strlen(buf));

    sprintf(buf, "score: %d", s->score);
    printf("%s", buf);
    printf("\033[%zuD\033[2B", strlen(buf));

    fputs("Next 3 tets:", stdout);
    fputs("\033[12D\033[1B", stdout);

    for (int i=0; i<3; i++) {
        print_board(s->next_tets[i], (Size){4, 4});
        fputs("\033[4A\033[10C", stdout);
    }

    fputs("\033[5B\033[30DStored tet:\033[1B\033[11D", stdout);
    print_board(s->stored_tet, (Size){4, 4});

    fflush(stdout);
    fputs("\0338", stdout); //return to saved cursor pos
}

void setup_keypress_reading(struct termios* og_term, struct termios* ctet_term) {
    tcgetattr(STDIN_FILENO, og_term);
    ctet_term = og_term;
    ctet_term->c_lflag &= ~(ECHO | ICANON);
    ctet_term->c_cc[VMIN] = 0;
    ctet_term->c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, ctet_term);
}

struct termios og_term, ctet_term;
State* state;
void cleanup(const int sig) {
    printf("\033[%dB", state->size.rows); //move below screen so don't erase board on game over
    puts("\033[?25h"); //make cursor visible
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_term); //terminal back to original settings
    if (state) free(state);
    exit(sig);
}

//TODO - Print final results as a "GOOD JOB" msg, and cleanup (leave)
void game_ended() {
    cleanup(0);
}


int main() {
    signal(SIGINT, cleanup); //so if ctrl-c, still clean up and end program properly

    fputs("\033[?25l", stdout); //this makes the cursor invisible. Not supported on all systems, but most terminals provide it.
    setup_keypress_reading(&og_term, &ctet_term);

    state = new_state((Size){20, 10});
    char key_ch = 0;
    puts(""); //line above tetris game for debug printing
    print_state(state);

    struct timespec last_ts, now_ts;
    clock_gettime(CLOCK_MONOTONIC, &last_ts);

    while (state->gamerunning) {
        clock_gettime(CLOCK_MONOTONIC, &now_ts);
        u64 delta_milliseconds = diff_ms(now_ts, last_ts);
        Result update = CTET_DO_NOTHING;

        read(STDIN_FILENO, &key_ch, 1);
        if (key_ch != 0) {
            update = update_state(state, key_ch);
            if (update == CTET_GAME_ENDED) {
                printfdebug("val:%c_r%dc%d", BOARD_AT(state, state->cur_pos.rows, state->cur_pos.cols), state->cur_pos.rows, state->cur_pos.cols);
                print_state(state);
                game_ended();
            }
            if (update == CTET_PLACED_TETRONIMO) clock_gettime(CLOCK_MONOTONIC, &last_ts);
            if (key_ch == 'q') game_ended();
            print_state(state);
        }

        //for stack overflow timing reference?
        //https://gamedev.stackexchange.com/questions/159835/understanding-tetris-speed-curve
        if (delta_milliseconds >= ctet_update_time(state)) {
            update = update_state(state, MOVE_DOWN);
            if (update == CTET_GAME_ENDED) game_ended();
            else print_state(state);

            clock_gettime(CLOCK_MONOTONIC, &last_ts);
        }

        key_ch = 0;
        nanosleep(&(struct timespec){.tv_sec=0, .tv_nsec=14000000}, NULL); //about 60 frames per second in my testing
    }

    game_ended();
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
