/*
 * mandel.c: Este programa realiza o cálculo do fractal de mandelbrot usando
 *           POSIX threads. O espaço de cálculo é dividido em 't' linhas onde
 *           't' é o número de threads usadas.
 *
 * usage:    mandel [+n size] [+c cr0 ci0 cr1 ci] [+t threads] [+l lim] [+p]
 *           onde:
 *                size=tamanho da imagem, (cr0,ci0) e (cr1, ci1) são coordenadas
 *                da diagonal do plano de cálculo, t=nro de threads, lim=limite
 *                de convergência e p=flag para geração da imagem.
 *
 * compilaçao: %gcc -o mandel mandel.c -lm -lpthread -W
 *
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

struct work{
    double c0_r;
    double c0_i;
    double c1_r;
    double c1_i;
    int    lim;
    int    n;
    int    t;
    int    tid;
    int*  region;
};


double  distance(double a, double b) {
    return (a*a + b*b);
}

void*   mandel(void *arg) {
    double dx, dy, z_r, z_i, c_r, c_i, c0_r, c0_i, c1_r, c1_i, aux;
    int i, j, k, lim, n, t, tid, yi, yf;
    int *plan;


    /* Faz uma copia da regiao global para manter em memoria local */

    c0_r = ((struct work*)(arg))->c0_r;
    c0_i = ((struct work*)arg)->c0_i;
    c1_r = ((struct work*)arg)->c1_r;
    c1_i = ((struct work*)arg)->c1_i;
    lim  = ((struct work*)arg)->lim;
    n = ((struct work*)arg)->n;
    t = ((struct work*)arg)->t;
    tid = ((struct work*)arg)->tid;

    plan = (int *)malloc( n/t * n * sizeof(int));
    ((struct work*)arg)->region = plan;

    /* Escala para cada ponto, em cada direcao */

    dx = (c1_r - c0_r)/n;
    dy = (c0_i - c1_i)/n;

    /* Definicao da região de trabalho. Divisão por linhas */

    yi = tid * n/t;
    yf = (tid+1) * n/t - 1;

    for (i=yi; i<=yf; i++ ) {
        for ( j = 0 ; j < n ; j++ ) {
            z_r = 0;
            z_i = 0;
            c_r = c0_r + j*dx;
            c_i = c0_i - i*dy;

            k = 0;
            while ( distance(z_r, z_i) < 4 && k < lim) {
                aux = z_r*z_r - z_i*z_i + c_r;
                z_i = 2*z_r*z_i + c_i;
                z_r = aux;
                k++;
            }

            plan[(i-yi)*n+j] = (int)(k*((double)255/lim)+ 0.5i);
          }
      }
}

void usage(void) {
    fprintf(stderr, "usage: mandel [-t thread] [-n size] [-l limit] [-p] \n");
    exit(1);
}

void error(char *code){
    fprintf(stderr, "%s\n", code);
    exit(1);
}

int main(int argc, char **argv) {
    int i, r, print=0, t=1, n=1024, lim=1000;
    long int elapsed;
    struct timeval t0, t1;
    double c0_r=-2.0, c0_i=1.25, c1_r=0.5, c1_i=-1.25;
    struct work *workload;
    pthread_t *workers;

    for (i = 1; i < argc; i++){
        if (argv[i][0] != '-') usage();
        switch (argv[i][1]) {
            case 't': i++; t = atoi(argv[i]);
                      break;
            case 'n': i++; n = atoi(argv[i]);
                      break;
            case 'l': i++; lim = atoi(argv[i]);
                      break;
            case 'p': print = 1;
                      break;
            default : usage();
                      break;
         }
    }


    workload = (struct work*)malloc(t * sizeof(struct work));
    if (workload == NULL)
       error("ERRO: Alocacao de memória para workload");

    workers = (pthread_t *)malloc(t * sizeof(pthread_t));
    if (workers == NULL)
       error("ERRO: Alocacao de memória para workers");

    for (i=0; i< t; i++) {
        workload[i].c0_r = c0_r;
        workload[i].c0_i = c0_i;
        workload[i].c1_r = c1_r;
        workload[i].c1_i = c1_i;
        workload[i].lim = lim;
        workload[i].n = n;
        workload[i].t = t;
        workload[i].tid = i;
        workload[i].region = NULL;
    }

    gettimeofday(&t0, 0);  /*Start timer*/
    for (i=0; i<t; i++) {
        r = pthread_create(&workers[i], NULL, mandel, (void *)(&workload[i]));
        if (r != 0)
           error("ERRO: Problema na criacao de thread worker\n");
    }

    for (i=0; i<t; i++) {
        r = pthread_join(workers[i], NULL);
        if (r != 0)
           error("ERRO: Problema no join de thread worker\n");
    }

    gettimeofday(&t1, 0); /*End Tmer*/

    elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;

    printf("\n*Time to evaluate Mandelbrot: %d usec with %d threads.\n\n ", (int)elapsed, t);

    /*
     * Geração de um arquivo ppm com a figura do fractal. P3 é um número
     * mágico, seguido pelo tamanho da mensagem, profundiade do pixel e um
     * conjunto de valores RGB para cada pixel.
     */

    if (print) {
       printf("P3 %d %d 3\n", n, n);
       for (i=0; i< t; i++) {
           for( r= 0; r < n*n/t; r++)
             printf("%d %d %d \n", (workload[i].region[r]>>5) & 6,
                                   (workload[i].region[r]>>3)& 7,
                                   workload[i].region[r] & 7  );
           free(workload[i].region);
       }
    }
}
