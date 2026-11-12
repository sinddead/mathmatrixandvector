// Copyright 2026 Malakhov Roman
#pragma once
#include <iostream>
#include <limits>
#include <cstddef>
#include "memdata.h"

template <class T>
class Vector {
    MemData<T> _mem;      // хранилище данных + размер  + вместимость
    size_t _front;        // индекс первого элемента
    size_t _back;         // индекс элемента после последнего

    void grow();          // вспомогательная функция: увеличение вместимости

public:
    class Iterator {
        Vector* _v;
        size_t _i;
    public:
        Iterator(Vector* v = nullptr, size_t i = 0) : _v(v), _i(i) {}

        T& operator*() const { return (*_v)[_i]; }
        T* operator->() const { return &(*_v)[_i]; }

        Iterator& operator++() { ++_i; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++_i; return tmp; }
        Iterator& operator--() { --_i; return *this; }
        Iterator operator--(int) { Iterator tmp = *this; --_i; return tmp; }

        Iterator operator+(size_t n) const { return Iterator(_v, _i + n); }
        Iterator operator-(size_t n) const { return Iterator(_v, _i - n); }

        bool operator==(const Iterator& other) const { return _v == other._v && _i == other._i; }
        bool operator!=(const Iterator& other) const { return !(*this == other); }
    };

    Vector(size_t size = 0);                 // конструктор по размеру + по умолчанию
    Vector(std::initializer_list<T>);        // конструктор по списку инициализации
    Vector(const T*, size_t);                // конструктор инициализации
    Vector(const Vector&);                   // конструктор копирования
    Vector(Vector&&) noexcept;               // конструктор с move-семантикой
    ~Vector() = default;                     // деструктор

    inline bool is_empty() const noexcept;          // проверка на пустоту
    inline bool is_full() const noexcept;           // проверка на переполнение

    inline size_t size() const noexcept;            // геттер размера
    inline size_t capacity() const noexcept;        // геттер вместимости
    inline const T& front() const { return _mem[_front]; }       // геттер первого элемента
    inline const T& back() const { if (_back == 0) return _mem[_mem._capacity - 1]; return _mem[_back - 1]; }  // геттер последнего элемента

    inline T& front();                              // сеттер первого элемента
    inline T& back();                               // сеттер последнего элемента

    void shrink_to_fit();
    void push_front(const T&);                      // вставка элемента в начало
    void push_back(const T&);                       // вставка элемента в конец
    void insert(const T&, size_t);                  // вставка элемента по позиции
    void pop_front();                               // удаление элемента из начала
    void pop_back();                                // удаление элемента из конца
    void erase(size_t);                             // удаление элемента по позиции

    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, size()); }

    Vector& operator=(const Vector&) noexcept;      // оператор присваивания
    Vector& operator=(Vector&&) noexcept;           // оператор присваивания с move-семантикой

    const T& operator[](size_t) const noexcept;     // оператор обращения по индексу константный
    T& operator[](size_t) noexcept;                 // оператор обращения по индексу

    friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        os << "{";
        for (size_t i = 0; i < v.size(); i++) {
            os << v[i];
            if (i + 1 < v.size()) os << ", ";
        }
        os << "}";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Vector& v) {
        v = Vector();
        T value;
        while (is >> value) {
            v.push_back(value);
        }
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return is;
    }
};

template <class T>
Vector<T>::Vector(size_t size) {
    _mem = MemData<T>(size);
    _front = 0;
    _back = 0;
}
template <class T>
Vector<T>::Vector(std::initializer_list<T> list) {
    _mem = MemData<T>(list);
    _front = 0;
    _back = list.size();
}
template <class T>
Vector<T>::Vector(const T* arr, size_t size) {
    _mem = MemData<T>(arr, size);
    _front = 0;
    _back = size;
}
template <class T>
Vector<T>::Vector(const Vector& other) {
    _mem = other._mem;
    _front = other._front;
    _back = other._back;
}
template <class T>
Vector<T>::Vector(Vector&& other) noexcept {
    if (&other != this) {
        _mem = std::move(other._mem);
        _front = other._front;
        _back = other._back;
        other._front = 0;
        other._back = 0;
    }
}

