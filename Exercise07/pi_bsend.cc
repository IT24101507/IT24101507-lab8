#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long long N = (argc > 1) ? atoll(argv[1]) : 10000000LL;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    long long mine = N / size + (rank < N % size ? 1 : 0);

    std::mt19937_64 gen(12345ULL + rank);              // different seed per rank
    std::uniform_real_distribution<double> uni(0.0, 1.0);
    long long hits = 0;
    for (long long i = 0; i < mine; i++) {
        double x = uni(gen), y = uni(gen);
        if (x * x + y * y <= 1.0) hits++;
    }

    if (rank == 0) {
        long long total = hits, h;
        MPI_Status status;
        int order[1024], n = 0;                        // record arrival order
        for (int k = 1; k < size; k++) {
            // Receive from any worker that finishes first
            MPI_Recv(&h, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total += h;
            if (n < 1024) order[n++] = status.MPI_SOURCE;
        }
        double pi = 4.0 * (double)total / (double)N;
        double elapsed = MPI_Wtime() - t0;
        printf("RESULT np=%d N=%lld hits=%lld pi=%.6f time=%.6f\n", size, N, total, pi, elapsed);
        printf("arrival order at rank 0:");
        for (int i = 0; i < n; i++) printf(" %d", order[i]);
        printf("\n");
    } else {
        //  Buffered Send S
        // Calculate required buffer size 
        int bufsize = sizeof(long long) + MPI_BSEND_OVERHEAD;
        std::vector<char> buf(bufsize);

        // Attach the user buffer to MPI
        MPI_Buffer_attach(buf.data(), bufsize);

        // Send using buffered send
        MPI_Bsend(&hits, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);

        // Detach buffer 
        void* old_buf;
        int old_size;
        MPI_Buffer_detach(&old_buf, &old_size);
    }

    MPI_Finalize();
    return 0;
}