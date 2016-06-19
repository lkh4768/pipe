#include<stdio.h>
#include<stdlib.h>

#include<sys/select.h>
#include<sys/types.h>
#include<unistd.h>

#include<sys/wait.h>

#define MSGSIZ 64

void fatal(char *s)
{
	perror(s);
	exit(1);
}

void for_child(int p[2])
{
	close(p[0]);

	char msg[MSGSIZ];
	int i;

	sprintf(msg, "hello %d", getpid()); 

	for(i = 0; i < 3; i++){
		if(write(p[1], msg, MSGSIZ) == -1)
			fatal("write call");
		sleep(1);
	}

	exit(0);
}

void for_parent(int p[2][2])
{
	close(p[0][1]);
	close(p[1][1]);

	int i, nread;
	char buf[MSGSIZ];
	fd_set set, master;

	FD_ZERO(&master);
	FD_ZERO(&set);
	FD_SET(p[0][0], &master);
	FD_SET(p[1][0], &master);

	while(1)
	{
		printf("dd-1\n");
		set = master;
		if(select(p[1][0] + 1, &set, NULL, NULL, NULL) == -1)
			fatal("select call");
		for(i = 0; i < 2; i++)
		{
			if(FD_ISSET(p[i][0], &set))
			{
				nread = read(p[i][0], buf, MSGSIZ);
				if(nread == -1)
					fatal("read call");
				else if(nread > 0)
					printf("%s\n", buf);
				else if(nread == 0){
					printf("close child\n");
					FD_CLR(p[i][0], &master);
				}
			}
		} 
		printf("dd\n");

		if(waitpid(-1, NULL, WNOHANG) == -1)
			return;

		printf("dd2\n");
	}

	printf("dd3\n");
	return;

}

int main()
{
	int p[2][2];
	int i;

	for(i = 0; i < 2; i++)
	{
		if(pipe(p[i]) == -1){
			fatal("pipe call");
		}

		switch(fork())
		{
			case 0:
				for_child(p[i]);
				break;
			case -1:
				fatal("fork call");
		}
	}

	for_parent(p);

	exit(0);
}
