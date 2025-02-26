// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/UTL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::math
// Documentation: https://github.com/DmitriBogdanov/UTL/blob/master/docs/module_math.md
// Source repo:   https://github.com/DmitriBogdanov/UTL
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_MATH)
#ifndef UTLHEADERGUARD_MATH
#define UTLHEADERGUARD_MATH

// _______________________ INCLUDES _______________________

#include <cassert>          // assert()
#include <cstddef>          // size_t
#include <functional>       // function<>
#include <type_traits>      // enable_if_t<>, void_t<>, is_floating_point<>, is_arithmetic<>,
                            // conditional_t<>, is_integral<>, true_type, false_type
#include <algorithm>        // sort()
#include <cstdint>          // int64_t
#include <initializer_list> // initializer_list<>
#include <utility>          // declval<>(), move()
#include <vector>           // vector<>

// ____________________ DEVELOPER DOCS ____________________

// Coordinate transformations, mathematical constants and technical helper functions.
// A bit of a mix-bag-of-everything, but in the end pretty useful.
//
// # ::PI, ::PI_TWO, ::PI_HALF, ::E, ::GOLDEN_RATION #
// Constants.
//
// # ::is_addable_with_itself<Type> #
// Integral constant, returns in "::value" whether Type supports 'operator()+' with itself.
//
// # ::is_multipliable_by_scalar<Type> #
// Integral constant, returns in "::value" whether Type supports 'operator()*' with double.
//
// # ::is_sized<Type> #
// Integral constant, returns in "::value" whether Type supports '.size()' method.
//
// # ::abs(), ::sign(), ::sqr(), ::cube(), ::midpoint(), deg_to_rad(), rad_to_deg() #
// Constexpr templated math functions, useful when writing expressions with a "textbook form" math.
//
// # ::uint_difference() #
// Returns abs(uint - uint) with respect to uint size and possible overflow.
//
// # ::linspace() #
// Tabulates [min, max] range with N evenly spaced points and returns it as a vector.
//
// # ::ssize() #
// Returns '.size()' of the argument casted to 'int'.
// Essentially a shortcut for verbose 'static_cast<int>(container.size())'.
//
// # ::ternary_branchless() #
// Branchless ternary operator. Slightly slower that regular ternary on most CPUs.
// Should not be used unless branchess qualifier is necessary (like in GPU computation).
//
// # ::ternary_bitselect() #
// Faster branchless ternary for integer types.
// If 2nd return is ommited, 0 is assumed, which allows for significant optimization.

// ____________________ IMPLEMENTATION ____________________

