#include"socklib.h"
#include"headers.h"

/* The following are some function need after, let's make a declatation. */

/* to deal the remaining infomation that follows the 'GET' sentence */
void read_til_end(FILE *fp);
/* This is the function that deals with the request */
void process_on(char *, int);
/* if the parameter is illegle then call this function */
void cannot_do(int);
/* if the parameter is a dirictory then show what's in it */
int isdir(char *);
void ls_dir(char *, int);
/* if the parameter is a runnable program */
int can_run(char *);
void do_exe(char *, int);
/* if the parameter is a file ,picture or someting like that, then display it */
void show_file(char *,int);
/* to wirte the header like "HTTP/1.0 200 OK" */
void header(FILE *, char *);

int main(int ac, char **args)
{
	if (ac != 2)
		oops("parameters",1);
	int sock_id,sock_fd;
	char request[BUFSIZ];
	void wait_child(int);
	FILE *sock_fp;
	sock_id = make_server_socket(atoi(args[1]));

	if (sock_id == -1)
		oops("socket",2);
	/* to handle SIGCHLD */
	signal(SIGCHLD,wait_child);

	while (1)
	{
		sock_fd = accept(sock_id,NULL,NULL);
		sock_fp = fdopen(sock_fd,"r");

		/* get the very first line */
		fgets(request,BUFSIZ,sock_fp);
		printf("get a call:request = %s\n",request);
		/* deal with the infomation that left*/
		read_til_end(sock_fp);
		/* to deal with command and parameter */
		process_on(request,sock_fd);

		fclose(sock_fp);
	}
	return 0;
}

/* wait till all child terminate */
void wait_child(int signum)
{
	while (waitpid(-1,NULL,WNOHANG)>0)
	;
}

void read_til_end(FILE *fp)
{
	char buf[BUFSIZ];

	while (fgets(buf,BUFSIZ,fp) != NULL && strcmp(buf,"\r\n") != 0)
		;
}


void process_on(char *request, int fd)
{
	char cmd[BUFSIZ],args[BUFSIZ];
	
	strcpy(args,"./");

	if (fork() != 0)
		return ;
 
	if (sscanf(request,"%s%s",cmd,args+2) != 2)
		return;


	if (strcmp(cmd,"GET") != 0)
		cannot_do(fd);
	else if (isdir(args))
		ls_dir(args,fd);
	else if (can_run(args))
		do_exe(args,fd);
	else
		show_file(args,fd);
	exit(0); /* to terminate this process */
}

void cannot_do(int fd)
{
	FILE *fp=fdopen(fd,"w");

	fprintf(fp,"HTTP/1.1 501\r\n");
	fprintf(fp,"\r\n");
	fclose(fp);
}

void header(FILE *fp, char *type)
{
	fprintf(fp,"HTTP/1.1 200\r\n");
	if (type)
		fprintf(fp,"Content-type:%s\r\n",type);
}

int isdir(char *args)
{
	struct stat info;

	if (stat(args,&info) == -1)
		return 0;
	return (S_ISDIR(info.st_mode)); /* man 7 inode */
}

void ls_dir(char *args, int fd)
{
	FILE *fp=fdopen(fd,"w");

	header(fp,"text/plain");

	fprintf(fp,"\r\n");
	fflush(fp);

	/* redirect */
	if (dup2(fd,1) == -1)	
		oops("dup2",1);

	if (dup2(fd,2) == -1)
		oops("dup2",2);
	
	close(fd);
	fclose(fp);

	execlp("ls","ls","-l",args,NULL);

	oops("should not be here!",3);
}

char *get_suffix(char *f)
{
	char *cp;

	if ((cp=strrchr(f,'.')) != NULL)
		return cp+1;
	return "";
}

int can_run(char *args)
{
	if (get_suffix(args) == NULL)
		return 0;
	else	 
	{
		if (strcmp(get_suffix(args),"cgi") == 0)
			return 1;
		else 
			return 0;
	}
}

void do_exe(char *args, int fd)
{
	if (dup2(fd,1) == -1)
		oops("dup2",2);
	if (dup2(fd,2) == -1)
		oops("dup2",2);

	FILE *fp = fdopen(fd,"w");
	close(fd);
	header(fp,NULL);
	fprintf(fp,"\r\n");
	fflush(fp);
	fclose(fp);
	execl(args,args,NULL);
	oops(args,1);
	
}

void show_file(char *args, int fd)
{
	FILE *to_write = fdopen(fd,"w");
	FILE *to_read = fopen(args,"r");
	int c;
	char *content = "text/plain";
	char *suffix=get_suffix(args);

	if (strcmp(suffix,"html") == 0)
		content = "text/html";
	else if (strcmp(suffix,"gif") == 0)
		content = "image/gif";
	else if (strcmp(suffix,"jpg") == 0)
		content = "image/jpeg";
	else if (strcmp(suffix,"png") == 0)
		content = "image/jpeg";
	else 
		content = "text/plain";
	
	header(to_write,content);
	fprintf(to_write,"\r\n");
	fflush(to_write);

	if (to_write != NULL && to_read != NULL)
	{
		while ((c=getc(to_read)) != EOF)
		{
			putc(c,to_write);
		}
	}

	fclose(to_read);
	fclose(to_write);
}
