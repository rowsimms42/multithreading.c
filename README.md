# multithreading.c

Creates 4 threads to process input from standard input as follows:

Thread 1 reads in lines of characters from the standard input.

Thread 2 replaces every line separator in the input by a space.

Thread replaces every pair of plus signs, i.e., "++", by a "^".

Thread 4 writes the processed data to standard output as lines of exactly 80 characters.

The four threads communicate with each other using the Producer-Consumer approach.

-----------------------------------------------------------------------------------------

Compile and Run: 

gcc -std=gnu99 -pthread -o pipe consumer_pipeline.c

./pipe
