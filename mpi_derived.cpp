#include <mpi.h>
#include <cstddef>
#include "mpi_derived.h"
#include "common.h"

// Here we describe our datastructures to MPI
void MPI_MakeSolverReportType()
{
	int count = 2; // number of blocks
	int blocklens[] = {1,1}; // number of elements in each block
	MPI_Aint indices[2]; // byte displacements of each block
	indices[0] = (MPI_Aint)offsetof (struct SolverReport, state);
	indices[1] = (MPI_Aint)offsetof (struct SolverReport, watch_scans);

	MPI_Datatype old_types[] = {MPI_INT, MPI_INT}; // types of elements in each block 
	MPI_Type_struct (count, blocklens, indices, old_types, &mpiT_SolverReport);
}

void MPI_InitDatatypes()
{
	MPI_MakeSolverReportType();
}
