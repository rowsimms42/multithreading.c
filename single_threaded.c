#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#define MAX 1000
// gcc -std=gnu99 -o t temp_cp2.c
//./t < input3.txt

struct holder {
    char buffer[MAX];
    char buffer_string[MAX];
    char new_string[MAX];
    char share_buff[MAX];
    int count;
};

void output_data(struct holder* vars){
    char* output = vars->new_string;
    int i = 80;
    int j = 0;
    int counter = 0;

    while (1){
        printf ("%.*s\n", i, output + j);
        counter++;
        j = i * counter;
        if ((i+j) > vars->count)
            break;
    }
}

void replace_plus_sign(struct holder* vars){
    char sym = '^';
    char* ptr = NULL;
    ptr = &sym;
    int i = 0;
    int adj_counter = 0;
    int c = vars->count;

    while (i < c) {
        char c = vars->share_buff[i];
        if (vars->share_buff[i] == '+') {
            char d = vars->share_buff[i + 1];
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
    while (vars->share_buff[i] != 0) {
        if (vars->share_buff[i] == '\n'){
            vars->share_buff[i] = ' ';
        }
        i++;
    }
    vars->count = i;
}

void get_user_input(struct holder* vars){
    char* stop = "STOP";
    int size = strlen(stop)+2;
    char new_str[size];
    strcpy(new_str, stop);
    strcat(new_str, "\n");

    while(1){
        fgets(vars->buffer, MAX, stdin);
        if (strcmp(new_str, vars->buffer) == 0)
            break;
        strncat(vars->share_buff, vars->buffer, strlen(vars->buffer));
    }
}

int main(int argc, char *argv[]){
    struct holder* vars = malloc(sizeof(struct holder));
    get_user_input(vars);
    line_separator(vars);
    replace_plus_sign(vars);
    output_data(vars);

    return 0;
}
