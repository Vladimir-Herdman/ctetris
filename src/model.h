#ifndef CTETRIS_MODEL_H
#define CTETRIS_MODEL_H

#define MAX_UNC_ACTIONS 16

typedef enum ctet_Action {
    CTET_DO_NOTHING=0,
    CTET_MOVE_UP,
    CTET_MOVE_DOWN,
    CTET_MOVE_LEFT,
    CTET_MOVE_RIGHT,
    CTET_STORE_TETRONIMO,
    CTET_UNSTORE_TETRONIMO,
} ctet_Action;

typedef struct ctet_Size {
    int rows;
    int cols;
} ctet_Size;

typedef unsigned char ctet_board_t;
typedef struct ctet_State {
    ctet_Action uncommitted_actions[MAX_UNC_ACTIONS];
    ctet_Size size;
    int action_count;
    bool malloced;
    ctet_board_t* board;
    int  gravity;
    int  score;
} ctet_State;

#endif //CTETRIS_MODEL_H
