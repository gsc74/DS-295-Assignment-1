#include <iostream>
#include <fstream>
#include <random>
#include <mpi.h>
#include <chrono>

int main(int argc, char** argv) {

    // Define the matrix dimensions
    int rows = atoi(argv[1]);
    int cols = rows;
    int* A = nullptr;
    int* B = nullptr;

    MPI_Init(&argc, &argv);

    int rank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    if (rank == 0) {
        std::cout << "Number of processes: " << numProcs << std::endl;
        std::cout << "Rows: " << rows << " Cols: " << cols << std::endl;
    }

    // Calculate the number of elements per process
    const int loc_elements = (rows * cols) / numProcs;

    // Generate random matrices
    A = new int[loc_elements];
    B = new int[loc_elements];

    auto gen_time_start = std::chrono::high_resolution_clock::now();

    // Parallelly generate matrix A and B
    for (int i = 0; i < loc_elements; i++) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }

    auto gen_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> gen_time = gen_time_end - gen_time_start;
    if (rank == 0) {
        std::cout << "Matrix generation time: " << gen_time.count() << "s" << std::endl;
    }

    auto write_time_start = std::chrono::high_resolution_clock::now();

    // Open the file for writing
    MPI_File file_1, file_2;
    MPI_File_open(MPI_COMM_WORLD, "A.bin", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_1);
    MPI_File_open(MPI_COMM_WORLD, "B.bin", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_2);

    // Write the matrix to the file and verify the write
    MPI_File_write_at(file_1, rank * loc_elements * sizeof(int), A, loc_elements, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_write_at(file_2, rank * loc_elements * sizeof(int), B, loc_elements, MPI_INT, MPI_STATUS_IGNORE);

    // Close the file
    MPI_File_close(&file_1);
    MPI_File_close(&file_2);

    auto write_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> write_time = write_time_end - write_time_start;
    if (rank == 0) {
        std::cout << "Matrix write time: " << write_time.count() << "s" << std::endl;
    }

    // Clean up
    delete[] A;
    delete[] B;

    MPI_Finalize();
    return 0;
}
