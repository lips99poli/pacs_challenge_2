Second challenge/assignment of the PACS course (Advanced programming for scientific computing). 



# Template Sparse Matrix Project

This project provides a C++ implementation for Sparse Matrix operations with a focus on performance. It includes functionality for different types of matrix norms and compression.

## Prerequisites

- A modern C++ compiler with support for C++20.
- `mk_modules` installed 
- GNU Make

## Building

The project uses a Makefile for building. To build the project, navigate to the project directory `/src` and run:

```bash
make
```

This will compile all the source files and generate the executable files.

## Usage

After building the project, you can run the executables. For example:

```bash
./main -n 5 -v 0
```

This will run the executable with 5 runs and all the possible tests.

## Options

- `--n_iter`: Specifies the number of runs for the benchmark. Default is 1, so 1 run.
- `--verbose`: Specifies the verbosity level. Default is 0, so tests are executed both for scalar and complex case. 1 is for only scalar and 2 is for only complex

## Cleaning Up

To clean up the build artifacts, run:

```bash
make clean
```

To also remove any generated documentation and temporary files, run:

```bash
make distclean
```

## Technical Notes

- The implementation is designed to work correctly with various data types including `double`, `int`, `float`, and `std::complex`.
- The following operations are supported:
  - Norm calculations: The `norm` method supports the Frobenius norm (`FROBENIUS`), 1-norm (`ONE`), and max norm (`MAX`).
  - Matrix-vector product: The `matrix_vector_product` method performs the multiplication of a matrix by a vector.
  - Matrix-Matrix product: The `matrix_matrix_product` method performs the multiplication of a matrix (Sparse) by a matrix (Sparse). Since the result could be not Sparse the type returned is std::vector of std::vector of the type of the matrix (the idea is that in a more complex environment there is also a class for Dense matrixes).
  - Compression and decompression: The `compress` and `uncompress` methods allow for efficient storage and retrieval of matrix data.
  - Indexing: General indexing operations are supported for accessing and modifying matrix elements.

## Quick Note on Compressed state

Note that the compressed state container structure depends on the Storage Options of the matrix (RowMajor or ColumnMajor). This issue has been solved by building the three vectors in a generalized way (values, non_major_index, major_change_index) and then filling them by using the conversion of the Enum Type of StorageOptions to int. Observe the beauty of this mechanism in the `compress` method: there are no if statements!
This improves the compactness and the performances of the code.

## Tests
There is a file named main.cpp which contains some tests of the performances of some of the method of the class.
To measure the time I used an additional library, namely `chrono.hpp` (inside folder `include`)


## License

This project is thought to have no license.