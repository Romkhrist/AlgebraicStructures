module;

#include <type_traits>
#include <concepts>
#include <complex>
#include <string>
#include <functional>
#include <utility>

export module algebraic;

export namespace algebraic
{
    // Our concepts and traits are designed for binary functors,
    // so we add some additional functors that are not in the standard library.

#pragma region functors
    template <class T = void>
    struct max
    {
        [[nodiscard]] constexpr T operator()(const T& left, const T& right) const
        {
            return left > right ? left : right;
        }
    };

    template <>
    struct max<void>
    {
        template <class T, class U>
        [[nodiscard]] constexpr auto operator()(T&& left, U&& right) const
        {
            return left > right ? std::forward<T>(left) : std::forward<U>(right);
        }

        using is_transparent = int;
    };

    template <class T = void>
    struct min
    {
        [[nodiscard]] constexpr auto operator()(const T& left, const T& right) const
        {
            return left < right ? left : right;
        }
    };

    template <>
    struct min<void>
    {
        template <class T, class U>
        [[nodiscard]] constexpr auto operator()(T&& left, U&& right) const
        {
            return left < right ? std::forward<T>(left) : std::forward<U>(right);
        }

        using is_transparent = int;
    };
#pragma endregion

    namespace properties
    {
#pragma region associativity
        // ===== Associativity =====

        template <class T, class Func>
        requires std::invocable<Func, T, T>
        struct is_associative : std::false_type {};

        // (a + b) + c == a + (b + c)
        template <class T>
        struct is_associative<T, std::plus<T>> : std::true_type {};

        // (a * b) * c == a * (b * c) 
        template <class T>
        struct is_associative<T, std::multiplies<T>> : std::true_type {};

        // (a && b) && c == a && (b && c) 
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct is_associative<T, std::logical_and<T>> : std::true_type {};

        // (a || b) || c == a || (b || c) 
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct is_associative<T, std::logical_or<T>> : std::true_type {};

        // (a ^ b) ^ c == a ^ (b ^ c) 
        template <class T>
        requires std::integral<std::remove_cvref_t<T>>
        struct is_associative<T, std::bit_xor<T>> : std::true_type {};

        // (a & b) & c == a & (b & c) 
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_associative<T, std::bit_and<T>> : std::true_type {};

        // (a | b) | c == a | (b | c) 
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_associative<T, std::bit_or<T>> : std::true_type {};

        // max(a, max(b, c)) == max(max(a, b), c))
        template <class T>
        struct is_associative<T, algebraic::max<T>> : std::true_type {};

        // min(a, min(b, c)) == min(min(a, b), c))
        template <class T>
        struct is_associative<T, algebraic::min<T>> : std::true_type {};
#pragma endregion

#pragma region commutativity
        // ===== Commutativity =====

        template <class T, class Func>
        requires std::invocable<Func, T, T>
        struct is_commutative : std::false_type {};

        // a + b == b + a
        template <class T>
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>> && 
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_commutative<T, std::plus<T>> : std::true_type {};

        // For std::complex
        template <class T>
        requires std::same_as<std::complex<typename std::remove_cvref_t<T>::value_type>,
                              std::remove_cvref_t<T>>
        struct is_commutative<T, std::plus<T>> : std::true_type {};

        // a * b == b * a
        template <class T>
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_commutative<T, std::multiplies<T>> : std::true_type {};

        // For std::complex
        template <class T>
        requires std::same_as<std::complex<typename std::remove_cvref_t<T>::value_type>,
                              std::remove_cvref_t<T>>
        struct is_commutative<T, std::multiplies<T>> : std::true_type {};

        // a && b == b && a
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct is_commutative<T, std::logical_and<T>> : std::true_type {};

        // a || b == b || a
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct is_commutative<T, std::logical_or<T>> : std::true_type {};

        // a ^ b == b ^ a
        template <class T>
        requires std::integral<std::remove_cvref_t<T>>
        struct is_commutative<T, std::bit_xor<T>> : std::true_type {};

