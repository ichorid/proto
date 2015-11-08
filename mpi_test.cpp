#include <mpi.h>
#include <stdio.h>
 
int main (int argc, char* argv[])
{
  int rank, size;
 
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  if (rank==0){
	  int arr_send [3] = {0,1,2};
	  MPI_Send (arr_send, 3, MPI_INT, 1, 0, MPI_COMM_WORLD);
  }else{
	  int arr_recv[3] = {9,9,9};
	  MPI_Recv (arr_recv, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          printf( "process %d of %d\n received arr %d", rank, size, arr_recv[2] );
  }
  printf( "Hello world from process %d of %d\n", rank, size );
  MPI_Finalize();
  return 0;
}
