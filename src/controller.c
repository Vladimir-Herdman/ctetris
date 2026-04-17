#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "controller.h"
#include "model.h"

typedef struct {
    ctet_board_t tet[16];
} TetBoard;

//only used for testing
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
    0,2,0,0,
    0,2,0,0,
    0,2,0,0,
    2,2,0,0,
};
static ctet_board_t tet_sqr[TET_SIZE] = {
    0,0,0,0,
    0,0,0,0,
    3,3,0,0,
    3,3,0,0,
};
static ctet_board_t tet_beam[TET_SIZE] = {
    4,0,0,0,
    4,0,0,0,
    4,0,0,0,
    4,0,0,0,
};
static ctet_board_t tet_cross[TET_SIZE] = {
    0,0,0,0,
    0,0,0,0,
    0,5,0,0,
    5,5,5,0,
};
static ctet_board_t tet_dr[TET_SIZE] = {
    0,0,0,0,
    0,0,0,0,
    0,6,6,0,
    6,6,0,0,
};
static ctet_board_t tet_dl[TET_SIZE] = {
    0,0,0,0,
    0,0,0,0,
    7,7,0,0,
    0,7,7,0,
};
static ctet_board_t* tetronimo_baselist[7] = {tet_lr, tet_ll, tet_sqr, tet_beam, tet_cross, tet_dr, tet_dl};

static void draw_cur_tet_on_board(ctet_State* s) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            const ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
            if (curval == 0) continue;

            CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j) = curval;
        }
    }
}

//TODO - finish this method
static void clear_full_rows(ctet_State* s) {
    int score_gained=0, count;
    for (int i=0; i<s->size.rows; i++) {
        if (CTET_BOARD_AT(s, i, 0) == 0) continue;
        count = 0;
        for (int j=0; j<s->size.cols; j++)
            if (CTET_BOARD_AT(s, i, j) != 0) ++count;

        //TODO - Currently, after a single row is cleaned, it moves everything above
        //it down. Would probably be more efficient if you could figure out a way to
        //remove all lines that are full in a block, and then memcpy or something.
        //So, a block memcpy based off the number of lines in a row, so not 1 at a
        //time 4 times in a row for a big clear.
        if (count != s->size.cols) continue;
        while (--i > 0)
            memcpy(&CTET_BOARD_AT(s, i+1, 0), &CTET_BOARD_AT(s, i, 0), s->size.cols);
        ++score_gained;
    }
    s->score += score_gained; //TODO - remove the +1, just a temp for now until actual calculation.
}

//compare passed tet to board from s->curpos, and determine if it would
//conflict at all (ignore cur_tet nonzero values for check).
//  true -> no conflict.
static bool valid_on_board(const ctet_State* s, const ctet_board_t* tet) {
    for (int j=0; j<4; j++) { //get bottom '1' in each col, and check if any conflict in board below it.
        for (int i=3; i>=0; i--) {
            const ctet_board_t tet_val = CTET_TET_AT(tet, i, j);
            if (tet_val == 0) continue;

            const ctet_board_t cur_tet_val = CTET_TET_AT(s->cur_tet, i, j);
            const ctet_board_t bval = CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (bval != 0 && cur_tet_val == 0) return false;
        }
    }
    return true;
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
                    const ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
                    if (curval == 0) continue;

                    const ctet_board_t bval = CTET_BOARD_AT(s, s->cur_pos.rows+i+1, s->cur_pos.cols+j);
                    if (bval != 0) return false;
                    break;
                }
            }
            break;

        case CTET_MOVE_LEFT:
            if (s->cur_pos.cols-1 < 0) return false;
            for (int i=0; i<4; i++) { //get left '1' in each row, and check if any conflict in board left of it.
                for (int j=0; j<4; j++) {
                    const ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
                    if (curval == 0) continue;

                    const ctet_board_t bval = CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j-1);
                    if (bval != 0) return false;
                    break;
                }
            }
            break;

        //This one's a lil weird: no matter what cur_tet is, it's always rotated
        //and shifted such that it's in the bottom left of the cur_tet array.
        //Because of this, to do 'checkright' here, we'll have to get the righternmost
        //column of cur_tet that has a 1 in it, so then we can see if it'll go out of bounds.
        case CTET_MOVE_RIGHT:
            int srightcol = s->size.cols; //should always have a tet inside cur_tet, but just in case, assign over so next check fails.
            for (int j=3; j>=0; j--) {
                for (int i=0; i<4; i++) {
                    if (CTET_TET_AT(s->cur_tet, i, j) == 0) continue;
                    srightcol = s->cur_pos.cols+j; //got the righernmost col, so now start the actual check
                    goto rightcheck;
                }
            }
            
            rightcheck:
            if (srightcol+1 >= s->size.cols) return false;
            for (int i=0; i<4; i++) { //get right '1' in each row, and check if any conflict in board right of it.
                for (int j=3; j>=0; j--) {
                    const ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
                    if (curval == 0) continue;

                    const ctet_board_t bval = CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j+1);
                    if (bval != 0) return false;
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
            ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval != 0) *pbval = 0;
        }
    }
}

