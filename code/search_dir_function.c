#include <stdbool.h>

bool search_directory(char *program_name, char *directory);
bool search_env_path_directory(char *program_name);


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
