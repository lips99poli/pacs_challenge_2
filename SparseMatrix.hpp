#ifndef SM_HPP
#define SM_HPP

#include "SparseMatrixTraits.hpp"
#include <iostream>
#include <limits>
#include <algorithm>
#include <numeric>
#include <execution>
#include <complex>

// Aggiungere overload operator << per aggiungere elementi
// Leggere matrice da file
// operatore*

enum State {
    UNCOMPRESSED=0,
    COMPRESSED=1
};

template<StorageOptions SO, typename T>
class SparseMatrix{

    using size_type = typename Value_Traits<SO,T>::size_type;
    using value_type = typename Value_Traits<SO,T>::value_type; //in realtà questo è T quindi non serve
    using uncompressed_container_type = typename Value_Traits<SO,T>::uncompressed_container;
    using compressed_container_type = typename Value_Traits<SO,T>::compressed_container;
    using key_type = typename Value_Traits<SO,T>::key_type;

    private:
    size_type rows;
    size_type cols;
    uncompressed_container_type uncompressed_data;
    compressed_container_type compressed_data;
    State state;
    T nan = std::numeric_limits<T>::quiet_NaN();
    T buffer = std::numeric_limits<T>::quiet_NaN();

    static constexpr StorageOptions otherSO = (SO==RowMajor)? ColumnMajor : RowMajor; //perchè il prof la fa static?? se lo tolgo da errore

    inline bool in_bound(size_type r, size_type c) const;

    inline bool verify_presence(size_type r, size_type c) const;

    public:
    // Constructor    
    SparseMatrix(size_type R = 0, size_type C = 0, const uncompressed_container_type& init = {}):
    rows(R), cols(C), uncompressed_data(init), state(UNCOMPRESSED){
        if (!init.empty()){
            bool adjusted = false;
            for (auto cit=init.cbegin(); cit!=init.cend(); ++cit){
                if (cit->first[0]>=rows){
                    adjusted = true;
                    resize(cit->first[0]+1,cols);
                }
                if(cit->first[1]>=cols){
                    adjusted = true;
                    resize(rows,cit->first[1]+1);
                }
            }
            if (adjusted) std::cout << "Dimensions adjusted: R = " << rows << ", C = " << cols << std::endl;
        }
    };   

    std::vector<T> get_row(size_type r)const;
    std::vector<T> get_col(size_type c)const;

    template<ExtractOptions RorC>
    std::vector<T> extract_vector(size_type k) const;

    inline bool is_compressed() const;

    void resize(size_type R, size_type C);

    void compress();

    void uncompress();

    // const version of call operator
    const T& operator() (const size_type r, const size_type c) const;

    // non-const version of call operator
    T& operator()(const size_type r, const size_type c);

    // Delete wrongly added element
    bool delete_element(const size_type r, const size_type c);

    // Multiply operator by vectors
    std::vector<T> operator*(const std::vector<T>& v) const;

    // Multiply operator by matrix
    std::vector<std::vector<T>> operator*(const SparseMatrix<SO,T>& v) const;

    // Read matrix from file
    friend SparseMatrix<SO, T> read_matrix_from_file(const std::string& filename);

    // Norm
    template<NormType N>
    T norm() const;
};


// PRIVATE METHODS:

// Verify indexes are in bound
template<StorageOptions SO, typename T>
bool SparseMatrix<SO,T>::in_bound(size_type r, size_type c) const {
    bool in_bound = (r<=rows && c<=cols);
    if (!in_bound) std::cerr << "Indexes {" << r << "," << c << "} not compatible with Matrix dimensions" << std::endl;
    return in_bound;
};

// Verify if an element is already present
template<StorageOptions SO, typename T>
bool SparseMatrix<SO,T>::verify_presence(size_type r, size_type c) const {
    key_type position({r,c});
    bool present = false;

    if (state == UNCOMPRESSED) {
        present = uncompressed_data.find(position) != uncompressed_data.end();
        }
    else{
        for (size_type i=compressed_data.major_change_index[position[SO]]; !present && i<compressed_data.major_change_index[position[SO]+1]; ++i){
            present = compressed_data.non_major_index[i] == position[otherSO];
        }
    }
    return present;
};

