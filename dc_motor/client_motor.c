#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IN1 20
#define IN2 21
#define MAX_BUFFER_SIZE 1024

// 에러 핸들링 (과제 코드 참조)
void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main (int argc, char *argv[])
{
    int clnt_sock;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER_SIZE];

    // GPIO 모드로 설정
    wiringPiSetupGpio();

    // GPIO 출력 모드로 설정
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    
    // argument로 IP, 포트 번호 입력
    if(argc!=3){
        printf("Usage : %s <IP> <port>\n",argv[0]);
        exit(1);
    }

    // 소켓 통신, 실패 시 에러 반환
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);

    if(clnt_sock == -1) {
        error_handling("Failed to create socket.");
    }
        
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));  
        
    // 서버와 소켓 통신 연결
    if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) {
        error_handling("Failed to connect to server.");
    }
    
    // 무한 루프를 통해 메시지를 받을 때마다 모터가 돌아가게끔
    while (1) {
        // recv 함수를 통해 서버로부터 메시지를 받음
        memset(buffer, '\0', MAX_BUFFER_SIZE);
        int bytes_received = recv(clnt_sock, buffer, MAX_BUFFER_SIZE, 0);

        // 메시지 반환 실패 시 오류 반환, 서버에서 소켓 통신 종료 시 무한 루프 탈출 후 통신 종료
        if (bytes_received == -1) {
            error_handling("Failed to receive message.");
        } else if (bytes_received == 0) {
            printf("서버와의 연결이 종료되었습니다.\n");
            break;
        }

        printf("Receive message from Server : %s\n", buffer);

        // 서버로부터 받은 메시지가 'Motor Start'인 경우 DC 모터 작동
        if (strcmp(buffer, "Motor Start") == 0) {
            softPwmCreate(IN1, 0, 100);
            softPwmCreate(IN2, 0, 100);

            // 80% 듀티 사이클 적용
            softPwmWrite(IN1, 80);
            softPwmWrite(IN2, 0);

            // 5초간 작동
            delay(5000);

            // 모터 정지
            softPwmWrite(IN1, 0);
            softPwmWrite(IN2, 0);

            // 모터 초기화
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, LOW);
        }
    }

    // 소켓 통신 종료
    close(clnt_sock);

    return 0;
}