#include "SparseMatrix.hpp"

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

int main(){
    //std::map<key_type<ColumnMajor,double>,double> dati = {{{0,1},1.},{{1,2},2.},{{3,0},3.},{{3,3},4.},{{4,3},5.},{{4,5},6.}};
    //std::map<std::array<std::size_t,2>,double> dati = {{{0,1},1.},{{1,2},2.},{{3,0},3.},{{3,3},4.},{{4,3},5.},{{4,5},6.}};
    //std::map<Key_Uncompressed_Struct<RowMajor,double>,double> dati = {{{0,1},1.},{{1,2},2.},{{3,0},3.},{{3,3},4.},{{4,3},5.},{{4,5},6.}};
    constexpr StorageOptions SO = RowMajor;
    uncompressed_container_type<SO,double> dati = {{{0,1},1.},{{0,2},2.9},{{1,2},2.},{{3,0},3.},{{1,9},40.3},{{3,3},4.},{{4,3},5.},{{4,5},6.}};

    //uncompressed_container_type<ColumnMajor,double> dati = {{{0,1},1.},{{1,2},2.},{{3,0},3.},{{3,3},4.},{{4,3},5.},{{4,5},6.}};
    SparseMatrix<SO,double> m (5,10,dati);
    // m.compress();
    // std::cout << m(4,9) << std::endl;
    // m(4,9) = 10.5;
    // std::cout << m(4,9) << std::endl;

    m.compress();

    for(std::size_t i=0;i<5;++i, std::cout << std::endl){
        std::vector<double> r1 = m.get_row(i);
        for (auto cit = r1.cbegin(); cit != r1.cend(); ++cit){
            std::cout << *cit << " ";
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;

    for(std::size_t i=0;i<10;++i, std::cout << std::endl){
        std::vector<double> r1 = m.get_col(i);
        for (auto cit = r1.cbegin(); cit != r1.cend(); ++cit){
            std::cout << *cit << " ";
        }
    }

    std::cout << std::endl;
    std::cout << std::endl;
    m.uncompress();
    for(std::size_t i=0;i<5;++i, std::cout << std::endl){
        std::vector<double> r1 = m.get_row(i);
        for (auto cit = r1.cbegin(); cit != r1.cend(); ++cit){
            std::cout << *cit << " ";
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;

    for(std::size_t i=0;i<10;++i, std::cout << std::endl){
        std::vector<double> r1 = m.get_col(i);
        for (auto cit = r1.cbegin(); cit != r1.cend(); ++cit){
            std::cout << *cit << " ";
        }
    }
    // m(1,8) = 19.4;
    // std::cout << m(1,8) << std::endl;

    // m(3,2) = 7.9;
    // std::cout << m(3,2) << std::endl;

    // m(13,14) = 2.4;
    // std::cout << m(13,14) << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    // std::cout << m.is_compressed() << std::endl;
    // m.compress();
    // // std::cout << m.is_compressed() << std::endl;

    // // uncompressed_container_type<RowMajor,double> dati2 = {{{2,2},5.},{{3,8},3.},{{3,9},3.},{{10,3},3.}};
    // // const SparseMatrix<RowMajor,double> mc (10,15,dati2);
    // // std::cout << mc(2,2) << std::endl;
    // // std::cout << mc.is_compressed() << std::endl;

    // std::cout << std::endl;
    // std::cout << std::endl;
    
    
    // std::cout << m(0,1) << std::endl;
    // std::cout << m(4,9) << std::endl;
    // std::cout << m(3,3) << std::endl;
    // std::cout << m(4,3) << std::endl;
    // m(1,1) = 10.5;
    // std::cout << m(1,1) << std::endl;

    // std::cout << std::endl;
    // std::cout << std::endl;

    // m.uncompress();
    // std::cout << m.is_compressed() << std::endl;
    // std::cout << std::endl;

    // std::cout << m(0,1) << std::endl;
    // std::cout << m(4,9) << std::endl;
    // std::cout << m(3,3) << std::endl;
    // std::cout << m(4,3) << std::endl;
    // m(1,1) = 10.5;
    // std::cout << m(1,1) << std::endl;

    
    // std::cout << m(4,9) << std::endl;
    // m.delete_element(4,9);

    // m.compress();

    // std::cout << std::endl;
    // std::cout << std::endl;

    std::vector<double> vec = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
    std::vector<double> res = m*vec;

    for(auto cit=res.cbegin();cit!=res.cend();++cit){
        std::cout <<"el "<< *cit << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::endl;
    m.compress();
    std::vector<double> res2 = m*vec;

    for(auto cit=res2.cbegin();cit!=res2.cend();++cit){
        std::cout <<"el "<< *cit << std::endl;
    }


    // m(3,3) = 14;
    // std::cout << m(3,3);
    // m(20,20) = 89;
    //const SparseMatrix<RowMajor,double>& mc = m;
    //std::cout << mc(2,3) << std::endl;

    uncompressed_container_type<SO,std::complex<double>> dati_complex = {{{0,1},{1.,3.}},{{0,2},{2.9,4.5}},{{1,2},{2.,3.1}},{{3,0},{3.,2.9}},{{1,9},{40.3,1}},{{3,3},{4.,0.6}},{{4,3},{5.,2.32}},{{4,5},{6.,9.8}}};

    SparseMatrix<SO,std::complex<double>> m_complex (5,10,dati_complex);


    m_complex.compress();

    for(std::size_t i=0;i<5;++i, std::cout << std::endl){
        std::vector<std::complex<double>> r1 = m_complex.get_row(i);
        for (auto cit = r1.cbegin(); cit != r1.cend(); ++cit){
            std::cout << "re: " << cit->real() << " im: "<< cit->imag() << " ";
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;

    for(std::size_t i=0;i<10;++i, std::cout << std::endl){
        std::vector<std::complex<double>> r1 = m_complex.get_col(i);
        for (auto cit = r1.cbegin(); cit != r1.cend(); ++cit){
            std::cout << "re: " << cit->real() << " im: "<< cit->imag() << " ";
        }
    }

    std::cout << std::endl;
    std::cout << std::endl;
    m_complex.uncompress();
    for(std::size_t i=0;i<5;++i, std::cout << std::endl){
        std::vector<std::complex<double>> r1 = m_complex.get_row(i);
        for (auto cit = r1.cbegin(); cit != r1.cend(); ++cit){
            std::cout << "re: " << cit->real() << " im: "<< cit->imag() << " ";
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;

    for(std::size_t i=0;i<10;++i, std::cout << std::endl){
        std::vector<std::complex<double>> r1 = m_complex.get_col(i);
        for (auto cit = r1.cbegin(); cit != r1.cend(); ++cit){
            std::cout << "re: " << cit->real() << " im: "<< cit->imag() << " ";
        }
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::vector<std::complex<double>> vec2 = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
    std::vector<std::complex<double>> res3 = m_complex*vec2;

    for(auto cit=res3.cbegin();cit!=res3.cend();++cit){
        std::cout <<"el "<< "re: " << cit->real() << " im: "<< cit->imag() << " ";
    }

    std::cout << std::endl;
    std::cout << std::endl;
    m_complex.compress();
    std::vector<std::complex<double>> res4 = m_complex*vec2;

    for(auto cit=res4.cbegin();cit!=res4.cend();++cit){
        std::cout <<"el "<< "re: " << cit->real() << " im: "<< cit->imag() << " ";
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "Norma 1: " << m.norm<One>() << std::endl;

    std::cout << "Norma Inf: " << m.norm<Infinity>() << std::endl;

    std::cout << "Norma Fro: " << m.norm<Froebenius>() << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "Norma 1: " << m_complex.norm<One>() << std::endl;

    std::cout << "Norma Inf: " << m_complex.norm<Infinity>() << std::endl;

    std::cout << "Norma Fro: " << m_complex.norm<Froebenius>() << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;



    SparseMatrix<SO,double> m7 = read_matrix_from_file<SO,double>("lnsp_131.mtx");
    std::cout << "Matrix read from file" << std::endl;
    std::cout << "Norma 1: " << m7.norm<One>() << std::endl;
    std::cout << "Norma Inf: " << m7.norm<Infinity>() << std::endl;
    std::cout << "Norma Fro: " << m7.norm<Froebenius>() << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;
    
    std::cout <<"El:" << m7(0,0) << std::endl;
    std::cout <<"El:" << m7(8,0) << std::endl;
    std::cout <<"El:" << m7(35,10) << std::endl;

    

    std::cout << std::endl;
    std::cout << std::endl;
    m7.compress();
    
    std::cout <<"El:" << m7(0,0) << std::endl;
    std::cout <<"El:" << m7(8,0) << std::endl;
    std::cout <<"El:" << m7(35,10) << std::endl;

    return 0;
};
