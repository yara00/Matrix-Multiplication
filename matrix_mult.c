#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define ROWS_NO 3;
#define COL_NO 3;
const int rows = 3;
const int cols = 3;
int r1, r2, c, c2;
int a[3][3] = {{2,3,1}, {7,4,1}, {9,-2,1}};
int b[3][3] = {{9,-2,-1}, {5,7,3}, {8,1,0}};
int c1[3][3];

struct Array
{
    int row;
    int col;
};

void multiply_per_matrix() {
    int i,j, k;
    int sum = 0;
    for(i=0; i<rows; i++) {
        for(j=0; j<cols; j++) {
            sum = 0;
            for(k=0; k<rows; k++) {
            sum += (a[i][k] * b[k][j]); 
            }
            c1[i][j] = sum;
        }
    }
}

void multiply_per_row(int i) {
    int j, k;
    int sum = 0;
    for(j=0; j<cols; j++) {
        sum = 0;
        for(k=0; k<rows; k++) {
            sum += (a[i][k] * b[k][j]); 
        }
        c1[i][j] = sum;
    }

}

void multiply_per_element(int i, int j) {
    int k;
    int sum = 0;
    for(k=0; k<rows; k++) {
        sum += (a[i][k] * b[k][j]); 
    }
    c1[i][j] = sum;
}

void *routine_per_matrix(void *i) {
    multiply_per_matrix();
    //printf("Test thread\n");
    //int p = (int) (intptr_t) i;
   // printf("i is %d\n", p);
  //  int *p = (int *) i;
   // int threadID = *p;

}
void *routine_per_row(void *i) {
    int row = (int) (intptr_t) i;
    multiply_per_row(row);
}
void *routine_per_element(void *i) {
    struct Array *my_arr = (struct Array*)i;
    multiply_per_element((*my_arr).row, (*my_arr).col);
    free (my_arr);
}

void c_per_matrix() {
    printf("Multiply Per Matrix\n");
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    multiply_per_matrix();
    gettimeofday(&stop, NULL); //end checking time
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    int l,m;
    for(l=0; l<rows; l++) {
        for(m=0; m<cols; m++) {
            printf("%d ", c1[l][m]);
        }
        printf("\n");
    }
    printf("--------------\n");
}
void c_per_row() {
    printf("Multiply Per Row\n");
    int NUM_THREADS = 3;
    pthread_t threads[NUM_THREADS]; 
    int thread;
    int t;
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    for(t=0; t<NUM_THREADS; t++){
   // printf("In main: creating thread %d\n", t);
    thread = pthread_create(&threads[t], NULL, &routine_per_row, (void *) (intptr_t) t);
    if (thread){
    printf("ERROR; return code from pthread_create() is %d\n", thread);
    exit(-1);
    }
    }
    
   int l,m;
    for(t=0; t<NUM_THREADS; t++){
        pthread_join(threads[t], NULL);
        
    }
    gettimeofday(&stop, NULL); //end checking time

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    
    for(l=0; l<rows; l++) {
        for(m=0; m<cols; m++) {
            printf("%d ", c1[l][m]);
        }
        printf("\n");
    }
    printf("--------------\n");
}
void c_per_element() {
    printf("Multiply Per Element\n");
    struct Array *array;
    struct timeval stop, start;
    int NUM_THREADS = 9;
    pthread_t threads[NUM_THREADS]; 
    int r=3;
    int c =3;
    int rc;
    int t;
    int f;
    int num=0;
    gettimeofday(&start, NULL); //start checking time
    for(t=0; t<r; t++){
        for(f=0; f<c; f++) {
            array = malloc(sizeof(struct Array));
            (*array).row = t;
            (*array).col = f;
            rc = pthread_create(&threads[num], NULL, &routine_per_element, (void *) array);
            num++;
            if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
            }
        }
    }
    
   int l,m;
    for(t=0; t<NUM_THREADS; t++){
        pthread_join(threads[t], NULL);
        
    }
    gettimeofday(&stop, NULL); //end checking time

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    
    
    for(l=0; l<rows; l++) {
        for(m=0; m<cols; m++) {
            printf("%d ", c1[l][m]);
        }
        printf("\n");
    }
    printf("--------------\n");
}
/*
void split_string(char *line) {
    const char delimiter[] = "\t";
    char *tmp;

    tmp = strtok(line, delimiter);
    if (tmp == NULL)
    return;

    printf("%s\n", tmp);

    for (;;) {
        tmp = strtok(NULL, delimiter);
        if (tmp == NULL)
            break;
        printf("%s\n", tmp);
    }
}
*/
int main(int argc, char *argv[])
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("/home/yara/Desktop/a.txt", "r");
    if (fp == NULL){
        printf("lol");
        exit(EXIT_FAILURE);
    }
    read = getline(&line, &len, fp);
    char *tmp;
    tmp = strtok(line," ");
    printf("%s\n", tmp);
    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line\n");
        printf("%s", line);
        char *tmp;
        tmp = strtok(line,"\t");
        int count = 4; // columns no - 1
        while(count != 0) {
            tmp = strtok(NULL,"\t");
            printf("%s\n", tmp);
            count--;
        }
        
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
  /*
    // method 1
    c_per_matrix();
    
    // method 2
    c_per_row();

   // method 3
    c_per_element();
    */
    return 0;
}
