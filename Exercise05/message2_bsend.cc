#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int MSGS = 3;
    int numbers[MSGS];                 
    int received;

    if (rank == 0) {
       
        int bufsize = MSGS * (sizeof(int) + MPI_BSEND_OVERHEAD);
        std::vector<char> buf(bufsize);
        MPI_Buffer_attach(buf.data(), bufsize);

        for (int i = 0; i < MSGS; i++) {
            numbers[i] = i * 10;
            MPI_Bsend(&numbers[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            std::cout << "Process 0 sent " << numbers[i] << "\n";
        }

    
        void* old_buf; int old_size;
        MPI_Buffer_detach(&old_buf, &old_size);
    } else if (rank == 1) {
        for (int i = 0; i < MSGS; i++) {
            MPI_Recv(&received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Process 1 received " << received << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}
