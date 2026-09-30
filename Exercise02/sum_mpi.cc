#include <mpi.h>
#include <cstdio>
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long long N = (argc > 1) ? atoll(argv[1]) : 10000000LL;

    MPI_Barrier(MPI_COMM_WORLD);          // start everyone together
    double t0 = MPI_Wtime();

    // Block distribution 
    long long base = N / size, rem = N % size;
    long long count = base + (rank < rem ? 1 : 0);
    long long start = (long long)rank * base + (rank < rem ? rank : rem) + 1;

    long long local = 0;                   // 1..1e7 sums to ~5e13, so use 64 bit
    for (long long i = start; i < start + count; i++) local += i;

    if (rank == 0) {
        long long total = local, partial;
        for (int src = 1; src < size; src++) {
            MPI_Recv(&partial, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += partial;
        }
        double elapsed = MPI_Wtime() - t0;
        long long expected = N * (N + 1) / 2;
        printf("RESULT np=%d N=%lld sum=%lld expected=%lld %s time=%.6f\n",
               size, N, total, expected, total == expected ? "OK" : "WRONG", elapsed);
    } else {
        MPI_Send(&local, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
