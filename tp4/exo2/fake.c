#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define MAXServerResquest 1024

int main()
{
    int     f2s, s2f;                                       // fifo file descriptors
    char    *f2sName = "/tmp/f2s_aggounphan";                       // filo names
    char    *s2fName = "/tmp/s2f_aggounphan";                       //
    char    serverRequest[MAXServerResquest];               // buffer for the request
    fd_set  rfds;                                           // flag for select
    int	    nfds;
    struct  timeval tv;                                     // timeout
    tv.tv_sec = 1;                                          // 1 second
    tv.tv_usec = 0;                                         //
    char    buff[100];

    mkfifo(s2fName, 0666);                                  // fifo creation
    mkfifo(f2sName, 0666);

    /* open both fifos */
    s2f = open(s2fName, O_RDWR);                            // fifo openning
    f2s = open(f2sName, O_RDWR);

    do {
        FD_ZERO(&rfds);                                     // erase all flags
        FD_SET(s2f, &rfds);                                 // wait for s2f
        FD_SET(STDIN_FILENO, &rfds);                        // wait for stdin

	nfds = s2f > STDIN_FILENO ? s2f : STDIN_FILENO;
	nfds++;
        if (select(nfds, &rfds, NULL, NULL, &tv) != 0) {   // wait until timeout
	    // If there's something coming from the pipe
            if (FD_ISSET(s2f, &rfds)) {                     // something to read
                int nbchar;
                if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
                serverRequest[nbchar]=0;
		printf("Receive : %s\n", serverRequest);
                write(f2s, buff, 100);
            }
 	    // If there's something coming from the standard input
	    if (FD_ISSET(STDIN_FILENO, &rfds)) {
		    read(STDIN_FILENO, &buff, 100);
	    }
        }
    }
    while (1);

    close(f2s);
    close(s2f);

    return 0;
}
