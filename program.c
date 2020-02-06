/*

fonsyion FonsiyonAdi()
değişken degiskenAdi;
sabit SABIT;

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include "program.h"

int main(){
    init();
    return 0;
}

void init()
{
    printf("\n**Ghost In Shell**\n");
    int quit=1;
    char command[MAX];
    while(quit){
        printf(">:: ");
        fflush(stdout);
        fgets(command,MAX,stdin);
        commandControl(command,&quit);
    }
}
void commandControl(char command[MAX],int *env)
{
    fflush(stdout);
    char *commands[256];
    int commandCount = 0,i=0,lastComma=0;
    for (i=0 ; i< strlen(command);i++) //Virgül ile ayrılmış birden fazla komutu bulup commands[] a atıyor.
    {
        if(command[i]==';'||command[i]=='\n') // Virgülden sonra veya önce veya hem sonra hem önce boşluk olduğu durumlarda da çalışır.
        {
            if(i-1>=0 && command[i-1]==' ')
            {
                commands[commandCount]= (char*) malloc((i-lastComma-1)*sizeof(char));
                strncpy(commands[commandCount],command+lastComma,i-lastComma-1);
            }
            else
            {
                commands[commandCount]= (char*) malloc((i-lastComma+1)*sizeof(char));
                strncpy(commands[commandCount],command+lastComma,i-lastComma);
                commands[commandCount][i-lastComma]=NULL;
            }
            if(i+1<strlen(command)&&command[i+1]==' ')
            {
                lastComma=i+2;
            }
            else
            {    
                lastComma=i+1;
            }
            ++commandCount;
        }
    }
    for (i=0;i<commandCount;i++) // commands[] da bulunan komutlar sırasıyla çağrılıyor.
    {
        executeCommand(commands[i],env);
        //*commands[i]="";
        free(commands[i]);
    }
}
void executeCommand(char *commandt,int *env)
{
    char command[strlen(commandt)+1];int j;
    for(j=0;j<strlen(commandt)+1;j++)
    {
        command[j]=commandt[j];
    }
    fflush(stdout);
    char *args[256];
    int i=0,back=0;
    if(strstr(command, "&") != NULL)
    {
        back=1;
    }
    if(strstr(command, "<") != NULL)
    {
        if((args[i]=strtok(command," &<\n"))==NULL)
        {
            return;
        }
        i++;
        while((args[i]=strtok(NULL," &<\n"))!=NULL) // command ve parametreleri args[] a atanıyor.
        {
            i++;
        }
        //args[i]=NULL;
        if(!strcmp(args[0],"quit")){
            *env=0;
            return;
        }
        if(back==1)
        {
            processBackground(&args,i-1,0);
        }
        else
        {
            process(&args,i-1,0);
        }
    }
    else if(strstr(command, ">") != NULL)
    {
        if((args[i]=strtok(command," &>\n"))==NULL)
        {
            return;
        }
        i++;
        while((args[i]=strtok(NULL," &>\n"))!=NULL) // command ve parametreleri args[] a atanıyor.
        {
            i++;
        }
        if(!strcmp(args[0],"quit")){
            *env=0;
            return;
        }
        if(back==1)
        {
            processBackground(&args,i-1,1);
        }
        else
        {
            process(&args,i-1,1);
        }
    }
    else
    {
        if((args[i]=strtok(command," &\n"))==NULL)
        {
            return;
        }
        i++;
        while((args[i]=strtok(NULL," &\n"))!=NULL) // command ve parametreleri args[] a atanıyor.
        {
            i++;
        }
        if(!strcmp(args[0],"quit")){
            *env=0;
            return;
        }
        if(back==1)
        {
            processBackground(&args,i-1,2);
        }
        else
        {
            process(&args,i-1,2);
        }
    }
}
void process(char **args,int argCount, int type)
{
        int status;
        pid_t pid;
        
        pid = fork();
        if (pid < 0) 
        { 
            perror("\nERR"); 
            return; 
        } 
        else if (pid == 0) 
        {
            if(type==0)
            {
                if ((access (args[argCount],F_OK) == -1))
                {	
                    printf("Giriş dosyası(%s) bulunamadı\n",args[argCount]);
                    kill(getpid(),SIGTERM);
                    return;
                }
                int fileDescriptor=open(args[argCount], O_RDONLY, 0600);
                dup2(fileDescriptor,STDIN_FILENO);
                if (execvp(args[0],args)==-1)	
                {
                    perror("\nERR");
                    close(fileDescriptor);
                    kill(getpid(),SIGTERM);
                    return;
                }
                close(fileDescriptor);
                //kill(getpid(),SIGTERM);
            }
            else if(type==1)
            {
                if ((access (args[argCount],F_OK) == -1))
                {	
                    printf("Çıkış dosyası(%s) bulunamadı\n",args[argCount]);
                    kill(getpid(),SIGTERM);
                    return;
                }
                int fileDescriptor=open(args[argCount], O_WRONLY);
                dup2(fileDescriptor,STDOUT_FILENO);
                char *tmp[argCount+1];
                int i=0;
                for(i;i<argCount;i++)
                {
                    tmp[i]=args[i];
                }
                tmp[argCount]=NULL;
                if (execvp(tmp[0],tmp)==-1)	
                {
                    perror("\nERR");
                    close(fileDescriptor);
                    kill(getpid(),SIGTERM);
                    return;
                }
                close(fileDescriptor);
               //kill(getpid(),SIGTERM);
                    //exit(getpid());
            }
            else
            {
                execvp(args[0],args);
                //kill(getpid(),SIGTERM); //args[0] da komutun kendisi var, dizinin devamında tüm parametreler.
            }
        }
        else
        {
            waitpid(pid,NULL,0);
        }
    
}
void processBackground(char **args,int argCount,int type)
{
    struct sigaction sg;
	sg.sa_handler = backgroundSignal;
	sigemptyset(&sg.sa_mask);
	sg.sa_flags = SA_NOCLDSTOP;
	if (sigaction(SIGCHLD,&sg,NULL)<0)
	{
		fprintf(stderr,"ERR\n");
		return;
	}
    pid_t pid;
	int status;
    pid = fork();
    if (pid < 0) 
    { 
        perror("\nERR"); 
        return; 
    } 
    else if (pid == 0) 
    {
        if(type==0)
        {
            if ((access (args[argCount],F_OK) == -1))
	        {	
		        printf("Giriş dosyası(%s) bulunamadı\n",args[argCount]);
		        kill(getpid(),SIGTERM);
                return;
	        }
            int fileDescriptor=open(args[argCount], O_RDONLY, 0600);
            dup2(fileDescriptor,STDIN_FILENO);
            if (execvp(args[0],args)==-1)	
            {
                perror("\nERR");
                close(fileDescriptor);
                kill(getpid(),SIGTERM);
                return;
            }
            close(fileDescriptor);
            kill(getpid(),SIGTERM);
            return;
        }
        else if(type==1)
        {
            if ((access (args[argCount],F_OK) == -1))
	        {	
		        printf("Çıkış dosyası(%s) bulunamadı\n",args[argCount]);
                kill(getpid(),SIGTERM);
		        return;
	        }

            int fileDescriptor=open(args[argCount], O_WRONLY);
            dup2(fileDescriptor,STDOUT_FILENO);
            char *tmp[argCount+1];
            int i=0;
            for(i;i<argCount;i++)
            {
                tmp[i]=args[i];
            }
            tmp[argCount]=NULL;
            if (execvp(args[0],tmp)==-1)	
            {
                perror("\nERR");
                close(fileDescriptor);
                kill(getpid(),SIGTERM);
                return;
            }
            close(fileDescriptor);
            kill(getpid(),SIGTERM);
            return;
        }
        else
        {
            execvp(args[0],args); //args[0] da komutun kendisi var, dizinin devamında tüm parametreler.
            kill(getpid(),SIGTERM);
        }
    }
    else
    {
    }
    return;
}
void backgroundSignal(int signum)
{
    int id,child,status;
	id = waitpid(-1, &status, WNOHANG);
	if (id > 0)
	{
		if (WIFEXITED(status))
	    {
	        child = WEXITSTATUS(status);
	        printf("[%d] retval:%d\n",id, child);
	    }
	}
}