# utl::math

[<- to README.md](..)

[<- to implementation.hpp](https://github.com/DmitriBogdanov/UTL/blob/master/include/UTL/math.hpp)

**math** module adds various math that help express code in mathematical notation, main features are:

-  `constexpr` versions of some [`<cmath>`](https://en.cppreference.com/w/cpp/header/cmath) functions
- Expressive functions to make code look closer to the mathematical notation
- Some frequently needed, buy annoying to implement utils (such as sorting multiple arrays in sync, meshing, 1D integration and etc.)

## Definitions

```cpp
// Constants
namespace constants {
    constexpr double pi      = 3.14159265358979323846;
    constexpr double two_pi  = 2.  * pi;
    constexpr double half_pi = 0.5 * pi;
    constexpr double e       = 2.71828182845904523536;
    constexpr double phi     = 1.6180339887498948482;
}

// Basic functions
template <class T> constexpr T   abs(T x) noexcept;
template <class T> constexpr T  sign(T x) noexcept;
template <class T> constexpr T bsign(T x) noexcept;
template <class T> constexpr T   sqr(T x) noexcept;
template <class T> constexpr T  cube(T x) noexcept;
template <class T> constexpr T   inv(T x) noexcept;

template <class T> constexpr T      pow(T x, std::size_t p) noexcept;
template <class T> constexpr T midpoint(T a,           T b) noexcept;
template <class T> constexpr T  absdiff(T a,           T b) noexcept;

constexpr int signpow(int p) noexcept;

// Indicator functions
template <class T> constexpr T heaviside(T x) noexcept;

template <class Integer> constexpr Integer kronecker_delta(Integer i, Integer j           ) noexcept;
template <class Integer> constexpr Integer     levi_civita(Integer i, Integer j, Integer k) noexcept;

// Degrees and radians
template <class Float> constexpr Float deg_to_rad(Float degrees) noexcept;
template <class Float> constexpr Float rad_to_deg(Float radians) noexcept;

// Sequence operations
template <class Idx, class Func> constexpr auto  sum(Idx low, Idx high, Func&& func);
template <class Idx, class Func> constexpr auto prod(Idx low, Idx high, Func&& func);

// Indexation
template <class Container> auto ssize(const Container& container);

template <class T> constexpr T reverse_idx(T idx, T size) noexcept;

// Permutations
template <class Array>
bool is_permutation(const Array& array);

template <class Array, class Permutation = std::initializer_list<std::size_t>>
void apply_permutation(Array& array, const Permutation& permutation);

template <class Array, class Cmp = std::less<>>
std::vector<std::size_t> sorting_permutation(const Array& array, Cmp cmp = Cmp());

template <class Array, class... SyncedArrays>
void sort_together(Array& array, SyncedArrays&... synced_arrays);

// Branchless ternary
template <class T>
T      ternary_branchless(bool condition,       T return_if_true,       T return_if_false) noexcept;

template <class Integer>
Integer ternary_bitselect(bool condition, Integer return_if_true, Integer return_if_false) noexcept;

template <class Integer>
Integer ternary_bitselect(bool condition, Integer return_if_true  /* returns 0 if false*/) noexcept;

// Meshing
struct Points {
    std::size_t count;
    constexpr explicit Points(std::size_t count);
};

struct Intervals {
    std::size_t count;
    constexpr explicit Intervals(std::size_t count );
    constexpr          Intervals(Points      points);
};

template <class Float> std::vector<Float>  linspace(Float L1, Float L2, Intervals N);
template <class Float> std::vector<Float> chebspace(Float L1, Float L2, Intervals N);

template <class Float, class Func>
Float integrate_trapezoidal(Func&& f, Float L1, Float L2, Intervals N);

// Memory usage
enum class MemoryUnit { BYTE, KiB, MiB, GiB, TiB, KB, MB, GB, TB };

template <MemoryUnit units = MemoryUnit::MiB>
constexpr double to_memory_units(std::size_t bytes) noexcept;

template <MemoryUnit units = MemoryUnit::MiB, class T>
constexpr double quick_memory_estimate(const T& value);
```

All methods have appropriate `enable_if<>` conditions and `constexpr` qualifiers, which are omitted in documentation for reduced verbosity.

Methods that deal with floating-point values require explicitly floating-point inputs for mathematical strictness.

## Methods

### Constants

> ```cpp
> namespace constants {
>     constexpr double pi      = 3.14159265358979323846;
>     constexpr double two_pi  = 2.  * pi;
>     constexpr double half_pi = 0.5 * pi;
>     constexpr double e       = 2.71828182845904523536;
>     constexpr double phi     = 1.6180339887498948482;
> }
> ```

Basic mathematical constant. In **C++20** most of these get standardized as a part of [`<numbers>`](https://en.cppreference.com/w/cpp/numeric/constants) header.

### Basic functions

> ```cpp
> template <class T> constexpr T   abs(T x) noexcept;
> template <class T> constexpr T  sign(T x) noexcept;
> template <class T> constexpr T bsign(T x) noexcept;
> template <class T> constexpr T   sqr(T x) noexcept;
> template <class T> constexpr T  cube(T x) noexcept;
> template <class T> constexpr T   inv(T x) noexcept;
> ```

Returns $|x|$, $\mathrm{sign} (x)$, $x^2$, $x^3$ or $x^{-1}$ of an appropriate type.

**Note:** `sign()` is a standard sign function which returns $-1$, $0$ or $1$. `bsign()` is a binary variation that returns $1$ instead of $0$.

> ```cpp
> template <class T> constexpr T      pow(T x, std::size_t p) noexcept;
> template <class T> constexpr T midpoint(T a,           T b) noexcept;
> template <class T> constexpr T  absdiff(T a,           T b) noexcept;
> ```

Returns $x^p$, $\dfrac{a + b}{2}$ or $|a - b|$ of an appropriate type.

> ```cpp
> constexpr int signpow(int p) noexcept;
> ```

Returns $-1^p$. Faster than generic `pow` algorithms.

### Indicator functions

> ```cpp
> template <class T> constexpr T heaviside(T x) noexcept;
> ```

Computes [Heaviside step function](https://en.wikipedia.org/wiki/Heaviside_step_function).

> ```cpp
> template <class Integer> constexpr Integer kronecker_delta(Integer i, Integer j) noexcept;
> ```

Computes [Kronecker delta](https://en.wikipedia.org/wiki/Kronecker_delta) symbol: `1` if `i == j`, `0` otherwise.

> ```cpp
> template <class Integer> constexpr Integer levi_civita(Integer i, Integer j, Integer k) noexcept;
> ```

Computes [Levi-Civita](https://en.wikipedia.org/wiki/Levi-Civita_symbol) symbol: `1` if `(i, j, k)` form an [even permutation](https://en.wikipedia.org/wiki/Parity_of_a_permutation), `-1` if `(i, j, k)` form an odd permutation, and `0` if any the adjacent letters are equal.

### Degrees and radians

> ```cpp
> template <class Float> constexpr Float deg_to_rad(Float degrees) noexcept;
> template <class Float> constexpr Float rad_to_deg(Float radians) noexcept;
> ```

Converts degrees to radians and back.

### Sequence operations

> ```cpp
> template <class Idx, class Func> constexpr auto  sum(Idx low, Idx high, Func&& func);
> template <class Idx, class Func> constexpr auto prod(Idx low, Idx high, Func&& func);
> ```

Computes $\sum_{i = low}^{high} func(i)$ or $\prod_{i = low}^{high} func(i)$.

### Indexation

> ```cpp
> template <class Container> auto ssize(const Container& container);
> ```

Returns `container.size()` using a corresponding signed type.

Equivalent to C++20 [`std::ssize()`](https://en.cppreference.com/w/cpp/iterator/size).

> ```cpp
> template <class T> constexpr T reverse_idx(T idx, T size) noexcept;
> ```

Returns `size - 1 - idx` which corresponds to a reverse index `idx`.

Useful for reversing indexation, especially when working with unsigned indices.

### Permutations

```cpp
template <class Array>
bool is_permutation(const Array& array);
```

Returns whether `array` contains an index permutation.

**Note:** Here an "index permutation" means any permutation of `{0 ... array.size() - 1}`.

```cpp
template <class Array, class Permutation = std::initializer_list<std::size_t>>
void apply_permutation(Array& array, const Permutation& permutation);
```

Applies index permutation to an `array`.

```cpp
template <class Array, class Cmp = std::less<>>
std::vector<std::size_t> sorting_permutation(const Array& array, Cmp cmp = Cmp());
```

Returns index permutation which would make `array` sorted relative to the comparator `cmp`. By default `cmp` corresponds to a regular `<` comparison.

```cpp
template <class Array, class... SyncedArrays>
void sort_together(Array& array, SyncedArrays&... synced_arrays);
```

Sorts `array` and applies the same sorting permutation to all `synced_arrays...`.

**Note:** In layman's terms it sorts two (or more) arrays based on one.

### Branchless ternary

> ```cpp
> template <class T>
> T      ternary_branchless(bool condition,       T return_if_true,       T return_if_false) noexcept;
> 
> template <class Integer>
> Integer ternary_bitselect(bool condition, Integer return_if_true, Integer return_if_false) noexcept;
> 
> template <class Integer>
> Integer ternary_bitselect(bool condition, Integer return_if_true  /* returns 0 if false*/) noexcept;
> ```

Computes `condition ? return_if_true : return_if_false` branchlessly using properties of arithmetic types. Overloads **(2)** and **(3)** use a more efficient algorithm suitable for integer types. Overload **(3)** simplifies it even more using the `return_if_false == 0` assumption.

**Note:** This is mostly needed for architectures with expensive branches, such as most GPUs. In usual context a regular ternary should be preferred.

### Meshing

> ```cpp
> struct Points {
>        std::size_t count;
>        constexpr explicit Points(std::size_t count);
> };
> 
> struct Intervals {
>        std::size_t count;
>        constexpr explicit Intervals(std::size_t count );
>        constexpr          Intervals(Points      points);
> };
> ```

"Strong typedefs" for grid size in 1D meshing operations, which allows caller to express their intent more clearly.

`Points` implicitly converts to `Intervals` ($N + 1$ points corresponds to $N$ intervals), allowing most meshing functions to accept both types as an input.

> ```cpp
> template <class Float> std::vector<Float>  linspace(Float L1, Float L2, Intervals N);
> template <class Float> std::vector<Float> chebspace(Float L1, Float L2, Intervals N);
> ```

Function **(1)** meshes $[L_1, L_2]$ range into a regular 1D grid with $N$ intervals (which corresponds to $N + 1$ grid points). Similar to `linspace()` from [Matlab](https://www.mathworks.com/products/matlab.html) and [numpy](https://numpy.org). Function **(2)** does the same using a [Chebyshev grid](https://en.wikipedia.org/wiki/Chebyshev_nodes) instead of a regular one.

> ```cpp
> template <class Float, class Func>
> Float integrate_trapezoidal(Func&& f, Float L1, Float L2, Intervals N);
> ```

Numerically computes integral $I_h = \int\limits_{L_1}^{L_2} f(x) dx$ over $N$ integration intervals using [trapezoidal rule](https://en.wikipedia.org/wiki/Trapezoidal_rule).

### Memory usage

> ```cpp
> enum class MemoryUnit { BYTE, KiB, MiB, GiB, TiB, KB, MB, GB, TB };
> ```

Enum listing all memory units, see [multi-byte units in SI](https://en.wikipedia.org/wiki/Byte#Multiple-byte_units).

> ```cpp
> template <MemoryUnit units = MemoryUnit::MiB>
> constexpr double to_memory_units(std::size_t bytes) noexcept;
> ```

Converts number of `bytes` to given memory `units`.

> ```cpp
> template <MemoryUnit units = MemoryUnit::MiB, class T>
> constexpr double quick_memory_estimate(const T& value);
> ```

Returns a quick memory usage estimate for `value` using its stack size, `size()`, `capacity()`, `std::tuple_size_v<>` and etc.

Does not expand over the nested containers. This means the estimate has $O(1)$ complexity, but may be inaccurate if `T` is a container of non-POD elements. 

Useful to estimate memory usage of arrays, matrices and other data structures in a human-readable way.

**Note 1:** It is impossible to accurately measure memory usage of something without knowing its implementation or providing a custom allocator that gathers statistics. It is however possible to get a rough estimate which often helps identify the general scale of data.

**Note 2:** Contiguous containers (such as arrays, strings and vectors) are likely to have a perfectly accurate estimate, associative containers (such as maps and sets) and linked containers (such lists, queues and etc.) are likely to be underestimated.

## Examples

### Using basic math functions

[ [Run this code]() ]

```cpp
TODO:
```

Output:
```
```

### Using indicator functions

[ [Run this code]() ]
```cpp
TODO:
```

Output:
```
```

### Summation & integration

[ [Run this code]() ]
```cpp
TODO:
```

Output:
```
```

### Reversing loop indexation

[ [Run this code]() ]
```cpp
TODO:
```

Output:
```
```

### Sorting arrays together

[ [Run this code]() ]
```cpp
TODO:
```

Output:
```
```

### Meshing and integrating

[ [Run this code]() ]

```cpp
TEMP:

// Mesh interval [0, PI] into 100 equal intervals => 101 linearly spaced points 
auto grid_1 = math::linspace(0., math::PI, math::Intervals(100));
auto grid_2 = math::linspace(0., math::PI, math::Points(   101)); // same as above

// Get array memory size
std::cout << "'grid_1' occupies " << math::memory_size<double, math::MemoryUnit::KB>(grid_1.size()) << " KB in memory\n\n";

// Integrate a function over an interval
auto f = [](double x){ return 4. / (1. + std::tan(x)); };
double integral = math::integrate_trapezoidal(f, 0., math::PI_HALF, math::Intervals(200));
std::cout << "Integral evaluates to: " << integral << " (should be ~PI)\n";
```

Output:
```
'grid_1' occupies 0.808 KB in memory

Integral evaluates to: 3.14159 (should be ~PI)
```

### Meshing & memory usage

[ [Run this code]() ]
```cpp
TODO:
```

Output:
```
```