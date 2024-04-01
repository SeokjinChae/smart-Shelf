#ifndef MOVE_H
#define MOVE_H

#include <unistd.h>
#include <stdio.h>
#include "pwm.h"

#define M_PERIOD	20000000
#define	M_CYCLE_O	((M_PERIOD)*0.075)
#define M_CYCLE_L	((M_PERIOD)*0.03)
#define	M_CYCLE_R	((M_PERIOD)*0.12)

void init_motor(int pwmnum);
void move_forward(int pwmnum);
void move_backward(int pwmnum);
void fin_motor(int pwmnum);
int move_motor(int pwmnum);

#endif