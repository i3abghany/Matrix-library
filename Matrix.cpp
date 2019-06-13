#include <assert.h>
#include <algorithm>
#include "Matrix.h"

template <typename T> Matrix<T>::Matrix(size_t ROWS, size_t COLS, T initial) {
    this->data.resize(ROWS);
    for(size_t i = 0; i < this->data.size(); i++) {
        this->data[i].resize(COLS, initial);
    }
}

template <typename T> Matrix<T>::Matrix(T ** data_to_copy, size_t ROWS, size_t COLS) {
    this->data.resize(ROWS);
    for(size_t i = 0; i < this->data.size(); i++) {
        this->data[i].resize(COLS);
    }
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            this->data[i][j] = data_to_copy[i][j];
        }
    }
}

template <typename T> Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> data_to_copy) {
    int ROWS = data_to_copy.size();
    int COLS = data_to_copy.begin()->size();
    for(auto row : data_to_copy) {
        if (row.size() != COLS) {
            throw std::runtime_error{"Rows can't have different numbers of elements."};
        }
    }
    this->data.resize(ROWS);
    for(size_t i = 0; i < this->data.size(); i++) {
        this->data[i].resize(COLS);
    }
    int i = 0, j = 0;
    for(auto row : data_to_copy) {
        for(auto el : row) {
            this->data[i][j++] = el;
        }
        i++;
        j = 0;
    }
}

template<typename T>
Matrix<T>::Matrix(std::vector<std::vector<T>> data_to_copy) {
    int ROWS = data_to_copy.size();
    int COLS = data_to_copy.begin()->size();
    for(auto &row : data_to_copy) {
        if (row.size() != COLS) {
            throw std::runtime_error{"Rows can't have different numbers of elements."};
        }
    }
    this->data = data_to_copy;
}

template<typename T> void Matrix<T>::exchange_cols(int c1, int c2) {
    for(int i = 0; i < this->get_rows(); i++) {
        std::swap(this->data[i][c1], this->data[i][c2]);
    }
}

// eexchanges rows r1 and r2.
template <typename T> void Matrix<T>::exchange_rows(int r1, int r2) {
    std::swap(this->data[r1], this->data[r2]);
}

// Matrix multiplication.
template <typename T> Matrix<T> Matrix<T>::matmul(const Matrix<T> & first, const Matrix<T> & second) {

    if(first.get_cols() != second.get_rows())
        throw std::runtime_error{"Size doesn't match for matrix multiplication."};
    Matrix<T> result{first.get_rows(), second.get_cols()};
    for(int i = 0; i < first.get_rows(); i++) {
        for(int j = 0; j < second.get_cols(); j++) {
            for(int k = 0; k < first.get_cols(); k++)
                result[i][j] += first[i][k] * second[k][j];
        }
    }
    return result;
}

// Transposes a matrix and returns the transposed copy.
template <typename T> Matrix<T> Matrix<T>::transpose(const Matrix<T>& mat) {
    Matrix<T> result{mat.get_cols(), mat.get_rows()};
    for(int i = 0; i < result.get_rows(); i++) {
        for(int j = 0; j < result.get_cols(); j++) {
            result[i][j] = mat[j][i];
        }
    }
    return result;
}

// returns an identity matrix of size NxN
template <typename T> Matrix<T> Matrix<T>::eye(size_t N) {
    Matrix<T> result = {N, N};
    for(int i = 0; i < N; i++) {
        result[i][i] = 1;
    }
    return result;
}

template <typename T> int Matrix<T>::zero_rows() {
    int rank = this->get_rows();
    for(int i = 0; i < this->get_rows(); i++) {
        bool is_free_row = zero_row(i);
        if(is_free_row) {
            rank--;
        }
    }
    return this->get_rows() - rank;
}

