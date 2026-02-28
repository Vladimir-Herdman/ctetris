#ifndef CTETRIS_MODEL_H
#define CTETRIS_MODEL_H

#include <stdint.h>

#ifdef CTET_REMOVE_PREFIX
#define ctet(text) text
#define CTET(text) text
#else
#define ctet(text) ctet_##text
#define CTET(text) CTET_##text
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
struct ctet(State) {
    ctet(Action) uncommitted_actions[16];
    ctet(Size) size;
    int* board;
    int  gravity;
    int  score;
};

//Commit actions currently uncommitted, update board and score
ctet(State)* new_state(const ctet(Size));
void free_state(ctet(State)*);
void ctet(update_state)(ctet(State)* state);

//All int functions return the points for that action
int ctet(clear_full_rows)(int* board, const ctet(Size) size);

//Add action to action list
void ctet(action)(ctet(Action) action);

#undef ctet
#undef CTET

#endif //CTETRIS_MODEL_H