template <class T>
inline bool Vector<T>::is_empty() const noexcept { return _mem._size == 0; }
template <class T>
inline bool Vector<T>::is_full() const noexcept { return _mem._size == _mem._capacity; }
template <class T>
inline T& Vector<T>::front() { return _mem[_front]; }
template <class T>
inline T& Vector<T>::back() { if (_back == 0) return _mem[_mem._capacity - 1]; return _mem[_back - 1]; }
template <class T>
inline size_t Vector<T>::size() const noexcept { return _mem._size; }
template <class T>
inline size_t Vector<T>::capacity() const noexcept { return _mem._capacity; }

template <class T>
void Vector<T>::grow() {
    size_t new_capacity = (_mem._capacity == 0) ? 10 : _mem._capacity * MEM_STEP;
    _mem.reset_memory(new_capacity, _front);
    _front = 0;
    _back = _mem._size;
}

template <class T>
void Vector<T>::push_front(const T& element) {
    if (is_full()) grow();
    if (_front == 0) _front = _mem._capacity - 1;
    else _front--;
    _mem[_front] = element;
    _mem._size++;
}
template <class T>
void Vector<T>::push_back(const T& element) {
    if (is_full()) grow();
    if (_back == _mem._capacity) {
        _back = 0;
    }
    _mem[_back] = element;
    _back++;
    _mem._size++;
}
template <class T>
void Vector<T>::insert(const T& element, size_t index) {
    if (index > _mem._size) return; //или throw добавить
    if (is_full()) grow();
    T* new_data = new T[_mem._capacity]();
    size_t pos = 0;
    for (size_t i = 0; i < index; i++) {
        new_data[pos++] = _mem[(_front + i) % _mem._capacity];
    }
    new_data[pos++] = element;
    for (size_t i = index; i < _mem._size; i++) {
        new_data[pos++] = _mem[(_front + i) % _mem._capacity];
    }
    delete[] _mem._data;
    _mem._data = new_data;
    _mem._size++;
    _front = 0;
    _back = _mem._size;
}
template <class T>
void Vector<T>::pop_front() {
    if (!is_empty()) {
        _front = (_front + 1) % _mem._capacity;
        _mem._size--;
    }
}
template <class T>
void Vector<T>::pop_back() {
    if (!is_empty()) {
        if (_back == 0) _back = _mem._capacity - 1;
        else _back = (_back - 1);
        _mem._size--;
    }
}
template <class T>
void Vector<T>::erase(size_t index) {
    if (!is_empty()) {
        if (index >= _mem._size) {
            return; // можно throw
        }
        T* new_data = new T[_mem._capacity]();
        size_t pos = 0;
        for (size_t i = 0; i < index; i++) {
            new_data[pos++] = _mem[(_front + i) % _mem._capacity];
        }
        for (size_t i = index + 1; i < _mem._size; i++) {
            new_data[pos++] = _mem[(_front + i) % _mem._capacity];
        }
        delete[] _mem._data;
        _mem._data = new_data;
        _mem._size--;
        _front = 0;
        _back = _mem._size;
    }
}

template <class T>
T& Vector<T>::operator[](size_t index) noexcept {
    return _mem[(_front + index) % _mem._capacity];
}

template <class T>
const T& Vector<T>::operator[](size_t index) const noexcept {
    return _mem[(_front + index) % _mem._capacity];
}

template <class T>
Vector<T>& Vector<T>::operator=(const Vector& other) noexcept {
    if (this != (&other)) {
        _mem = other._mem;
        _front = other._front;
        _back = other._back;
    }
    return (*this);
}

template <class T>
Vector<T>& Vector<T>::operator=(Vector&& other) noexcept {
    if (&other != this) {
        _mem = std::move(other._mem);
        _front = other._front;
        _back = other._back;
        other._front = 0;
        other._back = 0;
    }
    return *this;
}

template <class T>
void Vector<T>::shrink_to_fit() {
    if (_mem._capacity == _mem._size) return;
    if (_mem._size == 0) {
        _mem.clear_memory();
        _front = 0;
        _back = 0;
        return;
    }
    _mem.reset_memory(_mem._size, _front);
    _front = 0;
    _back = _mem._size;
}