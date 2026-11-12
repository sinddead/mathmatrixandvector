// Copyright 2026 Malakhov Roman
#pragma once
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "Tmathvector.h"

template <class T>
class TTriangleMatrix {
 private:
  using Row = TMathVector<T>;

  TMathVector<Row> _rows;
  T _trash = T();

  void check_size(const TTriangleMatrix& other) const {
    if (size() != other.size())
      throw std::invalid_argument("TTriangleMatrix: sizes differ");
  }

 public:
  TTriangleMatrix() = default;
  explicit TTriangleMatrix(size_t n) : _rows(n) {
    for (size_t i = 0; i < n; i++) _rows[i] = Row(n - i);
  }
  TTriangleMatrix(std::initializer_list<Row> list) : _rows(list.size()) {
    const size_t n = list.size();
    size_t i = 0;
    for (const Row& full : list) {
      if (full.size() != n)
        throw std::invalid_argument("TTriangleMatrix: matrix must be square");
      Row part(n - i);
      for (size_t j = i; j < n; j++) part[j - i] = full[j];
      _rows[i] = std::move(part);
      i++;
    }
  }

  TTriangleMatrix(const TTriangleMatrix&) = default;
  TTriangleMatrix(TTriangleMatrix&&) noexcept = default;
  TTriangleMatrix& operator=(const TTriangleMatrix&) = default;
  TTriangleMatrix& operator=(TTriangleMatrix&&) noexcept = default;
  ~TTriangleMatrix() = default;

  size_t size() const noexcept { return _rows.size(); }

  T& operator()(size_t i, size_t j) {
    if (i >= size() || j >= size())
      throw std::out_of_range("TTriangleMatrix: index");
    if (j >= i) return _rows[i][j - i];
    _trash = T();
    return _trash;
    // throw std::out_of_range("TTriangleMatrix: index");
  }
  T operator()(size_t i, size_t j) const {
    if (i >= size() || j >= size())
      throw std::out_of_range("TTriangleMatrix: index");
    return j >= i ? _rows[i][j - i]
                  : T() /* throw std::out_of_range("TTriangleMatrix: index")*/;
  }

  TTriangleMatrix operator+(const TTriangleMatrix& other) const {
    check_size(other);
    TTriangleMatrix r(*this);
    for (size_t i = 0; i < size(); i++) r._rows[i] += other._rows[i];
    return r;
  }
  TTriangleMatrix operator-(const TTriangleMatrix& other) const {
    check_size(other);
    TTriangleMatrix r(*this);
    for (size_t i = 0; i < size(); i++) r._rows[i] -= other._rows[i];
    return r;
  }
  TTriangleMatrix operator-() const {
    TTriangleMatrix r(*this);
    for (size_t i = 0; i < size(); i++) r._rows[i] = -r._rows[i];
    return r;
  }
  TTriangleMatrix operator*(const T& s) const {
    TTriangleMatrix r(*this);
    for (size_t i = 0; i < size(); i++) r._rows[i] *= s;
    return r;
  }

  TTriangleMatrix operator*(const TTriangleMatrix& other) const {
    check_size(other);
    const size_t n = size();
    TTriangleMatrix r(n);
    for (size_t i = 0; i < n; i++)
      for (size_t j = i; j < n; j++) {
        T sum = T();
        for (size_t k = i; k <= j; k++)
          sum += _rows[i][k - i] * other._rows[k][j - k];
        r._rows[i][j - i] = sum;
      }
    return r;
  }

  T determinant() const {
    T d = T(1);
    for (size_t i = 0; i < size(); i++) d *= _rows[i][0];
    return d;
  }

  bool operator==(const TTriangleMatrix& other) const {
    return _rows == other._rows;
  }
  bool operator!=(const TTriangleMatrix& other) const {
    return !(*this == other);
  }

  friend std::ostream& operator<<(std::ostream& os, const TTriangleMatrix& m) {
    const size_t n = m.size();
    for (size_t i = 0; i < n; i++) {
      os << "[";
      for (size_t j = 0; j < n; j++)
        os << " " << (j >= i ? m._rows[i][j - i] : T());
      os << " ]";
      if (i + 1 < n) os << '\n';
    }
    return os;
  }
};