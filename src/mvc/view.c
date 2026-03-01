#include <stdbool.h>
#include <stdio.h>

#define CTET_REMOVE_PREFIX
#include "model.h"
#include "view.h"
#undef CTET_REMOVE_PREFIX

//TODO - make board a 2d array type pointer
void print_board(const State* state) {
    const Size size = state->size;
    board_t* board = state->board;
    char board_str[size.rows*size.cols+size.rows];
    int count = 0;
    for (int i=0; i<size.rows; i++) {
        for (int j=0; j<size.cols; j++) {
            board_str[count] = '0' + BOARD_AT(board, size, i, j);
            ++count;
        }
        board_str[count++] = '\n';
    }
    board_str[count-1] = '\0';
    printf("%s\n", board_str);
    printf("\033[%dA\r", size.rows); //Go up and left to start of board
}
