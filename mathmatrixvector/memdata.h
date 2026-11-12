// Copyright 2026 Malakhov Roman
#pragma once
#include <initializer_list>
#include <cstddef>
#include <utility>

#define MEM_STEP 2

template <class T>
class Vector;

template <class T>
class MemData {
    T* _data;                  // хранилище данных
    size_t _size;              // размер заполненной части хранилища
    size_t _capacity;          // вместимость хранилища
public:
    MemData(size_t size = 0);                // конструктор по размеру + по умолчанию
    MemData(std::initializer_list<T>);       // конструктор по списку инициализации
    MemData(const T*, size_t);               // конструктор инициализации
    MemData(const MemData&);                 // конструктор копирования
    MemData(MemData&& other) noexcept;       // конструктор с move-семантикой
    ~MemData();                              // деструктор

    inline bool is_empty() const noexcept;   // проверка на пустоту
    inline bool is_full() const noexcept;    // проверка на переполнение

    inline size_t size() const noexcept;                 // геттер размера
    inline size_t capacity() const noexcept;             // геттер вместимости
    inline const T* const data() const noexcept;         // геттер хранилища

    void set_memory(size_t) noexcept;
    void reset_memory(size_t size, size_t start_index) noexcept;
    void clear_memory() noexcept;                                         // очистка памяти

    MemData& operator=(const MemData&) noexcept;         // оператор присваивания
    T& operator[](const size_t) noexcept;
    const T& operator[](const size_t) const noexcept;
    MemData& operator=(MemData&&) noexcept;              // оператор присваивания с move-семантикой

    friend class Vector<T>;
};

template <class T>
MemData<T>::~MemData() {
    delete[] _data;
}
template <class T>
MemData<T>::MemData(size_t size) {
    _capacity = size * MEM_STEP + 10;
    _size = 0;
    _data = new T[_capacity];
}
template <class T>
MemData<T>::MemData(const T* arr, size_t size) {
    _capacity = size * MEM_STEP + 10;
    _size = size;
    _data = new T[_capacity];
    for (size_t i = 0; i < size; i++) {
        _data[i] = arr[i];
    }
}
template <class T>
MemData<T>::MemData(std::initializer_list<T> list) {
    size_t n = list.size();
    size_t i = 0;
    _capacity = n * MEM_STEP + 10;
    _size = n;
    _data = new T[_capacity];
    for (const T& val : list) {
        _data[i++] = val;
    }
}
template <class T>
MemData<T>::MemData(const MemData& other) {
    _capacity = other._capacity;
    _size = other._size;
    _data = new T[_capacity];
    for (size_t i = 0; i < _capacity; i++) {
        _data[i] = other._data[i];
    }
}
template <class T>
MemData<T>& MemData<T>::operator=(const MemData& other) noexcept {
    if (this != &other) {
        T* new_data = new T[other._capacity];
        for (size_t i = 0; i < other._capacity; ++i) {
            new_data[i] = other._data[i];
        }
        delete[] _data;
        _data = new_data;
        _capacity = other._capacity;
        _size = other._size;
    }
    return *this;
}
template <class T>
inline bool MemData<T>::is_empty() const noexcept { return _size == 0; }
template <class T>
inline bool MemData<T>::is_full() const noexcept { return _size == _capacity; }
template <class T>
inline size_t MemData<T>::size() const noexcept { return _size; }
template <class T>
inline size_t MemData<T>::capacity() const noexcept { return _capacity; }
template <class T>
inline const T* const MemData<T>::data() const noexcept { return _data; }

template <class T>
void MemData<T>::set_memory(size_t new_capacity) noexcept {
    delete[] _data;
    _data = new T[new_capacity];
    _capacity = new_capacity;
    _size = 0;
}

template <class T>
void MemData<T>::reset_memory(size_t new_capacity, size_t start_index) noexcept {
    if (new_capacity == 0) { //верим, что new_capacity>=size
        clear_memory();
        return;
    }

    T* new_data = new T[new_capacity];
    for (size_t i = 0; i < _size; i++) {
        new_data[i] = std::move(_data[(start_index + i) % _capacity]);
    }
    delete[] _data;
    _data = new_data;
    _capacity = new_capacity;
}

template <class T>
void MemData<T>::clear_memory() noexcept {
    delete[] _data;
    _data = nullptr;
    _size = 0;
    _capacity = 0;
}

template <class T>
T& MemData<T>::operator[](const size_t index) noexcept {
    return _data[index];
}

template <class T>
const T& MemData<T>::operator[](const size_t index) const noexcept {
    return _data[index];
}

template <class T>
MemData<T>::MemData(MemData&& other) noexcept {
    _data = other._data;
    _size = other._size;
    _capacity = other._capacity;
    other._data = nullptr;
    other._size = 0;
    other._capacity = 0;
}

template <class T>
MemData<T>& MemData<T>::operator=(MemData&& other) noexcept {
    if (&other != this) {
        delete[] _data;
        _data = other._data;
        _size = other._size;
        _capacity = other._capacity;
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }
    return *this;
}