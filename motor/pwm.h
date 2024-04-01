#define IN 			0
#define OUT 		1
#define LOW 		0
#define HIGH 		1
#define VALUE_MAX 	256
#define PWM0		0
#define PWM1		1

int PWMExport(int pwmnum);
int PWMEnable(int pwmnum);
int PWMUnenable(int pwmnum);
int PWMWritePeriod(int pwmnum, int value);
int PWMWriteDutyCycle(int pwmnum, int value);
