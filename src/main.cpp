#include "Utils.hpp"

int main(int argc, char* argv[]){
    // Parse command line arguments
    int verbose = 0;
    int n_iter = 1;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose"|| arg == "-v") {
            if (i + 1 < argc) {
                int check = std::stoi(argv[i + 1]);
                if(check == 0 || check == 1 || check == 2){
                    verbose = check;
                    ++i;
                }else{
                    std::cerr << "Error: --verbose option requires an argument between 0 and 2" << std::endl;
                    return 1;
                } 
            }else{
                std::cerr << "Error: --verbose option requires an argument" << std::endl;
                return 1;
            }
        }else if (arg == "--n_iter" || arg == "-n") {
            if (i + 1 < argc) {
                n_iter = std::stoi(argv[i + 1]);
                ++i; 
            }else{
                std::cerr << "Error: --n_iter option requires an argument" << std::endl;
                return 1;
            }
        }
    }

    Timings::Chrono timer;
    constexpr StorageOptions SO_R = RowMajor;
    constexpr StorageOptions SO_C = ColumnMajor;

    auto run_real = [&]() {
        std::cout << "SCALAR EXPERIENCE -- ROW_MAJOR" << std::endl;
        std::cout <<"--------------------------------------" << std::endl;

        SparseMatrix<SO_R,double> m_r = read_matrix_from_file<SO_R,double>("lnsp_131.mtx");

        // Generate a random vector for the multiplication experiment
        std::vector<double> v = vector_generator<double>(m_r.get_n_cols());
        // Generate a random matrix for the multiplication experiment
        SparseMatrix<SO_R,double> m_r2 = generate_random_matrix<SO_R,double>(131,131,536,-50,50);
        
        test_matrix(timer,m_r,m_r2,v,n_iter);
    
        std::cout << "SCALAR EXPERIENCE -- COLUMN_MAJOR" << std::endl;
        std::cout <<"--------------------------------------" << std::endl;

        SparseMatrix<SO_C,double> m_c = read_matrix_from_file<SO_C,double>("lnsp_131.mtx");
        // Generate a random matrix for the multiplication experiment
        SparseMatrix<SO_C,double> m_c2 = generate_random_matrix<SO_C,double>(131,131,536,-50,50);

        test_matrix(timer,m_c,m_c2,v,n_iter);
    };

    auto run_complex = [&]() {
        std::cout << "COMPLEX EXPERIENCE -- ROW_MAJOR" << std::endl;
        std::cout <<"--------------------------------------" << std::endl;
        // Crea la matrice
        SparseMatrix<SO_R,std::complex<double>> m_r_complex = generate_random_matrix<SO_R,std::complex<double>>(131,131,536,{-50,-50},{50,50});

        // Generate a random vector for the multiplication experiment
        std::vector<std::complex<double>> v_complex = vector_generator<std::complex<double>>(m_r_complex.get_n_cols(),std::complex<double>(-100,-100),std::complex<double>(100,100));
        
        // Generate a random matrix for the multiplication experiment
        SparseMatrix<SO_R,std::complex<double>> m_r_complex2 = generate_random_matrix<SO_R,std::complex<double>>(131,131,536,{-50,-50},{50,50});

        test_matrix(timer,m_r_complex,m_r_complex2,v_complex,n_iter);

        std::cout << "COMPLEX EXPERIENCE -- COLUMN_MAJOR" << std::endl;
        std::cout <<"--------------------------------------" << std::endl;
        // Crea la matrice
        SparseMatrix<SO_C,std::complex<double>> m_c_complex = generate_random_matrix<SO_C,std::complex<double>>(131,131,536,{-50,-50},{50,50});
        // Generate a random matrix for the multiplication experiment
        SparseMatrix<SO_C,std::complex<double>> m_c_complex2 = generate_random_matrix<SO_C,std::complex<double>>(131,131,536,{-50,-50},{50,50});

        test_matrix(timer,m_c_complex,m_c_complex2,v_complex,n_iter);
    };


    if(verbose == 1){
        run_real();
    }else if(verbose == 2){
        run_complex();
    }else{
        run_real();
        run_complex();
    }

    return 0;
};