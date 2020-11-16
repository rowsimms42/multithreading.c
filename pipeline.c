/* Rowan Simmons */
/* November 16, 2020 */

/* gcc -std=gnu99 -pthread -o pipe consumer_pipeline.c*/
/*./pipe < input3.txt (for file)*/
/*./pipe (input through terminal)*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h> // must link with -lm
#include <string.h>
#define SIZE 1000
#define NUM_ITEMS 49

/*shared resources with input and line sep thread*/
char buffer_1[NUM_ITEMS][SIZE];
int count_1 = 0;
int prod_idx_1 = 0;
int con_idx_1 = 0;
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;
/*shared resources with line sep and replace plus thread*/
char buffer_2[NUM_ITEMS][SIZE];
int count_2 = 0;
int prod_idx_2 = 0;
int con_idx_2 = 0;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;
/*shared resources with replace plus and output thread*/
char buffer_3[NUM_ITEMS][SIZE];
int count_3 = 0;
int prod_idx_3 = 0;
int con_idx_3 = 0;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

/*function gets output from user, either through command line or file.*/
void *get_input(void *args)
{
    char line[SIZE];
    char* stop = "STOP";
    size_t size = strlen(stop)+2;
    char stop_str[size];
    strcpy(stop_str, stop);
    strcat(stop_str, "\n");
    char sig_stp[] = "~";
    int i;
    int exiting = 0;
    int x = 0;

    /*loop until program receives STOP signal*/
    for(i=0; i<NUM_ITEMS; i++)
    {
        fgets(line, SIZE, stdin);
        if (strcmp(line, stop_str) == 0){ /*STOP received*/
            x = 1;
            exiting++;
        }
        pthread_mutex_lock(&mutex_1);
        if (x == 1)
            strncpy(buffer_1[prod_idx_1], sig_stp, strlen(sig_stp));
        else /*copy line to shared buffer*/
            strncpy(buffer_1[prod_idx_1], line, strlen(line));
        prod_idx_1++;
        count_1++;
        pthread_cond_signal(&full_1); /*signal that buffer was filled*/
        pthread_mutex_unlock(&mutex_1);
        if (exiting == 1){
            break; /*exit loop*/
        }
        x = 0;
    }
    return NULL;
}

/*function gets input from input thread and replaces '\n' with ' '.*/
void *line_separator(void *args)
{
    char sig_stp[] = "~";
    int exiting = 0;
    int i;

    /*loop until signal is received*/
    for (i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex_1);
        while (count_1 == 0)
            pthread_cond_wait(&full_1, &mutex_1);
        char* line = buffer_1[con_idx_1];
        con_idx_1++;
        count_1--;
        pthread_mutex_unlock(&mutex_1);
        if (strcmp(line, sig_stp) == 0){
            exiting++;
        }
        /*loop through line and replace new line with space*/
        int j;
        int k;
        for (k=0;k<NUM_ITEMS;k++)
        {
            for (j=0;j<SIZE;j++)
            {
                if (*(line+j+k) == '\n'){
                    *(line+j+k) = ' ';
                }
            }
        }
        pthread_mutex_lock(&mutex_2);
        /*copy new line to shared buffer*/
        strcpy(buffer_2[prod_idx_2], line);
        prod_idx_2++;
        count_2++;
        /*signal that buffer was filled*/
        pthread_cond_signal(&full_2);
        pthread_mutex_unlock(&mutex_2);

        if (exiting > 0){
            break;/*exit loop*/
        }
    }
    return NULL;
}

/*function replaces '++' with '^'.*/
void *replace_plus_sign(void *args)
{
    char sig_stp[] = "~";
    int exiting = 0;
    char sym = '^';
    char *ptr = NULL;
    ptr = &sym;
    int i;
    char* new_str = calloc(1000, sizeof(char));

    /*loop until stop signal is received*/
    for (i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex_2);
        while (count_2 == 0)
            pthread_cond_wait(&full_2, &mutex_2);
        char* line = buffer_2[con_idx_2];
        con_idx_2++;
        count_2--;
        pthread_mutex_unlock(&mutex_2);
        if (strcmp(line, sig_stp) == 0){
            exiting++;
        }
        /*traverse line and replace characters*/
        int j = 0;
        int k;
        for (k = 0; k < SIZE; k++)
        {
            char c = *(line + j);
            if (c == '+') {
                if (*(line + (j + 1)) == '+') {
                    strncat(new_str, ptr, 1); /*add replaced char*/
                    j++;
                } else {
                    if (c!=0) {
                        strncat(new_str, &c, 1); /*add existing char*/
                    }
                }
            }
            else {
                if (c!=0) {
                    strncat(new_str, &c, 1); /*add existing char*/
                }
            }
            j++;
        }
        pthread_mutex_lock(&mutex_3);
        /*copy new string to shared buffer*/
        strncpy(buffer_3[prod_idx_3], new_str, strlen(new_str));
        prod_idx_3++;
        count_3++;
        /*signal that buffer was filled*/
        pthread_cond_signal(&full_3);
        pthread_mutex_unlock(&mutex_3);
        /*reset string to 0*/
        memset(new_str, 0, strlen(new_str));

        if (exiting > 0){
            break; /*exit loop*/
        }
    }
    free(new_str);
    return NULL;
}

/*function to write output of exactly 80 characters plus new line.*/
void *write_output(void *args){
    char sig_stp[] = "~";
    int exiting = 0;
    int i;
    char* str_out = calloc(1000, sizeof(char));
    char* ptr = str_out;

    /*loop until stop signal is received*/
    for (i = 0; i < NUM_ITEMS; i++) {
        char* line = NULL;
        pthread_mutex_lock(&mutex_3);
        while (count_3 == 0)
            pthread_cond_wait(&full_3, &mutex_3);
        line = buffer_3[con_idx_3];
        con_idx_3++;
        count_3--;
        pthread_mutex_unlock(&mutex_3);

        if (strcmp(line, sig_stp) == 0){
            exiting++;
        }
        /*only add lines that aren't exiting signal to output string*/
        if (exiting == 0) {
            strncat(str_out, line, strlen(line));
        }

        /*output string and add offset to string*/
        while (strlen(str_out)>79){
            printf("%.*s\n", 80, str_out);
            str_out = str_out+80;
        }

        /*if STOP was received, output last 80 characters if possible and exit*/
        if (exiting > 0){
            if (strlen(str_out) > 79){
                printf("%.*s\n", 80, str_out);
            }
            break;
        }
    }
    free(ptr);
    return NULL;
}

/*main function. create and join 4 threads, and wait for threads to terminate.*/
int main()
{
    pthread_t input_t, line_sep_t, replace_plus_t, output_t;
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&line_sep_t, NULL, line_separator, NULL);
    pthread_create(&replace_plus_t, NULL, replace_plus_sign, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);
    pthread_join(input_t, NULL);
    pthread_join(line_sep_t, NULL);
    pthread_join(replace_plus_t, NULL);
    pthread_join(output_t, NULL);
    return EXIT_SUCCESS;
}
