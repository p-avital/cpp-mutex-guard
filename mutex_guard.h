/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef RS_LIKE_MUTEX_MUTEX_GUARD_H
#define RS_LIKE_MUTEX_MUTEX_GUARD_H
#include <mutex>

template<typename T>
/// A move-only smart pointer to a locked value.
/// Dropping it will unlock the value.
/// If returned from a `Mutex<T>::try_lock()` call, the pointer may be invalid.
/// You can cast it to a boolean to check for validity, like you would any pointer.
class MutexGuard {
private:
    T *value;
    std::mutex *mutex;
public:
    MutexGuard(T *value, std::mutex *mutex) : value(value), mutex(mutex) {}

    ~MutexGuard() {
        if (mutex != nullptr) {
            mutex->unlock();
        }
    }

    T &operator*() const noexcept {
        return *value;
    }

    T *operator->() const noexcept {
        return value;
    }

    // Ensure that MutexGuard is a move-only type
    // to avoid unwanted copies and ensure dropping happens only once, as soon as possible
    // The moved from guard is mutated to forbid it from unlocking the mutex, due to C++'s
    // moved from objects still being destroyed, rather than simply having their memory
    // freed like in Rust.

    MutexGuard(MutexGuard &&guard) noexcept {
        value = guard.value;
        mutex = guard.mutex;
        guard.mutex = nullptr;
    }

    MutexGuard &operator=(MutexGuard &&guard) noexcept {
        value = guard.value;
        mutex = guard.mutex;
        guard.mutex = nullptr;
    }

    MutexGuard(const MutexGuard &) = delete;

    MutexGuard &operator=(const MutexGuard &) = delete;

    operator bool() const { // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        return mutex != nullptr;
    }
};

template<typename T>
/// A C++ equivalent of Rust's std::sync::Mutex
class Mutex {
private:
    std::mutex mutex;
    T value;
public:
    explicit Mutex(T value) : value(value) {}

    /// Tries to lock the mutex and returns a POTENTIALLY INVALID smart pointer to the value.
    /// Dropping the smart pointer will unlock the mutex if locking succeeded.
    MutexGuard<T> try_lock() {
        if (mutex.try_lock()) {
            return MutexGuard<T>(&value, &mutex);
        }
        return MutexGuard<T>(nullptr, nullptr);
    }

    /// Locks the mutex and returns a smart pointer to the value.
    /// Dropping the smart pointer will unlock the mutex.
    MutexGuard<T> lock() {
        mutex.lock();
        return MutexGuard<T>(&value, &mutex);
    }

    static T unwrap(Mutex<T>&& self) {
        return std::move(self.value);
    }

    // Ensure that Mutex is a move-only type to avoid unwanted copies

    Mutex(Mutex &&) noexcept = default;

    Mutex &operator=(Mutex &&) noexcept = default;

    Mutex(const Mutex &) = delete;

    Mutex &operator=(const Mutex &) = delete;
};

#endif //RS_LIKE_MUTEX_MUTEX_GUARD_H