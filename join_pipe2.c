#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>

char *one[2] = {"ls", NULL};
char *two[2] = {"wc", NULL};

void fatal(char* s)
{
	perror(s);
	exit(1);
}

void child()
{
	int p[2];

	if(pipe(p) == -1)
		fatal("pipe call");

	switch(fork())
	{
		case -1:
			fatal("2nd fork call");
			break;
		case 0:
			dup2(p[1], 1);
			
			close(p[0]);
			close(p[1]);

			execvp(one[0], one);

			fatal("1st execp call");
		default:
			dup2(p[0], 0);
			
			close(p[0]);
			close(p[1]);

			execvp(two[0], two);

			fatal("2nd execp call");
	}
}


int main(void)
{
	int status;

	switch(fork())
	{
		case -1:
			fatal("fork call");
			break;
		case 0:
			child();
			break;
	}
	wait(&status);
	return status;
}
