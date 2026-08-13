#ifndef MOTION_H
#define MOTION_H

enum { MOTION_NONE, MOTION_ENTER, MOTION_EXIT };

void motion_init(void);
int motion_update(int *inside);
int motion_inside_active(void);

#endif