#ifndef SMTRAITS_HPP
#define SMTRAITS_HPP

#include<type_traits>
#include<concepts>
#include<map>
#include<array>
#include<vector>
#include<fstream>
#include<string>
#include<stdexcept>

#include <iostream>
#include <limits>
#include <algorithm>
#include <numeric>
#include <execution>
#include <complex>
#include <random>

// Enumerator for Storage options
enum StorageOptions{
    RowMajor,
    ColumnMajor
};

enum ExtractOptions{
    Row,
    Col
};

enum NormOptions{
    One,
    Infinity,
    Froebenius
};

// Forward declaration, needed for defining Value_Traits
template<StorageOptions SO,typename T>
struct Custom_Compare;

// Forward declaration, needed for defining Value_Traits
template<StorageOptions SO,typename T>
struct Compressed_Container_Struct;

// Define the Value_Traits struct: defines the types used in the SparseMatrix class
template<StorageOptions SO, typename T>
struct Value_Traits{
    using size_type = std::size_t;
    using value_type = T;
    using uncompressed_container = std::map< std::array< size_type ,2 >, T, Custom_Compare<SO,T>>;
    using compressed_container = Compressed_Container_Struct<SO,T>;
    using key_type = std::array< size_type ,2 >;
    using norm_type = double;
};

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

// Define the struct that contains the operator< for the type of the key of the uncompressed matrix
template<StorageOptions SO,class T>
struct Custom_Compare{
    bool operator() (const std::array< size_type<SO,T> ,2 >& lhs, const std::array< size_type<SO,T> ,2 >& rhs)const{
        if constexpr (SO == ColumnMajor) {
            return (lhs[1]<rhs[1] || (lhs[1]==rhs[1] && lhs[0]<rhs[0]));
        } else {
            return lhs < rhs;
        }
    };
};

// Define the Compressed_Container_Struct struct: defines the structure of the compressed container to hold the data
template<StorageOptions SO,typename T>
struct Compressed_Container_Struct{
    std::vector< value_type<SO,T> > values;
    std::vector< size_type<SO,T> > non_major_index;
    std::vector< size_type<SO,T> > major_change_index;
    void clear() {
        values.clear();
        non_major_index.clear();
        major_change_index.clear();
    };
    bool empty() const {
        return (values.empty() && non_major_index.empty() && major_change_index.empty());
    };
};

#endif