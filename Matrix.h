#ifndef UNTITLED_MATRIX_H
#define UNTITLED_MATRIX_H

#include <iostream>
#include <vector>
#include <iomanip>
#include <math.h>

const double EPS = 1e-7;
//template<typename T> class Matrix;
//template <typename T> std::ostream & operator << (std::ostream &out, const Matrix<T> &);



template<typename T> class Matrix {
    T **data;
    int cols, rows;


public:
    Matrix();
    Matrix(int, int); // initialized diminsions.
    Matrix(T**, int, int);
    Matrix(std::initializer_list<std::initializer_list<T>>);

    int get_cols() const {return this->cols;}
    int get_rows() const {return this->rows;}

    void exchange_rows(int, int);
    void exchange_cols(int, int);

    static Matrix<T> matmul(const Matrix<T>&, const Matrix<T>&);
    static Matrix<T> transpose(const Matrix<T>&);

    friend std::ostream& operator<<(std::ostream &out, const Matrix<T> & mat) {
        for(int i = 0; i < mat.get_rows(); i++) {
            out << "{";
            for(int j = 0; j < mat.get_cols(); j++) {
                if(fabs(mat[i][j] - 0) < EPS)
                    out << std::fixed << std::setprecision(4) << std::setw(5) << (mat[i][j] = 0);
                else out << std::fixed << std::setprecision(4) << std::setw(5) << mat[i][j];
                if(j != mat.get_cols() - 1)
                    out << ' ';
            }
            out << "}";
            if(i != mat.get_rows() - 1)
                out << ',' << std::endl;
        }
        return out;
    }

    // returns the identity matrix of size N
    static Matrix<T> eye(int N);


    T* operator[](int i) const {
        return data[i];
    }

    void gaussian_elemination();
    int zero_rows();
    bool is_inconsistent();

    void upper();
    ~Matrix();
};




#endif //UNTITLED_MATRIX_H