// only applied after elimination, checks if the last non-zero row has all zeros but b (in Ax=b) is not zero.
template <typename T> bool Matrix<T>::is_inconsistent() {
    const int free_rows = this->zero_rows();
    const int last_row = this->get_rows() - free_rows - 1;
    bool is_all_zero_but_last = true;
    for(int elem = 0; elem < this->get_cols() - 1; elem++) {
        is_all_zero_but_last &= ((*this)[last_row][elem] == 0);
    }
    return is_all_zero_but_last && ((*this)[last_row][this->get_cols() - 1] != 0);
}

template <typename T> void Matrix<T>::gaussian_elimination(bool mode) {
    std::vector<size_t> piv_idxs;
    bool free_col = false;
    for (int row = 0; row < this->get_rows(); row++) {
        T pivot = (*this)[row][row];
        int piv_idx = row;
        if(free_col) {
            for(int c = row; c < this->get_rows(); c++) {
                if((*this)[row][c] != 0) {
                    pivot = (*this)[row][c];
                    piv_idx = c;
                    break;
                }
            }
        }
        if (pivot != 0) {
            if (pivot != 1) {
                for (int elem = 0; elem < this->get_cols(); elem++)
                    (*this)[row][elem] /= pivot;
            }
            piv_idxs.push_back(piv_idx);
            for (int r = 0; r < this->get_rows(); r++) {
                if (r == row)
                    continue;
                T multiplier = (*this)[r][piv_idx/* row */];
                for (int elem = 0; elem < this->get_cols(); elem++) {
                    (*this)[r][elem] -= multiplier * (*this)[row][elem];
                }
                if(!mode) {
                    std::cout << *(this) << "\n\n\n";
                }
            }
        }

        // if a pivot is equal to 0, free column.
        else /* if(pivot == 0) */ {
            bool found_piv = false;
            for (int successor_row = row + 1; successor_row < this->get_rows(); successor_row++) {
                if ((*this)[successor_row][row] != 0) {
                    found_piv = true;
                    exchange_rows(successor_row, row);
                    break;
                }
            }

            bool finish = true;
            for(int fr = row; fr < this->get_rows(); fr++) {
                finish &= zero_row(fr);
            }
            if(!finish) row--;
            free_col = !found_piv;
        }
    } // END OF ELIMINATION.
    if(!mode) {
        print_solutions(piv_idxs);
    }
}

template<typename T>
void Matrix<T>::print_solutions(const std::vector<size_t> &piv_idxs) {
    const int num_of_zero_rows = this->zero_rows();
    if (is_inconsistent()) {
        std::cout << "the system is inconsistent." << std::endl;
        return;
    }
    std::vector<std::string> solutions;
    std::cout << std::endl;
    for(int row = 0; row < this->get_rows() && !zero_row(row); row++) {
        std::string sol = ("x" + std::to_string(piv_idxs[row]) + " = ");
        for(int i = piv_idxs[row] + 1; i < this->get_cols() - 1; i++) {
            if(fabs((*this)[row][i]) <= EPS) {
                (*this)[row][i] = 0;
                continue;
            }
            sol += ((*this)[row][i] > 0 ? "-" : "") + std::to_string((*this)[row][i]) + "x" +
                    std::to_string(i + 1) + " ";
        }
        sol += ((*this)[row][this->get_cols() - 1] >= 0 ? "+" : "") +
                std::to_string((*this)[row][this->get_cols() - 1]);
        solutions.push_back(sol);
    }

    for (auto &s : solutions)
        std::cout << s << std::endl;
}

