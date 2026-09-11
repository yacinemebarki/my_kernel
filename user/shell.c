#include "user_space.h"
#define COMMAND_SIZE 128


int strcmp(char *str1, char *str2){
    while(*str1 != '\0' && *str1 == *str2){
        str1++;
        str2++;
    }
    return (unsigned char )*str1 - (unsigned char)*str2; 
}



extern unsigned char _binary_user_hello_elf_start[];
extern int command_ready;
extern int command_index;
extern char command[COMMAND_SIZE];

void empty_string(){
    for (int k = 0; k < COMMAND_SIZE; k++)
        command[k] = '\0';
}

void command_exec(char *command){
    if(strcmp(command, "hello") == 0){
        int pid = user_fork();

        if (pid == 0){
            user_exec(_binary_user_hello_elf_start);
            user_exit(1);
        }
        else{
            int status;
            user_wait(&status);
        }
    }
}

void shell(void) {
    write_string("my>>os: ");

    while (1) {
        if (command_ready == 1) {
            command_exec(command);

            command_ready = 0;
            command_index = 0;
            empty_string();

            write_string("my>>os: ");
        }

        user_yield();
    }
}
