// Author: Kevin Chang: tc3149@nyu.edu

#ifndef matrix_hpp
#define matrix_hpp

#pragma once
#include <iostream>
#include <vector>
#include <cassert>

namespace matrix {

template <typename T>
class Matrix {
private:
  unsigned _rows, _cols;
  std::vector<T> _data;

public:
  Matrix() = default;
  Matrix(const unsigned &, const unsigned &);
  Matrix(Matrix<T> &&);
  Matrix(const Matrix<T> &);
  Matrix(std::vector<T> &&);
  Matrix(const std::vector<T> &);
  ~Matrix() = default;
  unsigned rows() const;
  unsigned cols() const;
  unsigned size() const;
  void resize(const unsigned &, const unsigned &);
  const T& at (const unsigned &) const;
  const T& at (const unsigned &, const unsigned &) const;
  T& operator() (const unsigned &);
  T& operator() (const unsigned &, const unsigned &);
  Matrix<T> operator+ (const Matrix<T> &) const;
  Matrix<T> operator* (const Matrix<T> &) const;
  Matrix<T> operator* (const T &) const;
  Matrix<T>& operator= (Matrix<T> &&) = default;
  Matrix<T>& operator= (const Matrix<T> &) = default;
  Matrix<T>& operator= (std::vector<T> &&);
  Matrix<T>& operator= (const std::vector<T> &);
  bool operator== (const Matrix<T> &) const;
};

template <typename T>
std::ostream& operator<< (std::ostream&, const Matrix<T> &);

}

template <typename T>
matrix::Matrix<T>::Matrix(const unsigned &rows, const unsigned &cols) : _rows(rows), _cols(cols), _data(rows*cols) {}

template <typename T>
matrix::Matrix<T>::Matrix(Matrix<T> &&mat) {
  *this = std::move(mat);
}

template <typename T>
matrix::Matrix<T>::Matrix(const Matrix<T> &mat) {
  *this = mat;
}

template <typename T>
matrix::Matrix<T>::Matrix(std::vector<T> &&vec) : _rows(1), _cols(vec.size()) {
  *this = std::move(vec);
}

template <typename T>
matrix::Matrix<T>::Matrix(const std::vector<T> &vec) : _rows(1), _cols(vec.size()) {
  *this = vec;
}

template <typename T>
unsigned matrix::Matrix<T>::rows() const {
  return _rows;
}

template <typename T>
unsigned matrix::Matrix<T>::cols() const {
  return _cols;
}

template <typename T>
unsigned matrix::Matrix<T>::size() const {
  return _rows*_cols;
}

template <typename T>
void matrix::Matrix<T>::resize(const unsigned &r, const unsigned &c) {
  assert(r*c == _data.size());
  _rows = r;
  _cols = c;
}

template <typename T>
T& matrix::Matrix<T>::operator() (const unsigned &x) {
  assert(x < _data.size());
  return _data[x];
}

template <typename T>
T& matrix::Matrix<T>::operator() (const unsigned &r, const unsigned &c) {
  assert(r*_cols+c < _data.size());
  return _data[r*_cols+c];
}

template <typename T>
const T& matrix::Matrix<T>::at(const unsigned &x) const {
  assert(x < _data.size());
  return _data.at(x);
}

template <typename T>
const T& matrix::Matrix<T>::at(const unsigned &r, const unsigned &c) const {
  assert(r*_cols+c < _data.size());
  return _data.at(r*_cols+c);
}

template <typename T>
matrix::Matrix<T> matrix::Matrix<T>::operator+ (const Matrix<T> &mat) const {
  assert(_rows == mat._rows && _cols == mat._cols);
  Matrix<T> sum(_rows, _cols);
  for (unsigned i = 0; i < _data.size(); ++i) {
    sum(i) = _data.at(i) + mat.at(i);
  }
  return sum;
}

template <typename T>
matrix::Matrix<T> matrix::Matrix<T>::operator* (const Matrix<T> &mat) const {
  assert(_cols == mat._rows);
  Matrix<T> product(_rows, mat._cols); // std::vector default init to 0
  for (unsigned r1 = 0; r1 < _rows; ++r1) {
    for (unsigned c2 = 0; c2 < mat._cols; ++c2) {
      for (unsigned x = 0; x < _cols; ++x) {
        product(r1, c2) += _data.at(r1*_rows+x) * mat.at(x, c2);
      }
    }
  }
  return product;
}

template <typename T>
matrix::Matrix<T> matrix::Matrix<T>::operator* (const T &val) const {
  Matrix<T> product(_rows, _cols);
  for (unsigned i = 0; i < _data.size(); ++i) {
    product(i) = _data.at(i) * val;
  }
  return product;
}

template <typename T>
matrix::Matrix<T>& matrix::Matrix<T>::operator= (const std::vector<T> &vec) {
  assert(vec.size() == _rows*_cols);
  _data = vec;
  return *this;
}

template <typename T>
matrix::Matrix<T>& matrix::Matrix<T>::operator= (std::vector<T> &&vec) {
  assert(vec.size() == _rows*_cols);
  _data = std::move(vec);
  return *this;
}

template <typename T>
bool matrix::Matrix<T>::operator== (const Matrix<T> &mat) const {
  if (_rows != mat._rows || _cols != mat._cols) return false;
  for (unsigned i = 0; i < _data.size(); ++i) {
    if (_data.at(i) != mat.at(i)) return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////

template <typename T>
std::ostream& matrix::operator<< (std::ostream& os, const Matrix<T> &mat) {
  for (unsigned i = 0; i < mat.rows(); ++i) {
    for (unsigned j = 0; j < mat.cols(); ++j) {
      os << mat.at(i,j) << " ";
    }
    os << "\n";
  }
  return os;
}

///////////////////////////////////////////////////////////////////////////////

template <typename T>
matrix::Matrix<T> operator* (const T& val, const matrix::Matrix<T> &mat) {
  matrix::Matrix<T> product(mat.rows(), mat.cols());
  for (unsigned i = 0; i < mat.size(); ++i) {
    product(i) = val * mat.at(i);
  }
  return product;
}

#endif /* matrix_hpp */