// _______________ TEST FRAMEWORK & MODULE  _______________

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "thirdparty/doctest.h"

#include "test.hpp"

#include "UTL/bit.hpp"

// _______________________ INCLUDES _______________________

#include <cstdint> // testing bit operation on small fixed size ints

// ____________________ DEVELOPER DOCS ____________________

// Nothing fancy here, just trying all operations on 8-bit integers and checking 
// that results correspond to analytical ones.

// ____________________ IMPLEMENTATION ____________________

// ===========================
// --- Bit operation tests ---
// ===========================

TEST_CASE("Individual bit operations behave as expected") {
    constexpr std::uint8_t x = 19; // 19 ~ 00010011
    // human-readable notation is big-endian, which means bits are indexed right-to-left
    
    // Read bits
    static_assert(bit::get(x, 0) == 1);
    static_assert(bit::get(x, 1) == 1);
    static_assert(bit::get(x, 2) == 0);
    static_assert(bit::get(x, 3) == 0);
    static_assert(bit::get(x, 4) == 1);
    static_assert(bit::get(x, 5) == 0);
    static_assert(bit::get(x, 6) == 0);
    static_assert(bit::get(x, 7) == 0);
    
    // Modify bits
    static_assert(bit::set(  x, 2) == 23); // 23 ~ 00010111
    static_assert(bit::clear(x, 0) == 18); // 18 ~ 00010010
    static_assert(bit::flip( x, 1) == 17); // 17 ~ 00010001
}

TEST_CASE("Group bit operations behave as expected") {
    constexpr std::uint8_t x = 19; // 19 ~ 00010011
    
    // Group bit operations
    static_assert(bit::rotl(  x, 6) == 196); // 196 ~ 11000100
    static_assert(bit::rotr(  x, 1) == 137); // 137 ~ 10001001
    static_assert(bit::lshift(x, 6) == 192); // 192 ~ 11000000
    static_assert(bit::rshift(x, 1) ==   9); //   9 ~ 00001001
}

TEST_CASE("Bit utils behave as expected") {
    constexpr std::uint8_t x = 19; // 19 ~ 00010011
    
    static_assert(bit::width(x) == 5); // 00010011 has 5 significant bits
    
    static_assert(bit::size_of<std::uint16_t> == 16);
    static_assert(bit::size_of<std::uint32_t> == 32);
    static_assert(bit::size_of<std::uint64_t> == 64);
}

// =====================
// --- Bitflag tests ---
// =====================

enum class IOMode { IN = 1 << 0, OUT = 1 << 1, APP = 1 << 2 };

TEST_CASE("Bitflag creation behaves as expected") {
    constexpr auto flags_1 = bit::Flags{IOMode::OUT, IOMode::APP};
    constexpr auto flags_2 = bit::Flags(IOMode::OUT) | bit::Flags(IOMode::APP);
    constexpr auto flags_3 = bit::Flags(IOMode::OUT) | IOMode::APP;
    constexpr auto flags_4 = bit::Flags(IOMode::OUT).add(IOMode::APP);
    constexpr auto flags_5 = bit::Flags<IOMode>{}.add(IOMode::OUT).add(IOMode::APP);
    
    static_assert(flags_1 == flags_2 && flags_2 == flags_3 && flags_3 == flags_4 && flags_4 == flags_5);
}

TEST_CASE("Bitflag method chaining behaves as expected") {
    auto flags = bit::Flags<IOMode>{};
    
    flags.remove(IOMode::APP);
    flags.add(bit::Flags{IOMode::OUT, IOMode::APP});
    
    CHECK(flags.contains(bit::Flags{IOMode::OUT, IOMode::APP}));
    
    CHECK(flags.contains(IOMode::OUT));
    CHECK(flags.contains(IOMode::APP));
    
    flags.remove(IOMode::APP);
    
    CHECK(flags.contains(IOMode::OUT));
    CHECK(!flags.contains(IOMode::APP));
}