//TODO - is main the controller here? do you just have to implement the controller and view every time?
#include <ncurses.h> //documentation for learning: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "typedefs.h"

#define get_timespec_ms(timespec) ((u64)(timespec).tv_sec * 1000ULL + (u64)(timespec).tv_nsec / 1000000ULL)
#define milliseconds_delta(now_ts, last_ts) get_timespec_ms(now_ts) - get_timespec_ms(last_ts)

#define CTET_REMOVE_PREFIX
#include "mvc/model.h"
#include "mvc/view.h"
#undef CTET_REMOVE_PREFIX

int main() {
#ifdef TEXTVIEW
    State* state = new_state((Size){10, 10});

    struct timespec last_ts, now_ts;
    clock_gettime(CLOCK_MONOTONIC, &last_ts);
    int count=0;
    //Arbitrary, while for use in testing
    while (count != 10) {
        clock_gettime(CLOCK_MONOTONIC, &now_ts);
        u64 delta_milliseconds = milliseconds_delta(now_ts, last_ts);

        //probably move down tetrino
        if (delta_milliseconds >= 1000) {
            print_board(state);
            ++count;
            BOARD_AT(state->board, state->size, 0, count % 10) = 1;
            clock_gettime(CLOCK_MONOTONIC, &last_ts);
        }

        nanosleep(&(struct timespec){.tv_sec=0, .tv_nsec=25000000}, NULL); //about 34-35 frames per second
    }
    free_state(state);
#else
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
#endif

    return 0;
}
