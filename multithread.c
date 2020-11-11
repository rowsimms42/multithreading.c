#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <pthread.h>
#define MAX 1000
//gcc -std=gnu99 -pthread -o t temp_cp2.c
//./t < input3.txt
int count = 0;

char buffer_1[MAX]; // shared between input and line sep
int count_1 = 0; // Number of items in the buffer
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;// Initialize the mutex for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;// Initialize the condition variable for buffer 1
//buffer 2
char buffer_2[MAX]; // shared between line sep and replace char
int count_2 = 0; // Number of items in the buffer
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;// Initialize the mutex for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;// Initialize the condition variable for buffer 2
//buffer 3
char buffer_3[MAX]; // shared between replace char and output
int count_3 = 0; // Number of items in the buffer
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;// Initialize the mutex for buffer 3
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;// Initialize the condition variable for buffer 3

struct holder {
    char buffer[MAX];
    char share_buff[MAX];
};

void put_buff_3(char* line){
    pthread_mutex_lock(&mutex_3);
    strcpy(buffer_3, line);
    count_3++;
    pthread_cond_signal(&full_3);
    pthread_mutex_unlock(&mutex_3);
}

char* get_buff_3(){
    pthread_mutex_lock(&mutex_3);
    while (count_3 == 0)
        pthread_cond_wait(&full_3, &mutex_3);
    char* line = buffer_3;
    count_3--;
    pthread_mutex_unlock(&mutex_3);
    return line;
}

void put_buff_2(char* line){
    pthread_mutex_lock(&mutex_2);
    strcpy(buffer_2, line);
    count_2++;
    pthread_cond_signal(&full_2);
    pthread_mutex_unlock(&mutex_2);
}

char* get_buff_2(){
    pthread_mutex_lock(&mutex_2);
    while (count_2 == 0)
        pthread_cond_wait(&full_2, &mutex_2);
    char* line = buffer_2;
    count_2--;
    pthread_mutex_unlock(&mutex_2);
    return line;
}

void put_buff_1(char* line){
    pthread_mutex_lock(&mutex_1);
    strcpy(buffer_1, line);
    count_1++;
    pthread_cond_signal(&full_1);
    pthread_mutex_unlock(&mutex_1);
}

char* get_buff_1(){
    pthread_mutex_lock(&mutex_1);
    while (count_1 == 0)
        pthread_cond_wait(&full_1, &mutex_1);
    char* line = buffer_1;
    count_1--;
    pthread_mutex_unlock(&mutex_1);
    return line;
}

void* output_data(void* args){
    char* output = get_buff_3();
    int i = 80;
    int j = 0;
    int counter = 0;

    while (1){
        printf ("%.*s\n", i, output + j);
        counter++;
        j = i * counter;
        if ((i+j) > count)
            break;
    }

    return NULL;
}

void* replace_plus_sign(void* args){
    char* line = get_buff_2();
    char* new_str = malloc(sizeof(line));
    char sym = '^';
    char* ptr = NULL;
    ptr = &sym;
    int i = 0;
    int adj_counter = 0;
    int c = count;

    while (i < c) {
        char c = line[i];
        if (line[i] == '+') {
            char d = line[i + 1];
            if (d == '+') {
                strncat(new_str, ptr, 1);
                i++;
                adj_counter++;
            } else {
                strncat(new_str, &c, 1);
            }
        } else {
            strncat(new_str, &c, 1);
        }
        i++;
    }
    count = c - adj_counter;
    put_buff_3(new_str);

    return NULL;
}

void* line_separator(void* args){
    char* line = get_buff_1();
    int i = 0;
    while (line[i] != 0) {
        if (line[i] == '\n'){
            line[i] = ' ';
        }
        i++;
    }
    count = i;
    put_buff_2(line);

    return NULL;
}

void* get_user_input(void* args){
    struct holder* vars = malloc(sizeof(struct holder));
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
    put_buff_1(vars->share_buff);

    return NULL;
}

int main(int argc, char *argv[]){
    pthread_t input_t, line_sep_t, char_replace_t, output_t;
    // Create the threads
    pthread_create(&input_t, NULL, get_user_input, NULL);
    pthread_create(&line_sep_t, NULL, line_separator, NULL);
    pthread_create(&char_replace_t, NULL, replace_plus_sign, NULL);
    pthread_create(&output_t, NULL, output_data, NULL);
    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(line_sep_t, NULL);
    pthread_join(char_replace_t, NULL);
    pthread_join(output_t, NULL);

    return 0;
}
