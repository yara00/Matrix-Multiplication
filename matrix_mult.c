#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#define ROWS_NO 3;
#define COL_NO 3;
const int rows = 3;
const int cols = 3;
int r1;
int r2;
int c1;
int c2;
int **a, **b, **c;
struct Array
{
    int row;
    int col;
};

void multiply_per_matrix() {
    int i,j, k;
    int sum = 0;
    for(i=0; i<r1; i++) {
        for(j=0; j<c2; j++) {
            sum = 0;
            for(k=0; k<r2; k++) {
            sum += (a[i][k] * b[k][j]); 
            }
            c[i][j] = sum;
        }
    }
}

void multiply_per_row(int i) {
    int j, k;
    int sum = 0;
    for(j=0; j<c2; j++) {
        sum = 0;
        for(k=0; k<r2; k++) {
            sum += (a[i][k] * b[k][j]); 
        }
        c[i][j] = sum;
    }

}

void multiply_per_element(int i, int j) {
    int k;
    int sum = 0;
    for(k=0; k<r2; k++) {
        sum += (a[i][k] * b[k][j]); 
    }
    c[i][j] = sum;
}
void *routine_per_row(void *i) {
    long row = (long) i;
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
    for(l=0; l<r1; l++) {
        for(m=0; m<c2; m++) {
            printf("%d ", c[l][m]);
        }
        printf("\n");
    }
    printf("--------------\n");
}

void c_per_row() {
    printf("Multiply Per Row\n");
    int NUM_THREADS = r1;
    pthread_t threads[NUM_THREADS]; 
    int thread;
    int t;
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    for(t=0; t<NUM_THREADS; t++){
        thread = pthread_create(&threads[t], NULL, &routine_per_row, (void *) t);
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
    
    for(l=0; l<r1; l++) {
        for(m=0; m<c2; m++) {
            printf("%d ", c[l][m]);
        }
        printf("\n");
    }
    printf("--------------\n");
}

void c_per_element() {
    printf("Multiply Per Element\n");
    struct Array *array;
    struct timeval stop, start;
    int NUM_THREADS = r1 * c2;
    pthread_t threads[NUM_THREADS]; 
    int rc;
    int t;
    int f;
    int num=0;
    gettimeofday(&start, NULL); //start checking time
    for(t=0; t<r1; t++){
        for(f=0; f<c2; f++) {
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
    
    
    for(l=0; l<r1; l++) {
        for(m=0; m<c2; m++) {
            printf("%d ", c[l][m]);
        }
        printf("\n");
    }
    printf("--------------\n");
}

void readInput(FILE *fp, int **matrix, int row, int col) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            int number;
            fscanf(fp, "%d", &number);
            matrix[i][j] = number;
        }
    }
    fclose(fp);
}
int main(int argc, char *argv[])
{
    FILE * fp;
    fp = fopen("/home/yara/Desktop/a.txt", "r");
    if (fp == NULL){
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }
    fscanf(fp, "row=%d col=%d", &r1, &c1);
    a = (int **)malloc(r1 * sizeof(int *));
    for (int i=0; i<r1; i++) {
        a[i] = (int *)malloc(c1 * sizeof(int));
    }
    readInput(fp, a, r1, c1);
    
    fp = fopen("/home/yara/Desktop/b.txt", "r");
    if (fp == NULL){
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }
    fscanf(fp, "row=%d col=%d", &r2, &c2);
    b = (int **)malloc(r2 * sizeof(int *));
    for (int i=0; i<r2; i++) {
        b[i] = (int *)malloc(c2 * sizeof(int));
    }
    readInput(fp, b, r2, c2);
    int l,m;
    for(l=0; l<r1; l++) {
        for(m=0; m<c1; m++) {
            printf("%d ", a[l][m]);
        }
        printf("\n");
    }

    for(l=0; l<r2; l++) {
        for(m=0; m<c2; m++) {
            printf("%d ", b[l][m]);
        }
        printf("\n");
    }


    c = (int **)malloc(r1 * sizeof(int *));
    for (int i=0; i<r1; i++) {
        c[i] = (int *)malloc(c2 * sizeof(int));
    }



    FILE *fp2;
    // method 1
    c_per_matrix();
    fp2 = fopen ("c_per_matrix.txt", "a");
    int x,y;
    fputs("Method: A thread per matrix\n", fp2);
    fprintf(fp2, "row=%d\tcol=%d\n", r1,c2);
    for(x=0; x<r1; x++) {
        for(y=0; y<c2; y++) {
            fprintf(fp2,"%d", c[x][y]);
            fprintf(fp2, "\t"); 
        }
        fprintf(fp2, "\n");
    }
    
    fclose(fp2);
    
    // method 2
    c_per_row();
    fp2 = fopen ("c_per_row.txt", "a");
    fputs("Method: A thread per row\n", fp2);
    fprintf(fp2, "row=%d\tcol=%d\n", r1,c2);
    for(x=0; x<r1; x++) {
        for(y=0; y<c2; y++) {
            fprintf(fp2,"%d", c[x][y]);
            fprintf(fp2, "\t"); 
        }
        fprintf(fp2, "\n");
    }
    
    fclose(fp2);

   // method 3
    c_per_element();
    fp2 = fopen ("c_per_element.txt", "a");
    fputs("Method: A thread per element\n", fp2);
    fprintf(fp2, "row=%d\tcol=%d\n", r1,c2);
    for(x=0; x<r1; x++) {
        for(y=0; y<c2; y++) {
            fprintf(fp2,"%d", c[x][y]);
            fprintf(fp2, "\t"); 
        }
        fprintf(fp2, "\n");
    }
    
    fclose(fp2);
    return 0;
}
