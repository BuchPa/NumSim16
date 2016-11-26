// For debugging with printf
#include <stdio.h>
// For MPI_DOUBLE/INT
#include <mpich/mpi.h>
// For variable input args
#include <stdarg.h>

// Printf function for master
int mprintf(const char *format, ...){
  
  // Check, if current process is master
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  // In case of master plot current info
  if(rank == 0){
    
    va_list arg; // Get variable input args
    int done;
    
    va_start(arg, format);
    done = vprintf(format, arg); // Call original printf
    va_end(arg);
   
    return done;
  }else{
    return 1;
  }
}

int main(int argc, char **argv) {
  
  MPI_Init(&argc, &argv);
  
  mprintf("Some String %s and the int %d, the double %f.\n", "'with another String'", 42, 13.37);
  printf("And I'm a dublicate %s\n", "'- sad story bro'");
  
  MPI_Finalize();
}
