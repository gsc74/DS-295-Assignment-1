#include <iostream>
#include <mpi.h>
#include <chrono>

int main(int argc, char** argv) {

    // Define the matrix dimensions
    int rows = atoi(argv[1]);
    int cols = rows;

    MPI_Init(&argc, &argv);

    int rank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    if (rank == 0) {
        std::cout << "Number of processes: " << numProcs << std::endl;
        std::cout << "Rows: " << rows << " Cols: " << cols << std::endl;
    }

    int loc_elements = (rows * cols) / numProcs;

    int* C = new int[loc_elements];
    int* C_test = new int[loc_elements];

    // Read C.bin parallel
    auto read_time_start = std::chrono::high_resolution_clock::now();
    // Open the file for reading
    MPI_File file_1;
    MPI_File file_2;
    MPI_File_open(MPI_COMM_WORLD, "C.bin", MPI_MODE_RDONLY, MPI_INFO_NULL, &file_1);
    MPI_File_read_at(file_1, rank * loc_elements * sizeof(int), C, loc_elements, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_open(MPI_COMM_WORLD, "C_test.bin", MPI_MODE_RDONLY, MPI_INFO_NULL, &file_2);
    MPI_File_read_at(file_2, rank * loc_elements * sizeof(int), C_test, loc_elements, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_close(&file_1);
    MPI_File_close(&file_2);
    auto read_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> read_time = read_time_end - read_time_start;
    if (rank == 0) {
        std::cout << "Matrix read time: " << read_time.count() << "s" << std::endl;
    }

    // Verify the result
    int correct = 1;
    for (int i = 0; i < loc_elements; i++) {
        if (C[i] != C_test[i]) {
            correct = 0;
            break;
        }
    }

    int global_correct;
    MPI_Reduce(&correct, &global_correct, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        if (global_correct == numProcs) {
            std::cout << "Result is correct!" << std::endl;
        }
        else {
            std::cout << "Result is incorrect!" << std::endl;
        }
    }

    delete[] C;
    delete[] C_test;

    MPI_Finalize();

    return 0;
}
    