        // a & b == b & a
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_commutative<T, std::bit_and<T>> : std::true_type {};

        // a | b == b | a
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_commutative<T, std::bit_or<T>> : std::true_type {};

        // max(a, b) == max(b, a)
        template <class T>
        struct is_commutative<T, algebraic::max<T>> : std::true_type {};

        // min(a, b) == min(b, a)
        template <class T>
        struct is_commutative<T, algebraic::min<T>> : std::true_type {};
#pragma endregion

#pragma region distributivity
        // ===== Distributivity =====

        template <class T, class Func1, class Func2>
        requires (std::invocable<Func1, T, T> && std::invocable<Func2, T, T>)
        struct is_distributive : std::false_type {};

        // a * (b + c) == (a * b) + (a * c)
        template <class T>
        struct is_distributive<T, std::multiplies<T>, std::plus<T>> : std::true_type {};

        // a * (b - c) == (a * b) - (a * c)
        template <class T>
        struct is_distributive<T, std::multiplies<T>, std::minus<T>> : std::true_type {};

        // a && (b || c) == (a && b) || (a && c)
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct is_distributive<T, std::logical_and<T>, std::logical_or<T>> : std::true_type {};

        // a || (b && c) == (a || b) && (a || c)
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct is_distributive<T, std::logical_or<T>, std::logical_and<T>> : std::true_type {};

        // a && (b ^ c) == (a && b) ^ (a && c)
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct is_distributive<T, std::logical_and<T>, std::bit_xor<T>> : std::true_type {};

        // a & (b | c) == (a & b) | (a & c)
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_distributive<T, std::bit_and<T>, std::bit_or<T>> : std::true_type {};

        // a | (b & c) == (a | b) & (a | c)
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_distributive<T, std::bit_or<T>, std::bit_and<T>> : std::true_type {};

        // a & (b ^ c) == (a ^ b) & (a ^ c)
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct is_distributive<T, std::bit_and<T>, std::bit_xor<T>> : std::true_type {};

        // max(a, min(b, c)) == min(max(a, b), max(a, c))
        template <class T>
        struct is_distributive<T, algebraic::max<T>, algebraic::min<T>> : std::true_type {};

        // min(a, max(b, c)) == max(min(a, b), min(a, c))
        template <class T>
        struct is_distributive<T, algebraic::min<T>, algebraic::max<T>> : std::true_type {};
#pragma endregion

#pragma region identity
        // Template of identity value to get identity for all monoids
        // which you can use in your computations
        template <class T, class Func>
        requires std::invocable<Func, T, T>
        struct identity;

        // Identity for all arithmetic types by addition is 0 or 0.0
        template <class T>
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct identity<T, std::plus<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr auto get() { return static_cast<type>(0); }
        };

        // Identity for complex numbers by addition is 0 + 0i
        template <class T>
        requires std::same_as<std::complex<typename std::remove_cvref_t<T>::value_type>,
                              std::remove_cvref_t<T>>
        struct identity<T, std::plus<T>>
        {
            using type = std::remove_cvref_t<T>;
            static auto get() { return type{ 0, 0 }; }
        };

        // For string concatination identity is an empty string
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>,
                              std::basic_string<typename std::remove_cvref_t<T>::value_type,
                                                typename std::remove_cvref_t<T>::traits_type,
                                                typename std::remove_cvref_t<T>::allocator_type>>
        struct identity<T, std::plus<T>>
        {
            using type = std::remove_cvref_t<T>;
            static auto get() { return type{}; };
        };

