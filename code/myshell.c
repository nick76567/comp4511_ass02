#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>


#define MAX_PROGRAM_NAME_LEN 128
#define MAX_CMDLINE_LEN 256
#define MAX_OPTION_LEN ((MAX_CMDLINE_LEN - MAX_PROGRAM_NAME_LEN) / 2 - 2)
#define MAX_ARG_ADDRERSS_LEN (MAX_CMDLINE_LEN - MAX_OPTION_LEN)
#define MAX_DIR_LEN 512


/* function prototypes go here... */

void show_prompt();
int get_cmd_line(char *cmdline);
void process_cmd(char *cmdline);


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


void process_cmd(char *cmdline)
{
	// printf("%s\n", cmdline);
    char *token, program_name[MAX_PROGRAM_NAME_LEN], option[MAX_OPTION_LEN];
    char arg_address[MAX_ARG_ADDRERSS_LEN];
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
        }else if(token[0] == '/'){
            strcpy(arg_address, token);
        }
        token = strtok(NULL, " \t");
    }

    printf("Program_name: %s\n", program_name);
    printf("Option: %s\n", option);
    printf("arg_address: %s\n", arg_address);
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