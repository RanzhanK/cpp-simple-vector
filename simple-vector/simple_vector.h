#pragma once

#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <algorithm>
#include<stdexcept>

using namespace std;


class ReserveProxyObj {
public:
    ReserveProxyObj(size_t i) {
        capacity_to_reserve = i;
    }
    size_t capacity_to_reserve;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
    SimpleVector() noexcept = default;


    SimpleVector(ReserveProxyObj other) {
        Reserve(other.capacity_to_reserve);
    }

    SimpleVector(const SimpleVector& other) {
        if (!other.IsEmpty()) {
            SimpleVector tmp(other.size_);
            std::copy(other.begin(), other.end(), tmp.array_ptr_.Get());
            swap(tmp);
        }
    }
    SimpleVector(SimpleVector&& other) {
        swap(other);
    }


    explicit SimpleVector(size_t size) :
            array_ptr_(size) {
        size_ = size;
        capacity_ = size;
        std::generate(array_ptr_.Get(), array_ptr_.Get() + size_, [] { return Type(); });
    }

    SimpleVector(size_t size, const Type& value) :
            array_ptr_(size) {
        size_ = size;
        capacity_ = size;
        for (size_t i = 0; i < size; i++) {
            array_ptr_[i] = value;
        }
    }

    SimpleVector(std::initializer_list<Type> init) : array_ptr_(init.size()) {
        std::copy(init.begin(), init.end(), array_ptr_.Get());
        size_ = init.size();
        capacity_ = size_;

    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return array_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return array_ptr_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("index out of range");
        }
        return array_ptr_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("index out of range");
        }
        return array_ptr_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> tmp(new_capacity);
            std::move(begin(), end(), tmp.Get());
            array_ptr_.swap(tmp);
            capacity_ = new_capacity;
        }
    }
/*
    void Resize(size_t new_size) {
        if (new_size < size_ && new_size < capacity_) {
            size_ = new_size;
        }

        if (new_size > size_ || new_size > capacity_) {
            SimpleVector<Type> tmp(new_size);
            std::copy(begin(), end(), tmp.begin());
            array_ptr_.swap(tmp.array_ptr_);
            size_ = new_size;
            capacity_ = new_size;
        }

        if (new_size > size_ || new_size < capacity_) {
            SimpleVector<Type> tmp(new_size);
            std::fill(tmp.begin(), tmp.end(), Type());
            std::copy(begin(), end(), tmp.begin());
            array_ptr_.swap(tmp.array_ptr_);
            size_ = new_size;
        }

    }
*/
    void Resize(size_t new_size) {
        if (new_size > size_) {
            if (new_size <= capacity_) {
                std::generate(end(), array_ptr_.Get() + new_size, [] { return Type(); });
            }
            else {
                Reserve(new_size);
                std::generate(end(), array_ptr_.Get() + new_size, [] { return Type(); });
            }
        }
        size_ = new_size;
    }

    Iterator begin() noexcept {
        return array_ptr_.Get();
    }

    Iterator end() noexcept {
        return array_ptr_.Get() + size_;

    }

    ConstIterator begin() const noexcept {
        return array_ptr_.Get();
    }

    ConstIterator end() const noexcept {
        return array_ptr_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return array_ptr_.Get();
    }

    ConstIterator cend() const noexcept {
        return array_ptr_.Get() + size_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        delete[] array_ptr_.Release();
        ArrayPtr<Type> newptr(rhs.size_);
        std::copy(rhs.begin(), rhs.end(), newptr.Get());
        array_ptr_.swap(newptr);
        size_ = rhs.size_;
        capacity_ = size_;
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        delete[] array_ptr_.Release();
        ArrayPtr<Type> newptr(rhs.size_);
        std::move(rhs.begin(), rhs.end(), newptr.Get());
        array_ptr_.swap(newptr);
        size_ = std::move(rhs.size_);
        capacity_ = size_;
        return *this;
    }

    void PushBack(const Type& item) {
        if (IsEmpty()) {
            if (!capacity_) {
                Reserve(10);
            }
            array_ptr_[0] = item;
            size_ = 1;
            return;
        }
        if (size_ < capacity_) {
            array_ptr_[size_] = item;
            ++size_;
        }
        else {
            ArrayPtr<Type> tmp(capacity_ * 2);
            std::copy(array_ptr_.Get(), array_ptr_.Get() + size_, tmp.Get());
            tmp[size_] = item;
            array_ptr_.swap(tmp);
            size_ = size_ + 1;
            capacity_ = capacity_ * 2;
        }
    }

    void PushBack(Type&& item) {
        if (IsEmpty()) {
            if (!capacity_) {
                Reserve(10);
            }
            array_ptr_[0] = std::move(item);
            size_ = 1;
            return;
        }
        if (size_ < capacity_) {
            array_ptr_[size_] = std::move(item);
            ++size_;
        }
        else {
            ArrayPtr<Type> tmp(capacity_ * 2);
            std::move(array_ptr_.Get(), array_ptr_.Get() + size_, tmp.Get());
            tmp[size_] = std::move(item);
            array_ptr_.swap(tmp);
            size_ = size_ + 1;
            capacity_ = capacity_ * 2;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        if (pos == end()) {
            PushBack(value);
            return end() - 1;
        }
        if (size_ < capacity_) {
            std::copy_backward((Iterator)pos, end(), end() + 1);
            *((Iterator)pos) = value;
            ++size_;
            return (Iterator)pos;
        }

        SimpleVector<Type> swap_ptr((2 * capacity_));
        std::copy(begin(), (Iterator)pos, swap_ptr.begin());
        std::copy((Iterator)pos, end(), swap_ptr.begin() + ((Iterator)pos - begin() + 1));
        auto return_it = swap_ptr.begin() + (pos - begin());
        *return_it = value;
        capacity_ = 2 * capacity_;
        ++size_;
        array_ptr_.swap(swap_ptr.array_ptr_);
        return return_it;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        if (pos == end()) {
            PushBack(std::move(value));
            return end() - 1;
        }
        if (size_ < capacity_) {
            for (auto p = end(); p != (Iterator)pos; p = prev(p)) {
                *p = std::move(*prev(p));
            }
            *((Iterator)pos) = std::move(value);
            ++size_;
            return (Iterator)pos;
        }


        SimpleVector<Type> swap_ptr((2 * capacity_));
        std::move(begin(), (Iterator)pos, swap_ptr.begin());
        std::move((Iterator)pos, end(), swap_ptr.begin() + ((Iterator)pos - begin() + 1));
        auto return_it = swap_ptr.begin() + (pos - begin());
        *return_it = std::move(value);
        capacity_ = 2 * capacity_;
        ++size_;
        array_ptr_.swap(swap_ptr.array_ptr_);
        return return_it;
    }


    void PopBack() noexcept {
        if (!IsEmpty()) --size_;
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        std::move(std::next(Iterator(pos)), end(), Iterator(pos));
        --size_;
        return Iterator(pos);
    }


    void swap(SimpleVector& other) noexcept {
        array_ptr_.swap(other.array_ptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);

    }

    void swap(SimpleVector&& other) noexcept {
        array_ptr_.swap(other.array_ptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }


private:
    ArrayPtr<Type> array_ptr_;
    size_t size_ = 0;
    size_t capacity_ = 0;


};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template<typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(operator==(lhs, rhs));
}

template<typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template<typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (operator==(lhs, rhs)) || (operator<(lhs, rhs));
}

template<typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(operator<=(lhs, rhs));
}

template<typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (operator==(lhs, rhs)) || (operator>(lhs, rhs));
}