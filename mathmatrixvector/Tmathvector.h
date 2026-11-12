// Copyright 2026 Malakhov Roman
#pragma once
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

#include "MyVector.h"

template <class T>
class TMathVector : public Vector<T> {
 private:
  void check_size(const TMathVector& other) const {
    if (this->size() != other.size())
      throw std::invalid_argument("TMathVector: sizes differ");
  }

 public:
  TMathVector() = default;
  explicit TMathVector(const size_t n, const T& value = T()) : Vector<T>(n) {
    for (size_t i = 0; i < n; i++) this->push_back(value);
  }
  TMathVector(std::initializer_list<T> list) : Vector<T>(list) {}
  TMathVector(const T* arr, size_t n) : Vector<T>(arr, n) {}
  explicit TMathVector(const Vector<T>& other) : Vector<T>(other) {}

  TMathVector(const TMathVector&) = default;
  TMathVector(TMathVector&&) noexcept = default;
  TMathVector& operator=(const TMathVector&) = default;
  TMathVector& operator=(TMathVector&&) noexcept = default;
  ~TMathVector() = default;

  TMathVector& operator+=(const TMathVector& other) {
    check_size(other);
    for (size_t i = 0; i < this->size(); i++) (*this)[i] += other[i];
    return *this;
  }
  TMathVector& operator-=(const TMathVector& other) {
    check_size(other);
    for (size_t i = 0; i < this->size(); i++) (*this)[i] -= other[i];
    return *this;
  }
  TMathVector& operator*=(const T& num) {
    for (size_t i = 0; i < this->size(); i++) (*this)[i] *= num;
    return *this;
  }

  TMathVector operator+(const TMathVector& other) const {
    TMathVector r(*this);
    r += other;
    return r;
  }
  TMathVector operator-(const TMathVector& other) const {
    TMathVector r(*this);
    r -= other;
    return r;
  }

  TMathVector operator-() const {
    TMathVector tmp(*this);
    for (size_t i = 0; i < this->size(); i++) tmp[i] = -tmp[i];
    return tmp;
  }

  TMathVector operator*(const T& num) const {
    TMathVector r(*this);
    r *= num;
    return r;
  }
  T operator*(const TMathVector& other) const {
    check_size(other);
    T result = T();
    for (size_t i = 0; i < this->size(); i++) result += (*this)[i] * other[i];
    return result;
  }

  bool operator==(const TMathVector& other) const {
    if (this->size() != other.size()) return false;
    for (size_t i = 0; i < this->size(); i++)
      if (!((*this)[i] == other[i])) return false;
    return true;
  }
  bool operator!=(const TMathVector& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& os, const TMathVector& v) {
    os << "[";
    for (size_t i = 0; i < v.size(); i++) os << " " << v[i];
    os << " ]";
    return os;
  }
};