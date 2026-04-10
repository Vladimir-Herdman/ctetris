#include <stdlib.h>
#include <string.h>

#include "controller.h"
#include "model.h"

#define TETRONIMO_AT(tet, r, c) (tet->tetronimo)[4*(r)+(c)]

void ctet_init_state(ctet_State* state, const ctet_Size size) {
    state->board = calloc(size.rows * size.cols, sizeof(ctet_board_t));
    memset(state->uncommitted_actions, 0, MAX_UNC_ACTIONS*sizeof(state->uncommitted_actions[0]));
    state->malloced = false;
    state->size = size;
    state->gravity = 1;
    state->score = 0;
    state->action_count = 0;
}

ctet_State* ctet_new_state(const ctet_Size size) {
    ctet_State* state = malloc(sizeof(ctet_State));
    ctet_init_state(state, size);
    state->malloced = true;
    return state;
}

void ctet_free_state(ctet_State* state) {
    free(state->board);
    if (state->malloced) free(state);
}

//TODO - what are all the base pieces?
static ctet_board_t tet_lr[4][4] = {
    {1,0,0,0},
    {1,0,0,0},
    {1,0,0,0},
    {1,1,0,0}
};
static ctet_board_t tet_ll[4][4] = {
    {0,1,0,0},
    {0,1,0,0},
    {0,1,0,0},
    {1,1,0,0}
};
static ctet_board_t tet_sqr[4][4] = {
    {0,0,0,0},
    {0,0,0,0},
    {1,1,0,0},
    {1,1,0,0}
};
static ctet_board_t tet_beam[4][4] = {
    {1,0,0,0},
    {1,0,0,0},
    {1,0,0,0},
    {1,0,0,0}
};
static ctet_board_t tet_cross[4][4] = {
    {0,0,0,0},
    {0,0,0,0},
    {0,1,0,0},
    {1,1,1,0}
};
static ctet_board_t tet_dr[4][4] = {
    {0,0,0,0},
    {0,0,0,0},
    {0,1,1,0},
    {1,1,0,0}
};
static ctet_board_t tet_dl[4][4] = {
    {0,0,0,0},
    {0,0,0,0},
    {1,1,0,0},
    {0,1,1,0}
};
//Stores first row, so use like TETRONIMO_AT(tet_lr, 3, 0) for tet_lr[3][0]
static ctet_board_t* tetronimo_baselist[7] = {
    tet_lr[0], tet_ll[0], tet_sqr[0], tet_beam[0],
    tet_cross[0], tet_dr[0], tet_dl[0],
};

//TODO - finishe this method
static int clear_full_rows(ctet_board_t* board, const ctet_Size size) {
    return -1;
}

//TODO - make switch statements do something
void ctet_update_state(ctet_State* state) {
    for (int i=0; i<state->action_count; i++) {
        switch (state->uncommitted_actions[i]) {
            case CTET_MOVE_UP:
                break;
            case CTET_MOVE_DOWN:
                break;
            case CTET_MOVE_LEFT:
                break;
            case CTET_MOVE_RIGHT:
                break;
            case CTET_STORE_TETRONIMO:
                break;
            case CTET_UNSTORE_TETRONIMO:
                break;
        }
        state->uncommitted_actions[i] = 0;
    }
}

void ctet_action(ctet_State* state, ctet_Action action) {
    if (state->action_count > (int)sizeof(state->uncommitted_actions)) return;
    state->uncommitted_actions[state->action_count++] = action;
}
