// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/UTL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::integral
// Documentation: https://github.com/DmitriBogdanov/UTL/blob/master/docs/module_integral.md
// Source repo:   https://github.com/DmitriBogdanov/UTL
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_INTEGRAL)
#ifndef UTLHEADERGUARD_INTEGRAL
#define UTLHEADERGUARD_INTEGRAL

// _______________________ INCLUDES _______________________

#include <array>       // array<>
#include <cassert>     // assert()
#include <climits>     // CHAR_BIT
#include <cstddef>     // size_t
#include <cstdint>     // uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t
#include <exception>   //
#include <limits>      // numeric_limits<>::digits, numeric_limits<>::min(), numeric_limits<>::max()
#include <stdexcept>   // std::domain_error
#include <string>      // string, to_string()
#include <type_traits> // enable_if_t<>, is_integral_v<>, is_unsigned_v<>, make_unsigned_t<>

// ____________________ DEVELOPER DOCS ____________________

// NOTE: DOCS

// ____________________ IMPLEMENTATION ____________________

namespace utl::integral {

// --- Implementation utils ---
// ----------------------------

template <bool Cond>
using _require = std::enable_if_t<Cond, bool>; // makes SFINAE a bit less cumbersome

template <class T>
using _require_integral = _require<std::is_integral_v<T>>;

template <class T>
using _require_uint = _require<std::is_integral_v<T> && std::is_unsigned_v<T>>;

using _ull = unsigned long long;

// =====================
// --- Bit twiddling ---
// =====================

// Note:
// With C++20 following functions will be added into 'std::':
// - bit_width()
// - rotl()
// - rotr()

namespace bits {

using bit_type = bool;

template <class T>
constexpr std::size_t bit_sizeof = sizeof(T) * CHAR_BIT;

// Get individual bits,
// undefined behavior if 'bit >= bit_sizeof<T>'
template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T get(T value, std::size_t bit) noexcept {
    assert(bit < bit_sizeof<T>);
    return static_cast<bit_type>((value >> bit) & T(1));
}

// Set individual bits,
// undefined behavior if 'bit >= bit_sizeof<T>'
template <class T, _require_integral<T> = true>
constexpr void set(T& value, std::size_t bit, bit_type state) noexcept {
    assert(bit < bit_sizeof<T>);
    value |= (T(state) << bit);
}

// Flip individual bits,
// undefined behavior if 'bit >= bit_sizeof<T>'
template <class T, _require_integral<T> = true>
constexpr void flip(T& value, std::size_t bit) noexcept {
    assert(bit < bit_sizeof<T>);
    value ^= (T(1) << bit);
}

template <class T, _require_uint<T> = true>
[[nodiscard]] constexpr std::size_t bit_width(T value) noexcept {
    std::size_t count = 0;
    while (value) ++count, value >>= 1;
    return count;
}

// Circular left rotate,
// undefined behavior if 'shift >= bit_sizeof<T>'
template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T rotl(T value, std::size_t shift) noexcept {
    assert(shift < bit_sizeof<T>);
    return (value << shift) | (value >> (std::numeric_limits<T>::digits - shift));
}

// Circular right rotate,
// undefined behavior if 'shift >= bit_sizeof<T>'
template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T rotr(T value, std::size_t shift) noexcept {
    assert(shift < bit_sizeof<T>);
    return (value << (std::numeric_limits<T>::digits - shift)) | (value >> shift);
}

} // namespace bits

// ===============================
// --- Integral math functions ---
// ===============================

// Note:
// With C++20 following functions will be added into 'std::':
// - cmp_equal()
// - cmp_not_equal
// - cmp_less
// - cmp_greater
// - cmp_less_equal
// - cmp_greater_equal
// With C++26 following functions will be added into 'std::':
// - add_sat()
// - sub_sat()

namespace math {

// --- Rounding integer division ---
// ---------------------------------

// Rounding integer division functions that can properly handle all signed
// values and don't run into overflow issues are surprisingly tricky to
// implement, most implementation found inline are blatantly errogenous,
// some good details on the topic can be found in <intdiv> C++26 proposal
// see [https://gist.github.com/Eisenwave/2a7d7a4e74e99bbb513984107a6c63ef]

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T div_floor(T dividend, T divisor) noexcept {
    assert(divisor != T(0));

    const bool quotient_negative = (dividend < T(0)) != (divisor < T(0));
    return dividend / divisor - (dividend % divisor != T(0) && quotient_negative);
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T div_ceil(T dividend, T divisor) noexcept {
    assert(divisor != T(0));

    const bool quotient_positive = (dividend < T(0)) == (divisor < T(0));
    return dividend / divisor + (dividend % divisor != T(0) && quotient_positive);
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T div_down(T dividend, T divisor) noexcept {
    assert(divisor != T(0));

    return dividend / divisor;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T div_up(T dividend, T divisor) noexcept {
    assert(divisor != T(0));

    const T quotient_sign = (dividend < T(0) ? T(-1) : T(1)) * (divisor < T(0) ? T(-1) : T(1));
    return dividend / divisor + (dividend % divisor != T(0)) * quotient_sign;
}

// --- Saturated math ---
// ----------------------

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr bool add_overflows(T lhs, T rhs) noexcept {
    if (rhs > T(0) && lhs > std::numeric_limits<T>::max() - rhs) return false;
    if (rhs < T(0) && lhs < std::numeric_limits<T>::min() - rhs) return false;
    return true;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr bool sub_overflows(T lhs, T rhs) noexcept {
    if (rhs < T(0) && lhs > std::numeric_limits<T>::max() + rhs) return false;
    if (rhs > T(0) && lhs < std::numeric_limits<T>::min() + rhs) return false;
    return true;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr bool mul_overflows(T lhs, T rhs) noexcept {
    constexpr auto max = std::numeric_limits<T>::max();
    constexpr auto min = std::numeric_limits<T>::min();

    if (lhs < T(0) && rhs < T(0) && rhs < max / lhs) return true;
    if (lhs < T(0) && rhs > T(0) && lhs < min / rhs) return true;
    if (lhs > T(0) && rhs < T(0) && rhs < min / lhs) return true;
    if (lhs > T(0) && rhs > T(0) && lhs > max / rhs) return true;
    return false;

    // Note 1:
    // There is no portable way to implement performant saturated multiplication, C++26 standard
    // saturated functions are implemented in terms of '__builtin_mul_overflow' and '__mulh'
    // intrinsics which can speed this up by a factor of 30 to 80 times

    // Note 2:
    // We have to use different branches depending on the lhs/rhs signs and swap division order due to assymetry
    // in signed integer range, for example, for 32-bit int 'min = -2147483648', while 'max = 2147483647',
    // -2147483648 * -1  =>  positive  =>  can overflow max  =>  mul overflows, 'max / rhs' overflows, 'max / lhs' fine
    // -2147483648 *  1  =>  negative  =>  can overflow min  =>  mul      fine, 'min / rhs'      fine, 'min / lhs' fine
    //  2147483647 * -1  =>  negative  =>  can overflow min  =>  mul      fine, 'min / rhs'      fine, 'min / lhs' fine
    //  2147483647 *  1  =>  positive  =>  can overflow max  =>  mul      fine, 'max / rhs'      fine, 'max / lhs' fine

    return false;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr bool div_overflows(T lhs, T rhs) noexcept {
    assert(rhs != T(0));

    // Unsigned division can't overflow
    if constexpr (std::is_unsigned_v<T>) return false;
    // Signed division overflows only for 'min / -1', this case is illustrated in 'mul_overflows()' comments
    else return lhs == std::numeric_limits<T>::min() && rhs == T(-1);
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T add_sat(T lhs, T rhs) noexcept {
    if (rhs > T(0) && lhs > std::numeric_limits<T>::max() - rhs) return std::numeric_limits<T>::max();
    if (rhs < T(0) && lhs < std::numeric_limits<T>::min() - rhs) return std::numeric_limits<T>::min();
    return lhs + rhs;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T sub_sat(T lhs, T rhs) noexcept {
    if (rhs < T(0) && lhs > std::numeric_limits<T>::max() + rhs) return std::numeric_limits<T>::max();
    if (rhs > T(0) && lhs < std::numeric_limits<T>::min() + rhs) return std::numeric_limits<T>::min();
    return lhs - rhs;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T mul_sat(T lhs, T rhs) noexcept {
    constexpr auto max = std::numeric_limits<T>::max();
    constexpr auto min = std::numeric_limits<T>::min();

    if (lhs < 0 && rhs < 0 && rhs < max / lhs) return max;
    if (lhs < 0 && rhs > 0 && lhs < min / rhs) return min;
    if (lhs > 0 && rhs < 0 && rhs < min / lhs) return min;
    if (lhs > 0 && rhs > 0 && lhs > max / rhs) return max;
    return lhs * rhs;
} // see 'mul_overflows()' comments for a detailed explanation

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T div_sat(T lhs, T rhs) noexcept {
    assert(rhs != T(0));

    // Unsigned division can't overflow
    if constexpr (std::is_unsigned_v<T>) return lhs / rhs;
    // Signed division overflows only for 'min / -1', this case is illustrated in 'mul_overflows()' comments
    else return (lhs == std::numeric_limits<T>::min() && rhs == T(-1)) ? std::numeric_limits<T>::max() : lhs / rhs;
}

// --- Heterogenous integer comparators ---
// ----------------------------------------

// Integer comparators that properly handle differently signed integers,
// become part of 'std' in C++20

template <class T1, class T2>
[[nodiscard]] constexpr bool cmp_equal(T1 lhs, T2 rhs) noexcept {
    if constexpr (std::is_signed_v<T1> == std::is_signed_v<T2>) return lhs == rhs;
    else if constexpr (std::is_signed_v<T1>) return lhs >= 0 && std::make_unsigned_t<T1>(lhs) == rhs;
    else return rhs >= 0 && std::make_unsigned_t<T2>(rhs) == lhs;
}

template <class T1, class T2>
[[nodiscard]] constexpr bool cmp_not_equal(T1 lhs, T2 rhs) noexcept {
    return !cmp_equal(lhs, rhs);
}

template <class T1, class T2>
[[nodiscard]] constexpr bool cmp_less(T1 lhs, T2 rhs) noexcept {
    if constexpr (std::is_signed_v<T1> == std::is_signed_v<T2>) return lhs < rhs;
    else if constexpr (std::is_signed_v<T1>) return lhs < 0 || std::make_unsigned_t<T1>(lhs) < rhs;
    else return rhs >= 0 && lhs < std::make_unsigned_t<T2>(rhs);
}

template <class T1, class T2>
[[nodiscard]] constexpr bool cmp_greater(T1 lhs, T2 rhs) noexcept {
    return cmp_less(rhs, lhs);
}

template <class T1, class T2>
[[nodiscard]] constexpr bool cmp_less_equal(T1 lhs, T2 rhs) noexcept {
    return !cmp_less(rhs, lhs);
}

template <class T1, class T2>
[[nodiscard]] constexpr bool cmp_greater_equal(T1 lhs, T2 rhs) noexcept {
    return !cmp_less(lhs, rhs);
}

// Returns if 'value' is in range of type 'To'
template <class To, class From>
[[nodiscard]] constexpr bool in_range(From value) noexcept {
    return cmp_greater_equal(value, std::numeric_limits<To>::min()) &&
           cmp_less_equal(value, std::numeric_limits<To>::max());
}

// --- Casts ---
// -------------

// Integer-to-integer cast that throws if conversion overflow/underflows the result,
// no '[[nodiscard]]' because cast may be used for the side effect of throwing
template <class To, class From, _require_integral<To> = true, _require_integral<From> = true>
constexpr To narrow_cast(From value) {
    if (!in_range<To>(value)) throw std::domain_error("narrow_cast() overflows the result.");
    return static_cast<To>(value);
}

template <class To, class From, _require_integral<To> = true, _require_integral<From> = true>
[[nodiscard]] constexpr To saturate_cast(From value) noexcept {
    constexpr auto to_min      = std::numeric_limits<To>::min();
    constexpr auto to_max      = std::numeric_limits<To>::max();
    constexpr int  to_digits   = std::numeric_limits<To>::digits;
    constexpr int  from_digits = std::numeric_limits<From>::digits;

    // signed -> signed
    if constexpr (std::is_signed_v<From> && std::is_signed_v<To>) {
        // value outside of type range => clamp to range
        if constexpr (to_digits < from_digits) {
            if (value < static_cast<From>(to_min)) return to_min;
            if (value > static_cast<From>(to_max)) return to_max;
        }
    }
    // signed -> unsigned
    if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
        // value negative => clamp to 0
        if (value < static_cast<From>(to_min)) return to_min;
        // value too big after casting => clamp to max
        // note that we rely on operator '>' being able to compare unsigned types of different sizes,
        // a more explicit way would be to compare 'std::common_type_t<std::make_unsigned_t<From>, To>,
        // but it doesn't really achieve anything except verbosity
        else if (std::make_unsigned_t<From>(value) > to_max) return to_max;
    }
    // unsigned -> signed
    if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
        // value too big => clamp to max
        // like before 'make_unsigned_t' is here to make both sides of comparison unsigned
        if (value > std::make_unsigned_t<To>(to_max)) return to_max;
    }

    // unsigned -> unsigned
    // + everything that didn't trigger a runtime saturating condition
    return static_cast<To>(value);
}

// Utility used to reverse indexation logic, mostly useful when working with unsigned indeces
template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T reverse_idx(T idx, T size) noexcept {
    return size - T(1) - idx;
}

// Just so we don't have to include the entirety of <algorithm> for 2 one-liners
template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr const T& max(const T& lhs, const T& rhs) noexcept {
    return (lhs < rhs) ? rhs : lhs;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr const T& min(const T& lhs, const T& rhs) noexcept {
    return (lhs < rhs) ? lhs : rhs;
}

} // namespace math

// --- Literals ---
// ----------------

namespace literals {

// Literals for all fixed-size and commonly used integer types, 'narrow_cast()'
// ensures there is no overflow during initialization from 'unsigned long long'
// clang-format off
[[nodiscard]] constexpr auto operator"" _i8( _ull v) noexcept { return math::narrow_cast<std::int8_t  >(v); }
[[nodiscard]] constexpr auto operator"" _u8( _ull v) noexcept { return math::narrow_cast<std::uint8_t >(v); }
[[nodiscard]] constexpr auto operator"" _i16(_ull v) noexcept { return math::narrow_cast<std::int16_t >(v); }
[[nodiscard]] constexpr auto operator"" _u16(_ull v) noexcept { return math::narrow_cast<std::uint16_t>(v); }
[[nodiscard]] constexpr auto operator"" _i32(_ull v) noexcept { return math::narrow_cast<std::int32_t >(v); }
[[nodiscard]] constexpr auto operator"" _u32(_ull v) noexcept { return math::narrow_cast<std::uint32_t>(v); }
[[nodiscard]] constexpr auto operator"" _i64(_ull v) noexcept { return math::narrow_cast<std::int64_t >(v); }
[[nodiscard]] constexpr auto operator"" _u64(_ull v) noexcept { return math::narrow_cast<std::uint64_t>(v); }
[[nodiscard]] constexpr auto operator"" _sz( _ull v) noexcept { return math::narrow_cast<std::size_t  >(v); }
// clang-format on

} // namespace literals

// --- Big int ---
// ---------------

// 'BigUint' emulates an integer type sufficiently large to fit a given amount of bits.
// All values are stored in a "classic" binary format split over an array of 32-bit "words"
// (or less, word size can be adjusted). All operations work the usual way, including the overflow.
//
template <std::size_t bits_to_fit = 128>
struct BigUint {
    using self                              = BigUint;
    using word_type                         = std::uint32_t;
    using carry_type                        = std::uint64_t;
    using wide_type                         = std::uint64_t;
    constexpr static std::size_t word_bits  = std::numeric_limits<word_type>::digits;
    constexpr static std::size_t carry_bits = std::numeric_limits<carry_type>::digits;
    constexpr static std::size_t wide_bits  = std::numeric_limits<wide_type>::digits;
    constexpr static std::size_t words      = math::div_ceil(bits_to_fit, word_bits);
    constexpr static std::size_t bits       = words * word_bits;
    constexpr static std::size_t size       = bits_to_fit;
    using word_storage_type                 = std::array<word_type, words>;
    using carry_storage_type                = std::array<carry_type, self::words + 1>;
    // carry-over array has one additional word at the end, this gracefuly provides us with an integer
    // overflow since carry-over just naturaly "overflows" into another word that will not be copied
    // into the result. It's critical that integer overflow works the usual way, otherwise a lot
    // of usual integer algorithms will not work (including one we use for 'operator-')

    constexpr static carry_type carry_threshold =
        static_cast<carry_type>(std::numeric_limits<word_type>::max()) + carry_type(1);

    // Requirement 1:
    // Carry type must be twice as wide to support carry during all arithmetic operations
    static_assert(carry_bits >= 2 * word_bits);

    word_storage_type s{};

    constexpr BigUint()                    = default;
    constexpr BigUint(const self&)         = default;
    constexpr BigUint(self&&)              = default;
    constexpr self& operator=(const self&) = default;
    constexpr self& operator=(self&&)      = default;

    constexpr explicit BigUint(wide_type number) noexcept {
        // Split wide number into words so we don't bind integer conversion API to possibly small word size
        constexpr std::size_t extent = math::min(self::wide_bits / self::word_bits, self::words);
        for (std::size_t i = 0; i < extent; ++i)
            this->word(i) = static_cast<word_type>(number >> (self::word_bits * i));
    }

    template <std::size_t chars>
    constexpr explicit BigUint(const char (&str)[chars]) noexcept {
        for (std::size_t i = 0; i < self::size; ++i)
            this->bit_set(math::reverse_idx(i, self::size), str[i] == '0' ? false : true);
    }

    // --- Getters ---
    // ---------------

    constexpr word_type& word(std::size_t idx) noexcept {
        assert(idx < self::words);
        return this->s[idx];
    }
    constexpr const word_type& word(std::size_t idx) const noexcept {
        assert(idx < self::words);
        return this->s[idx];
    }
    constexpr bits::bit_type bit_get(std::size_t bit) const noexcept {
        assert(bit < self::bits);
        const std::size_t word_idx = bit / self::word_bits;
        const std::size_t bit_idx  = bit % self::word_bits;
        return bits::get(this->word(word_idx), bit_idx);
    }
    constexpr void bit_set(std::size_t bit, bits::bit_type value) noexcept {
        assert(bit < self::bits);
        const std::size_t word_idx = bit / self::word_bits;
        const std::size_t bit_idx  = bit % self::word_bits;
        return bits::set(this->word(word_idx), bit_idx, value);
    }
    constexpr void bit_flip(std::size_t bit) noexcept {
        assert(bit < self::bits);
        const std::size_t word_idx = bit / self::word_bits;
        const std::size_t bit_idx  = bit % self::word_bits;
        return bits::flip(this->word(word_idx), bit_idx);
    }
    constexpr std::size_t significant_bits() const noexcept {
        for (std::size_t i = 0; i < self::words; ++i) {
            const std::size_t reverse_i     = math::reverse_idx(i, self::words);
            const std::size_t word_sig_bits = bits::bit_width(this->word(reverse_i));
            if (word_sig_bits != 0) return i * self::word_bits + word_sig_bits;
        }
        return 0;
    }
    constexpr std::size_t significant_words() const noexcept {
        for (std::size_t i = 0; i < self::words; ++i) {
            const std::size_t reverse_i = math::reverse_idx(i, self::words);
            if (this->word(reverse_i) != 0) return reverse_i + 1;
        }
        return 0;
    }

    constexpr explicit operator bool() const noexcept {
        for (const auto& e : this->s)
            if (e != 0) return true;
        return false;
    }

    // --- Bit operators (implementation) ---
    // --------------------------------------

    constexpr static void bitwise_lshift(self& x, std::size_t shift) {
        if (shift == 0) return;

        if (shift >= self::bits) {
            x = self{};
            return;
        }

        // Implementation based on libstd++ 'std::bitset' l-shift operator
        const std::size_t wshift = shift / self::word_bits;
        const std::size_t offset = shift % self::word_bits;

        if (offset == 0) {
            for (std::size_t i = self::words - 1; i >= wshift; --i) x.word(i) = x.word(i - wshift);
        } else {
            const std::size_t suboffset = self::word_bits - offset;
            for (std::size_t i = self::words - 1; i > wshift; --i)
                x.word(i) = ((x.word(i - wshift) << offset) | (x.word(i - wshift - 1) >> suboffset));
            x.word(wshift) = x.word(0) << offset;
        }

        // Zero-fill shifted-from region
        for (std::size_t i = 0; i < wshift; ++i) x.word(i) = word_type(0);
    }

    constexpr static void bitwise_rshift(self& x, std::size_t shift) {
        if (shift == 0) return;

        if (shift >= self::bits) {
            x = self{};
            return;
        }

        // Implementation based on libstd++ 'std::bitset' r-shift operator
        const std::size_t wshift = shift / self::word_bits;
        const std::size_t offset = shift % self::word_bits;
        const std::size_t limit  = self::words - wshift - 1;

        if (offset == 0) {
            for (std::size_t i = 0; i <= limit; ++i) x.word(i) = x.word(i + wshift);
        } else {
            const std::size_t suboffset = self::word_bits - offset;
            for (std::size_t i = 0; i < limit; ++i)
                x.word(i) = ((x.word(i + wshift) >> offset) | (x.word(i + wshift + 1) << suboffset));
            x.word(limit) = x.word(self::words - 1) >> offset;
        }

        // Zero-fill shifted-from region
        for (std::size_t i = limit + 1; i < self::words; ++i) x.word(i) = word_type(0);
    }

    constexpr static void bitwise_and(self& x, const self& y) {
        for (std::size_t i = 0; i < self::words; ++i) x.word(i) &= y.word(i);
    }
    constexpr static void bitwise_or(self& x, const self& y) {
        for (std::size_t i = 0; i < self::words; ++i) x.word(i) |= y.word(i);
    }
    constexpr static void bitwise_xor(self& x, const self& y) {
        for (std::size_t i = 0; i < self::words; ++i) x.word(i) ^= y.word(i);
    }
    constexpr static void bitwise_flip(self& x) {
        for (std::size_t i = 0; i < self::words; ++i) x.word(i) = ~x.word(i);
    }


    // --- Arithmetic operators (implementation) ---
    // ---------------------------------------------

    constexpr static void increment(self& x) noexcept {
        if (x.word(0) < carry_threshold - 2) ++x.word(0);
        else self::add(x, self(1));
        // vast majority of the time we can just increment the last word without doing while O(N) addition
    }

    constexpr static void decrement(self& x) noexcept {
        if (x.word(0) > 0) --x.word(0);
        else self::substract(x, self(1));
    }
    
    // Classic "long addition" with carry-over
    //
    // This is just like a column addition one would see in school, except instead of operating on digits
    // we operate one on whole integers. Essentially, binary addition with bit-wise operations is long addition
    // with base 2 carry-over, "long addition" used in school is carry-over base 10, and here we have carry-over 
    // base 2^32. In all those cases we have to essentially provide a "wider" type that can store an intermediate
    // result larger than what fits into a single "digit" of number, in binary wider type is "emulated" with a 
    // 3rd array, in classic long addition we can do carry-over subconsciously because we can conceive numbers
    // larger that 10, and here we use a wider integer type. To do carry-over for addition in base-N, we need
    // at least base-2N "carry type". Same for substraction. For multiplication we need at least base-(N^2) carry
    // type. For long division things are more difficult, but at least short division (aka long division by a 
    // 1-digit number) is easily implementable with the same idea.
    //
    // We could just work with bit-wise operations purely and implement binary logic as-is, but that proves to
    // be MUCH slower than reducing things to base 2^32 arithmetics.
    //
    // Note:
    // If 'word_type' is set to anything smaller than 'std::uint32_t' just replace '32' with a new size in this
    // explanation, all logic is generic so it will work the same way.

    constexpr static void add(self& x, const self& y) noexcept {
        carry_storage_type carry{};

        for (std::size_t i = 0; i < self::words; ++i) {
            carry[i] += static_cast<carry_type>(x.word(i)) + static_cast<carry_type>(y.word(i));

            if (carry[i] >= carry_threshold) ++carry[i + 1], carry[i] -= carry_threshold;

            x.word(i) = static_cast<word_type>(carry[i]);
        }
    }

    constexpr static void substract(self& x, const self& y) noexcept {
        // Substract using bit-wise formula: res = x + (~y + self(1))
        self temp = ~y;
        ++temp;
        add(x, temp);
    }

    constexpr static void short_multiply(self& x, word_type y) noexcept {
        // when 'y' ia only one word we can multiply with O(N) complexity instead of O(N^2)

        carry_storage_type carry{};

        for (std::size_t i = 0; i < self::words; ++i) {
            carry[i] += static_cast<carry_type>(x.word(i)) * static_cast<carry_type>(y);

            carry[i + 1] += (carry[i] >> self::word_bits);

            x.word(i) = static_cast<word_type>(carry[i]);
        }
    }

    constexpr static void long_multiply(self& x, const self& y) noexcept {
        // when 'y' has multiple words we have to do generic O(N^2) multiplication

        self res{};

        for (std::size_t digit = 0; digit < y.significant_words(); ++digit) {
            self sum_for_digit = x;
            short_multiply(sum_for_digit, y.word(digit));
            sum_for_digit <<= digit * self::word_bits;

            res += sum_for_digit;
        }

        x = res;
    }

    constexpr static void multiply(self& x, const self& y) noexcept {
        const std::size_t x_sig_words = x.significant_words();
        const std::size_t y_sig_words = y.significant_words();

        // Reorder RHS / LHS if necessary
        // prod = x * y
        if (x_sig_words >= y_sig_words) {

            if (y_sig_words <= 1) self::short_multiply(x, y.word(0));
            else self::long_multiply(x, y);
        }
        // prod = y * x
        else {
            self temp = y;

            if (x_sig_words <= 1) self::short_multiply(temp, x.word(0));
            else self::long_multiply(temp, x);

            x = temp;
        }
    }

    constexpr static void short_divide(self& quot, self& rem, const self& x, word_type y) noexcept {
        // when 'y' is 1-word long we can use the same wider 'carry_type' trick to perform operations on
        // intermediate results, we can do this because we only work on 1-2 digits at a time and never more,
        // total complexity O(N)
        carry_type carry{};
        
        for (std::size_t i = 0; i < self::words; ++i) {
            const std::size_t reverse_i = math::reverse_idx(i, self::words);;
            
            const carry_type w = static_cast<carry_type>(x.word(reverse_i));
            
            carry = (carry << self::word_bits ) | (w);
            
            quot.word(reverse_i) = static_cast<word_type>(carry / y);
            rem.word(reverse_i) = static_cast<word_type>(carry % y);
            
            carry = rem.word(reverse_i);
        }
    }

    constexpr static void long_divide(self& quot, self& rem, const self& x, const self& y) noexcept {
        // generic long division, O(N^2) complexity
        
        assert(y); // prevent division by zero
        
        // TODO:

        // Standard long division algorithm from [https://en.wikipedia.org/wiki/Division_algorithm]
        quot                 = self{};
        rem                  = self{};
        std::size_t sig_bits = x.significant_bits();

        for (std::size_t i = 0; i < sig_bits; ++i) {
            rem <<= 1;
            rem.bit_set(0, x.bit_get(math::reverse_idx(i, sig_bits)));
            if (rem >= y) {
                rem -= y;
                quot.bit_set(math::reverse_idx(i, sig_bits), true);
            }
        }
    }

    constexpr static void divide(self& quot, self& rem, const self& x, const self& y) noexcept {
        const std::size_t x_sig_words = x.significant_words();
        const std::size_t y_sig_words = y.significant_words();

        if (x_sig_words <= 1 && y_sig_words <= 1) {
            quot.word(0) = x.word(0) / y.word(0);
            rem.word(0)  = x.word(0) % y.word(0);
            return;
        }

        if (y_sig_words == 1) {
            self::short_divide(quot, rem, x, y.word(0));
            return;
        }

        self::long_divide(quot, rem, x, y);
    }

    // --- Operators (API) ---
    // -----------------------

    // clang-format off
    // Unary operators
    constexpr self& operator++()       noexcept { self::increment(   *this); return *this; }
    constexpr self& operator--()       noexcept { self::decrement(   *this); return *this; }
    constexpr self  operator~ () const noexcept { self res = *this; self::bitwise_flip(res); return res; }
    
    // Binary operators
    constexpr self operator<<(std::size_t shift) const noexcept { return self(*this) <<= shift; }
    constexpr self operator>>(std::size_t shift) const noexcept { return self(*this) >>= shift; }
    constexpr self operator& (const self& other) const noexcept { return self(*this)  &= other; }
    constexpr self operator| (const self& other) const noexcept { return self(*this)  |= other; }
    constexpr self operator^ (const self& other) const noexcept { return self(*this)  ^= other; }
    constexpr self operator+ (const self& other) const noexcept { return self(*this)  += other; }
    constexpr self operator- (const self& other) const noexcept { return self(*this)  -= other; }
    constexpr self operator* (const self& other) const noexcept { return self(*this)  *= other; }
    constexpr self operator/ (const self& other) const noexcept { return self(*this)  /= other; }
    constexpr self operator% (const self& other) const noexcept { return self(*this)  %= other; }
    
    // Augmented assignment
    constexpr self& operator<<=(std::size_t shift) { self::bitwise_lshift(*this, shift); return *this; }
    constexpr self& operator>>=(std::size_t shift) { self::bitwise_rshift(*this, shift); return *this; }
    constexpr self& operator&= (const self& other) { self::bitwise_and(   *this, other); return *this; }
    constexpr self& operator|= (const self& other) { self::bitwise_or(    *this, other); return *this; }
    constexpr self& operator^= (const self& other) { self::bitwise_xor(   *this, other); return *this; }
    constexpr self& operator+= (const self& other) { self::add(           *this, other); return *this; }
    constexpr self& operator-= (const self& other) { self::substract(     *this, other); return *this; }
    constexpr self& operator*= (const self& other) { self::multiply(      *this, other); return *this; }
    constexpr self& operator/= (const self& other) { self q{}, r{}; divide(q, r, *this, other); return *this = q; }
    constexpr self& operator%= (const self& other) { self q{}, r{}; divide(q, r, *this, other); return *this = r; }
    // clang-format on

    // --- Comparison ---
    // ------------------

    constexpr bool operator==(const self& other) const noexcept {
        for (std::size_t i = 0; i < self::words; ++i)
            if (this->word(i) != other.word(i)) return false;
        return true;
    }
    constexpr bool operator<=(const self& other) const noexcept {
        // Compare lexicographically from highest to lowest bits
        for (std::size_t i = 0; i < self::words; ++i)
            if (this->word(i) <= other.word(i)) return true;
        return false;
    }
    constexpr bool operator<(const self& other) const noexcept {
        // Compare lexicographically from highest to lowest bits
        for (std::size_t i = 0; i < self::words; ++i)
            if (this->word(i) < other.word(i)) return true;
        return false;
    }

    constexpr bool operator!=(const self& other) const noexcept { return !(*this == other); }
    constexpr bool operator>=(const self& other) const noexcept { return !(*this < other); }
    constexpr bool operator>(const self& other) const noexcept { return !(*this <= other); }

    // --- Serialization ---
    // ---------------------

    constexpr wide_type to_int() const noexcept {
        // Merge words into wide number so we don't bind integer conversion API to possibly small word size
        constexpr std::size_t extent = math::min(self::wide_bits / self::word_bits, self::words);
        wide_type             res{};
        for (std::size_t i = 0; i < extent; ++i) res |= static_cast<wide_type>(this->word(i)) << i * self::word_bits;
        return res;
    }

    template <bool prettify>
    std::string to_string() const {
        constexpr auto color_red       = "\033[31m";
        constexpr auto color_blue      = "\033[34m";
        constexpr auto color_green     = "\033[32m";
        constexpr auto color_magenta   = "\033[35m";
        constexpr auto color_bold_gray = "\033[90;1m";
        constexpr auto color_reset     = "\033[0m";

        std::string str;

        if constexpr (prettify) {
            str += color_green;
            str += "BigInt<";
            str += std::to_string(self::size);
            str += ">";
            str += color_reset;
        }

        if constexpr (prettify) str += color_bold_gray;
        str += "[";
        if constexpr (prettify) str += color_reset;

        for (std::size_t i = 0; i < self::bits; ++i) {
            int bit = this->bit_get(math::reverse_idx(i, self::bits));
            if constexpr (prettify) str += bit ? color_red : color_blue;
            str += std::to_string(bit);
            if constexpr (prettify) str += color_reset;
        }

        if constexpr (prettify) str += color_bold_gray;
        str += "]";
        if constexpr (prettify) str += color_reset;

        if constexpr (prettify) {
            str += color_magenta;
            str += "(";
            str += std::to_string(this->significant_bits());
            str += " sig. bits)";
            str += "( words: ";
            for (const auto& e : s) (str += std::to_string(e)) += " ";
            str += ")";
            str += color_reset;
        }

        return str;
    }
};

// CTAD so we can deduce size from integer & string literals
BigUint(BigUint<>::word_type)->BigUint<>;

template <std::size_t chars>
BigUint(const char (&str)[chars]) -> BigUint<chars - 1>;

} // namespace utl::integral

#endif
#endif // module utl::integral
