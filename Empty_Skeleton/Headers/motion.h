#ifndef MOTION_H
#define MOTION_H

enum { MOTION_NONE, MOTION_ENTER, MOTION_EXIT };

void motion_init(void);
int motion_update(int *inside);
void motion_mark_inside(void);

#endif