// PUBBLIC METHODS:

// Extract vector
template<StorageOptions SO, typename T>
template<ExtractOptions RorC>
std::vector<T> SparseMatrix<SO,T>::extract_vector(size_type k) const{
    size_type dim;
    if constexpr(RorC == Row){dim = cols;}
    else{ dim = rows;}
    std::vector<T> vec(dim);
    
    if constexpr(static_cast<int>(RorC) == static_cast<int>(SO)){
        if(state == COMPRESSED){
            for(size_type i=compressed_data.major_change_index[k]; i<compressed_data.major_change_index[k+1]; ++i){
                vec[compressed_data.non_major_index[i]] = compressed_data.values[i];
            }
        }else{//uncompressed
            key_type lb;
            key_type ub;
            if constexpr (RorC==Row){ lb = {k,0}; ub = {k,cols};}
            else{ lb = {0,k}; ub = {rows,k};}

            for(auto it=uncompressed_data.lower_bound(lb); it!=uncompressed_data.upper_bound(ub); ++it){
                vec[it->first[otherSO]] = it->second;
            }
        }
    }else{
        if(state == COMPRESSED){
            for(size_type i=0; i<compressed_data.major_change_index.size()-1; ++i){
                for(size_type j=compressed_data.major_change_index[i]; j<compressed_data.major_change_index[i+1]; ++j){
                    if(compressed_data.non_major_index[j] == k){
                        vec[i] = compressed_data.values[j];
                    }
                }
            }
        }else{
            if constexpr (RorC==Col){
                for(size_type i=0; i<dim; ++i){
                    key_type position ({i,k});
                    auto it = uncompressed_data.find(position);
                    vec[i] = (it != uncompressed_data.end()) ? it->second : 0;
                }
            }else{
                for(size_type i=0; i<dim; ++i){
                    key_type position ({k,i});
                    auto it = uncompressed_data.find(position);
                    vec[i] = (it != uncompressed_data.end()) ? it->second : 0;
                }
            }
        }
    }
    return vec;
};

template<StorageOptions SO, typename T>
std::vector<T> SparseMatrix<SO,T>::get_row(size_type r)const{
    return extract_vector<Row>(r);
};

template<StorageOptions SO, typename T>
std::vector<T> SparseMatrix<SO,T>::get_col(size_type c)const{
    return extract_vector<Col>(c);
};

// Verify state
template<StorageOptions SO, typename T>
bool SparseMatrix<SO,T>::is_compressed() const {
    return state;
};

// Resize
template<StorageOptions SO, typename T>
void SparseMatrix<SO,T>::resize(size_type R, size_type C){
    // si potrebbe controllare se ho degli elementi con indici maggiori delle nuove taglie e se si eliminare quegli elementi
    rows = R;
    cols = C;
};

// Compress 
template<StorageOptions SO, typename T>
void SparseMatrix<SO,T>::compress(){
    if(state==UNCOMPRESSED){

        // riserva le dimensioni per i vettori dentro il container
        compressed_data.values.reserve(uncompressed_data.size());
        compressed_data.non_major_index.reserve(uncompressed_data.size());
        compressed_data.major_change_index.resize((SO==RowMajor)?rows+1:cols+1,0);

        // leggi la mappa nell'ordine giusto e mano a mano inserisci i valori dentro il container: i valori non nulli dentro values, gli indici non_major dentro il vettore apposito e dentro l'ultimo vettore il counter di quanti valori hai inserito(se è un for il numero delle iterazioni che hai fatto)
        // se voglio leggere la mappa in un modo solo allora devo fare l'overload di operator< per il container e poi qua devo selezionare il giusto indice da prendere dalla chiave della mappa
        // per fortuna gli enumerator sono convertibili ad interi quindi il non_major per rowmajor sarà colmajor = 1, e per colmajor sarà rowmajor = 0
        size_type count = 0;
        size_type old = 0;
        for(auto cit=uncompressed_data.cbegin(); cit!=uncompressed_data.cend(); ++cit, ++count){
            compressed_data.values.emplace_back(cit->second);
            compressed_data.non_major_index.emplace_back(cit->first[otherSO]);
            std::for_each(compressed_data.major_change_index.begin()+cit->first[SO]+1,compressed_data.major_change_index.end(),[](size_type& r){++r;});
        }

        // svuota la mappa
        uncompressed_data.clear();

        // cambia lo stato
        state = COMPRESSED;
    }else{
        std::cerr << "Matrix already compressed" << std::endl;
    }
};

