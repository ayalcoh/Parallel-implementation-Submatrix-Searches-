#ifndef MPI_DATATYPES_H
#define MPI_DATATYPES_H

#include "header.h"
#include <mpi.h>

extern MPI_Datatype MPI_Picture;
extern MPI_Datatype MPI_Object;

void create_mpi_datatypes();

#endif /* MPI_DATATYPES_H */
