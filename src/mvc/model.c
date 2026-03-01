#include <stdlib.h>

#define CTET_REMOVE_PREFIX
#include "model.h"
#undef CTET_REMOVE_PREFIX

State* new_state(const Size size) {
    State* state = malloc(sizeof(State));
    state->board = calloc(size.rows * size.cols, sizeof(board_t));
    state->size = size;
    state->gravity = 1;
    state->score = 0;
    state->action_count = 0;
    return state;
}

void free_state(State* state) {
    free(state->board);
    free(state);
}

//Here's all the tetrinos as needed
//TODO - do I need to static a struct declaration?
typedef struct Tetrino Tetrino;
static struct Tetrino {
    Size size;
    board_t* tetrino;
};
//TODO - what are all the base pieces I need?
static board_t tet_l[4][2] = {
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 1}
};
static board_t tet_sqr[2][2] = {
    {1, 1},
    {1, 1}
};
static board_t tet_beam[4][1] = {
    {1},
    {1},
    {1},
    {1}
};
// static board_t tet_l[4][2] = {
//     {1, 0},
//     {1, 0},
//     {1, 0},
//     {1, 1}
// };

//TODO - make pointer go by 2d array style
static int clear_full_rows(board_t* board, const Size size) {
    return -1;
}

//TODO - make switch statements do something
void update_state(State* state) {
    for (int i=0; i<state->action_count; i++) {
        switch (state->uncommitted_actions[i]) {
            case MOVE_UP:
                break;
            case MOVE_DOWN:
                break;
            case MOVE_LEFT:
                break;
            case MOVE_RIGHT:
                break;
            case STORE_TETRINO:
                break;
            case UNSTORE_TETRINO:
                break;
        }
        state->uncommitted_actions[i] = 0;
    }
}

void action(State* state, Action action) {
    if (state->action_count > (int)sizeof(state->uncommitted_actions)) return;
    state->uncommitted_actions[state->action_count++] = action;
}
