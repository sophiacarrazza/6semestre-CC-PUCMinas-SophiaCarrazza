#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <omp.h>

// Para paralelizar este código, dividi o loop que encontra os múltiplos dos números primos
// em chunks de 150, 450 e 1000 iterações por thread, também variando entre as políticas, e 
// utilizei a diretiva reduce para contar os números primos no loop final.

// Meu computador:
// Linux Ubuntu 24.04.4 LTS com 16gb de RAM e processador AMD Ryzen 7 PRO 4750U com Radeon Graphics × 16

// Entrada: n = 100000000
// Resultado retornado: 5761455

//Resultados de Tempo de Execução:
// Tempo de execucao Sequencial: 2.290695 segundos
// Tempo de execucao Paralelo, com política static e chunksize 150: 1.531732 segundos
// Tempo de execucao Paralelo, com política static e chunksize 450: 1.606236 segundos
// Tempo de execucao Paralelo, com política static e chunksize 1000: 1.559859 segundos
// Tempo de execucao Paralelo, com política dynamic e chunksize 150: 1.569440 segundos
// Tempo de execucao Paralelo, com política dynamic e chunksize 450: 1.680968 segundos
// Tempo de execucao Paralelo, com política dynamic e chunksize 1000: 1.559049 segundos
// Tempo de execucao Paralelo, com política guided e chunksize 150: 1.498506 segundos
// Tempo de execucao Paralelo, com política guided e chunksize 450: 1.627830 segundos
// Tempo de execucao Paralelo, com política guided e chunksize 1000: 1.585294 segundos

// real    0m16,622s
// user    1m31,441s
// sys     0m0,961s

// Rodei com os comandos indicados:
//$ gcc sieve.c -o sieve -fopenmp -lm
//$ time ./sieve

// Código:

int sieve_parallel(int n, char* scaling_policy, int chunksize)
{
   int primes = 0; 
   bool *prime = (bool*) malloc((n+1)*sizeof(bool));
   int sqrt_n = sqrt(n);
   memset(prime, true,(n+1)*sizeof(bool));

   omp_sched_t policy; // usei isso para definir a politica em tempo de execução do codigo
   //e nao precisar compilar 3 vezes
   int chunk = chunksize;

   if (strcmp(scaling_policy, "static") == 0) {
      policy = omp_sched_static;
   } else if (strcmp(scaling_policy, "dynamic") == 0) {
      policy = omp_sched_dynamic;
   } else if (strcmp(scaling_policy, "guided") == 0) {
      policy = omp_sched_guided;
   }

   omp_set_schedule(policy, chunk);

   #pragma omp parallel for schedule(runtime)
   for (int p=2; p <= sqrt_n; p++)
   {
       // If prime[p] is not changed, then it is a prime
       if (prime[p] == true)
       {
           // Update all multiples of p
           for (int i=p*2; i<=n; i += p)
           prime[i] = false;
        }
    }
    // count prime numbers
    #pragma omp parallel for reduction(+:primes)
    for (int p=2; p<=n; p++)
       if (prime[p])
         primes++;
 
    return(primes);
}

int sieve_sequential(int n)
{
   int primes = 0; 
   bool *prime = (bool*) malloc((n+1)*sizeof(bool));
   int sqrt_n = sqrt(n);
 
   memset(prime, true,(n+1)*sizeof(bool));
 
   for (int p=2; p <= sqrt_n; p++)
   {
       // If prime[p] is not changed, then it is a prime
       if (prime[p] == true)
       {
           // Update all multiples of p
           for (int i=p*2; i<=n; i += p)
           prime[i] = false;
        }
    }
    // count prime numbers
    for (int p=2; p<=n; p++)
       if (prime[p])
         primes++;
 
    return(primes);
}
 
int main()
{
   double inicio_sequential = omp_get_wtime();
   int n = 100000000;

   printf("%d\n",sieve_sequential(n));
   double fim_sequential = omp_get_wtime();
   printf("Tempo de execucao Sequencial: %f segundos\n", fim_sequential - inicio_sequential);

   char* scaling_policy[] = {"static", "dynamic", "guided"};
   char* scaling_policy_usada;
   int chunksizes[] = {150, 450, 1000};
   int chunksize_usado;

   double inicio_parallel;
   double fim_parallel;

   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
      inicio_parallel = omp_get_wtime();
      scaling_policy_usada = scaling_policy[i];
      chunksize_usado = chunksizes[j];
      printf("%d\n",sieve_parallel(n, scaling_policy_usada, chunksize_usado));
      fim_parallel = omp_get_wtime();
      printf("Tempo de execucao Paralelo, com política %s e chunksize %d: %f segundos\n", scaling_policy_usada, chunksize_usado, fim_parallel - inicio_parallel);
      }
   }
   return 0;
} 

// Sophia Carrazza