static void move_down(ctet_State* s) {
    s->cur_pos.rows += 1;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval != 0) *pbval = curval;
        }
    }
}
static void move_left(ctet_State* s) {
    s->cur_pos.cols -= 1;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval != 0) *pbval = curval;
        }
    }
}
static void move_right(ctet_State* s) {
    s->cur_pos.cols += 1;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
            ctet_board_t* pbval = &CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval != 0) *pbval = curval;
        }
    }
}

//inplace
static void transpose_tet(ctet_board_t* tet) {
    for (int i=1; i<4; i++) {
        for (int j=0; j<i; j++) {
            const ctet_board_t temp = CTET_TET_AT(tet, i, j);
            CTET_TET_AT(tet, i, j) = CTET_TET_AT(tet, j, i);
            CTET_TET_AT(tet, j, i) = temp;
        }
    }
}

//Not the most beautiful code I've ever written, this rotate and shift stuff,
//but it certainly gets the job done. And, this stuff moves at lightspeed, get
//off me.
static void rotate_tet_left(ctet_board_t* tet) {
    //reverse each row (for going left rotation)
    transpose_tet(tet);
    for (int i=0; i<4; i++) {
        for (int j=0; j<2; j++) {
            const ctet_board_t temp = CTET_TET_AT(tet, i, j); //                             '  j  3-j  '
            CTET_TET_AT(tet, i, j) = CTET_TET_AT(tet, i, 3-j); //3-j to get other side of row, so '0 1  0   0'
            CTET_TET_AT(tet, i, 3-j) = temp;
        }
    }

    //get shift left and down values
    //  left is from 0 to 3, so 0 means no shift needed
    //  down is from 3 to 0, so 3 means no shift needed (because going up tet, so '3' means already at bottom)
    int shift_left = -1, shift_down = -1;
    for (int j=0; j<4 && shift_left<0; j++)
        for (int i=0; i<4; i++) 
            if (CTET_TET_AT(tet, i, j) != 0) shift_left = j;
    for (int i=3; i>=0 && shift_down<0; i--) 
        for (int j=0; j<4; j++) 
            if (CTET_TET_AT(tet, i, j) != 0) shift_down = i;

    //shift left and down
    for (int i=0; i<4 && shift_left>0; i++)
        for (int j=shift_left; j<4; j++)
            CTET_TET_AT(tet, i, j-shift_left) = CTET_TET_AT(tet, i, j);
    for (int i=shift_down; i>=0 && shift_down<3; i--)
        for (int j=0; j<4; j++) 
            CTET_TET_AT(tet, i+(3-shift_down), j) = CTET_TET_AT(tet, i, j);

    //set shifts from end to 0s in array (so no old values left). I've found
    //this section alone works for the left rotation, but not for the right.
    //It's a large reason why I'm sticking with just left rotations, I was
    //having trouble with fixing the right.
    for (int i=0; i<(3-shift_down); i++) memset(&CTET_TET_AT(tet, i, 0), 0, 4);
}

static void store_tet(ctet_State* s) {
    return;
}
static void unstore_tet(ctet_State* s) {
    return;
}

//This'll reset everything that needs to be reset for a new piece at the top.
//It'll also be the check for if the game's over, as if there's already a piece
//where we're trying to reset to, you've had too many pieces at the top, and game over.
static void next_tet(ctet_State* s); //forward declare so next tet in state can be assigned.
static ctet_Result tetronimo_placed_reset(ctet_State* s) {
    next_tet(s);
    s->cur_pos = (ctet_Size){.rows=-1, .cols=s->size.cols/2};
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            ctet_board_t curval = CTET_TET_AT(s->cur_tet, i, j);
            ctet_board_t bval = CTET_BOARD_AT(s, s->cur_pos.rows+i, s->cur_pos.cols+j);
            if (curval != 0 && bval != 0) {
                s->gamerunning = false;
                return CTET_GAME_ENDED;
            }
        }
    }
    move_down(s);
    return CTET_PLACED_TETRONIMO;
}