        // Identity for all arithmetic types by multiplication is 1 or 1.0
        template <class T>
        requires std::is_arithmetic_v<std::remove_cvref_t<T>>
        struct identity<T, std::multiplies<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr auto get() { return static_cast<type>(1); }
        };

        // Identity for complex numbers by multiplication is 1 + 0i
        template <class T>
        requires std::same_as<std::complex<typename std::remove_cvref_t<T>::value_type>,
                              std::remove_cvref_t<T>>
        struct identity<T, std::multiplies<T>>
        {
            using type = std::remove_cvref_t<T>;
            static auto get() { return type{ 1, 0 }; }
        };

        // Identity for bool by && is "true"
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct identity<T, std::logical_and<T>>
        {
            static constexpr auto get() { return true; }
        };

        // Identity for bool by || is "false"
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>, bool>
        struct identity<T, std::logical_or<T>>
        {
            static constexpr auto get() { return false; }
        };

        // Identity for integral types by ^ is 0 (false for bool)
        template <class T>
        requires std::integral<std::remove_cvref_t<T>>
        struct identity<T, std::bit_xor<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr auto get() { return static_cast<type>(0); }
        };

        // Identity for integral types by & is all bites set to 1
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct identity<T, std::bit_and<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr auto get() { return ~static_cast<type>(0); }
        };

        // Identity for integral types by & is all bites set to 0
        template <class T>
        requires (std::integral<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct identity<T, std::bit_or<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr auto get() { return static_cast<type>(0); }
        };

        // Identity for integral types by max(a, b) is the lowest value.
        // Identity for floating point types by max(a, b) is the -infinity.
        template <class T>
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct identity<T, algebraic::max<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr type get()
            {
                if constexpr (std::integral<type>)
                {
                    return std::numeric_limits<type>::min();
                }
                else
                {
                    return -std::numeric_limits<type>::infinity();
                }
            }
        };

        // Identity for std::basic_string by max(a, b) is an empty string
        template <class T>
        requires std::same_as<std::remove_cvref_t<T>,
                              std::basic_string<typename std::remove_cvref_t<T>::value_type,
                                                typename std::remove_cvref_t<T>::traits_type,
                                                typename std::remove_cvref_t<T>::allocator_type>>
        struct identity<T, algebraic::max<T>>
        {
            using type = std::remove_cvref_t<T>;
            static auto get() { return type{}; };
        };

        // Identity for integral types by min(a, b) is the highest value.
        // Identity for floating point types by min(a, b) is the infinity.
        template <class T>
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>> &&
                 !std::same_as<std::remove_cvref_t<T>, bool>)
        struct identity<T, algebraic::min<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr type get()
            {
                if constexpr (std::integral<type>)
                {
                    return std::numeric_limits<type>::max();
                }
                else
                {
                    return std::numeric_limits<type>::infinity();
                }
            }
        };
#pragma endregion

#pragma region inverse
        // Inverse element for according value
        template <class T, class Func>
        requires std::invocable<Func, T, T>
        struct inverse;

        // Inverse element for signed arithmetic types
        template <class T>
        requires std::is_signed_v<std::remove_cvref_t<T>>
        struct inverse<T, std::plus<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr auto get(type value) { return -value; }
        };

        // Specialization for std::complex
        template <class T>
        requires std::is_same_v<std::remove_cvref_t<T>,
                                std::complex<typename std::remove_cvref_t<T>::value_type>>
        struct inverse<T, std::plus<T>>
        {
            using type = std::remove_cvref_t<T>;
            static auto get(type value) { return -value; }
        };

        // Inverse element for floating point types is 1/value
        template <class T>
        requires std::floating_point<std::remove_cvref_t<T>>
        struct inverse<T, std::multiplies<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr auto get(type value)
            { 
                return static_cast<type>(1.0)/value;
            }
        };

        // Inverse element for std::complex types
        template <class T>
        requires std::same_as<std::complex<typename std::remove_cvref_t<T>::value_type>,
                              std::remove_cvref_t<T>>
        struct inverse<T, std::multiplies<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr auto get(type value)
            {
                return type{ 1.0, 0.0 } / value;
            }
        };

        // Inverse element for integral types is the same value
        template <std::integral T>
        struct inverse<T, std::bit_xor<T>>
        {
            using type = std::remove_cvref_t<T>;
            static constexpr type get(type value) { return value; }
        };
#pragma endregion

