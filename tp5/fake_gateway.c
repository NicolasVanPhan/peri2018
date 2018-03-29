#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MAXServerRequest 1024

void write_in_login()
{

	/*On ouvre le fichier log_in dans lequel on va écrire*/
	FILE* log_in;
	time_t date;
	int random_value = rand();
	
	log_in = fopen("./log_in", "w");
	setbuf(log_in, NULL);

	if (log_in == NULL){
		printf("Error open file\n");
		exit(1);
	}
	else{
		srand(time(NULL));
		while(1){
			sleep(1);
			date = time(NULL);
			random_value += rand() % 100;
		 	printf("%ld; %d\n", date, random_value);
		 	fprintf(log_in,"%ld; %d\n", date, random_value);
		//	fflush(log_in);
		}
	}
	fclose(log_in);
}

void read_fifo()
{
	FILE* log_out = fopen("./log_out", "w");
	int f2s;
	char *f2sName = "/tmp/f2s_tp5";
	char serverResquest[MAXServerRequest];
	fd_set rfds;
	int nfds;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	char buff[100];
	
	setbuf(log_out, NULL);

	mkfifo(f2sName, 0666);

	f2s = open(f2sName, O_RDWR);

	do {
		FD_ZERO(&rfds);
		FD_SET(f2s, &rfds);
		if (select(nfds, &rfds, NULL, NULL, &tv) != 0) {
			int nbchar;
			if (nbchar = read(f2s, serverResquest, MAXServerRequest) == 0) break;
			serverResquest[nbchar] = 0;
			fprintf(log_out, "%s", serverResquest);
			printf("Receive : %s\n", serverResquest);
		}
	} while(1);

	close(f2s);
	fclose(log_out);
}


int main(int argc, char *argv[])
{

	if(fork())
		write_in_login();
	read_fifo();

	return 0;
}


