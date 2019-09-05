# MutexGuards: Rust-like lock-managing smart pointers
While C++'s `std::lock_guard` is a good step towards better lock management, 
and a bit more polyvalent than Rust's pattern of locks owning the data they protect,
Rust's pattern does have an advantage of letting the type-system express what data
the lock is protecting.

This header-only library is an attempt at translating as much of Rust's owning mutex pattern as possible.

## `Mutex<T>`
Just like in Rust, `Mutex<T>` owns a value of type `T`, and protects it with a mutex.
And just like in Rust, calling `lock()` or `try_lock()` on a Mutex object will return a guard.

## `MutexGuard<T>`
`MutexGuard<T>` is a smart pointer which, upon destruction, will unlock the `Mutex` from which it was taken.

But unlike Rust, C++ cannot guarantee that the `Mutex` (and the value it contains) outlives the `MutexGuard`
taken from it (which points to the value, which might then have been freed before the guard).

Having this guarantee at runtime is too costly in my opinion to be the default, and I believe such cases
can only be caused by either patterns that may render any pointer invalid, that hopefully you will know
to avoid as a C++ programmer; or by states that are difficult to reach due to these types being _move-only_.

And of course, as with any smart pointer in C++: avoid taking raw pointers to what's inside the guards,
it will probably not end well. 