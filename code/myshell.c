#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

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

void handle_sigchld(int sig);
void input_arg_handler(char *cmdline, char *program_name, char *option, char *arg_address, char *background);
//bool search_directory(char *program_name, char *directory);
//bool search_env_path_directory(char *program_name);
void create_child(char *time);
void create_linux_program_child(char *program_name, char *option, char *arg_address, char *background);

/* The main function implementation */
int main()
{
	char cmdline[MAX_CMDLINE_LEN];
	
	//reference: http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
	struct sigaction sa;
	sa.sa_handler = &handle_sigchld;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, 0) == -1) {
	  perror(0);
	  exit(1);
	}

	while (1) 
	{
		show_prompt();
		if ( get_cmd_line(cmdline) == -1 )
			continue; /* empty line handling */
		
		process_cmd(cmdline);

	}
	return 0;
}

//reference: http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
void handle_sigchld(int sig) {
  int saved_errno = errno;
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
  errno = saved_errno;
}

void input_arg_handler(char *cmdline, char *program_name, char *option, char *arg_address, char *background){

    char *token;
    token = strtok(cmdline, " \t");
    strcpy(program_name, token);
    token = strtok(NULL, " \t");

    while(token != NULL){
        if(token[0] == '-'){
            if(option[0] == '\0') option[0] = '-';
            strcat(option, token + 1);
        }else if(token[0] == '&'){
            *background = '&';
        }else{
            strcpy(arg_address, token);
        }
        token = strtok(NULL, " \t");
    }
}
/*
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
*/
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

void create_linux_program_child(char *program_name, char *option, char *arg_address, char *background){
    pid_t pid = fork();

    if(pid > 0){
    	if(*background == '\0'){
        	wait(0);
    	}
 
    }else if(pid == 0){
        int result = -1;
        if(arg_address[0] == '\0' && option[0] == '\0'){
            result = execlp(program_name, program_name, (char *)NULL);
        }else if(arg_address[0] == '\0'){
            result = execlp(program_name, program_name, option, (char *)NULL);
        }else if(option[0] == '\0'){
            result = execlp(program_name, program_name, arg_address, (char *)NULL);
        }else{
            result = execlp(program_name, program_name, option, arg_address, (char *)NULL);
        }

        if(result < 0){
            printf("%s: Command not found.\n", program_name);
        }
        exit(0); 
    }else{
        printf("Error.\n");
    }

}

void process_cmd(char *cmdline)
{
    char program_name[MAX_PROGRAM_NAME_LEN] = {0}, option[MAX_OPTION_LEN] = {0};
    char arg_address[MAX_ARG_ADDRERSS_LEN] = {0}, background = '\0';
    input_arg_handler(cmdline, program_name, option, arg_address, &background);

    if(strcmp(program_name, "exit") == 0){
        exit(0);
    }else if(strcmp(program_name, "cd") == 0){
        if(chdir(arg_address) == -1) printf("Path not found\n");
    }else if(strcmp(program_name, "child") == 0){
        create_child(arg_address);
    }else{
        create_linux_program_child(program_name, option, arg_address, &background);
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