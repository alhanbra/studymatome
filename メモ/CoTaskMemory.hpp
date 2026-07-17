#pragma once
#include <windows.h>
#include <utility>   // std::exchange のために必要
#include <cstddef>   // std::size_t のために必要
#include <concepts>  // C++20 概念（コンセプト）のため

template<typename T>
class CoTaskMemory {
public:
    CoTaskMemory() noexcept : ptr_(nullptr) {}

    explicit CoTaskMemory(const std::size_t size) noexcept : ptr_(nullptr) {
        ptr_ = static_cast<T*>(::CoTaskMemAlloc(size));
    }

    ~CoTaskMemory() noexcept {
        ::CoTaskMemFree(std::exchange(ptr_, nullptr));
    }

    CoTaskMemory(const CoTaskMemory&) = delete;
    CoTaskMemory& operator=(const CoTaskMemory&) = delete;

    CoTaskMemory(CoTaskMemory&& other) noexcept 
        : ptr_(std::exchange(other.ptr_, nullptr)) {}

    CoTaskMemory& operator=(CoTaskMemory&& other) noexcept {
        if (this != &other) {
            // 自分の古いメモリを解放しつつ nullptr にし、相手のメモリを奪って相手側を nullptr にする
            ::CoTaskMemFree(std::exchange(ptr_, nullptr));
            ptr_ = std::exchange(other.ptr_, nullptr);
        }
        return *this;
    }

    T** operator&() noexcept {
        ::CoTaskMemFree(std::exchange(ptr_, nullptr));
        return &ptr_;
    }

    operator T*() const noexcept { return ptr_; }
    [[nodiscard]] T* Get() const noexcept { return ptr_; }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    T* ptr_;
};