template <typename T> Matrix<T> Matrix<T>::upper() {
    Matrix<T> L{ eye(this->get_rows()) };
    Matrix<T> tmp_L {eye(this->get_rows())};
    bool free_col = false;
    for (int row = 0; row < this->get_rows(); row++) {
        T pivot = (*this)[row][row];
        int piv_idx = row;
        if(free_col) {
            for(int c = row; c < this->get_rows(); c++) {
                if((*this)[row][c] != 0) {
                    pivot = (*this)[row][c];
                    piv_idx = c;
                    break;
                }
            }
        }
        if (pivot != 0) {
            for (int successor_row = row + 1; successor_row < this->get_rows(); successor_row++) {
                T multiplier = (*this)[successor_row][/*row*/piv_idx] / pivot;

                for (int elem = 0; elem < this->get_cols(); elem++) {
                    (*this)[successor_row][elem] -= multiplier * (*this)[row][elem];
                    if(elem == row) {
                        tmp_L = eye(this->get_rows());
                        tmp_L[successor_row][elem] = -1 * multiplier;
                        L = tmp_L * L;
                    }
                }
            }
        } else {
            bool found_piv = false;
            for (int successor_row = row + 1; successor_row < this->get_rows(); successor_row++) {
                if ((*this)[successor_row][row] != 0) {
                    found_piv = true;
                    exchange_rows(successor_row, row);
                    L = inverse(permutation_matrix(this->get_rows(), successor_row, row)) * L;
                    break;
                }
            }
            bool finish = true;
            for(int fr = row; fr < this->get_rows(); fr++) {
                finish &= zero_row(fr);
            }
            if(!finish) row--;
            free_col = !found_piv;
        }
    }
    return L;
}

template<typename T>
Matrix<T> Matrix<T>::operator+=(const Matrix<T> &m) {
    *this = (*this) + m;
    return (*this);
}

template<typename T>
Matrix<T> Matrix<T>::operator*=(const Matrix<T> &m) {
    *this = (*this) * m;
    return *this;
}

template<typename T>
std::pair<Matrix<T>, Matrix<T>> Matrix<T>::LU(const Matrix &m) {
    Matrix<T> mat = m;
    Matrix<T> L = inverse(mat.upper()), U = mat;
    return {L, U};

}

template<typename T>
Matrix<T> Matrix<T>::upper(Matrix<T> m) {
    m.upper();
    return m;
}

template<typename T>
bool Matrix<T>::is_symmetric(const Matrix &m) {
    if(m.get_rows() != m.get_cols()) {
        return false;
    }
    for(int i = 0; i < m.get_rows(); ++i) {
        for(int j = 0; j < i; ++j) {
            if(m[i][j] != m[j][i]) {
                return false;
            }
        }
    }
    return true;
}


template<typename T> Matrix<T>::~Matrix() {
    this->data.clear();
}

template<typename T>
Matrix<T> Matrix<T>::permutation_matrix(const size_t &size, const size_t &f, const size_t &s) {
    auto P = Matrix<T>::eye(size);
    P.exchange_rows(f, s);
    return P;
}

template<typename T>
Matrix<T> Matrix<T>::inverse(const Matrix<T> &A) {
    if(A.get_rows() != A.get_cols()) {
        throw std::runtime_error {"Non-square Matrix."};
    }
    Matrix<T> AI = A;
    const size_t dim = A.get_rows();
    Matrix<T> I = Matrix<T>::eye(dim);
    size_t i = 0;
    for(auto &R : AI.data) {
        R.insert(R.end(),
                std::make_move_iterator(I[i].begin()),
                std::make_move_iterator(I[i].end()));
        ++i;
    }
    I = Matrix<T> {dim, dim}, i = 0;
    AI.gaussian_elimination(true);
    for(auto &R : AI.data) {
        I[i++] = {std::make_move_iterator(R.begin()), std::make_move_iterator(R.begin() + dim)};
        R.erase(R.begin(), R.begin() + dim);
    }
    if(I != eye(dim)) {
        throw std::runtime_error{"Singular."};
    }
    return AI;
}

template<typename T>
bool Matrix<T>::zero_row(const size_t &i) {
    bool is_free_row{true};
    for(int j = 0; j < this->get_cols(); j++) {
        is_free_row &= ((*this)[i][j] == 0);
    }
    return is_free_row;
}


template class Matrix<short>;
template class Matrix<int>;
template class Matrix<long>;
template class Matrix<long long>;
template class Matrix<unsigned short>;
template class Matrix<unsigned int>;
template class Matrix<unsigned long>;
template class Matrix<unsigned long long>;
template class Matrix<float>;
template class Matrix<double>;
template class Matrix<long double>;