// Uncompress 
template<StorageOptions SO, typename T>
void SparseMatrix<SO,T>::uncompress(){
    if(state == COMPRESSED){
        uncompressed_container_type new_uncompressed_data;

        for(size_type major=0; major<compressed_data.major_change_index.size()-1; ++major){
            for(size_type non_major=compressed_data.major_change_index[major]; non_major<compressed_data.major_change_index[major+1]; ++non_major){
                key_type position;
                if constexpr (SO == RowMajor) {
                    position[0] = major;
                    position[1] = compressed_data.non_major_index[non_major];
                } else {
                    position[1] = compressed_data.non_major_index[non_major];
                    position[0] = major;
                }
                new_uncompressed_data[position] = compressed_data.values[non_major];
            }
        }

        uncompressed_data = std::move(new_uncompressed_data);
        compressed_data.clear();

        // cambia lo stato
        state = UNCOMPRESSED;
    }else{
        std::cerr << "Matrix already uncompressed" << std::endl;
    }
};

// const version of call operator
template<StorageOptions SO, typename T>
const T& SparseMatrix<SO,T>::operator() (const size_type r, const size_type c) const {
    if(in_bound(r,c)){
        //typename uncompressed_container_type::key_type position = {r,c};
        key_type position({r,c});

        if (verify_presence(r,c)){

            if(state == UNCOMPRESSED){
                return uncompressed_data.at(position);
            }else{
                for (size_type i=compressed_data.major_change_index[position[SO]]; i<compressed_data.major_change_index[position[SO]+1]; ++i)
                    if (compressed_data.non_major_index[i] == position[otherSO]) return compressed_data.values[i];
            }

        }else{
            return T(0);
        }
    }
    else {
        return buffer;
    }
};

// non-const version of call operator
template<StorageOptions SO, typename T>
T& SparseMatrix<SO,T>::operator()(const size_type r, const size_type c) {
    if(in_bound(r,c)){

        //typename uncompressed_container_type::key_type position = {r,c};
        key_type position ({r,c});
        if (verify_presence(r,c)){
            if(state == UNCOMPRESSED) return uncompressed_data[position];
            else{
                for (size_type i=compressed_data.major_change_index[position[SO]]; i<compressed_data.major_change_index[position[SO]+1]; ++i)
                    if (compressed_data.non_major_index[i] == position[otherSO]) return compressed_data.values[i];
            return buffer;
            }
        }else{
            if(state == UNCOMPRESSED){
                std::cout << "L'elemento in posizione {" << r << "," << c << "} è stato aggiunto" << std::endl;
                return uncompressed_data[position];
            }
            else{
                std::cerr << "L'elemento in posizione {" << r << "," << c << "} non è presente, il valore è casuale" << std::endl;
                return buffer;
            }
        }
    }

    else return buffer;
};

// Deletion of wrongly added element
template<StorageOptions SO, typename T>
bool SparseMatrix<SO,T>::delete_element(const size_type r, const size_type c){
    if(state == UNCOMPRESSED){
        return uncompressed_data.erase({r,c})>0;
    }
    return false;
};

