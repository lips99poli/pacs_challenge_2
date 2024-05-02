#ifndef UTILS_HPP
#define UTILS_HPP
#include "SparseMatrix.hpp"
#include "chrono.hpp"

// Define the type alias for type of the indexes of the matrix
template<StorageOptions SO,class T>
using size_type = typename Value_Traits<SO,T>::size_type;

// Define the type alias for the type of the values of the matrix
template<StorageOptions SO,class T>
using value_type = typename Value_Traits<SO,T>::value_type;

// Define the type alias for the type of the uncompressed container of data of the matrix
template<StorageOptions SO,class T>
using uncompressed_container_type = typename Value_Traits<SO,T>::uncompressed_container;

// Define the type alias for the type of the compressed container of data of the matrix
template<StorageOptions SO,class T>
using compressed_container_type = typename Value_Traits<SO,T>::compressed_container;

template<StorageOptions SO,class T>
using key_type = typename Value_Traits<SO,T>::key_type;

template<typename T>
std::vector<T> vector_generator(std::size_t dim, T min=std::numeric_limits<T>::lowest(), T max=std::numeric_limits<T>::max()){
    std::vector<T> vec(dim);
    std::random_device rd;
    std::mt19937 gen(rd());

    if constexpr (std::is_same<T, std::complex<float>>::value || 
                  std::is_same<T, std::complex<double>>::value || 
                  std::is_same<T, std::complex<long double>>::value) {
        std::uniform_real_distribution<typename T::value_type> dis(min.real(), max.real());
        for(std::size_t i=0;i<dim;++i){
            vec[i] = T(dis(gen), dis(gen));
        }
    } else {
        std::uniform_real_distribution<T> dis(min, max);
        for(std::size_t i=0;i<dim;++i){
            vec[i] = dis(gen);
        }
    }
    return vec;
}

template<StorageOptions SO,typename T>
void test_matrix(Timings::Chrono& timer, SparseMatrix<SO,T>& m, SparseMatrix<SO,T>& m2, std::vector<T>& v, std::size_t n_iter=5){

    for(std::size_t i=0;i<n_iter;++i){
        std::cout<<"Experiment: " << i+1 << std::endl;
        // Compression experiment
        timer.start();
        m.compress();
        timer.stop();
        std::cout << "Compression: " << timer;

        // Vector multiplication experiment COMPRESSED STATE
        timer.start();
        std::vector<T> res_comp = m*v;
        timer.stop();
        std::cout << "Vector Multiplication -- compressed state: " << timer;

        // Matrix multiplication experiment COMPRESSED STATE
        timer.start();
        std::vector<T> res_comp_m = m2*v;
        timer.stop();
        std::cout << "Matrix Multiplication -- compressed state: " << timer;

        // Uncompression experiment
        timer.start();
        m.uncompress();
        timer.stop();
        std::cout << "Uncompression: " << timer;

        // Vector multiplication experiment UNCOMPRESSED STATE
        timer.start();
        std::vector<T> res_uncomp = m*v;
        timer.stop();
        std::cout << "Vector Multiplication -- uncompressed state: " << timer;

        // Matrix multiplication experiment UNCOMPRESSED STATE
        timer.start();
        std::vector<T> res_uncomp_m = m2*v;
        timer.stop();
        std::cout << "Matrix Multiplication -- uncompressed state: " << timer;
        std::cout <<"xxxxxxxxxxxx\n" << std::endl;
    }
    std::cout << std::endl;
    
}
#endif