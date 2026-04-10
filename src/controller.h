#ifndef CTETRIS_MVC_CONTROLLER_H
#define CTETRIS_MVC_CONTROLLER_H

#include "model.h"

#define CTET_BOARD_AT(state, r, c) (state)->board[(state)->size.cols*(r)+(c)]

void ctet_init_state(ctet_State* state, const ctet_Size size);
ctet_State* ctet_new_state(const ctet_Size size);
void ctet_free_state(ctet_State* state);

//Commit actions currently uncommitted, update board and score
void ctet_update_state(ctet_State* state);

//Add action to action list
void ctet_action(ctet_State* state, ctet_Action action);


#endif //CTETRIS_MVC_CONTROLLER_H
