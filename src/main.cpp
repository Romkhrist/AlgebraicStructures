#include <iostream>
#include <typeinfo>
#include <string>
#include <complex>
#include <concepts>
#include <chrono>
#include <functional>
#include <valarray>
#include <optional>

import algebraic;

using namespace std;
using namespace algebraic;
using namespace properties;
using namespace algebraic::concepts;

template <class... Ts>
requires monoid<common_type_t<Ts...>, plus<common_type_t<Ts...>>>
constexpr auto sum(Ts... ts)
{
    return (ts + ...);
}

template <class... Ts>
requires monoid<common_type_t<Ts...>, multiplies<common_type_t<Ts...>>>
constexpr auto mul(Ts... ts)
{
    return (ts * ...);
}

int main()
{
    sum(complex{ 2.3, 2.5 }, complex{ 2.4, 2.1 });
    static_assert(magma<complex<double>, plus<complex<double>>>);
    static_assert(semigroup<complex<double>, plus<complex<double>>>);
    static_assert(monoid<complex<double>, plus<complex<double>>>);

    using type = float;
    static_assert(division_ring<type, plus<type>, multiplies<type>>);
    static_assert(distributive<type, multiplies<type>, plus<type>>);
    static_assert(field<type, plus<type>, multiplies<type>>);
}