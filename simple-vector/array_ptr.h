#include <cassert>
#include <cstdlib>
#include <algorithm>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size == 0) { raw_ptr_ = nullptr; }
        else {
            raw_ptr_ = new Type[size];
        }

    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    explicit ArrayPtr(ArrayPtr<Type>&& other) noexcept {
        raw_ptr_.swap(other.raw_ptr_);
        other.raw_ptr_ = nullptr;
    }


    ArrayPtr(const ArrayPtr&) = delete;

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr&& operator=(ArrayPtr&& other ) {
        if (raw_ptr_ != other.raw_ptr_) {
            raw_ptr_.swap(other.raw_ptr_);
        }
    };

    [[nodiscard]] Type* Release() noexcept {
        Type* p = raw_ptr_;
        raw_ptr_ = nullptr;
        return p;
    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return  raw_ptr_[index];
    }

    explicit operator bool() const {
        if (raw_ptr_ != nullptr) {
            return true;
        } else {
            return false;
        }
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};