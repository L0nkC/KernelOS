/* drivers/keyboard.h */
#ifndef KEYBOARD_H
#define KEYBOARD_H

void kb_init(void);
int kb_getchar(char* c);
char kb_read_char(void);

#endif
