#include <iostream>
#include <cstring>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <initializer_list>

class Vector {
public:
    struct Iterator {
        using IteratorCategory = std::random_access_iterator_tag;
        using DifferenceType = std::ptrdiff_t;

        explicit Iterator(int* ptr) : m_ptr_(ptr){};

        int& operator*() const;
        int* operator->();

        Iterator& operator++();
        Iterator operator++(int);

        Iterator& operator--();
        Iterator operator--(int);

        Iterator operator+(const DifferenceType& movement);
        Iterator operator-(const DifferenceType& movement);

        Iterator& operator+=(const DifferenceType& movement);
        Iterator& operator-=(const DifferenceType& movement);

        friend bool operator==(const Iterator& a, const Iterator& b);
        friend bool operator!=(const Iterator& a, const Iterator& b);

    private:
        int* m_ptr_;
    };

    Vector();

    explicit Vector(size_t n_size);

    Vector(const int* vals, size_t size);

    Vector(const Vector& vec);

    Vector(std::initializer_list<int> vals);

    ~Vector();

    size_t getSize() const;

    size_t getCapacity() const;

    bool isEmpty() const;

    void resize(size_t n_size);

    void pushBack(int value);

    void popBack();

    void clear();

    void insert(size_t pos, int value);

    void erase(size_t pos);

    int at(size_t pos);

    int front();

    int back();

    Iterator begin();

    Iterator end();

    int& operator[](size_t pos);

    const int& operator[](size_t) const;

    Vector& operator=(const Vector& other);

private:
    int* arr_;

    size_t size_;

    size_t capacity_;

    void swap(Vector& v);
};

int& Vector::Iterator::operator*() const {
    return *m_ptr_;
}

int* Vector::Iterator::operator->() {
    return m_ptr_;
}

Vector::Iterator& Vector::Iterator::operator++() {
    ++m_ptr_;
    return *this;
}

Vector::Iterator Vector::Iterator::operator++(int) {
    Iterator copy(m_ptr_);
    ++m_ptr_;
    return copy;
}

Vector::Iterator& Vector::Iterator::operator--() {
    --m_ptr_;
    return *this;
}

Vector::Iterator Vector::Iterator::operator--(int) {
    Iterator copy(m_ptr_);
    --m_ptr_;
    return copy;
}

Vector::Iterator Vector::Iterator::operator+(const Vector::Iterator::DifferenceType& movement) {
    Iterator copy(m_ptr_ + movement);
    return copy;
}

Vector::Iterator Vector::Iterator::operator-(const Vector::Iterator::DifferenceType& movement) {
    Iterator copy(m_ptr_ - movement);
    return copy;
}

Vector::Iterator& Vector::Iterator::operator+=(const Vector::Iterator::DifferenceType& movement) {
    m_ptr_ += movement;
    return *this;
}

Vector::Iterator& Vector::Iterator::operator-=(const Vector::Iterator::DifferenceType& movement) {
    m_ptr_ -= movement;
    return *this;
}

bool operator==(const Vector::Iterator& a, const Vector::Iterator& b) {
    return a.m_ptr_ == b.m_ptr_;
}

bool operator!=(const Vector::Iterator& a, const Vector::Iterator& b) {
    return a.m_ptr_ != b.m_ptr_;
}

Vector::Vector() : size_(0), capacity_(10) {
    arr_ = new int[capacity_];
}

Vector::Vector(size_t n_size) : size_(n_size), capacity_(n_size * 2) {
    arr_ = new int[capacity_];
    memset(arr_, 0, size_ * sizeof(int));
}

Vector::Vector(const int* vals, size_t size) : Vector(size) {
    memcpy(arr_, vals, size * sizeof(int));
}

Vector::Vector(const Vector& vec) : Vector(vec.arr_, vec.size_) {
    memcpy(arr_, vec.arr_, vec.size_ * sizeof(int));
}

