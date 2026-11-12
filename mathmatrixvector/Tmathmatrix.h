// Copyright 2026 Malakhov Roman
#pragma once
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

#include "Tmathvector.h"

template <class T>
class TMatrix : public TMathVector<TMathVector<T>> {
 private:
  using Row = TMathVector<T>;
  using Base = TMathVector<Row>;

 public:
  TMatrix() = default;
  explicit TMatrix(size_t n) : Base(n, Row(n)) {}
  TMatrix(std::initializer_list<Row> list) : Base(list) {
    for (size_t i = 0; i < this->size(); i++)
      if ((*this)[i].size() != this->size())
        throw std::invalid_argument("TMatrix: matrix must be square");
  }

  TMatrix(const TMatrix&) = default;
  TMatrix(TMatrix&&) noexcept = default;
  TMatrix& operator=(const TMatrix&) = default;
  TMatrix& operator=(TMatrix&&) noexcept = default;
  ~TMatrix() = default;

  TMatrix operator+(const TMatrix& other) const {
    TMatrix r(*this);
    r += other;
    return r;
  }
  TMatrix operator-(const TMatrix& other) const {
    TMatrix r(*this);
    r -= other;
    return r;
  }
  TMatrix operator-() const {
    TMatrix r(*this);
    for (size_t i = 0; i < r.size(); i++) r[i] = -r[i];
    return r;
  }

  TMatrix& operator*=(const T& s) {
    for (size_t i = 0; i < this->size(); i++) (*this)[i] *= s;
    return *this;
  }
  TMatrix operator*(const T& s) const {
    TMatrix r(*this);
    r *= s;
    return r;
  }

  TMatrix operator*(const TMatrix& other) const {
    const size_t n = this->size();
    if (other.size() != n) throw std::invalid_argument("TMatrix: sizes differ");
    TMatrix r(n);
    for (size_t i = 0; i < n; i++)
      for (size_t k = 0; k < n; k++)
        for (size_t j = 0; j < n; j++) r[i][j] += (*this)[i][k] * other[k][j];
    return r;
  }

  Row operator*(const Row& other) const {
    const size_t n = this->size();
    if (other.size() != n) throw std::invalid_argument("TMatrix: sizes differ");
    Row r(n);
    for (size_t i = 0; i < n; i++) r[i] = (*this)[i] * other;
    return r;
  }

  TMatrix transposed() const {
    const size_t n = this->size();
    TMatrix r(n);
    for (size_t i = 0; i < n; i++)
      for (size_t j = 0; j < n; j++) r[j][i] = (*this)[i][j];
    return r;
  }

  friend std::ostream& operator<<(std::ostream& os, const TMatrix& m) {
    for (size_t i = 0; i < m.size(); i++) {
      os << m[i];
      if (i + 1 < m.size()) os << '\n';
    }
    return os;
  }
  friend std::istream& operator>>(std::istream& is, TMatrix& m) {
    long long n;
    if (!(is >> n)) return is;
    if (n < 0) {
      is.setstate(std::ios::failbit);
      return is;
    }
    TMatrix tmp(static_cast<size_t>(n));
    for (size_t i = 0; i < tmp.size(); i++)
      for (size_t j = 0; j < tmp.size(); j++)
        if (!(is >> tmp[i][j])) return is;
    m = std::move(tmp);
    return is;
  }
};