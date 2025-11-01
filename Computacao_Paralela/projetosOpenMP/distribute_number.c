#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Para preencher o código, utilizei as funçoes MPI_Send e MPI_Recv (as ferrametnas básicas de comunicação do MPI)
// para enviar e receber as mensagens entre os processos.

//  O processo de rank 0 envia o valor 51 para todos os outros por meio do MPi_Send em um for.
//  Cada processo que não é o de rank 0 recebe essa mensagem e imprime esse valor.


void main(int argc, char* argv[]) {
  int i, rank, val, numProcs;
  MPI_Status status;

  MPI_Init(&argc, &argv) ;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  if (rank == 0) {
    val = 51;

    for (i = 1; i < numProcs; i++) 
      MPI_Send(&val, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      printf("Process %d sends a message to %d\n",rank,i);

  } else {

    MPI_Recv(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    printf("Process %d receives a message from %d with value %d\n",rank,0,val);

  }
  MPI_Finalize();
}

// Sophia Carrazza