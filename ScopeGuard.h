#pragma once

template <class Functor> class ScopeGuard {
    Functor f;

  public:
    explicit ScopeGuard(Functor&& func) : f(std::move(func)) {}
    ~ScopeGuard() { f(); }
};
