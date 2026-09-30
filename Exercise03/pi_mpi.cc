#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <random>

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
        for (int src = 1; src < size; src++) {         // fixed source: 1, 2, 3 ...
            MPI_Recv(&h, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += h;
        }
        double pi = 4.0 * (double)total / (double)N;
        double elapsed = MPI_Wtime() - t0;
        printf("RESULT np=%d N=%lld hits=%lld pi=%.6f time=%.6f\n", size, N, total, pi, elapsed);
    } else {
        MPI_Send(&hits, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
