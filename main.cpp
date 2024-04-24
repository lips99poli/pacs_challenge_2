#include "SparseMatrix.hpp"

int main(){
    std::map<std::array<std::size_t,2>,double> dati = {{{1,1},5.},{{1,8},3.},{{5,8},3.},{{10,3},3.}};
    SparseMatrix<RowMajor,double> m (10,15,dati);
    std::cout << m(1,6) << std::endl;
    m(1,1) = 10.5;
    std::cout << m(1,1) << std::endl;
    std::cout << m.is_compressed() << std::endl;
    m.compress();
    std::cout << m.is_compressed() << std::endl;
    std::map<std::array<std::size_t,2>,double> dati2 = {{{2,2},5.},{{3,8},3.},{{3,9},3.},{{10,3},3.}};
    const SparseMatrix<RowMajor,double> mc (10,15,dati2);
    std::cout << mc(2,2) << std::endl;
    std::cout << mc.is_compressed() << std::endl;



    // m(3,3) = 14;
    // std::cout << m(3,3);
    // m(20,20) = 89;
    //const SparseMatrix<RowMajor,double>& mc = m;
    //std::cout << mc(2,3) << std::endl;


    return 0;
};
