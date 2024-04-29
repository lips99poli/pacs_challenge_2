#ifndef SMTRAITS_HPP
#define SMTRAITS_HPP

#include<type_traits>
#include<concepts>
#include<map>
#include<array>
#include<vector>

// Enumerator for Storage options
enum StorageOptions{
    RowMajor,
    ColumnMajor
};

enum ExtractOptions{
    Row,
    Col
};

// Forward declaration, needed for defining Value_Traits
// template<StorageOptions SO, typename T>
// struct Uncompressed_Container_Struct;

template <StorageOptions SO, typename T>
struct Key_Uncompressed_Struct;

// Forward declaration, needed for defining Value_Traits
template<StorageOptions SO,typename T>
struct Compressed_Container_Struct;

// Define the Value_Traits struct: defines the types used in the SparseMatrix class
template<StorageOptions SO, typename T>
struct Value_Traits{
    using size_type = std::size_t;
    using value_type = T;
    using uncompressed_container = std::map< Key_Uncompressed_Struct<SO,T>, T>;
    using compressed_container = Compressed_Container_Struct<SO,T>;
    using key_type = Key_Uncompressed_Struct<SO,T>;
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

// Define a type that will be the key in the Uncompressed_Container_Struct map
template<StorageOptions SO, typename T>
struct Key_Uncompressed_Struct{
    std::array< size_type<SO,T> ,2 > key;
    
    // Overload the operator< for the uncompressed_container_type to adapt container sorting to COLUMNMAJOR case
    // template<StorageOptions S=SO>
    // std::enable_if_t<S==ColumnMajor,bool> operator< (const Key_Uncompressed_Struct<SO,T>& rhs) const{
    //     return (this->key[1]<rhs.key[1] || (this->key[1]==rhs.key[1] && this->key[0]<rhs.key[0]));
    // }

    Key_Uncompressed_Struct(const std::array< size_type<SO,T>,2 >& key_): key(key_){};
    Key_Uncompressed_Struct() = default;


    bool operator< (const Key_Uncompressed_Struct<SO,T>& rhs) const{
        if constexpr (SO == ColumnMajor) {
            return (this->key[1]<rhs.key[1] || (this->key[1]==rhs.key[1] && this->key[0]<rhs.key[0]));
        } else {
            return key < rhs.key;
        }
    };

    size_type<SO,T>& operator[](size_type<SO,T> index){
        return key[index];
    };

    const size_type<SO,T>& operator[](size_type<SO,T> index) const{
        return key[index];
    };

};

// Define the Uncompressed_Container_Struct struct: defines the structure of the compressed container to hold the data
// template<StorageOptions SO, typename T>
// struct Uncompressed_Container_Struct{
//     using key_type = Key<SO,T>;
//     std::map< key_type,T > data;

//     void clear() {data.clear();}
//     bool empty() const {return data.empty();}
//     auto begin() {return data.begin();}
//     auto end() { return data.end();}
//     auto cbegin() const { return data.cbegin();}
//     auto cend() const { return data.cend();}
//     auto size() const { return data.size();}
//     auto operator[] (const key_type& position){return data[position];}
//     Uncompressed_Container_Struct(const uncompressed_container_type<SO,T>& init):data(init){}
// };

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