#include <stdio.h>

#define CTET_REMOVE_PREFIX
#include "view.h"
#undef CTET_REMOVE_PREFIX

//TODO - make board a 2d array type pointer
void print_board(const board_t* board, const Size size) {
    char board_str[size.rows*size.cols+size.rows];
    int count = 0;
    for (int i=0; i<size.rows; i++) {
        for (int j=0; j<size.cols; j++) {
            board_str[count] = board[count];
            ++count;
        }
        board_str[count++] = '\n';
    }
    printf("%s\n\r", board_str);
}
