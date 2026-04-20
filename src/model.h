#ifndef CTETRIS_MODEL_H
#define CTETRIS_MODEL_H

#define TET_SIZE 16

typedef enum ctet_Action : unsigned char {
    CTET_MOVE_DOWN=1,
    CTET_MOVE_LEFT,
    CTET_MOVE_RIGHT,
    CTET_ROTATE_LEFT,
    CTET_ROTATE_RIGHT,
    CTET_INSTANT_DOWN,
    CTET_STORE_TETRONIMO,
    CTET_UNSTORE_TETRONIMO,
    CTET_END_GAME,
} ctet_Action;

typedef enum ctet_Result : unsigned char {
    CTET_DO_NOTHING=0,
    CTET_PLACED_TETRONIMO,
    CTET_MOVED_TETRONIMO,
    CTET_MOVE_NOTALLOWED,
    CTET_GAME_ENDED,
} ctet_Result;

typedef struct ctet_Size {
    int rows;
    int cols;
} ctet_Size;

typedef unsigned char ctet_board_t;
typedef struct ctet_State {
    ctet_Size size;
    bool malloced;
    ctet_board_t* board;
    ctet_board_t cur_tet[TET_SIZE];
    ctet_board_t next_tets[4][TET_SIZE]; //additional 1 out for stored tet logic
    ctet_board_t stored_tet[TET_SIZE];
    ctet_Size cur_pos;
    int gravity;
    int score;
    int level;
    bool gamerunning;
} ctet_State;

#endif //CTETRIS_MODEL_H
