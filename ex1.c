#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pwd.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#define SIZE 510
#define MIN_ARGSV 2
#define CHARS_TO_DELETE " \n"


int total_chars_in_cmd = 0, total_commands=0;

void commandLoop();
void getPath();
char* commandLine();
char** splitCommand(char* command);
void exec_launch(char** args);
void printSummary(double aver);
void free_memory(char** args,char* command, char* temp_cmd);

int main() 
{
    commandLoop();
    return 0;
}

void commandLoop() {
    char *command;
    char **args;
    double average=0;

    while(1){
        
        getPath();
        command = commandLine();       //command the ommand from the user
        if(command[0]=='\n'){
            continue;
        }
        total_chars_in_cmd+=strlen(command);
        total_commands++;
        char *temp_cmd = (char*)malloc((strlen(command)+1)*sizeof(char));
        assert(temp_cmd!=NULL);
        strncpy(temp_cmd,command,(strlen(command)+1)); //duplicate the command to prevent changes from using strtok
        args = splitCommand(temp_cmd);
        if (strcmp(args[0], "done") == 0) {
            average = (double)total_chars_in_cmd/(double)total_commands;
            printSummary(average);
            free_memory(args,command,temp_cmd);
            break;
        }else if(strcmp(args[0], "cd") == 0){
            printf("Comand not supperted (Yet)\n");
            free_memory(args,command,temp_cmd);
            continue;

        } else {
            exec_launch(args); 
        }
        
        free_memory(args,command,temp_cmd);   
    }
    
}


void getPath() {
    struct passwd *pws;
    long size;
    char *buf;
    char *ptr;
    uid_t uid=0; //for "root" user
    size = pathconf(".", _PC_PATH_MAX);
        if ((buf = (char*)malloc((size_t)size)) != NULL){
            ptr = getcwd(buf, (size_t)size);
        }
        pws = getpwuid(uid);
        //pws = getpwuid(geteuid()); // for the real user name
        printf("%s@%s>", pws->pw_name, ptr);
        free(buf);
}

char* commandLine() {
    char *line = NULL;
    ssize_t size = SIZE;
    getline(&line,&size, stdin); 
    return line;
}

char** splitCommand(char* command) {
    int args_size = MIN_ARGSV, position = 0;
    char **words = (char**)malloc(args_size * sizeof(char*));
    assert(words != NULL);
    char *word;

    word = strtok(command,CHARS_TO_DELETE); //cut the first word from cmd

    while(word!=NULL){
        words[position]=word;
        position++;
        args_size++;
        words=(char**)realloc(words,args_size*sizeof(char*));
        assert(words!=NULL);
        word = strtok(NULL,CHARS_TO_DELETE);// continue cutting each word
    }

    words[position]=NULL; // the last args->NULL(for exec func)
    
    return words;
}

void exec_launch(char** args){
    pid_t pid;
    pid = fork();
    /*child process*/
    if(pid==0){
       if(execvp(args[0],args)==-1){
           //perror("excec faild");
           exit(EXIT_FAILURE);
       }
       exit(EXIT_FAILURE);
    } else if(pid<0){
        //error forking
        //perror("forking faild");
        exit(EXIT_FAILURE);
     }else{
         wait(NULL);  
     }
}

void printSummary(double aver){
     aver = (double)total_chars_in_cmd/(double)total_commands;
     printf("Num of commands: %d\nTotal length of all commands: %d\n Average length of all commands: %lf\n See you Next time !\n"
     ,total_commands,total_chars_in_cmd,aver);

}

void free_memory(char** args,char* command, char* temp_cmd){
    free(args);
    free(command);
    free(temp_cmd);
}









