#include <mpi.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int mode = (argc > 1) ? atoi(argv[1]) : 0;

    int number;
    if (rank == 0) {
        number = 42;
        int dest = (mode == 2) ? 5 : 1;
        MPI_Send(&number, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        std::cout << "Process 0 finished MPI_Send" << std::endl;
    } else if (rank == 1) {
        int src = (mode == 0) ? 2  : 0;      // wrong source in mode 0
        int tag = (mode == 1) ? 99 : 0;      // wrong tag in mode 1
        std::cout << "Process 1 waiting for a message from rank " << src
                  << " with tag " << tag << std::endl;
        MPI_Recv(&number, 1, MPI_INT, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process 1 received " << number << std::endl;   // never reached in modes 0/1
    }

    MPI_Finalize();
    return 0;
}
