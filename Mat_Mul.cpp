#include <iostream>
#include <mpi.h>
#include <chrono>

int main(int argc, char** argv) {

    // Define the matrix dimensions
    int rows = atoi(argv[1]);
    int cols = rows;
    int* A = nullptr;
    int* B = nullptr;
    int* A_global = nullptr;
    int* B_global = nullptr;
    int* C = nullptr;


    MPI_Init(&argc, &argv);

    int rank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    if (rank == 0) {
        std::cout << "Number of processes: " << numProcs << std::endl;
        std::cout << "Rows: " << rows << " Cols: " << cols << std::endl;
    }

    if (rank == 0) {
        // Allocate memory for global matrices
        A_global = new int[rows * cols];
        B_global = new int[rows * cols];
        C = new int[rows * cols];
    }

    // Calculate the number of elements per process
    const int loc_elements = (rows * cols) / numProcs;

    // Allocate memory for local matrices
    A = new int[loc_elements];
    B = new int[loc_elements];

    auto read_time_start = std::chrono::high_resolution_clock::now();

    // Open the files for reading
    MPI_File file_1, file_2;
    MPI_File_open(MPI_COMM_WORLD, "A.bin", MPI_MODE_RDONLY, MPI_INFO_NULL, &file_1);
    MPI_File_open(MPI_COMM_WORLD, "B.bin", MPI_MODE_RDONLY, MPI_INFO_NULL, &file_2);

    // Read the local portion of matrix A and B from files
    MPI_File_read_at(file_1, rank * loc_elements * sizeof(int), A, loc_elements, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_read_at(file_2, rank * loc_elements * sizeof(int), B, loc_elements, MPI_INT, MPI_STATUS_IGNORE);

    // Close the files
    MPI_File_close(&file_1);
    MPI_File_close(&file_2);

    auto read_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> read_time = read_time_end - read_time_start;
    if (rank == 0) {
        std::cout << "Matrix read time: " << read_time.count() << "s" << std::endl;
    }

    // Fill the global matrices A and B with allgather on rank 0
    MPI_Gather(A, loc_elements, MPI_INT, A_global, loc_elements, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(B, loc_elements, MPI_INT, B_global, loc_elements, MPI_INT, 0, MPI_COMM_WORLD);

    // // Print the global matrices A and B
    // if (rank == 0) {
    //     std::cout << "Matrix A" << std::endl;
    //     for (int i = 0; i < rows; i++) {
    //         for (int j = 0; j < cols; j++) {
    //             std::cout << A_global[i * cols + j] << "\t";
    //         }
    //         std::cout << std::endl;
    //     }
    // }


    // Clean up
    delete[] A;
    delete[] B;

    int* C_local = new int[loc_elements];

    // Perform matrix multiplication sequentially
    if (rank == 0) {
        auto mult_time_start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                C[i * cols + j] = 0;
                for (int k = 0; k < cols; k++) {
                    C[i * cols + j] += A_global[i * cols + k] * B_global[k * cols + j];
                }
            }
        }

        auto mult_time_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> mult_time = mult_time_end - mult_time_start;
        std::cout << "Matrix multiplication time: " << mult_time.count() << "s" << std::endl;
    }

    // Scatter the result matrix C
    MPI_Scatter(C, loc_elements, MPI_INT, C_local, loc_elements, MPI_INT, 0, MPI_COMM_WORLD);

    // Write the local portion of matrix C to file
    MPI_File file_3;
    MPI_File_open(MPI_COMM_WORLD, "C.bin", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_3);
    MPI_File_write_at(file_3, rank * loc_elements * sizeof(int), C_local, loc_elements, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_close(&file_3);

    delete[] A_global;
    delete[] B_global;
    delete[] C;
    delete[] C_local;

    MPI_Finalize();

    return 0;
}
