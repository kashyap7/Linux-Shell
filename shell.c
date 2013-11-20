//Assumptions:
//There is uncertainity whether "kill" process terminates before the background process (specified in arguments of "kill" process)
//terminates. The handler for background process should run when background process terminates and vice-versa.
//"&" should be applied after a space

// 123,156,220

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<errno.h>
#include<fcntl.h>
char cw[100], host[100];
int fgbg[100]={0};
int pid1[100], line=1 , hist=0;
char l1[100][100], pid[100][100];
void handler_bg(int sig)
{
	int pid, i;
	char *u=getenv("USER");
	pid=wait(NULL);
	printf("PID %d exited\n", pid);
	for(i=1; i<line; i++)               
	{
		if(pid1[i]==pid)
		{
			fgbg[i]=0;
			break;
		}
	}
}
void handler_fg(int sig)
{

	return;
}
void pidh(char **t, int no_tokens)
{
	int i;
	if(no_tokens==1)
	{
		printf("%s process id: %d\n", pid[0], pid1[0]);
	}
	else if(no_tokens>0 && !strcmp(t[1], "all"))
	{
		printf("List of all processes spawned from this shell:\n");
		for(i=0; i<line; i++)
		{
			printf("command name:%s process id: %d\n", pid[i], pid1[i]);
		}
	}
	else if(no_tokens>0 && !strcmp(t[1], "current"))
	{
		printf("List of currently executing processes spawned from this shell:\n");
		for(i=0; i<line; i++)
		{
			if(fgbg[i])
			{
				printf("command name:%s process id: %d\n", pid[i], pid1[i]);
			}
		}
	}
}
void histh(char **t, int no_tokens)
{
	int i, j;
	char s[10];
	j=hist;
	if(strlen(t[0])!=4)
	{
		for(i=4; i<=strlen(t[0]); i++)
			s[i-4]=t[0][i];
		j=atoi(s);
	}
	if(j>hist)
		j=hist;
	for(i=hist-j; i<hist; i++)
	{
		printf("%d. %s\n",i-hist+j+1, l1[i]);
	}
}
void INThandler(){}