Vector::Vector(std::initializer_list<int> vals) : size_(0), capacity_(vals.size() * 2) {
    arr_ = new int[capacity_];
    for (auto x : vals) {
        this->pushBack(x);
    }
}

Vector::~Vector() {
    delete[] arr_;
}

size_t Vector::getSize() const {
    return size_;
}

size_t Vector::getCapacity() const {
    return capacity_;
}

bool Vector::isEmpty() const {
    return size_ == 0;
}

void Vector::resize(size_t n_size) {
    if (n_size < capacity_) {
        for (size_t i = size_; i < n_size; ++i) {
            arr_[i] = 0;
        }
        size_ = n_size;
    }
    if (n_size >= capacity_) {
        capacity_ = n_size * 2;
        int* new_arr = new int[capacity_];
        memset(new_arr, 0, n_size * sizeof(int));
        memcpy(new_arr, arr_, size_ * sizeof(int));

        size_ = n_size;

        delete[] arr_;
        arr_ = new_arr;
    }
}

void Vector::pushBack(int value) {
    if (size_ < capacity_) {
        arr_[size_] = value;
        ++size_;
    } else {
        capacity_ *= 2;
        int* new_arr = new int[capacity_];
        memcpy(new_arr, arr_, size_ * sizeof(int));

        new_arr[size_] = value;
        ++size_;

        delete[] arr_;
        arr_ = new_arr;
    }
}

void Vector::popBack() {
    if (size_ == 0) {
        throw std::runtime_error("Empty Array!");
    }

    size_--;
}

void Vector::clear() {
    size_ = 0;
}

void Vector::insert(size_t pos, int value) {
    if (pos > size_) {
        throw std::runtime_error("Wrong Position!");
    }

    if (size_ == capacity_) {
        capacity_ *= 2;
    }

    int* new_arr = new int[capacity_];

    memcpy(new_arr, arr_, pos * sizeof(int));

    new_arr[pos] = value;
    ++size_;

    memcpy(new_arr + pos + 1, arr_ + pos, (size_ - pos) * sizeof(int));

    delete[] arr_;
    arr_ = new_arr;
}

void Vector::erase(size_t pos) {
    if (pos >= size_) {
        throw std::runtime_error("Wrong Position!");
    }
    if (size_ == 0) {
        throw std::runtime_error("Empty Array!");
    }

    int* new_arr = new int[capacity_];

    memcpy(new_arr, arr_, pos * sizeof(int));

    size_--;

    memcpy(new_arr + pos, arr_ + pos + 1, (size_ - pos) * sizeof(int));

    delete[] arr_;
    arr_ = new_arr;
}

int Vector::at(size_t pos) {
    if (pos >= size_) {
        throw std::runtime_error("Wrong Position!");
    }

    return arr_[pos];
}

int Vector::front() {
    if (size_ == 0) {
        throw std::runtime_error("Empty Array!");
    }

    return arr_[0];
}

int Vector::back() {
    if (size_ == 0) {
        throw std::runtime_error("Empty Array!");
    }

    return arr_[size_ - 1];
}

Vector::Iterator Vector::begin() {
    return Vector::Iterator(arr_);
}

Vector::Iterator Vector::end() {
    return Vector::Iterator(arr_ + size_);
}

int& Vector::operator[](size_t pos) {
    if (pos >= size_) {
        throw std::runtime_error("Wrong Position!");
    }

    return arr_[pos];
}

const int& Vector::operator[](size_t pos) const {
    if (pos >= size_) {
        throw std::runtime_error("Wrong Position!");
    }

    return arr_[pos];
}

Vector& Vector::operator=(const Vector& other) {
    Vector copy = other;
    Vector::swap(copy);
    return *this;
}

void Vector::swap(Vector& v) {
    std::swap(arr_, v.arr_);
    std::swap(size_, v.size_);
    std::swap(capacity_, v.capacity_);
}

void mergeSort(Vector::Iterator begin, Vector::Iterator end) {
    int x;
}

void insertionSort(Vector::Iterator begin, Vector::Iterator end) {
    int x;
}
