// __________ BENCHMARK FRAMEWORK & LIBRARY  __________

#include "benchmark.hpp"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>


// _____________ BENCHMARK IMPLEMENTATION _____________

// =========================
// --- BigUint benchmark ---
// =========================

void benchmark_big_uint() {
    using namespace utl;

    constexpr std::size_t bits = 256;

    using BigUint = integral::BigUint<bits>;

    const auto big_uint_name = "BigUint<" + std::to_string(bits) + ">";

    bench.timeUnit(1ns, "ns").minEpochIterations(1'000'000);

    // Pregenerate random big uints so we don't accidentally benchmark their initialization instead,
    // also generate 2 sets of data so we can do substraction without overflow
    constexpr std::size_t pregen_size = 200;
    constexpr auto pregen_high = std::numeric_limits<int>::max();
    constexpr auto pregen_mid = std::numeric_limits<int>::max() / 2;
    constexpr auto pregen_low = 0;
    
    std::vector<BigUint>  pregen_data_high(pregen_size);
    std::vector<BigUint>  pregen_data_low(pregen_size);
    for (auto& e : pregen_data_high) e = BigUint(random::rand_uint(pregen_mid, pregen_high));
    for (auto& e : pregen_data_low) e = BigUint(random::rand_uint(pregen_low, pregen_mid));

    const auto rand_big_uint_high = [&]() { return pregen_data_high[random::rand_uint(0, pregen_size - 1)]; };
    const auto rand_big_uint_low = [&]() { return pregen_data_low[random::rand_uint(0, pregen_size - 1)]; };

    // --- Arithmetic operations ---
    // -----------------------------

    bench.title("Arithmetic operations").warmup(10).relative(true);

    benchmark(big_uint_name + ": x + y", [&]() {
        auto res = rand_big_uint_high() + rand_big_uint_high();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark(big_uint_name + ": x - y", [&]() {
        auto res = rand_big_uint_high() - rand_big_uint_low();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark(big_uint_name + ": x * y", [&]() {
        auto res = rand_big_uint_high() * rand_big_uint_high();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark(big_uint_name + ": x / y", [&]() {
        auto res = rand_big_uint_high() / rand_big_uint_low();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark(big_uint_name + ": ++x", [&]() {
        auto res = ++rand_big_uint_high();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark(big_uint_name + ": --x", [&]() {
        auto res = --rand_big_uint_high();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark("std::uint64_t: x + y", [&]() {
        auto res = datagen::rand_uint() + datagen::rand_uint();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark("std::uint64_t: x - y", [&]() {
        auto res = datagen::rand_uint() - datagen::rand_uint();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark("std::uint64_t: x * y", [&]() {
        auto res = datagen::rand_uint() * datagen::rand_uint();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark("std::uint64_t: x / y", [&]() {
        auto res = datagen::rand_uint() / datagen::rand_uint();
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark("std::uint64_t: ++x", [&]() {
        auto res = datagen::rand_uint();
        ++res;
        DO_NOT_OPTIMIZE_AWAY(res);
    });
    
    benchmark("std::uint64_t: --x", [&]() {
        auto res = datagen::rand_uint();
        --res;
        DO_NOT_OPTIMIZE_AWAY(res);
    });
}


int main() {
    using namespace utl;

    benchmark_big_uint();
}