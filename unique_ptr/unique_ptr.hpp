#pragma once

#include <utility>

template <typename T>
class unique_ptr {
public:
    // lvalue
    unique_ptr() noexcept : data_(nullptr) {}

    explicit unique_ptr(T* data_) noexcept : data_(data_) {}

    unique_ptr(unique_ptr& other)            = delete;
    unique_ptr& operator=(unique_ptr& other) = delete;

    ~unique_ptr() {
        this->reset();
    }

    // rvalue
    explicit unique_ptr(T&& data_) noexcept : data_(data_) {}

    unique_ptr(unique_ptr&& other) noexcept {
        data_ = other.data_;
        other.data_ = nullptr;
    }

    unique_ptr& operator=(unique_ptr other) noexcept {
        swap(*this, other);
        return *this;
    }

    explicit operator bool() const {
        return data_;
    }

    T& operator*() const {
        return *data_;
    }

    T* operator->() const {
        return data_;
    }

    void swap(unique_ptr& lhs, unique_ptr& rhs) {
        if (&lhs != &rhs) {
            T* tmp = lhs.data_;
            lhs.data_ = rhs.data_;
            rhs.data_ = tmp;
        }
    }

    T* get() const {
        return data_;
    }

    T* release() {
        T* tmp = data_;
        data_ = nullptr;
        return tmp;
    }

    void reset() {
        delete data_;
        data_ = nullptr;
    }

    void reset(T* new_data) {
        delete data_;
        data_ = new_data;
    }


private:
    T * data_;
};

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}
