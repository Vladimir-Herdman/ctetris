#include <stdlib.h>
#include <string.h>

#include "controller.h"
#include "model.h"

#define TET_AT(tet, r, c) (tet)[4*(r)+(c)]

static ctet_board_t tet_lr[TET_SIZE] = {
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,1,0,0,
};
static ctet_board_t tet_ll[TET_SIZE] = {
    0,1,0,0,
    0,1,0,0,
    0,1,0,0,
    1,1,0,0,
};
static ctet_board_t tet_sqr[TET_SIZE] = {
    0,0,0,0,
    0,0,0,0,
    1,1,0,0,
    1,1,0,0,
};
static ctet_board_t tet_beam[TET_SIZE] = {
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
};
static ctet_board_t tet_cross[TET_SIZE] = {
    0,0,0,0,
    0,0,0,0,
    0,1,0,0,
    1,1,1,0,
};
static ctet_board_t tet_dr[TET_SIZE] = {
    0,0,0,0,
    0,0,0,0,
    0,1,1,0,
    1,1,0,0,
};
static ctet_board_t tet_dl[TET_SIZE] = {
    0,0,0,0,
    0,0,0,0,
    1,1,0,0,
    0,1,1,0,
};
static ctet_board_t* tetronimo_baselist[7] = {tet_lr, tet_ll, tet_sqr, tet_beam, tet_cross, tet_dr, tet_dl};

//TODO - finishe this method
static int clear_full_rows(ctet_board_t* board, const ctet_Size size) {
    return -1;
}

//check looks at:
//  1) if moving direction would go off board.
//  2) if 2 rows closest move if would hit already placed piece.
static bool move_allowed(const ctet_State* s, const ctet_Action action) {
    switch (action) {
        case CTET_MOVE_UP:
            break;

        case CTET_MOVE_DOWN:
            if (s->cur_pos.rows+4 >= s->size.rows) return false;
            for (int i=2; i<4; i++) {
                for (int j=0; j<4; j++) {
                    const ctet_board_t pcurval = TET_AT(s->cur_tet, i, j);
                    const ctet_board_t pbval = CTET_BOARD_AT(s, s->cur_pos.rows+i+1, s->cur_pos.cols+j);
                    if (pcurval == 1 && pbval == 1) return false;
                }
            }
            break;

        case CTET_MOVE_LEFT:
            break;

        case CTET_MOVE_RIGHT:
            break;
    }
    return true;
}

static void clear_old_tet_loc(ctet_State* s) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval == 1 && *pbval == 1) *pbval = 0;
        }
    }
}

static bool move_up(ctet_State*s) {
    return CTET_PLACED_PIECE;
}
static bool move_down(ctet_State* s) {
    s->cur_pos.rows += 1;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval == 1) *pbval = curval;
        }
    }
    return CTET_PLACED_PIECE;
}
static bool move_left(ctet_State* s) {
    return CTET_PLACED_PIECE;
}
static bool move_right(ctet_State* s) {
    return CTET_PLACED_PIECE;
}
static bool store_tet(ctet_State* s) {
    return false;
}
static bool unstore_tet(ctet_State* s) {
    return false;
}

// switch (key_ch) {
//     case 'q':
//         ctet_running = false;
//         break;
//
//     case 'j':
//         if (bspot[0] < 9) {
//             BOARD_AT(state, bspot[0], bspot[1]) = 1;
//             ++bspot[0];
//             BOARD_AT(state, bspot[0], bspot[1]) = 'X';
//             print_board(state);
//         }
//         break;
//
//     case 'h':
//         if (bspot[1] > 0) {
//             BOARD_AT(state, bspot[0], bspot[1]) = 1;
//             --bspot[1];
//             BOARD_AT(state, bspot[0], bspot[1]) = 'X';
//             print_board(state);
//         }
//         break;
//
//     case 'k':
//         if (bspot[0] > 0) {
//             BOARD_AT(state, bspot[0], bspot[1]) = 1;
//             --bspot[0];
//             BOARD_AT(state, bspot[0], bspot[1]) = 'X';
//             print_board(state);
//         }
//         break;
//
//     case 'l':
//         if (bspot[1] < 9) {
//             BOARD_AT(state, bspot[0], bspot[1]) = 1;
//             ++bspot[1];
//             BOARD_AT(state, bspot[0], bspot[1]) = 'X';
//             print_board(state);
//         }
//         break;

ctet_Result ctet_update_state(ctet_State* s, const ctet_Action action) {
    switch (action) {
        case 'k':
        case CTET_MOVE_UP:
            if (!move_allowed(s, CTET_MOVE_UP)) return CTET_DO_NOTHING;
            clear_old_tet_loc(s);
            return move_up(s);

        case 'j':
        case CTET_MOVE_DOWN:
            if (!move_allowed(s, CTET_MOVE_DOWN)) return CTET_DO_NOTHING;
            clear_old_tet_loc(s);
            return move_down(s);

        case 'h':
        case CTET_MOVE_LEFT:
            if (!move_allowed(s, CTET_MOVE_LEFT)) return CTET_DO_NOTHING;
            clear_old_tet_loc(s);
            return move_left(s);

        case 'l':
        case CTET_MOVE_RIGHT:
            if (!move_allowed(s, CTET_MOVE_RIGHT)) return CTET_DO_NOTHING;
            clear_old_tet_loc(s);
            return move_right(s);

        case CTET_STORE_TETRONIMO:
            return store_tet(s);

        case CTET_UNSTORE_TETRONIMO:
            return unstore_tet(s);
    }
    return CTET_DO_NOTHING;
}

//TODO
static void init_cur_tet(ctet_board_t* tet) {
    memcpy(tet, tetronimo_baselist[0], TET_SIZE);
}

//TODO
static void init_nexttets_list(ctet_board_t* next_tets[NEXT_TET_LIST_SIZE]) {
    memset(next_tets, 0, sizeof(next_tets[0])*NEXT_TET_LIST_SIZE);
}

void ctet_init_state(ctet_State* s, const ctet_Size size) {
    s->board = calloc(size.rows * size.cols, sizeof(ctet_board_t));
    init_cur_tet(s->cur_tet);
    init_nexttets_list(s->next_tets);
    s->cur_pos = (ctet_Size){.rows=-1, .cols=size.cols/2};
    move_down(s);
    s->malloced = false;
    s->size = size;
    s->gravity = 1;
    s->score = 0;
    s->action_count = 0;
}

ctet_State* ctet_new_state(const ctet_Size size) {
    ctet_State* state = malloc(sizeof(ctet_State));
    ctet_init_state(state, size);
    state->malloced = true;
    return state;
}

void ctet_free_state(ctet_State* s) {
    free(s->board);
    if (s->malloced) free(s);
}

#undef TET_AT
