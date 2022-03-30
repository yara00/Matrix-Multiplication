#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
int r1;
int r2;
int c1;
int c2;
int **a, **b, **c;
// array struct holding row and column values
struct Array
{
    int row;
    int col;
};
// function to perfom matrix multiplication
void multiply_per_matrix() {
    int i,j, k;
    int sum = 0;
    for(i=0; i<r1; i++) {
        for(j=0; j<c2; j++) {
            c[i][j] = 0; // reset element
            sum = 0;
            for(k=0; k<r2; k++) {
            sum += (a[i][k] * b[k][j]); 
            }
            c[i][j] = sum; // set element
        }
    }
}
// function to perform row multiplication
void multiply_per_row(int i) {
    int j, k;
    int sum = 0;
    for(j=0; j<c2; j++) {
        c[i][j] = 0; // reset element
        sum = 0;
        for(k=0; k<r2; k++) {
            sum += (a[i][k] * b[k][j]); 
        }
        c[i][j] = sum; // set element
    }
}
// function to perform element multiplication
void multiply_per_element(int i, int j) {
    c[i][j] = 0; // reset element
    int k;
    int sum = 0;
    for(k=0; k<r2; k++) {
        sum += (a[i][k] * b[k][j]); 
    }
    c[i][j] = sum; // set element
}
// function to be called on row thread creation
void *routine_per_row(void *i) {
    long row = (long) i;
    multiply_per_row(row); // perform multiplication
}
// function to be called on element thread creation
void *routine_per_element(void *i) {
    struct Array *my_arr = (struct Array*)i;
    multiply_per_element((*my_arr).row, (*my_arr).col); // perform multiplication
    free(my_arr); // free struct pointer
}
// function to execute matrix thread
void c_per_matrix() {
    printf("Multiply Per Matrix\n");
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    multiply_per_matrix(); // perform multiplication
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of threads: 1\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
}
// fucntion to create row threads
void c_per_row() {
    printf("Multiply Per Row\n");
    int threadsNum = r1; // set no of threads
    pthread_t threads[threadsNum]; 
    int thread;
    int r;
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    for(r=0; r<threadsNum; r++){
        if(pthread_create(&threads[r], NULL, &routine_per_row, (void *) (unsigned long long) r)) {
            printf("ERROR! Couldn't create a thread!!");
            exit(0);
        }
    }
    for(r=0; r<threadsNum; r++){
        pthread_join(threads[r], NULL);
    }
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of threads: %d\n", threadsNum);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    
}
// function to create element threads
void c_per_element() {
    printf("Multiply Per Element\n");
    struct Array *array;
    struct timeval stop, start;
    int threadsNum = r1 * c2; // set no of threads
    pthread_t threads[threadsNum]; 
    int r;
    int c;
    int num=0;
    gettimeofday(&start, NULL); //start checking time
    for(r=0; r<r1; r++){
        for(c=0; c<c2; c++) {
            array = malloc(sizeof(struct Array)); // allocate array struct
            (*array).row = r; // set array elements
            (*array).col = c;
            if(pthread_create(&threads[num], NULL, &routine_per_element, (void *) array)) {
                printf("ERROR! Couldn't create a thread!!");
                exit(0);
            }
            num++;
        }
    }
    
    for(r=0; r<threadsNum; r++){
        pthread_join(threads[r], NULL);
    }
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of threads: %d\n", threadsNum);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    
}

// function to read input file and write its content to matrix
void readInput(FILE *fp, int **matrix, int row, int col) { 
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            int number;
            fscanf(fp, "%d", &number); // read all ints and skip spaces, tabs and new lines
            matrix[i][j] = number; // insert matrix elements
        }
    }
    fclose(fp); // close file
}

int main(int argc, char *argv[])
{
    char *fileName[100];
    char queue[100];
    char queue2[100];
    char queue3[100];
    
    if(argc < 2) { // default arguments
        strcpy(queue, "a.txt");
        strcpy(queue2, "b.txt");
        strcpy(queue3, "c");
     
    }
    else { // input arguments
        fileName[0] =  argv[1];
        strcpy(queue, fileName[0]);
        strcat(queue, ".txt");
 
        fileName[1] = argv[2];
        strcpy(queue2, fileName[1]);
        strcat(queue2, ".txt");

        fileName[2] = argv[3];
        strcpy(queue3, fileName[2]);   
    }


    // allocate and initialize array a
    FILE *fp;
    fp = fopen(queue, "r");
    if (fp == NULL){
        printf("Error opening file!");
        exit(0);
    }
    fscanf(fp, "row=%d col=%d", &r1, &c1); // read row and column values
    a = (int **)malloc(r1 * sizeof(int *));
    for (int i=0; i<r1; i++) {
        a[i] = (int *)malloc(c1 * sizeof(int));
    }
    readInput(fp, a, r1, c1);


    // allocate and initialize array b
    FILE *f1;
    f1 = fopen(queue2, "r");
    if (f1 == NULL){
        printf("Error opening file!");
        exit(0);
    }
    fscanf(f1, "row=%d col=%d", &r2, &c2); // read row and column values
    b = (int **)malloc(r2 * sizeof(int *));
    for (int i=0; i<r2; i++) {
        b[i] = (int *)malloc(c2 * sizeof(int));
    }
    readInput(f1, b, r2, c2);
    
    // input validation
    if(c1 != r2) {
        printf("ERROR! Matrix 1 columns count must equal Matrix 2 rows count!!\n");
        exit(0);
    }
    // allocate array c
    c = (int **)malloc(r1 * sizeof(int *));
    for (int i=0; i<r1; i++) {
        c[i] = (int *)malloc(c2 * sizeof(int));
    }

    FILE *fp2;
    // method 1
    c_per_matrix();
    char mat[100];
    strcpy(mat, queue3);
    strcat(mat, "_per_matrix.txt"); // name file
    fp2 = fopen (mat, "w+");
    if (fp2 == NULL){
        printf("Error opening file!");
        exit(0);
    }
    int x,y;
    // write to file
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
    char row[100];
    strcpy(row, queue3);
    strcat(row, "_per_row.txt"); // name file
    fp2 = fopen (row, "w+");
    if (fp2 == NULL){
        printf("Error opening file!");
        exit(0);
    }
    // write output to file
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
    char element[100];
    strcpy(element, queue3);
    strcat(element, "_per_element.txt"); // name file
    fp2 = fopen (element, "w+");
    if (fp2 == NULL){
        printf("Error opening file!");
        exit(0);
    }
    // write to file
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
