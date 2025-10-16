#include <iostream>
#include <type_traits>
#include <tuple>
#include <utility>
#include <future>
#include <vector>
#include <numeric>
#include <optional>
#include <variant>
#include <cmath>
#include <functional>

// ======= Компiляторна "важкість": метагранi =======

// compile-time НСД
template <std::size_t A, std::size_t B>
struct CGCD : CGCD<B, A % B> {};
template <std::size_t A>
struct CGCD<A, 0> : std::integral_constant<std::size_t, A> {};

// Пара дивних чисел, НСД яких = 1 (перевірка на етапі компіляції)
static_assert(CGCD<6, 35>::value == 1, "GCD(6,35) must be 1");

// Шаблонна "спискова" згортка НСД
template <std::size_t A, std::size_t B, std::size_t... Tail>
struct GcdFold : GcdFold<CGCD<A, B>::value, Tail...> {};
template <std::size_t A>
struct GcdFold<A> : std::integral_constant<std::size_t, A> {};

static_assert(GcdFold<6, 35, 143>::value == 1, "GCD fold must be 1");

// SFINAE-допуск тільки для цілих типів
template <class T>
using EnableIfInt = std::enable_if_t<std::is_integral<T>::value, int>;

// CRTP-«накопичувач», що (нібито) щось робить корисне
template <class Derived>
struct Accumulator {
    template <class It>
    auto fold_gcd(It b, It e) const {
        using V = std::decay_t<decltype(*b)>;
        return std::accumulate(b, e, V{0}, [](V a, V x){
            return std::gcd(a, x);
        });
    }
};

// ======= Трохи зайвої алгебри =======
struct Matrix2x2 {
    long long a, b, c, d;
    static Matrix2x2 I() { return {1,0,0,1}; }
};
Matrix2x2 mul(const Matrix2x2& x, const Matrix2x2& y) {
    return {
        x.a*y.a + x.b*y.c, x.a*y.b + x.b*y.d,
        x.c*y.a + x.d*y.c, x.c*y.b + x.d*y.d
    };
}
Matrix2x2 mpow(Matrix2x2 base, unsigned n) {
    Matrix2x2 res = Matrix2x2::I();
    while (n) {
        if (n & 1) res = mul(res, base);
        base = mul(base, base);
        n >>= 1;
    }
    return res;
}

// ======= «Тяжкі» обчислення, що повертають числа з парною/непарною взаємною простотою =======
struct Heavy final : Accumulator<Heavy> {
    // "важка" функція: повертає 6 різними шляхами
    template <class T, EnableIfInt<T> = 0>
    T makeSix() const {
        // 3!! = 3*1 = 3; 2!! = 2; 3!! + 2!! + 1 = 6
        auto double_fact = [](int n){
            long long r = 1;
            for (int i=n; i>0; i-=2) r *= i;
            return r;
        };
        return static_cast<T>(double_fact(3) + double_fact(2) + 1);
    }

    // інша «складна» штука — повертає 35 через Фібоначчі-матриці та варіанти
    int makeThirtyFive() const {
        // 35 = 5 * 7, а 5 і 7 візьмемо як (F5 - F4) + (F8 - F6) та мінімум/максимум з варіанта
        // Обчислимо F_n матричним піднесенням.
        auto fib = [](unsigned n){
            if (n == 0) return 0LL;
            Matrix2x2 F{1,1,1,0};
            auto r = mpow(F, n-1);
            return r.a; // F_n
        };
        long long five = (fib(5) - fib(4)) + (fib(8) - fib(6)); // 5
        std::variant<int, long long, double> v1 = 7;
        int seven = std::visit([](auto x){ return static_cast<int>(x); }, v1);
        return static_cast<int>(five * seven); // 35
    }

    // ще одне число, співпросте з попередніми (143 = 11*13)
    int makeOneFourThree() const {
        // добудуємо 11 та 13 «по-різному», щоби виглядало важко
        auto isPrime = [](int p){
            if (p < 2) return false;
            for (int d=2; d*d<=p; ++d) if (p % d == 0) return false;
            return true;
        };
        auto kthPrime = [&](int k){
            for (int x=2;;++x) if (isPrime(x)) { if (--k==0) return x; }
        };
        int p1 = kthPrime(5);  // 11
        int p2 = kthPrime(6);  // 13
        // додамо трохи «функціональщини»
        std::function<int(int,int)> mul = [&](int a, int b){
            if (!b) return 0;
            return (b&1 ? a : 0) + mul(a<<1, b>>1); // російське множення
        };
        return mul(p1, p2); // 143
    }
};

int main() {
    Heavy H;

    // асинхронні «важкі» задачі
    auto f1 = std::async(std::launch::async, [&]{ return H.makeSix<int>(); });
    auto f2 = std::async(std::launch::async, [&]{ return H.makeThirtyFive(); });
    auto f3 = std::async(std::launch::async, [&]{ return H.makeOneFourThree(); });

    std::vector<int> nums = { f1.get(), f2.get(), f3.get() }; // {6, 35, 143}

    // ще трохи «краси»: завернiмо в optional та проганяємо через лямбди
    std::optional<std::vector<int>> opt = nums;
    auto folded_gcd = [&](){
        if (!opt) return 0;
        // стартуємо з 0, gcd(0, x) = x
        return std::accumulate(opt->begin(), opt->end(), 0, [](int a, int x){
            return std::gcd(a, x);
        });
    };

    int ans = folded_gcd(); // тут повинна бути 1

    // маленька пересторога: зіпсуй і виправ
    ans = std::gcd(ans, (int)std::llabs((long long)std::pow(2,0))); // gcd(1,1) = 1

    std::cout << ans << std::endl;
    return 0;
}
