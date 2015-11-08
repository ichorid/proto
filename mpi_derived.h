#ifndef PROTO_MPI_DERIVED_H_
#define PROTO_MPI_DERIVED_H_
#include <mpi.h>


#ifdef MAIN
MPI_Datatype mpiT_SolverReport;
#else
extern MPI_Datatype mpiT_SolverReport;
#endif

void MPI_MakeSolverReportType ();
void MPI_InitDatatypes();
#endif
