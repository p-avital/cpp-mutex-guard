#include <iostream>
#include "mutex_guard.h"

void try_lock(Mutex<int>& test) {
    // When try_lock fails, guard is invalid: you WILL segfault at some point unless you check it.
    if (auto guard = test.try_lock()){
        std::cout << (*guard)++ << std::endl;
    }
}

void lock(Mutex<int>& test) {
    auto guard = test.lock();
    // Unlike `try_lock`, `lock` always returns valid guards.
    std::cout << (*guard)++ << std::endl;
}

/// Since C++ doesn't handle lifetimes like Rust can,
/// guaranteeing that you don't shoot yourself in the foot
/// by having your guards outlive the mutex they guard can't
/// be done at compile time, so just don't do that.
/// Bellow is what I believe to be the only pattern people might
/// try without realizing they are creating orphan pointers.
/// Hopefully the segfaults will make them realize they shouldn't.
MutexGuard<int> dont_do_this() {
    Mutex<int> local_mutex(0);
    return local_mutex.lock();
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    Mutex<int> mutex(0);
    try_lock(mutex);
    lock(mutex);
    dont_do_this(); // THIS LINE MIGHT CAUSE SEGFAULTS
    auto integer = Mutex<int>::unwrap(std::move(mutex));
    std::cout << integer << std::endl;
    return 0;
}