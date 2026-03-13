#ifndef CTETRIS_MODEL_H
#define CTETRIS_MODEL_H

#include <stdbool.h>

#define CTET_BOARD_AT(board, size, r, c) (board)[(size).cols*(r)+(c)]

typedef enum ctet_Action {
    CTET_MOVE_UP=1,
    CTET_MOVE_DOWN,
    CTET_MOVE_LEFT,
    CTET_MOVE_RIGHT,
    CTET_STORE_TETRONIMO,
    CTET_UNSTORE_TETRONIMO,
} ctet_Action;

typedef struct ctet_Size ctet_Size;
struct ctet_Size {
    int rows;
    int cols;
};

typedef int ctet_board_t;
typedef struct ctet_State {
    ctet_Action uncommitted_actions[32];
    ctet_Size size;
    int action_count;
    bool malloced;
    ctet_board_t* board;
    int  gravity;
    int  score;
} ctet_State;

ctet_State* ctet_new_state(const ctet_Size size);
void ctet_init_state(ctet_State* state, const ctet_Size size);

//Call on both stack initialized and malloced states, as board array is
//malloced to be fiven size.
void ctet_free_state(ctet_State* state);

//Commit actions currently uncommitted, update board and score
void ctet_update_state(ctet_State* state);

//Add action to action list
void ctet_action(ctet_State* state, ctet_Action action);

#endif //CTETRIS_MODEL_H