main(int n, char *arg[]){
	signal(SIGCHLD, handler_bg);
	int p, q, i, k, j, f, no_tokens, pf[10][2], m, i1, pipec=0, i2, in, out,hf, hf1;
	char *u=getenv("USER");
	char cwd[100], l[100], **t, cwd1[100],  ch,  **t1, **temptok, **t2, lt[100];
	pid1[0]=getpid();
	strcpy(pid[0], arg[0]);
	signal(SIGINT, INThandler);
	signal(SIGTSTP, INThandler);
	t=malloc(sizeof(char *)*10);
	t[0]=malloc(sizeof(char)*1000);
	temptok=malloc(sizeof(char *)*10);
	temptok[0]=malloc(sizeof(char)*1000);
	t1=t;
	t2=temptok;
	t1[0]=t[0];
	t2[0]=temptok[0];
	cw[0]='~';
	cw[1]='\0';
	gethostname(host, 100);
	getcwd(cwd, 100);                           // cwd is initial starting directory path and cw is for setting current dir as ~
	l[0]='\0';
	m=1;
	hf=1;
	while(1)
	{
		t=t1;
		t[0]=t1[0];
		temptok=t2;
		temptok[0]=t2[0];
		if(hf!=0)                                                        // initially hf=1;
		l[0]='\0';
		for(i=1; i<10; i++)
		{
			t[i]=t[i-1]+100;
			temptok[i]=temptok[i-1]+100;
		}
		if(hf!=0){
			printf("<%s@%s:%s>", u, host, cw);
			while(scanf("%[^\n]", l)==EOF);                      // l is input
			scanf("%c", &ch);                                       // ch is enter
		}
		if(l[0]=='\0')
			continue;
		//	printf("--%s---\n", l);
		k=0;
		j=0;
		hf=1;
		pipec=0;
		for(i=0; i<strlen(l); i++)                              // traversing along l (input string)
		{
			if(l[i]=='|')                                       // detecting pipe
				pipec++;
			if((l[i]==' '||l[i]=='\n')&& j!=0)
			{
				t[k][j]='\0';
				k++;                                   // storing input command in t
				j=0;                                 // ls -al stored as 01(l) 02(s) 10( ) 11(-) 12(a) 13(l)
			}
			else if(l[i]!=' ' && l[i]!='\n')            // ls   -al as 01(l) 02(s) 10( ) 10( ) 10( ) 11(-) 12(a) 13(l)
			{
				t[k][j++]=l[i];
			}
			printf("\n--%d %d--\n",k,j);
		}
		for(i2=0; i2<pipec; i2++)
		{
			pipe(pf[i2]);
		}
		if(j!=0)
		{
			t[k][j]='\0';
			k++;
		}
		no_tokens=k;
		if(k!=0)
			t[k]=(char *)NULL;
		if(k==0)
			continue;
		//	for(i=0; i<k; i++)
		//	{
		//		printf("%s\n", t[i]);
		//	}
		if(!strcmp(t[0], "cd"))
		{
			m=chdir(t[1]);
			if(m==-1)
				perror("Error!! Directory does not exist!!");

			getcwd(cwd1, 100);
			f=0;
			for(i=0; i<strlen(cwd); i++)
			{
				if(cwd[i]!=cwd1[i])
				{
					f=1;
					break;
				}
			}
			if(f==0)
			{
				k=1;
				cw[0]='~';
				for(i=strlen(cwd); i<=strlen(cwd1); i++)  // if command is cd documents  also cwd1 > cwd 
				{
					cw[k++]=cwd1[i];                    // and print as ~/documents
				}		
			}
			if(f==1)		
				strcpy(cw, cwd1);           // if command is cd .. also cwd1 < cwd and print actual full path
		}
		else if(!strcmp(t[0], "quit"))
		{
			exit(0);
		}
		else
		{
			if(!strcmp(t[no_tokens-1], "&"))
			{

				t[no_tokens-1]=(char *)NULL;
				no_tokens--;
				strcpy(pid[line], t[0]);
				k=fork();
				fgbg[line]=1;
				if(k==0)
				{
					//pid1[1]=getpid();
					//	exit(0);
					execvp(t[0],  t);
					perror("ERROR! Command Not Found!!");
					exit(0);
				}
				//	else
				//	{
				pid1[line]=k;                                         // pid of child process
				//				i=wait(NULL);
				//		printf("----%d\n", i);
				signal(SIGCHLD, handler_bg);
				//	}
				line++;
			}
			else
			{
				signal(SIGCHLD, handler_fg);
				j=0;
				k=0;
				hf1=0;
				for(i=0; i<no_tokens; i++)
				{
					if(!strncmp(t[i], "!hist", 5))
					{
						for(i2=5; i2<=strlen(t[i]);i2++)
							l[i2-5]=t[i][i2];
						i2=atoi(l);
						if(i2>hist)
						{
							hf1=1;
							perror("Error!!Command does not exist in History!!");
							//			exit(0);
						}
						hf=0;
						strcpy(l, l1[i2-1]);
						break;
					}
					else if(!strcmp(t[i], "|")||i==no_tokens-1)
					{
						if(i!=no_tokens-1)
						{
							//	pipe(pf[j]);
							temptok[k]=(char *)NULL;
						}
						else
						{
							strcpy(temptok[k],t[i]);
							temptok[k+1]=(char *)NULL;
							k++;
						}
						m=1;
						m=fork();
						if(m==0)
						{
							if(j!=0)
							{
								dup2(pf[j-1][0], 0);
							}
							if(j!=pipec)
							{
								dup2(pf[j][1], 1);
							}
							for(i2=0; i2<pipec; i2++)
							{
								close(pf[i2][0]);
								close(pf[i2][1]);
							}
							f=0;
							in=0;
							out=1;
							for(i2=0; i2<k; i2++)
							{
								if(!strcmp(temptok[i2], "<"))
								{
									in=open(temptok[i2+1], O_RDONLY);
									if(f==0)
										f=i2;
								}
								if(!strcmp(temptok[i2], ">"))
								{
									out=open(temptok[i2+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
									if(f==0)
										f=i2;
								}
							}
							if(f!=0)
								temptok[f]=(char *)NULL;
							if(in==-1)
							{
								perror("Error!! File doesn't exist!!");
								exit(0);
							}
							if(out==-1)
							{
								perror("Error!! File can't be created!!");
								exit(0);
							}
							dup2(in, 0);
							dup2(out, 1);
							if(in!=0)
								close(in);
							if(out!=1)
								close(out);							
							if(!strcmp(temptok[0], "pid"))
							{
								pidh(temptok, no_tokens);
								exit(0);
							}
							else if(!strncmp(temptok[0], "hist", 4))
							{
								histh(temptok, no_tokens);
								exit(0);
							}
							else
								execvp(temptok[0], temptok);
							perror("ERROR! Command Not Found!!");
							exit(0);
						}
						if(strcmp(temptok[0], "pid") && strncmp(temptok[0], "hist", 4))
						{							
							pid1[line]=m;
							strcpy(pid[line], temptok[0]);
							line++;
						}
						j++;
						k=0;
						temptok=t2;
						temptok[0]=t2[0];
						for(i1=1; i1<10; i1++)
						{
							temptok[i1]=temptok[i1-1]+100;
						}
						continue;
					}
					strcpy(temptok[k],t[i]);
					k++;
				}
				//			if(m==0)
				//				continue;
				if(hf1==1)
				{
					hf=1;
					continue;
				}
				for(i2=0; i2<pipec; i2++)
				{
					close(pf[i2][0]);
					close(pf[i2][1]);
				}
				for(i2=0; i2<pipec+1; i2++)
				{
					wait(NULL);
				}
				signal(SIGCHLD, handler_bg);
				if(getpid()!=pid1[0])
					exit(0);
				//			wait(NULL);
				//			wait(NULL);
				/*			k=vfork();
							if(k==0)
							{
							i=execvp(t[0],  t);
							perror("ERROR! Command Not Found!!");
							}
							else
							{
							wait(NULL);
							signal(SIGCHLD, handler_bg);
				//		printf("----%d\n", i);
				}
				if(i!=-1)
				{
				pid1[line]=k;
				strcpy(pid[line], t[0]);
				line++;
				}
				 */
			}
		}
		l[0]='\0';
		for(i=0; i<no_tokens; i++)
		{
			if(!strncmp(t[i], "!hist", 5))
			{
				for(i2=5; i2<=strlen(t[i]);i2++)
					lt[i2-5]=t[i][i2];
				i2=atoi(lt);
				strcat(l, l1[i2-1]);
			}
			else
				strcat(l, t[i]);
			strcat(l, " ");
		}
		if(hf!=0)
		{
			strcpy(l1[hist], l);
			l[0]='\0';
			hist++;
		}

		/*	for(i2=0; i2<hist; i2++)
			{
			printf("%s\n",l1[i2]);
			}*/
		//printf("<%s@%s:%s>", u, host, cw);
		//	line++;
	}
}
