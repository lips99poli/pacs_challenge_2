#ifndef SM_HPP
#define SM_HPP

#include "SparseMatrixTraits.hpp"
#include <iostream>
#include <limits>
#include <algorithm>

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
    T zero = 0.;
    T nan = std::numeric_limits<T>::quiet_NaN();
    T buffer = std::numeric_limits<T>::quiet_NaN();

    //static constexpr T zero = 0.; //lo zero in realtà potrebbe essere complesso

    static constexpr StorageOptions otherSO = (SO==RowMajor)? ColumnMajor : RowMajor; //perchè il prof la fa static?? se lo tolgo da errore

    inline bool in_bound(size_type r, size_type c) const;

    bool verify_presence(size_type r, size_type c) const;

    public:
    // Constructor    
    SparseMatrix(size_type R = 0, size_type C = 0, const std::map<std::array<std::size_t,2>,T>& init = {}):
    rows(R), cols(C), state(UNCOMPRESSED){
        // if (!init.empty()){
        //     bool adjusted = false;
        //     for (auto cit=init.cbegin(); cit!=init.cend(); ++cit){
        //         if (cit->first[0]>=rows){
        //             adjusted = true;
        //             resize(cit->first[0],cols);
        //         }
        //         if(cit->first[1]>=cols){
        //             adjusted = true;
        //             resize(rows,cit->first[1]);
        //         }
        //     }
        //     if (adjusted) std::cout << "Dimensions adjusted" << std::endl;
        // }

        if(!init.empty()){
            for(auto cit=init.cbegin(); cit!=init.cend(); ++cit){
                uncompressed_data[cit->first] = cit->second;
            }
        }
    };

    size_type get_rows()const {return rows;};
    size_type get_cols()const {return cols;};
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
    
    //typename uncompressed_container_type::key_type position = {r,c};
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

// Verify state
template<StorageOptions SO, typename T>
bool SparseMatrix<SO,T>::is_compressed() const {
    return state;
};

// Resize
template<StorageOptions SO, typename T>
void SparseMatrix<SO,T>::resize(size_type R, size_type C){
    // si potrebbe controllare se ho degli elementi con indici maggiori delle nuove taglie e in quel caso mandare lo user a cagare
    rows = R;
    cols = C;
};

// Compress 
template<StorageOptions SO, typename T>
void SparseMatrix<SO,T>::compress(){
    if(state==UNCOMPRESSED){
        std::cout << "otherSO "<<otherSO<<std::endl;
        std::cout << "SO "<<SO<<std::endl;

        std::cout<<"mappa:" <<std::endl;
        for(auto cit=uncompressed_data.cbegin(); cit!=uncompressed_data.cend(); ++cit){
            std::cout << cit->second << std::endl;
        }

        // riserva le dimensioni per i vettori dentro il constainer
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
        for(std::size_t i=0;i<compressed_data.values.size();++i){
            std::cout<< "value " <<compressed_data.values[i] << std::endl;
            std::cout<< "nm-index  " <<compressed_data.non_major_index[i] << std::endl;
        }
        for(std::size_t i=0;i<compressed_data.major_change_index.size();++i){
            std::cout<< "mj_change " <<compressed_data.major_change_index[i] << std::endl;
            }

        std::cout<< "values" << compressed_data.values.size() <<"values" <<  compressed_data.non_major_index.size() <<"values" <<  compressed_data.major_change_index.size() << std::endl;
        // svuota la mappa
        uncompressed_data.clear();

        // cambia lo stato
        state = COMPRESSED;
    }
};

// Uncompress 
template<StorageOptions SO, typename T>
void SparseMatrix<SO,T>::uncompress(){
    if(state == COMPRESSED){
        uncompressed_container_type new_uncompressed_data;

        for(size_type major=0; major<compressed_data.major_change_index.size()-1; ++major){

            for(size_type non_major=compressed_data.major_change_index[major]; non_major<compressed_data.major_change_index[major+1]; ++non_major){
                //typename uncompressed_container_type::key_type position;
                key_type position;
                if (SO == RowMajor) {
                    position = {major,compressed_data.non_major_index[non_major]};
                } else {
                    position = {compressed_data.non_major_index[non_major],major};
                }
                new_uncompressed_data[position] = compressed_data.values[non_major];
            }

        }

        uncompressed_data = std::move(new_uncompressed_data);
        compressed_data.clear();

        // cambia lo stato
        state = UNCOMPRESSED;
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
            return 0.;
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
            return zero;
            }
        }else{
            if(state == UNCOMPRESSED){
                
                std::cout << "L'elemento in posizione {" << r << "," << c << "} è stato aggiunto" << std::endl;
                return uncompressed_data[position];
            }
            else{
                return nan;
            }
        }
    }

    else return buffer;
};

// Deletion of wrongly added element
template<StorageOptions SO, typename T>
bool SparseMatrix<SO,T>::delete_element(const size_type r, const size_type c){
    if(state == UNCOMPRESSED){
        uncompressed_data.erase({r,c});
        return true;
    }
    return false;
};




#endif