// Multiply operator by vectors
template<StorageOptions SO, typename T>
std::vector<T> SparseMatrix<SO,T>::operator*(const std::vector<T>& v) const{
    std::vector<T> result(rows);
    if(state == UNCOMPRESSED){
        for(auto cit=uncompressed_data.cbegin(); cit!=uncompressed_data.cend(); ++cit){
            result[cit->first[0]] += cit->second * v[cit->first[1]];
        }
    }else{//compressed
        if constexpr (SO==RowMajor){
            for(size_type i=0; i<rows; ++i){
                std::vector<T> row = get_row(i);
                result[i] = std::inner_product(row.begin(),row.end(),v.begin(),T(0));
            }
        }else { //ColumnMajor
            for(size_type i=0; i<cols; ++i){
                std::vector<T> col = get_col(i);
                std::transform(std::execution::par,col.begin(),col.end(),col.begin(),[&v,i](T col_i){return v[i]*col_i;});
                std::transform(std::execution::par,result.begin(),result.end(),col.begin(),result.begin(),std::plus<T>());
            }
        }
    }
    return result;
}

// Multiply operator by Matrix
template<StorageOptions SO, typename T>
std::vector<std::vector<T>> SparseMatrix<SO,T>::operator*(const SparseMatrix<SO,T>& l) const{
    std::vector<std::vector<T>> result(rows,std::vector<T>(l.cols,0));
    if(state == UNCOMPRESSED){
        for (auto cit=uncompressed_data.cbegin(); cit!=uncompressed_data.cend(); ++cit){
            for(size_type i=0; i<l.cols; ++i){
                const auto lcit = l.uncompressed_data.find({cit->first[0],i});
                if(lcit != l.uncompressed_data.end()){
                    result[cit->first[0]][i] += cit->second * lcit->second;
                }
            }
        }
    }else{
        for(size_type i=0; i<rows; ++i){
            std::vector<T> row = get_row(i);
            for(size_type j=0; j<l.cols; ++j){
                std::vector<T> col = l.get_col(j);
                result[i][j] = std::inner_product(row.begin(),row.end(),col.begin(),T(0));
            }
        }
    }
    return result;
}

template<StorageOptions SO, typename T>
SparseMatrix<SO, T> read_matrix_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open file");
    }

    while (file.peek() == '%') {
        file.ignore(2048, '\n');
    }

    // Leggi le dimensioni della matrice
    std::size_t n_rows, n_cols, n_elements;
    file >> n_rows >> n_cols >> n_elements;

    // Creazione container per i dati
    uncompressed_container_type data_map;

    for (std::size_t i = 0; i < n_elements; ++i) {
        std::size_t row, col;
        T value;
        file >> row >> col >> value;
        data_map[{row-1, col-1}] = value;
    }

    return SparseMatrix<SO, T>(n_rows, n_cols, data_map);
}

template<StorageOptions SO, typename T>
template<NormType N>
T SparseMatrix<SO,T>::norm() const{
    T norm = 0;
    if(state == COMPRESSED){
        if constexpr (N == One){
            for(size_type i=0; i<cols; ++i){
                std::vector<T> col = get_col(i);
                T row_sum = std::accumulate(col.begin(),col.end(),T(0),[](T acc, const T& p){return acc+std::abs(p);});
                norm = std::max(norm,row_sum);
            }
        }else if constexpr (N == Infinity){
            for(size_type i=0; i<rows; ++i){
                std::vector<T> row = get_col(i);
                T col_sum = std::accumulate(row.begin(),row.end(),T(0),[](T acc, const T& p){return acc+std::abs(p);});
                norm = std::max(norm,col_sum);
            }
        }else if constexpr (N == Froebenius){
            norm = std::sqrt(std::accumulate(compressed_data.values.cbegin(),compressed_data.values.cend(),T(0),[](T acc, T v){return acc+std::pow(v,2);}));
        }
    }else{
        if constexpr (N == One){

        }else if constexpr (N == Infinity){

        }else if constexpr (N == Froebenius){

        }
    }
    return norm;
};

#endif