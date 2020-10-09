/*  Author: Chris Tucker 
    Date: 2/4/2020
    Distributed Systems
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <ctype.h> 
#include <time.h>
#include <sys/time.h>

bool isValid(char* arg);
int errorExit();
void fillMatrix(int r, int c, double m[r][c]);
void printMatrix(int r, int c, double m[r][c]);
long parallelMultiply(int m, int p, int n, double A[m][p], double B[p][n], double C[m][n]);
long serialMultiply(int m, int p, int n, double A[m][p], double B[p][n], double C[m][n]);
bool compareMatrix(int r, int c, double m1[r][c], double m2[r][c]);


int main(int argc, char* argv[]){
    int a1, a2, b1, b2, c1, c2;
    srand48(time(0));

    //First step is to process input arguments to determine the size of the matrices
    //Assign matrix sizes based on number of inputs
    switch(argc){
        case 2:
            if(!isValid(argv[1]))
                errorExit();
            a1 = a2 = b1 = b2 = c1 = c2 = atoi(argv[1]);
            break;
        case 3:
            if(!isValid(argv[1]) || !isValid(argv[2]))
                errorExit();
            a1 = b2 = c1 = c2 = atoi(argv[1]);
            a2 = b1 = atoi(argv[2]);
            break;
        case 4:
            if(!isValid(argv[1]) || !isValid(argv[2]) || !isValid(argv[3]))
                errorExit();
            a1 = c1 = atoi(argv[1]);
            a2 = b1 = atoi(argv[2]);
            b2 = c2 = atoi(argv[3]);
            break;
        default:
            //If there are no arguments, or more than 3, terminate the program
            errorExit();
        break;
    }

    //C seems to let me use arrays declaired like this but I get segmentation faults when I pass them as
    //a parammeter to a function, so I need to use malloc.
    //double A[a1][a2];
    //double B[b1][b2];
    //double C[c1][c2];
    
    double (*A)[a2] = malloc(a1*a2*sizeof(double));
    double (*B)[b2] = malloc(b1*b2*sizeof(double));
    double (*serC)[c2] = malloc(c1*c2*sizeof(double));
    double (*parC)[c2] = malloc(c1*c2*sizeof(double));
    fillMatrix(a1, a2, A);
    fillMatrix(b1, b2, B);
    //printMatrix(a1, a2, A);
    //printMatrix(b1, b2, B);

    //Time Serialized Implementation
    long serTime = serialMultiply(a1, a2, c2, A, B, serC);

    //Time Parallel Implementation
    long parTime = parallelMultiply(a1, a2, c2, A, B, parC);

    if(compareMatrix(c1, c2, serC, parC)){
        printf("***********************Multiplication success! result matrix:***********************\n");
        printMatrix(c1, c2, serC);
        printf("************************************************************************************\n\n");
        printf("Serialized Time: %ld u-seconds\n\n",serTime);
        printf("Paralellized Time: %ld u-seconds\n\n",parTime);
        printf("Paralellization saw a speed improvement of %.2f%% over the serialized method\n",((double)(parTime-serTime)/serTime)*(-100));
        printf("(Took %.2f%% as much time compared to the Serialized method)\n", (double)parTime/serTime*100);
    }else{
        printf("Matricies not equal, something is broken!\n");
    }

    free(A);
    free(B);
    free(serC);
    free(parC);
    return 0;
}

bool isValid(char* arg){
    //Checks that the given argument is a diget and greater than 0
    for (int i=0; arg[i] != '\0'; i++){
        if (!isdigit(arg[i]))
            return false;
    }
    if(atoi(arg) == 0)
        return false;
    return true;
}
void fillMatrix(int r, int c, double m[r][c]){
    for(int i=0; i<r; i++){
        for(int j=0; j<c; j++){
            m[i][j] = 100*drand48();
        }
    }
}
void printMatrix(int r, int c, double m[r][c]){
    for(int i=0; i<r; i++){
        for(int j=0; j<c; j++){
            printf("%.3f\t",m[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
bool compareMatrix(int r, int c, double m1[r][c], double m2[r][c]){
    for(int i=0; i<r; i++){
        for(int j=0; j<c; j++){
            if(m1[i][j] != m2[i][j]){
                printf("Matricies are not equal\n");
                return false;
            }
        }
    }
    printf("Matricies are equal\n");
    return true;
}
long serialMultiply(int m, int p, int n, double A[m][p], double B[p][n], double C[m][n]){
    struct timeval s0, s1;

    gettimeofday(&s0, 0);
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            for(int k=0; k<p; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
            //printf("%8.1f\t",parC[i][j]);
        }
        //printf("\n");
    }
    gettimeofday(&s1, 0);
    return (s1.tv_sec-s0.tv_sec)*1000000 + s1.tv_usec-s0.tv_usec;
}
long parallelMultiply(int m, int p, int n, double A[m][p], double B[p][n], double C[m][n]){
    struct timeval p0, p1;

    gettimeofday(&p0, 0);
    #pragma omp parallel for
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            for(int k=0; k<p; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
            //printf("%8.1f\t",parC[i][j]);
        }
        //printf("\n");
    }
    gettimeofday(&p1, 0);
    long elapsedP = (p1.tv_sec-p0.tv_sec)*1000000 + p1.tv_usec-p0.tv_usec;
    return elapsedP;
}
int errorExit(){
    printf("You must pass this application 1-3 integer command line arguments greater than 0. Usage:\n");
    printf("./hw m     => (m x m)*(m x m) = (m x m)\n");
    printf("./hw m p   => (m x p)*(p x m) = (m x m)\n");
    printf("./hw m p n => (m x p)*(p x n) = (m x n)\n");
    exit(1);
}
