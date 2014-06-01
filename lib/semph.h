#ifndef SEMPH_H_
#define SEMPH_H_

int init_sems(const char *path, int sum);

int close_sems();

int set_sem_value(int i, int value);

int get_sem_value(int i);

void p_sem(int i);

void v_sem(int i);

#endif
