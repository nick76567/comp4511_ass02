#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#define MAX_PROGRAM_NAME_LEN 128
#define MAX_CMDLINE_LEN 256
#define MAX_OPTION_LEN ((MAX_CMDLINE_LEN - MAX_PROGRAM_NAME_LEN) / 2 - 2)
#define MAX_ARG_ADDRERSS_LEN (MAX_CMDLINE_LEN - MAX_OPTION_LEN)
#define MAX_DIR_LEN 256
#define MAX_ENV_PATH_LEN 1024


/* function prototypes go here... */

void show_prompt();
int get_cmd_line(char *cmdline);
void process_cmd(char *cmdline);

unsigned int input_arg_handler(char *cmdline, char *program_name, char *option, char *arg_address);
bool search_directory(char *program_name, char *directory);
bool search_env_path_directory(char *program_name);
void create_child(char *time);


/* The main function implementation */
int main()
{
	char cmdline[MAX_CMDLINE_LEN];
	
	while (1) 
	{
		show_prompt();
		if ( get_cmd_line(cmdline) == -1 )
			continue; /* empty line handling */
		
		process_cmd(cmdline);
	}
	return 0;
}

unsigned int input_arg_handler(char *cmdline, char *program_name, char *option, char *arg_address){
    char *token;
    unsigned int option_num = 0;
    token = strtok(cmdline, " \t");
    strcpy(program_name, token);
    token = strtok(NULL, " \t");

    while(token != NULL){
        if(token[0] == '-'){
            option_num += (strlen(token) - 1);
            strcat(option, token + 1);
        }else if(token[0] == '&'){
            //background process
        }else{
            strcpy(arg_address, token);
        }
        token = strtok(NULL, " \t");
    }

    return option_num;
}

bool search_directory(char *program_name, char *directory){
    char buffer[MAX_DIR_LEN];

    if(directory == NULL){
        getcwd(buffer, MAX_DIR_LEN);
    }else{
        strcpy(buffer, directory);
    }
    
    struct dirent **namelist;
    int n;
    bool result = false;

    n = scandir(buffer, &namelist, NULL, NULL);
    if(n >= 0){
        while(n--){
            if(strcmp(namelist[n]->d_name, program_name) == 0) result = true;
            free(namelist[n]);
        }
        free(namelist);     
    }  
    return result;
}

bool search_env_path_directory(char *program_name){
    char env_path_buffer[MAX_ENV_PATH_LEN];
    strcpy(env_path_buffer, getenv("PATH"));

    char *token = strtok(env_path_buffer, ":");
    
    while(token != NULL){
        if(search_directory(program_name, token)){
            //printf("Found in %s\n", token);
            return true;
        }
        token = strtok(NULL, ":");
    }  
    return false;
}

void create_child(char *time){
    int status;
    pid_t pid = fork();

    if(pid > 0){
        printf("child pid %d is started\n", pid);
        wait(&status);
        printf("child pid %d is terminated with status %d\n", pid, status);
    }else if(pid == 0){
        sleep(atoi(time));
        exit(0);
    }else{
        printf("Error\n");
    }

}

void process_cmd(char *cmdline)
{
    char program_name[MAX_PROGRAM_NAME_LEN], option[MAX_OPTION_LEN];
    char arg_address[MAX_ARG_ADDRERSS_LEN] = {0};
    unsigned int option_num = input_arg_handler(cmdline, program_name, option, arg_address);
    
    if(strcmp(program_name, "exit") == 0){
        exit(0);
    } else if(strcmp(program_name, "cd") == 0){
        if(chdir(arg_address) == -1) printf("Path not found\n");
    } else if(strcmp(program_name, "child") == 0){
        create_child(arg_address);
    } else if(!search_directory(program_name, NULL) && !search_env_path_directory(program_name)){
        printf("%s: Command not found.\n", program_name);
    }
}


void show_prompt() 
{   
    char buffer[MAX_DIR_LEN], *print_directory;
    getcwd(buffer, MAX_DIR_LEN);
    print_directory = strrchr(buffer, '/');
    
	printf("[%s] myshell> ", print_directory + 1);
}

int get_cmd_line(char *cmdline) 
{
    int i;
    int n;
    if (!fgets(cmdline, MAX_CMDLINE_LEN, stdin))
        return -1;
    // Ignore the newline character
    n = strlen(cmdline);
    cmdline[--n] = '\0';
    i = 0;
    while (i < n && cmdline[i] == ' ')
    {
        ++i;
    }
    if (i == n)
    {
        // Empty command
        return -1;
    }
    return 0;
}