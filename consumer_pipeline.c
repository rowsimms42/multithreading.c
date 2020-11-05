#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#define MAX 1000
// gcc -std=gnu99 -o t temp_cp2.c
//./t < input1.txt

//char buffer[MAX]; //shared resource
//ssize_t res = getline(&line, &n, stdin);

struct holder {
    char buff_in[80];
    char buffer[MAX];
    char new_string[MAX];
    int count;
};

void replace_plus_sign(struct holder* vars){
    char sym = '^';
    char* ptr = NULL;
    ptr = &sym;
    int i = 0;
    int adj_counter = 0;
    int c = vars->count;

    while (i < c) {
        char c = vars->buffer[i];
        if (vars->buffer[i] == '+') {
            char d = vars->buffer[i + 1];
            if (d == '+') {
                strncat(vars->new_string, ptr, 1);
                i++;
                adj_counter++;
            } else {
                strncat(vars->new_string, &c, 1);
            }
        } else {
            strncat(vars->new_string, &c, 1);
        }
        i++;
    }
    vars->count = c - adj_counter;
}

void line_separator(struct holder* vars){
    int i = 0;
    while (vars->buffer[i] != 0) {
        if (vars->buffer[i] == '\n'){
            vars->buffer[i] = ' ';
        }
        i++;
    }
    vars->count = i;
}

int get_user_input(struct holder* vars){
    fgets(vars->buffer, MAX, stdin);
    char* stop = "STOP";
    int size = strlen(stop)+2;
    char new_str[size];
    strcpy(new_str, stop);
    strcat(new_str, "\n");
    int i = 0;

    while (vars->buffer[i]!=0){ //vars->buffer[i]!=0
        if (strcmp(new_str, vars->buffer) == 0)
            return 1;
        i++;
    }
    printf("i: %d\n", i);

    return 0;
}

int main(int argc, char *argv[]){
    struct holder* vars = malloc(sizeof(struct holder));
    while(1){
        if (get_user_input(vars) == 1){
            break;
        }
        //printf("%s", buffer);
        line_separator(vars);
        //printf("%i", counter);
        replace_plus_sign(vars);
        // output_lines(new_string, &counter);
    }
    printf("%s\n", vars->buffer);
    printf("%s\n", vars->new_string);
    return 0;
}