namespace utl::math {

// ============================
// --- Implementation Utils ---
// ============================

// Make SFINAE a bit nicer
template <bool Cond>
using _require = std::enable_if_t<Cond, bool>; // makes SFINAE a bit less cumbersome

template <class T>
using _require_scalar = _require<std::is_scalar_v<T>>;

template <class T>
using _require_arithmetic = _require<std::is_arithmetic_v<T>>;

template <class T>
using _require_integral = _require<std::is_integral_v<T>>;

template <class T>
using _require_uint = _require<std::is_integral_v<T> && std::is_unsigned_v<T>>;

template <class T>
using _require_float = _require<std::is_floating_point_v<T>>;

template <class Return, class T, class... Args>
using _require_invocable_r = _require<std::is_invocable_r_v<Return, T, Args...>>;

template <class T, class... Args>
using _require_invocable = _require<std::is_invocable_v<T, Args...>>;

// Workaround for 'static_assert(false)' making program ill-formed even
// when placed inide an 'if constexpr' branch that never compiles.
// 'static_assert(_always_false_v<T>)' on the the other hand doesn't,
// which means we can use it to mark branches that should never compile.
template <class>
inline constexpr bool _always_false_v = false;

// =================
// --- Constants ---
// =================

namespace constants {

constexpr double pi      = 3.14159265358979323846;
constexpr double two_pi  = 2. * pi;
constexpr double half_pi = 0.5 * pi;
constexpr double e       = 2.71828182845904523536;
constexpr double phi     = 1.6180339887498948482; // golden ration

} // namespace constants

// ===================
// --- Type Traits ---
// ===================

template <class Type, class = void>
struct is_sized : std::false_type {};

template <class Type>
struct is_sized<Type, std::void_t<decltype(std::declval<Type>().size())>> : std::true_type {};

template <class T>
constexpr bool is_sized_v = is_sized<T>::value;

// ======================
// --- Math functions ---
// ======================

template <class T, _require_scalar<T> = true>
[[nodiscard]] constexpr T abs(T x) noexcept {
    return (x > T(0)) ? x : -x;
}

template <class T, _require_scalar<T> = true>
[[nodiscard]] constexpr T sign(T x) noexcept {
    if constexpr (std::is_unsigned_v<T>) return (x > T(0)) ? T(1) : T(0);
    else return (x > T(0)) ? T(1) : (x < T(0)) ? T(-1) : T(0);
} // returns -1 / 0 / 1

template <class T, _require_scalar<T> = true>
[[nodiscard]] constexpr T binary_sign(T x) noexcept {
    if constexpr (std::is_unsigned_v<T>) return T(1);
    else return (x >= T(0)) ? T(1) : T(-1);
} // returns -1 / 1 (1 gets priority in x == 0)

template <class T, _require_arithmetic<T> = true>
[[nodiscard]] constexpr T sqr(T x) noexcept {
    return x * x;
}

template <class T, _require_arithmetic<T> = true>
[[nodiscard]] constexpr T cube(T x) noexcept {
    return x * x * x;
}

template <class T, _require_arithmetic<T> = true>
[[nodiscard]] constexpr T midpoint(T a, T b) noexcept {
    return (a + b) * 0.5;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr int kronecker_delta(T i, T j) noexcept {
    // 'IntegerType' here is necessary to prevent enforcing static_cast<int>(...) on the callsite
    return (i == j) ? 1 : 0;
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr int power_of_minus_one(T power) noexcept {
    return (power % T(2)) ? -1 : 1; // is there a faster way of doing it?
}

// Degree <-> radian conversion
template <class T, _require_float<T> = true>
[[nodiscard]] constexpr T deg_to_rad(T degrees) noexcept {
    constexpr T factor = T(constants::pi / 180.);
    return degrees * factor;
}

template <class T, _require_float<T> = true>
[[nodiscard]] constexpr T rad_to_deg(T radians) noexcept {
    constexpr T factor = T(180. / constants::pi);
    return radians * factor;
}

template <class Idx, class Func, _require_invocable<Func, Idx> = true>
[[nodiscard]] constexpr auto sum(Idx low, Idx high, Func&& func) {
    std::invoke_result_t<Func, Idx> res{};
    for (Idx i = low; i < high; ++i) res += func(i);
    return res;
}

// ====================
// --- Memory Units ---
// ====================

enum class MemoryUnit { BYTE, KiB, MiB, GiB, TiB, KB, MB, GB, TB };

template <class T, MemoryUnit units = MemoryUnit::MiB>
[[nodiscard]] constexpr double memory_size(std::size_t count) noexcept {
    const double size_in_bytes = count * sizeof(T); // cast to double is critical here
    if constexpr (units == MemoryUnit::BYTE) return size_in_bytes;
    else if constexpr (units == MemoryUnit::KiB) return size_in_bytes / 1024.;
    else if constexpr (units == MemoryUnit::MiB) return size_in_bytes / 1024. / 1024.;
    else if constexpr (units == MemoryUnit::GiB) return size_in_bytes / 1024. / 1024. / 1024.;
    else if constexpr (units == MemoryUnit::TiB) return size_in_bytes / 1024. / 1024. / 1024. / 1024.;
    else if constexpr (units == MemoryUnit::KB) return size_in_bytes / 1000.;
    else if constexpr (units == MemoryUnit::MB) return size_in_bytes / 1000. / 1000.;
    else if constexpr (units == MemoryUnit::GB) return size_in_bytes / 1000. / 1000. / 1000.;
    else if constexpr (units == MemoryUnit::TB) return size_in_bytes / 1000. / 1000. / 1000. / 1000.;
    else static_assert(_always_false_v<T>, "Function is a non-exhaustive visitor of enum class {MemoryUnit}.");
}

// ===============
// --- Meshing ---
// ===============

// Semantic helpers that allow user to directly pass both interval/point counts for grid subdivision,
// without thinking about whether function need +1 or -1 to its argument
struct Points {
    std::size_t count;

    Points() = delete;
    constexpr explicit Points(std::size_t count) noexcept : count(count) {}
};

struct Intervals {
    std::size_t count;

    Intervals() = delete;
    constexpr explicit Intervals(std::size_t count) noexcept : count(count) {}
    constexpr Intervals(Points points) noexcept : count(points.count > 0 ? points.count - 1 : 0) {}
};

template <class T, _require_float<T> = true>
[[nodiscard]] std::vector<T> linspace(T L1, T L2, Intervals N) {
    assert(L1 < L2);
    assert(N.count >= 1);

    const T step = (L2 - L1) / N.count;

    std::vector<T> res(N.count + 1);

    res[0] = L1;
    for (std::size_t i = 1; i < res.size(); ++i) res[i] = res[i - 1] + step;

    return res;
}

template <class T, class Func, _require_float<T> = true, _require_invocable_r<T, Func, T> = true>
[[nodiscard]] T integrate_trapezoidal(Func f, T L1, T L2, Intervals N) {
    assert(L1 < L2);
    assert(N.count >= 1);

    const T step = (L2 - L1) / N.count;

    T sum = 0;
    T x   = L1;

    for (std::size_t i = 0; i < N.count; ++i, x += step) sum += f(x) + f(x + step);

    return T(0.5) * sum * step;
}

// ====================
// --- Permutations ---
// ====================

template <class Array>
bool is_permutation(const Array& array) {
    std::vector<std::size_t> p(array.size()); // Note: "non-allocating range adapter" would fit like a glove here
    for (std::size_t i = 0; i < p.size(); ++i) p[i] = i;

    return std::is_permutation(array.begin(), array.end(), p.begin()); // I'm surprised it exists in the standard
}

template <class Array, class Permutation = std::initializer_list<std::size_t>>
void apply_permutation(Array& vector, const Permutation& permutation) {
    Array res(vector.size());

    typename Array::size_type emplace_idx = 0;
    for (auto i : permutation) res[emplace_idx++] = std::move(vector[i]);
    vector = std::move(res);
}

template <class Array, class Cmp = std::less<>>
std::vector<std::size_t> get_sorting_permutation(const Array& array, Cmp comp = Cmp()) {
    std::vector<std::size_t> permutation(array.size());
    for (std::size_t i = 0; i < permutation.size(); ++i) permutation[i] = i;

    std::sort(permutation.begin(), permutation.end(),
              [&](const auto& lhs, const auto& rhs) { return comp(array[lhs], array[rhs]); });

    return permutation;
}

template <class Array, class... SyncedArrays>
void sort_together(Array& array, SyncedArrays&... synced_arrays) {
    // Get permutation that would make the 1st array sorted
    const auto permutation = get_sorting_permutation(array);

    // Apply permutation to all arrays to "sort them in sync"
    apply_permutation(array, permutation);
    (apply_permutation(synced_arrays, permutation), ...);
}

// ====================
// --- Misc helpers ---
// ====================

template <class T, _require_uint<T> = true>
[[nodiscard]] constexpr T uint_difference(T a, T b) noexcept {
    return (a > b) ? (a - b) : (b - a);
}

template <class T, _require<is_sized_v<T>> = true>
[[nodiscard]] constexpr auto ssize(const T& container) noexcept {
    using signed_type = std::make_signed_t<decltype(container.size())>;
    return static_cast<signed_type>(container.size());
}

template <class T, _require_arithmetic<T> = true>
[[nodiscard]] constexpr T ternary_branchless(bool condition, T return_if_true, T return_if_false) noexcept {
    return (condition * return_if_true) + (!condition * return_if_false);
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T ternary_bitselect(bool condition, T return_if_true, T return_if_false) noexcept {
    return (return_if_true & -T(condition)) | (return_if_false & ~(-T(condition)));
}

template <class T, _require_integral<T> = true>
[[nodiscard]] constexpr T ternary_bitselect(bool condition, T return_if_true) noexcept {
    return return_if_true & -T(condition);
}

} // namespace utl::math

#endif
#endif // module utl::math