#pragma region helpers
        // ===== Helper concepts for algebraic traits =====

        template <class T, class Func>
        concept associative = is_associative<T, Func>::value;

        template <class T, class Func>
        concept commutative = is_commutative<T, Func>::value;

        template <class T, class Func1, class Func2>
        concept distributive = is_distributive<T, Func1, Func2>::value;

        template <class T, class Func>
        concept has_identity = requires {
            algebraic::properties::identity<T, Func>::get();
        };

        template <class T, class Func>
        concept has_inverse = requires (T a) {
            algebraic::properties::inverse<T, Func>::get(a);
        };
    }
#pragma endregion

    namespace concepts
    {
        // Note. We remove all cv-qualifiers and references because
        // they are irrelevant.

        // Magma: a type closed under a binary operation (T op T -> T)
        template <class T, class Func>
        concept magma = requires (Func func, T a, T b) { { func(a, b) } -> std::convertible_to<std::remove_cvref_t<T>>; };

        // Unitary magma: a magma with an identity element
        template <class T, class Func>
        concept unital_magma = magma<T, Func> && algebraic::properties::has_identity<T, Func>;

        // Semigroup: an associative magma
        template <class T, class Func>
        concept semigroup = magma<T, Func> && algebraic::properties::associative<T, Func>;

        // Inverse semigroup: a semigroup with an inverse element
        template <class T, class Func>
        concept inverse_semigroup = semigroup<T, Func> && algebraic::properties::has_inverse<T, Func>;

        // Commutative semigroup: a semigroup with commutativity
        template <class T, class Func>
        concept commutative_semigroup = semigroup<T, Func> && algebraic::properties::commutative<T, Func>;

        // Monoid: a semigroup with an identity element
        template <class T, class Func>
        concept monoid = semigroup<T, Func> && algebraic::properties::has_identity<T, Func>;

        // Commutative monoid: a monoid with a commutative operation
        template <class T, class Func>
        concept commutative_monoid = monoid<T, Func> && algebraic::properties::commutative<T, Func>;

        // Group: a monoid with an inverse for every element
        template <class T, class Func>
        concept group = monoid<T, Func> && algebraic::properties::has_inverse<T, Func>;

        // Abelian group: a group with a commutative operation (Commutative group)
        template <class T, class Func>
        concept abelian_group = group<T, Func> && algebraic::properties::commutative<T, Func>;

        //Semiring: a monoid by Func1 and semigroup by Func2 linked by distributivity
        template <class T, class Func1, class Func2>
        concept semiring = monoid<T, Func1> && semigroup<T, Func2> && 
                           algebraic::properties::distributive<T, Func2, Func1>;

        // Unital semiring: a semiring with identity by Func2
        template <class T, class Func1, class Func2>
        concept unital_semiring = monoid<T, Func1> && monoid<T, Func2> &&
                                   algebraic::properties::distributive<T, Func2, Func1>;

        // Rng: an abelian group by Func1 and a semigroup by Func2 linked by distributivity
        template <class T, class Func1, class Func2>
        concept rng = abelian_group<T, Func1> && semigroup<T, Func2> &&
                       algebraic::properties::distributive<T, Func2, Func1>;

        // Ring: an abelian group by Func1 and a monoid by Func2 linked by distributivity
        template <class T, class Func1, class Func2>
        concept ring = abelian_group<T, Func1> && monoid<T, Func2> &&
                               algebraic::properties::distributive<T, Func2, Func1>;

        // Commutative ring: a ring with commutativity by Func2
        template <class T, class Func1, class Func2>
        concept commutative_ring = ring<T, Func1, Func2> && algebraic::properties::commutative<T, Func2>;

        template <class T, class Func1, class Func2>
        concept division_ring = abelian_group<T, Func1> && group<T, Func2> && 
            algebraic::properties::distributive<T, Func2, Func1>;

        // Field: an abelian group by Func1
        template <class T, class Func1, class Func2>
        concept field = abelian_group<T, Func1> && abelian_group<T, Func2> && 
            algebraic::properties::distributive<T, Func2, Func1>;
    }
}