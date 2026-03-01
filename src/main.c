//TODO - is main the controller here? do you just have to implement the controller and view every time?
#include <ncurses.h> //documentation for learning: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define CTET_REMOVE_PREFIX
#include "mvc/model.h"
#include "mvc/view.h"
#undef CTET_REMOVE_PREFIX

int main() {
#ifdef TEXTVIEW
    State* state = new_state((Size){20, 10});
    Size size = state->size;
    board_t* board = state->board;
    char board_str[(size.rows*size.cols)+size.rows];
    int count = 0;
    for (int i=0; i<size.rows; i++) {
        for (int j=0; j<size.cols; j++) {
            board_str[count] = '0'+board[count];
            ++count;
        }
        board_str[count++] = '\n';
    }
    printf("%s", board_str);
    //printf("state with size: {cols:%d, rows:%d}\n", state->size.cols, state->size.rows);
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
