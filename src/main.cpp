#include "Utils.hpp"

int main(int argc, char* argv[]){
    // Parse command line arguments
    int verbose = 0; //is 0, 1, or 2
    int n_iter = 1;
    // Loop over all command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose"|| arg == "-v") {
            if (i + 1 < argc) {
                int check = std::stoi(argv[i + 1]); // Convert the next argument to an integer
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
                n_iter = std::stoi(argv[i + 1]); // Convert the next argument to an integer
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

    // Define a lambda function to run the real experiment
    auto run_real = [&]() {
        std::cout << "SCALAR EXPERIENCE -- ROW_MAJOR" << std::endl;
        std::cout <<"--------------------------------------" << std::endl;

        // Read the matrix from file
        SparseMatrix<SO_R,double> m_r = read_matrix_from_file<SO_R,double>("lnsp_131.mtx");

        // Generate a random vector for the multiplication experiment
        std::vector<double> v = vector_generator<double>(m_r.get_n_cols());
        // Generate a random matrix for the multiplication experiment
        SparseMatrix<SO_R,double> m_r2 = generate_random_matrix<SO_R,double>(131,131,536,-50,50);
        
        test_matrix(timer,m_r,m_r2,v,n_iter);
    
        std::cout << "SCALAR EXPERIENCE -- COLUMN_MAJOR" << std::endl;
        std::cout <<"--------------------------------------" << std::endl;

        // Read the matrix from file
        SparseMatrix<SO_C,double> m_c = read_matrix_from_file<SO_C,double>("lnsp_131.mtx");
        // Generate a random matrix for the multiplication experiment
        SparseMatrix<SO_C,double> m_c2 = generate_random_matrix<SO_C,double>(131,131,536,-50,50);

        test_matrix(timer,m_c,m_c2,v,n_iter);
    };

    // Define a lambda function to run the complex experiment
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


/*  //Old test for compress and uncompress methods.
    //If you want you can add these lines inside main file and run it. (better to comment the actual lines of main.cpp to avoid name conflicts)
    constexpr StorageOptions SO = RowMajor;
    uncompressed_container_type<SO,double> dati = {{{0,1},1.},{{0,2},2.9},{{1,2},2.},{{3,0},3.},{{1,9},40.3},{{3,3},4.},{{4,3},5.},{{4,5},6.}};

    //uncompressed_container_type<ColumnMajor,double> dati = {{{0,1},1.},{{1,2},2.},{{3,0},3.},{{3,3},4.},{{4,3},5.},{{4,5},6.}};
    SparseMatrix<SO,double> m (5,10,dati);

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
    std::cout <<"matrice compressa"<<std::endl;
    std::cout << std::endl;

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
    std::cout <<"matrice uncompressa"<<std::endl;
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

*/