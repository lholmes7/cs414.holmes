// Landon Holmes
// Monad.cpp
#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <ranges>
#include <cmath>

// Option helpers 
template <class A, class F>
auto maybe_bind(const std::optional<A>& mx, F f) -> decltype(f(*mx)) {
    if (!mx) return decltype(f(*mx)){};  // return empty optional of correct type
    return f(*mx);
}

template <class A>
std::optional<A> maybe_return(A x) { return std::optional<A>(std::move(x)); }

// Option division
std::optional<int> safe_div(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

// Pipeline
std::optional<int> pipeline_opt(int x) {
    return maybe_bind(safe_div(x, 2), [&](int a) {
        return maybe_bind(safe_div(a, 3), [&](int b) {
            return maybe_bind(safe_div(b, 2), [&](int c) {
                return maybe_return(c);
                });
            });
        });
}

// Min result
template <class T, class E>
class Result {
    std::variant<T, E> data_;
public:
    static Result ok(T v) { return Result(std::move(v)); }
    static Result err(E e) { return Result(std::move(e)); }

    bool is_ok() const { return std::holds_alternative<T>(data_); }
    const T& value() const { return std::get<T>(data_); }
    const E& error() const { return std::get<E>(data_); }

private:
    Result(T v) : data_(std::move(v)) {}
    Result(E e) : data_(std::move(e)) {}

public:
    template <class F>
    auto bind(F f) const -> decltype(f(std::declval<T>())) {
        using R = decltype(f(std::declval<T>()));
        if (!is_ok()) return R::err(error());
        return f(value());
    }

    template <class F>
    auto map(F f) const -> Result<decltype(f(std::declval<T>())), E> {
        using U = decltype(f(std::declval<T>()));
        if (!is_ok()) return Result<U, E>::err(error());
        return Result<U, E>::ok(f(value()));
    }
};

// helper
Result<int, std::string> parse_int(const std::string& s) {
    try {
        size_t idx = 0;
        int v = std::stoi(s, &idx, 10);
        if (idx != s.size()) return Result<int, std::string>::err("trailing chars");
        return Result<int, std::string>::ok(v);
    }
    catch (...) {
        return Result<int, std::string>::err("not an int");
    }
}

Result<int, std::string> nonneg(int x) {
    return x < 0 ? Result<int, std::string>::err("negative")
        : Result<int, std::string>::ok(x);
}

Result<int, std::string> bounded100(int x) {
    return (0 <= x && x < 100) ? Result<int, std::string>::ok(x)
        : Result<int, std::string>::err("out of range");
}

Result<int, std::string> validate_cpp(const std::string& s) {
    return parse_int(s).bind(nonneg).bind(bounded100);
}

// triple
std::vector<std::tuple<int, int, int>> pythagorean_loops(int max_n) {
    std::vector<std::tuple<int, int, int>> triples;
    for (int a = 1; a <= max_n; ++a) {
        for (int b = a; b <= max_n; ++b) {
            int c2 = a * a + b * b;
            int c = static_cast<int>(std::sqrt(c2));
            if (c * c == c2 && c <= max_n) {
                triples.emplace_back(a, b, c);
            }
        }
    }
    return triples;
}

// tests

int main() {
    // Option/pipeline
    auto p1 = pipeline_opt(36);      // 36/2=18 -> /3=6 -> /2=3
    assert(p1 && *p1 == 3);
    auto p2 = pipeline_opt(1);       // 1/2=0 -> /3=0 -> /2=0
    assert(p2 && *p2 == 0);

    // Result/validate
    auto v1 = validate_cpp("42");    // Ok(42)
    assert(v1.is_ok() && v1.value() == 42);
    auto v2 = validate_cpp("-5");    // Err("negative")
    assert(!v2.is_ok());

    // Triples (small n)
    auto ts = pythagorean_loops(20);
    bool found345 = false;
    for (auto& t : ts) {
        auto [a, b, c] = t;
        if (a == 3 && b == 4 && c == 5) found345 = true;
    }
    assert(found345);

    std::cout << "All checks passed.\n";
}
