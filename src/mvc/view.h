#ifndef CTETRIS_MVC_VIEW_H
#define CTETRIS_MVC_VIEW_H

#define CTET_REMOVE_PREFIX
#include "mvc/model.h"
#undef CTET_REMOVE_PREFIX

#ifdef CTET_REMOVE_PREFIX
#define ctet(text) text
#define CTET(text) text
#else
#define ctet(text) ctet_##text
#define CTET(text) CTET_##text
#endif

#ifdef TEXTVIEW
void ctet(print_board)(int* board, const Size size);
#else //ncurses view

#endif

#undef ctet
#undef CTET

#endif //CTETRIS_MVC_VIEW_H
