#include "gpio.h"
#include "ultra.h"

int main(int argc, char *argv[]) {
    int obstacle_0 = 0; // Flag to track obstacle presence for Sensor 0
    int obstacle_1 = 0; // Flag to track obstacle presence for Sensor 1
    clock_t start_t;
    clock_t end_t;
    double time;


    // Export GPIO pins
    if (-1 == GPIOExport(POUT_0) || 
    -1 == GPIOExport(PIN_0) || 
    -1 == GPIOExport(POUT_1) || 
    -1 == GPIOExport(PIN_1)) {
        perror("GPIO export error");
        exit(1);
    }

    usleep(100000);

    // Set GPIO directions
    if (-1 == GPIODirection(POUT_0, OUT) || -1 == GPIODirection(PIN_0, IN) ||
        -1 == GPIODirection(POUT_1, OUT) || -1 == GPIODirection(PIN_1, IN)) {
        perror("GPIO direction error");
        exit(1);
    }

    if(GPIOWrite(POUT_1, 0)) {
        perror("GPIO write error");
        exit(1);
    }

    fprintf(stderr, "Starting ultrasonic sensors...\n");

    int sock;
    struct sockaddr_in serv_addr;
    char obstacle_msg[20];

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        exit(1);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_addr.sin_port = htons(SERVER_PORT);  
    
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect() error");
        exit(1);
    }

    while (1) {
        int distance_0 = 0; // Distance measured by Sensor 0
        int distance_1 = 0; // Distance measured by Sensor 1

        // Trigger Sensor 0
        GPIOWrite(POUT_0, 1);
        usleep(10);

        GPIOWrite(POUT_0, 0);

        // Measure distance from Sensor 0
        while (GPIORead(PIN_0) == 0);
        start_t = clock();

        while (GPIORead(PIN_0) == 1);
        end_t = clock();
        time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
        
        distance_0 = (int)(time / 2 * 34000);

        // Trigger Sensor 1
        if(GPIOWrite(POUT_1, 1)) {
            perror("GPIO write error");
            exit(1);
        }

        usleep(10);
        if(GPIOWrite(POUT_1, 0)) {
            perror("GPIO write error");
            exit(1);
        }

        // Measure distance from Sensor 1
        while (GPIORead(PIN_1) == 0) {
            start_t = clock();
        }

        while (GPIORead(PIN_1) == 1) {
            end_t = clock();
        }
        time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
        distance_1 = (int)(time / 2 * 34000);

        // Check for obstacles
        if (distance_0 <= THRESHOLD_DISTANCE && !obstacle_0) {
            obstacle_0 = 1;
            snprintf(obstacle_msg, sizeof(obstacle_msg), "1");
            printf("bookshelf num: %s\n", obstacle_msg); 

            if(write(sock, obstacle_msg, strlen(obstacle_msg)) < 0) {
                perror("send1() error");
                exit(1);
            }
        }
        else if (distance_1 <= THRESHOLD_DISTANCE && !obstacle_1) {
            obstacle_1 = 1;
            snprintf(obstacle_msg, sizeof(obstacle_msg), "2");
            printf("bookshelf num: %s\n", obstacle_msg);

            if(write(sock, obstacle_msg, strlen(obstacle_msg)) < 0) {
                perror("send1() error");
                exit(1);
            }
        }
        else if (distance_0 > THRESHOLD_DISTANCE && obstacle_0) {
            obstacle_0 = 0;
            snprintf(obstacle_msg, sizeof(obstacle_msg), "-1");
            printf("bookshelf num: %s\n", obstacle_msg);

            if(write(sock, obstacle_msg, strlen(obstacle_msg)) < 0) {
                perror("send1() error");
                exit(1);
            }
        } 
        else if (distance_1 > THRESHOLD_DISTANCE && obstacle_1) {
            obstacle_1 = 0;
            snprintf(obstacle_msg, sizeof(obstacle_msg), "-2");
            printf("bookshelf num: %s\n", obstacle_msg);

            if(write(sock, obstacle_msg, strlen(obstacle_msg)) < 0) {
                perror("send2() error");
                exit(1);
            }
        }

        // Wait for a while before next measurement
        usleep(200000);
    }
    
    // Close socket
    close(sock);
    
    return 0;
}