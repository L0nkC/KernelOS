/* drivers/mouse.h */
#ifndef MOUSE_H
#define MOUSE_H

typedef struct {
    int x, y;
    int buttons;
    int changed;
} mouse_t;

void mouse_init(void);
mouse_t mouse_get(void);
int mouse_moved(void);

#endif
