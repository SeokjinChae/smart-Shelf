#include "move.h"

void init_motor(int pwmnum) {
	PWMExport(pwmnum);
	PWMWriteDutyCycle(pwmnum, M_CYCLE_L);
	PWMWritePeriod(pwmnum, M_PERIOD);
	PWMEnable(pwmnum);
}

void move_forward(int pwmnum) {
	PWMWriteDutyCycle(pwmnum, M_CYCLE_R);
}

void move_backward(int pwmnum) {
	PWMWriteDutyCycle(pwmnum, M_CYCLE_L);
}

void fin_motor(int pwmnum) {
	PWMUnenable(pwmnum);
}

int move_motor(int pwmnum) {
	fprintf(stderr, "initialize the motor%d...\n", pwmnum);
	init_motor(pwmnum);
	
	move_forward(pwmnum);
	sleep(5);

	move_backward(pwmnum);
	sleep(2);
	
	fprintf(stderr, "unenable the motor%d...\n", pwmnum);
	fin_motor(pwmnum);
}