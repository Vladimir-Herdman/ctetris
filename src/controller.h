#ifndef CTETRIS_MVC_CONTROLLER_H
#define CTETRIS_MVC_CONTROLLER_H

#include "model.h"

#define CTET_GRID_AT(board, size, r, c) (board)[(size).cols*(r)+(c)]
#define CTET_BOARD_AT(state, r, c) (state)->board[(state)->size.cols*(r)+(c)]
#define CTET_TET_AT(tet, r, c) (tet)[4*(r)+(c)]

void ctet_init_state(ctet_State* state, const ctet_Size size);
ctet_State* ctet_new_state(const ctet_Size size);
void ctet_free_state(ctet_State* state);

//perform action on state (also accepts char, so 'hjkl' or arrows for movement).
//if update made, return true
//Acceptable characters and result:
//  hjkl                    -> vim movements same as CTET_MOVE_<direction>
//  <up><down><left><right> -> arrow keys for CTET_MOVE_<direction>
//TODO: Set some keys for relevant options later on
ctet_Result ctet_update_state(ctet_State* state, ctet_Action action);
int ctet_update_time(const ctet_State* state);

void printfdebug(const char* fmt, ...);

#endif //CTETRIS_MVC_CONTROLLER_H
