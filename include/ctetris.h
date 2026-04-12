#ifndef CTETRIS_INCLUDE_H
#define CTETRIS_INCLUDE_H

#include "model.h"
#include "controller.h"

//TODO: clean up after src finished, so only used functions available
#ifdef CTET_REMOVE_PREFIX
#   define State             ctet_State
#   define Size              ctet_Size
#   define board_t           ctet_board_t
#   define init_state        ctet_init_state
#   define new_state         ctet_new_state
#   define free_state        ctet_free_state
#   define update_state      ctet_update_state
#   define BOARD_AT          CTET_BOARD_AT

#   define Action            ctet_Action
#   define Result            ctet_Result
#   define MOVE_UP           CTET_MOVE_UP
#   define MOVE_DOWN         CTET_MOVE_DOWN
#   define MOVE_LEFT         CTET_MOVE_LEFT
#   define MOVE_RIGHT        CTET_MOVE_RIGHT
#   define STORE_TETRONIMO   CTET_STORE_TETRONIMO
#   define UNSTORE_TETRONIMO CTET_UNSTORE_TETRONIMO
#endif

#endif //CTETRIS_INCLUDE_H
