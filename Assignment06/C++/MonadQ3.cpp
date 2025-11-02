// cpp/monads_pipeline.cpp
#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>

// ----------------- Option Monad Helpers -----------------
template <class A, class F>
auto maybe_bind(const std::optional<A>& mx, F f) -> decltype(f(*mx)) {
    if (!mx) return decltype(f(*mx)){};
    return f(*mx);
}

template <class A>
std::optional<A> maybe_return(A x) { return std::optional<A>(std::move(x)); }

// Safe division (Option style)
std::optional<int> safe_div(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

// Pipeline: ((x / y) / z)
std::optional<int> safe_div3(int x, int y, int z) {
    return maybe_bind(safe_div(x, y), [&](int a) {
        return maybe_bind(safe_div(a, z), [&](int b) {
            return maybe_return(b);
            });
        });
}

// ----------------- Result Monad -----------------
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

// Parse / validation functions
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
    return x < 0 ? Result<int, std::string>::err("negative") : Result<int, std::string>::ok(x);
}
Result<int, std::string> bounded100(int x) {
    return (0 <= x && x < 100) ? Result<int, std::string>::ok(x)
        : Result<int, std::string>::err("out of range");
}

Result<int, std::string> validate_cpp(const std::string& s) {
    return parse_int(s).bind(nonneg).bind(bounded100);
}

// ----------------- Main Test -----------------
int main() {
    // Option: single pipeline
    auto p1 = safe_div3(12, 3, 2);  // ((12/3)/2) = 2
    if (p1) std::cout << "safe_div3(12,3,2) = " << *p1 << "\n";
    else std::cout << "safe_div3(12,3,2) = Division by zero!\n";

    auto p2 = safe_div3(12, 0, 2);  // division by zero
    if (p2) std::cout << "safe_div3(12,0,2) = " << *p2 << "\n";
    else std::cout << "safe_div3(12,0,2) = Division by zero!\n";

    // Result: validation
    auto v1 = validate_cpp("42");
    if (v1.is_ok()) std::cout << "validate_cpp(\"42\") = Ok(" << v1.value() << ")\n";
    else std::cout << "validate_cpp(\"42\") = Err(" << v1.error() << ")\n";

    auto v2 = validate_cpp("-5");
    if (v2.is_ok()) std::cout << "validate_cpp(\"-5\") = Ok(" << v2.value() << ")\n";
    else std::cout << "validate_cpp(\"-5\") = Err(" << v2.error() << ")\n";

    return 0;
}
