#ifndef SM_HPP
#define SM_HPP

#include "SparseMatrixTraits.hpp"
#include <iostream>


template<StorageOptions SO, typename T>
class SparseMatrix{

    using size_type = typename Value_Traits<T>::size_type;
    using value_type = typename Value_Traits<T>::value_type; //in realtà questo è T quindi non serve
    using uncompressed_container_type = typename Value_Traits<T>::uncompressed_container;
    using compressed_container_type = typename Value_Traits<T>::compressed_container;

    private:
    size_type rows;
    size_type cols;
    uncompressed_container_type uncompressed_data;
    compressed_container_type compressed_data;

    static constexpr T zero = 0.; //lo zero in realtà potrebbe essere complesso

    static constexpr StorageOptions otherSO = (SO==RowMajor)? ColumnMajor : RowMajor; //perchè il prof la fa static??

    inline bool in_bound(size_type r, size_type c) const {
        bool in_bound = r<=rows && c<=cols;
        if (!in_bound) std::cerr << "Indexes not compatible with Matrix dimensions";
        return in_bound;
    };

    public:
    // Constructor
    SparseMatrix(size_type R = 0, size_type C = 0, const uncompressed_container_type& init = {}):
    rows(R), cols(C), uncompressed_data(init) {
        if (!init.empty()){
            bool check = true;
            for (auto cit=init.begin(); check && cit!=init.end(); ++cit){
                check = cit->first[0]<=R && cit->first[1]<=C;
            }
            //assert(check && "Given dimensions are not compatible with the given values");
        }
    };

    size_type get_rows()const {return rows;};
    size_type get_cols()const {return cols;};
    bool is_compressed()const { return compressed_data.empty();};
    /*T get_val(size_type r, size_type c) const{
        in_bound(r,c);
        typename uncompressed_container_type::key_type position = {r,c};
        T ret;

        if(is_compressed()){
            //to be completed
        }else{
            const auto it = uncompressed_data.find(position);
            if (it != uncompressed_data.end())
                return it->second;
        }
        return 0;
    };*/

    void resize(size_type R, size_type C){
        // si potrebbe controllare se ho degli elementi con indici maggiori delle nuove taglie e in quel caso mandare lo user a cagare
        rows = R;
        cols = C;
    };

    void compress(){
        // riserva le dimensioni per i vettori dentro il constainer
        compressed_data.values.reserve(uncompressed_data.size());
        compressed_data.non_major_index.reserve(uncompressed_data.size());
        compressed_data.major_change_index.reserve((SO==RowMajor)?rows:cols);

        // leggi la mappa nell'ordine giusto e mano a mano inserisci i valori dentro il container: i valori non nulli dentro values, gli indici non_major dentro il vettore apposito e dentro l'ultimo vettore il counter di quanti valori hai inserito(se è un for il numero delle iterazioni che hai fatto)
        // se voglio leggere la mappa in un modo solo allora devo fare l'overload di operator< per il container e poi qua devo selezionare il giusto indice da prendere dalla chiave della mappa
        // per fortuna gli enumerator sono convertibili ad interi quindi il non_major per rowmajor sarà colmajor = 1, e per colmajor sarà rowmajor = 0
        size_type count = 0;
        for(auto cit=uncompressed_data.cbegin(); cit!=uncompressed_data.cend(); ++cit, ++count){
            compressed_data.values.emplace_back(cit->second);
            compressed_data.non_major_index.emplace_back(cit->first[SO]);
            compressed_data.major_change_index.emplace_back(count);
        }
        // svuota la mappa
        uncompressed_data.clear();
    };

    void uncompress(){
        uncompressed_container_type new_uncompressed_data;

        for(size_type major=0; major<compressed_data.major_change_index.size()-1; ++major){
            for(size_type non_major=compressed_data.major_change_index[major]; non_major<compressed_data.major_change_index[major+1]; ++non_major){
                typename uncompressed_container_type::key_type position;
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
    };


    // const version of call operator
    const T& operator()(const size_type r, const size_type c) const {
        in_bound(r,c);

        typename uncompressed_container_type::key_type position = {r,c};

        if(is_compressed()){//compressed state
            for(size_type i=compressed_data.major_change_index[position[SO]]; i<compressed_data.major_change_index[position[SO]+1]; ++i){
                if (compressed_data.non_major_index == position[otherSO]){
                    return compressed_data.values[i];
                }
            }

        }else{//uncompressed state
            const auto it = uncompressed_data.find(position);
            if (it != uncompressed_data.end())
                return it->second;
        }

        // In bound but not present
        return zero;
    };
    // non-const version of call operator
    T& operator()(const size_type r, const size_type c) {
        in_bound(r,c);
        typename uncompressed_container_type::key_type position = {r,c};

        if(is_compressed()){
            //to be completed: devo restituire una reference a values nella posizione in cui inserisco il nuovo elemento.
            for(size_type i=compressed_data.major_change_index[position[SO]]; i<compressed_data.major_change_index[position[SO]+1]; ++i){
                if (compressed_data.non_major_index[i] == position[otherSO]){
                    return compressed_data.values[i];
                }
            }
            std::cerr << "In compressed state you can only change values of existing numbers, uncompress your data if you want to change zeros" << std::endl;
        }else{
            return uncompressed_data[position]; //attenzione che se poi lo user non lo cambia T rimane al default che io non ho ancora gestito
        }
    };

};

#endif