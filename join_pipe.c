#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>

#define RD 0
#define WR 1

void fatal(char* s)
{
	perror(s);
	exit(1);
}

int get_pipe_sym_index(char** src)
{
	int i = 0;
	while(strcmp(src[i++], "pi") != 0){
		if(src[i] == NULL)
			return -1;
	}

	return (i-1);
}

int get_arg_last_index(char** src)
{
	int i = 0;
	while(src[i++] != NULL);

	return (i-2);
}

void child_op(int p[2], char** com, int fd)
{	
	dup2(p[fd], fd);
	close(p[0]);
	close(p[1]);

	execvp(com[0], com);

	if(fd == 0)
		fatal("read child execvp call");
	else
		fatal("write child execvp call");
}

void for_child(char** lhs, char** rhs)
{
	int p[2];
	if(pipe(p) == -1)
		fatal("pipe call");

	switch(fork())
	{
		case -1:
			fatal("fork call");
			break;
		case 0:
			child_op(p, lhs, WR);
		default:
			child_op(p, rhs, RD);
	}
}

int main(int agrn, char** argv)
{
	int pipe_sym_index = 0;
	int argv_last_index = 0;

	if(agrn < 4){
		printf("Usage : ./join_pipe lhs pi rhs\n");
		exit(1);
	}

	if((pipe_sym_index = get_pipe_sym_index(argv)) < -1){
		printf("not found pi");
		exit(1);
	}

	int i = 0;
	int j = 0;
	char** lhs = (char**)malloc(sizeof(char*) * pipe_sym_index);
	while(++i < pipe_sym_index)
	{
		lhs[j] = (char*)malloc(sizeof(char) * strlen(argv[i]));
		lhs[j] = argv[i];
		j++;
	}
	lhs[j] = NULL;

	argv_last_index = get_arg_last_index(argv);
	j = 0;
	char** rhs = (char**)malloc(sizeof(char*) * (argv_last_index - pipe_sym_index + 1));
	while(++i <= argv_last_index)
	{
		rhs[j] = (char*)malloc(sizeof(char) * strlen(argv[i]));
		rhs[j] = argv[i];
		j++;
	}
	rhs[j] = NULL;

	switch(fork())
	{
		case -1:
			fatal("fork call");
			break;
		case 0:
			for_child(lhs, rhs);
			break;
		default:
			break;
	}

	wait(NULL);
	i = 0;
	while(lhs[i] != NULL){
		free(lhs[i]);
		i++;
	}

	i = 0;
	while(rhs[i] != NULL){
		free(rhs[i]);
		i++;
	}

	free(lhs);
	free(rhs);
	return 0;
}
