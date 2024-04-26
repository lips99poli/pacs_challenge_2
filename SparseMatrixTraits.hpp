#ifndef SMTRAITS_HPP
#define SMTRAITS_HPP

#include<type_traits>
#include<concepts>
#include<map>
#include<array>
#include<vector>

// Enumerator for Storage options
enum StorageOptions{
    ColumnMajor,
    RowMajor
};

// Forward declaration, needed for defining Value_Traits
template<typename T>
class Compressed_Container_Struct;

// Define the Value_Traits struct: defines the types used in the SparseMatrix class
template<typename T>
struct Value_Traits{
    using size_type = std::size_t;
    using value_type = T;
    using uncompressed_container = std::map<std::array<size_type,2>,T>;
    using compressed_container = Compressed_Container_Struct<T>;
};

// Define the type alias for type of the indexes of the matrix
template<class T>
using size_type = typename Value_Traits<T>::size_type;

// Define the type alias for the type of the values of the matrix
template<class T>
using value_type = typename Value_Traits<T>::value_type;

// Define the type alias for the type of the uncompressed container of data of the matrix
template<class T>
using uncompressed_container_type = typename Value_Traits<T>::uncompressed_container;

// Overload the operator< for the uncompressed_container_type to adapt container sorting to COLUMNMAJOR case
template<StorageOptions SO,typename T>
std::enable_if<SO==ColumnMajor,bool> operator<(const uncompressed_container_type<T>& lhs, const uncompressed_container_type<T>& rhs){
    return lhs.first[1]<rhs.first[1] || (lhs.first[1]==rhs.first[1] && lhs.first[0]<rhs.first[0]);
};

// Define the type alias for the type of the compressed container of data of the matrix
template<class T>
using compressed_container_type = typename Value_Traits<T>::compressed_container;

// Define the Compressed_Container_Struct struct: defines the structure of the compressed container to hold the data
template<typename T>
struct Compressed_Container_Struct{
    std::vector< value_type<T> > values;
    std::vector< size_type<T> > non_major_index;
    std::vector< size_type<T> > major_change_index;
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