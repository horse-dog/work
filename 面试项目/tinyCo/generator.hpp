#pragma once

#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>

template <typename T>
struct Generator {

    class ExhaustedException : std::exception {};

    struct promise_type {

        auto initial_suspend() {
            return std::suspend_always{};
        }
        
        auto final_suspend() noexcept {
            return std::suspend_always{};
        }
        
        void unhandled_exception() {}
        
        auto get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        void return_void() {}

        auto await_transform(T value) {
            this->value = value;
            is_ready = true;
            return std::suspend_always{};
        }

        auto yield_value(T value) {
            this->value = value;
            is_ready = true;
            return std::suspend_always{};
        }

        T value;
        bool is_ready = false;

    };

    T next() {
        if (has_next()) {
            handle.promise().is_ready = false;
            return handle.promise().value;
        }
        throw ExhaustedException{};
    }

    bool has_next() {
        if (handle.done()) {
            return false;
        }
        if (!handle.promise().is_ready) {
            handle.resume();
        }
        if (handle.done()) {
            return false;
        } else {
            return true;
        }
    }

    explicit Generator(std::coroutine_handle<promise_type> handle) noexcept
    : handle(handle) {}

    Generator(Generator &&generator) noexcept
    : handle(std::exchange(generator.handle, {})) {}

    Generator(Generator &) = delete;

    Generator &operator=(Generator &) = delete;

    ~Generator() {
        if (handle) {
            handle.destroy();
        }
    }
    
    std::coroutine_handle<promise_type> handle;

};
