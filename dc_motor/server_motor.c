#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <wiringPi.h>
#include <softPwm.h>

#define IN1 20
#define IN2 21

#define BUFFER_MAX 3
#define DIRECTION_MAX 45
#define VALUE_MAX 256

#define MAXTIMINGS	85
#define DHTPIN		7

int dht11_dat[5] = { 0, 0, 0, 0, 0 };

int read_dht11_dat()

{

	uint8_t laststate	= HIGH;

	uint8_t counter		= 0;

	uint8_t j		= 0, i;

	float	f; /* fahrenheit */



	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;



	/* pull pin down for 18 milliseconds */

	pinMode( DHTPIN, OUTPUT );

	digitalWrite( DHTPIN, LOW );

	delay( 18 );

	/* then pull it up for 40 microseconds */

	digitalWrite( DHTPIN, HIGH );

	delayMicroseconds( 40 );

	/* prepare to read the pin */

	pinMode( DHTPIN, INPUT );



	/* detect change and read data */

	for ( i = 0; i < MAXTIMINGS; i++ )

	{

		counter = 0;

		while ( digitalRead( DHTPIN ) == laststate )

		{

			counter++;

			delayMicroseconds( 1 );

			if ( counter == 255 )

			{

				break;

			}

		}

		laststate = digitalRead( DHTPIN );



		if ( counter == 255 )

			break;



		/* ignore first 3 transitions */

		if ( (i >= 4) && (i % 2 == 0) )

		{

			/* shove each bit into the storage bytes */

			dht11_dat[j / 8] <<= 1;

			if ( counter > 50 )

				dht11_dat[j / 8] |= 1;

			j++;

		}

	}



	/*

	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte

	 * print it out if data is good

	 */

	if ( (j >= 40) &&

	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{

		f = dht11_dat[2] * 9. / 5. + 32;

		printf( "Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n",

			dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
		return dht11_dat[0];
	}else  {

		printf( "Data not good, skip\n" );

	}

}


void error_handling(char *message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

int main(int argc, char *argv[]) {
	wiringPiSetupGpio(); // GPIO 모드로 설정

	int serv_sock,clnt_sock=-1;
	struct sockaddr_in serv_addr,clnt_addr;
	socklen_t clnt_addr_size;
	char msg[2];


    	pinMode (IN1, OUTPUT);
    	pinMode (IN2, OUTPUT);

	softPwmCreate(IN1, 0, 100);
	softPwmCreate(IN2, 0, 100);

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0 , sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock,5) == -1)
		error_handling("listen() error");

	if(clnt_sock<0){
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept() error");
	}

	if ( wiringPiSetup() == -1 )
		exit( 1 );

	while ( 1 ){
		int num = read_dht11_dat();
		printf("Humid : %d\n",num);
		delay(1000);
		if(num > 70){
			send(clnt_sock, "Motor Start", 11, 0);

			softPwmWrite(IN1, 100);
			softPwmWrite(IN2, 0);

			delay(5000); // 5초 대기

                        // 모터 정지
			softPwmWrite(IN1, 0);
        		softPwmWrite(IN2, 0);

                        // 프로그램 종료 전 핀 상태 초기화
        		digitalWrite(IN1, LOW);
        		digitalWrite(IN2, LOW);

        		send(clnt_sock, "Motor Stop", 10, 0);

			delay(5000); /* wait 5sec to refresh */
		}
	}
	close(clnt_sock);
	close(serv_sock);

	return(0);
}
