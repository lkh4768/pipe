#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MSGSIZE 10

char* msg1 = "hello";
char* msg2 = "bye";

void fatal(char *s)
{
	perror(s);
	exit(1);
}

void parent(int p[2])
{
	printf("start parent process\n");
	int nread;
	char buf[MSGSIZE];
	
	close(p[1]);
	for(;;)
	{
		switch(nread = read(p[0], buf, MSGSIZE))
		{
			case -1:
				if(errno == EAGAIN)
				{
					printf("(pipe empty)\n");
					sleep(1);
					break;
				}
				else
					fatal("read call");
			case 0:
				printf("End of conversation\n");
				exit(0);
			default:
				printf("MSG=%s\n", buf);
		}
	}
}

void child(int p[2], int pcnt)
{
	printf("start child process %d\n", pcnt);
	int cnt;
	char buf[MSGSIZE];

	close(p[0]);

	for(cnt = 0; cnt < 3; cnt++)
	{
		sprintf(buf, "%s %d", msg1, pcnt);
		write(p[1], buf, MSGSIZE);
		sleep(3);
	}

	sprintf(buf, "%s %d", msg2, pcnt);
	write(p[1], buf, MSGSIZE);
	exit(0);
}

int main()
{
	int pfd[2];
	int i;

	if(pipe(pfd) == -1)
		fatal("pipe call");

	if(fcntl(pfd[0], F_SETFL, O_NONBLOCK) == -1)
		fatal("fcntl call");

	for(i = 0; i < 2; i++){
		switch(fork())
		{
			case -1:
				fatal("fork call");
				break;
			case 0:
				child(pfd, i);
				break;
		}
	}

	parent(pfd);

	return 0;
}

