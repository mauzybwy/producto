#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "producto.h"

typedef enum display_state_e {
  START = 0,
  TASK,
  LIST,
  TIMER,
} DisplayState;

void display_init(Producto *p);
void display_draw();
void display_transition(DisplayState state);

#endif // _DISPLAY_H_