ctet_Result ctet_update_state(ctet_State* s, const ctet_Action action) {
    ctet_Result result = CTET_DO_NOTHING;
    TetBoard tb;
    switch (action) {
        case 'j':
        case CTET_MOVE_DOWN:
            if (!move_allowed(s, CTET_MOVE_DOWN)){ //if you tried to go down and hit a wall, it's been placed there. Game end if can't place a new piece at top.
                result = tetronimo_placed_reset(s);
                break;
            }
            clear_old_tet_loc(s);
            move_down(s);
            result = CTET_MOVED_TETRONIMO;
            break;

        case 'h':
        case CTET_MOVE_LEFT:
            if (!move_allowed(s, CTET_MOVE_LEFT)) {
                result = CTET_MOVE_NOTALLOWED;
                break;
            }
            clear_old_tet_loc(s);
            move_left(s);
            result = CTET_MOVED_TETRONIMO;
            break;

        case 'l':
        case CTET_MOVE_RIGHT:
            if (!move_allowed(s, CTET_MOVE_RIGHT)) {
                result = CTET_MOVE_NOTALLOWED;
                break;
            }
            clear_old_tet_loc(s);
            move_right(s);
            result = CTET_MOVED_TETRONIMO;
            break;

        case CTET_ROTATE_LEFT:
        case 'f':
            memcpy(tb.tet, s->cur_tet, TET_SIZE);
            rotate_tet_left(tb.tet);
            if (valid_on_board(s, tb.tet)) {
                clear_old_tet_loc(s);
                memcpy(s->cur_tet, tb.tet, TET_SIZE);
                draw_cur_tet_on_board(s);
                result = CTET_MOVED_TETRONIMO;
            }
            break;

        case 'g':
        case CTET_ROTATE_RIGHT:
            //I'm annoyed with some right rotation issues, so you get 3 left turns.
            memcpy(tb.tet, s->cur_tet, TET_SIZE);
            for (int i=0; i<3; i++) rotate_tet_left(tb.tet);
            if (valid_on_board(s, tb.tet)) {
                clear_old_tet_loc(s);
                memcpy(s->cur_tet, tb.tet, TET_SIZE);
                draw_cur_tet_on_board(s);
                result = CTET_MOVED_TETRONIMO;
            }
            break;

        case ' ':
        case CTET_INSTANT_DOWN:
            while ((result = ctet_update_state(s, CTET_MOVE_DOWN)) == CTET_MOVED_TETRONIMO);;
            if (result == CTET_PLACED_TETRONIMO) return result;
            break;

        case CTET_STORE_TETRONIMO:
            store_tet(s);
            break;

        case CTET_UNSTORE_TETRONIMO:
            unstore_tet(s);
            break;

        case CTET_END_GAME:
            s->gamerunning = false;
            result = CTET_GAME_ENDED;
            break;

        case 0:
            break;
    }
    if (result == CTET_PLACED_TETRONIMO) {
        clear_old_tet_loc(s);
        clear_full_rows(s);
        draw_cur_tet_on_board(s);
    }
    return result;
}

static void init_nexttets_list(ctet_State* s) {
    int random = rand() % 7;
    int last = -1;
    for (int i=0; i<3; i++) {
        if (random == last) random = rand() % 7; //only rerun once if duplicate to last piece
        memcpy(s->next_tets[i], tetronimo_baselist[random], TET_SIZE);
        last = random;
        random = rand() % 7;
    }
}

static void next_tet(ctet_State* s) {
    //use it like a queue with the next if statement.
    memcpy(s->cur_tet, s->next_tets[0], TET_SIZE);
    memcpy(s->next_tets[0], s->next_tets[1], TET_SIZE);
    memcpy(s->next_tets[1], s->next_tets[2], TET_SIZE);
    memcpy(s->next_tets[2], tetronimo_baselist[rand() % 7], TET_SIZE);
    init_nexttets_list(s);
}

void ctet_init_state(ctet_State* s, const ctet_Size size) {
    static bool srand_called = false;
    if (srand_called == false) {
        srand_called = true;
        srand(time(NULL));
    }
    s->board = calloc(size.rows * size.cols, sizeof(ctet_board_t));
    s->size = size; //TODO: add 4 to size.rows (don't print this top bit) to account for going off screen when getting higher.
    s->cur_pos = (ctet_Size){.rows=-1, .cols=size.cols/2};
    s->malloced = false;
    s->gamerunning = true;
    s->gravity = 1;
    s->score = 0;
    s->level = 0;
    init_nexttets_list(s);
    next_tet(s);
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
