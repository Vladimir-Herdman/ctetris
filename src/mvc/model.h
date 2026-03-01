#ifndef CTETRIS_MODEL_H
#define CTETRIS_MODEL_H

#ifdef CTET_REMOVE_PREFIX
#define ctet(text) text
#define CTET(text) text
#define BOARD_AT(board, size, r, c) (board)[(size).cols*(r)+(c)]
#else
#define ctet(text) ctet_##text
#define CTET(text) CTET_##text
#define CTET_BOARD_AT(board, size, r, c) (board)[(size).cols*(r)+(c)]
#endif

typedef enum ctet(Action) ctet(Action);
enum ctet(Action) {
    CTET(MOVE_UP)=1,
    CTET(MOVE_DOWN),
    CTET(MOVE_LEFT),
    CTET(MOVE_RIGHT),
    CTET(STORE_TETRINO),
    CTET(UNSTORE_TETRINO),
};

typedef struct ctet(Size) ctet(Size);
struct ctet(Size) {
    int rows;
    int cols;
};

typedef struct ctet(State) ctet(State);
typedef int ctet(board_t);
struct ctet(State) {
    ctet(Action) uncommitted_actions[16];
    int action_count;
    ctet(Size) size;
    ctet(board_t)* board;
    int  gravity;
    int  score;
};

//Commit actions currently uncommitted, update board and score
ctet(State)* ctet(new_state)(const ctet(Size) size);
void ctet(free_state)(ctet(State)* state);
void ctet(update_state)(ctet(State)* state);

//Add action to action list
void ctet(action)(ctet(State)* state, ctet(Action) action);

#undef ctet
#undef CTET

#endif //CTETRIS_MODEL_H
