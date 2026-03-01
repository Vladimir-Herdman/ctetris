#ifndef CTETRIS_MVC_VIEW_H
#define CTETRIS_MVC_VIEW_H

#ifdef CTET_REMOVE_PREFIX
#define ctet(text) text
#define CTET(text) text
#else
#define ctet(text) ctet_##text
#define CTET(text) CTET_##text
#endif

#define CTET_REMOVE_PREFIX
#include "mvc/model.h"
#undef CTET_REMOVE_PREFIX

void ctet(print_board)(const State* state);

#undef ctet
#undef CTET

#endif //CTETRIS_MVC_VIEW_H
