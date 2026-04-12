#ifndef CTETRIS_MODEL_H
#define CTETRIS_MODEL_H

#define NEXT_TET_LIST_SIZE 3
#define TET_SIZE 16

typedef enum ctet_Action : unsigned char {
    CTET_MOVE_UP=1,
    CTET_MOVE_DOWN,
    CTET_MOVE_LEFT,
    CTET_MOVE_RIGHT,
    CTET_STORE_TETRONIMO,
    CTET_UNSTORE_TETRONIMO,
} ctet_Action;

typedef enum ctet_Result : unsigned char {
    CTET_DO_NOTHING=0,
    CTET_PLACED_PIECE,
} ctet_Result;

typedef struct ctet_Size {
    int rows;
    int cols;
} ctet_Size;

typedef unsigned char ctet_board_t;
typedef struct ctet_State {
    ctet_Size size;
    int action_count;
    bool malloced;
    ctet_board_t* board;
    ctet_board_t cur_tet[TET_SIZE];
    ctet_board_t* next_tets[NEXT_TET_LIST_SIZE];
    ctet_Size cur_pos;
    int  gravity;
    int  score;
} ctet_State;

#endif //CTETRIS_MODEL_H
