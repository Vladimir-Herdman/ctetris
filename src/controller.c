#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controller.h"
#include "model.h"

#define TET_AT(tet, r, c) (tet)[4*(r)+(c)]

//REMOVE - only used for testing
void printfdebug(const char* fmt, ...) {
    printf("\0337"); //save cursor pos

    //go to debug area (bottom of terminal), clear last, color and print msg
    printf("\033[1A\033[K\033[38;2;255;0;0m");
    char msg[128];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    printf("%s", msg);
    printf("\0338"); //return to og position
    fflush(stdout);
}

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
        case CTET_MOVE_DOWN:
            if (s->cur_pos.rows+4 >= s->size.rows) return false;
            for (int j=0; j<4; j++) { //get bottom '1' in each col, and check if any conflict in board below it.
                for (int i=3; i>=0; i--) {
                    const ctet_board_t curval = TET_AT(s->cur_tet, i, j);
                    if (curval == 0) continue;

                    const ctet_board_t bval = CTET_BOARD_AT(s, s->cur_pos.rows+i+1, s->cur_pos.cols+j);
                    if (bval == 1) return false;
                    break;
                }
            }
            break;

        case CTET_MOVE_LEFT:
            if (s->cur_pos.cols-1 < 0) return false;
            for (int i=0; i<4; i++) { //get left '1' in each row, and check if any conflict in board left of it.
                for (int j=0; j<4; j++) {
                    const ctet_board_t curval = TET_AT(s->cur_tet, i, j);
                    if (curval == 0) continue;

                    const ctet_board_t bval = CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j-1);
                    if (bval == 1) return false;
                    break;
                }
            }
            break;

        //This one's a lil weird: no matter what cur_tet is, it's always rotated
        //and shifted such that it's in the bottom left of the cur_tet array.
        //Because of this, to do 'checkright' here, we'll have to get the righternmost
        //column of cur_tet that has a 1 in it, so then we can see if it'll go out of bounds.
        case CTET_MOVE_RIGHT:
            int srightcol;
            for (int j=3; j>=0; j--) {
                for (int i=0; i<4; i++) {
                    if (TET_AT(s->cur_tet, i, j) == 0) continue;
                    srightcol = s->cur_pos.cols+j; //got the righernmost col, so now start the actual check
                    goto rightcheck;
                }
            }
            
            rightcheck:
            if (srightcol+1 >= s->size.cols) return false;
            for (int i=0; i<4; i++) { //get right '1' in each row, and check if any conflict in board right of it.
                for (int j=3; j>=0; j--) {
                    const ctet_board_t curval = TET_AT(s->cur_tet, i, j);
                    if (curval == 0) continue;

                    const ctet_board_t bval = CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j+1);
                    if (bval == 1) return false;
                    break;
                }
            }
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

static void move_down(ctet_State* s) {
    s->cur_pos.rows += 1;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval == 1) *pbval = curval;
        }
    }
}
static void move_left(ctet_State* s) {
    s->cur_pos.cols -= 1;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval == 1) *pbval = curval;
        }
    }
}
static void move_right(ctet_State* s) {
    s->cur_pos.cols += 1;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval == 1) *pbval = curval;
        }
    }
}
static void store_tet(ctet_State* s) {
    return;
}
static void unstore_tet(ctet_State* s) {
    return;
}

int tet_count = 0; //TODO: make actual code here for choosing the tet from the next list
//This'll reset everything that needs to be reset for a new piece at the top.
//It'll also be the check for if the game's over, as if there's already a piece
//where we're trying to reset to, you've had too many pieces at the top, and game over.
static ctet_Result tetronimo_placed_reset(ctet_State* s) {
    memcpy(s->cur_tet, tetronimo_baselist[++tet_count%7], 16);
    s->cur_pos = (ctet_Size){.rows=-1, .cols=s->size.cols/2};
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = TET_AT(s->cur_tet, i, j);
            ctet_board_t pbval = CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval == 1 && pbval == 1) {
                s->gamerunning = false;
                return CTET_END_GAME;
            }
        }
    }
    move_down(s);
    return CTET_PLACED_TETRONIMO;
}

ctet_Result ctet_update_state(ctet_State* s, const ctet_Action action) {
    switch (action) {
        case 'j':
        case CTET_MOVE_DOWN:
            if (!move_allowed(s, CTET_MOVE_DOWN)){ //if you tried to go down and hit a wall, it's been placed there. Game end if can't place a new piece at top.
                return tetronimo_placed_reset(s);
            }
            clear_old_tet_loc(s);
            move_down(s);
            return CTET_MOVED_TETRONIMO;

        case 'h':
        case CTET_MOVE_LEFT:
            if (!move_allowed(s, CTET_MOVE_LEFT)) return CTET_MOVE_NOTALLOWED;
            clear_old_tet_loc(s);
            move_left(s);
            return CTET_MOVED_TETRONIMO;

        case 'l':
        case CTET_MOVE_RIGHT:
            if (!move_allowed(s, CTET_MOVE_RIGHT)) return CTET_MOVE_NOTALLOWED;
            clear_old_tet_loc(s);
            move_right(s);
            return CTET_MOVED_TETRONIMO;

        case CTET_STORE_TETRONIMO:
            store_tet(s);
            break;

        case CTET_UNSTORE_TETRONIMO:
            unstore_tet(s);
            break;

        case 0:
            break;
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
    s->malloced = false;
    s->gamerunning = true;
    s->size = size;
    s->gravity = 1;
    s->score = 0;
    move_down(s); //set's first tet on board at top
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
