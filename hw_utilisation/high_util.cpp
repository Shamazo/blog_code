#include <iostream>
#include <string>
#include <sched.h>
#include "PerfEvent.hpp"

#define ASM_MULTIPLY(in1, sum1, in2, sum2, max, counter)                 \
  __asm volatile(                                                        \
      "1:\n"                                                             \
      "add (%[IN1]), %[SUM1]\n"                                          \
      "cmp %[MAX], %[SUM1]\n"                                            \
      "jae 2f\n"                                                         \
      "add (%[IN2]), %[SUM2]\n"                                          \
      "dec %[COUNTER]\n"                                                 \
      "jnz 1b\n"                                                         \
      "2:"                                                               \
      : [SUM1] "+&r"(sum1), [SUM2] "+&r"(sum2), [COUNTER] "+&r"(counter) \
      : [IN1] "r"(in1), [IN2] "r"(in2), [MAX] "r"(max))

// #define ASM_MULTIPLY2(in1, sum1, in2, sum2, max, counter, tmp1, tmp2)                         \
//   __asm volatile(                                                                             \
//       "1:\n"                                                                                  \
//       "mov (%[IN1]), %[TMP1]\n"                                                               \
//       "add %[TMP1], %[SUM1]\n"                                                                \
//       "cmp %[MAX], %[SUM1]\n"                                                                 \
//       "jae 2f\n"                                                                              \
//       "mov (%[IN2]), %[TMP2]\n"                                                               \
//       "add %[TMP2], %[SUM2]\n"                                                                \
//       "dec %[COUNTER]\n"                                                                      \
//       "jnz 1b\n"                                                                              \
//       "2:"                                                                                    \
//       : [SUM1] "+&r"(sum1), [SUM2] "+&r"(sum2), [COUNTER] "+&r"(counter), [TMP1] "=&r"(tmp1), \
//         [TMP2] "=&r"(tmp2)                                                                    \
//       : [IN1] "r"(in1), [IN2] "r"(in2), [MAX] "r"(max))

int main(int argc, char* argv[]) {
  // Set CPU affinity to core #1 so we can record perf counters for just this core
  cpu_set_t cpuSet;
  CPU_ZERO(&cpuSet);
  CPU_SET(1, &cpuSet);

  if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuSet) == -1) {
    std::cerr << "Error: Failed to set CPU affinity" << std::endl;
    return 1;
  }
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <integer1> <integer2>" << std::endl;
    return 1;
  }
  PerfEvent e;

  auto start_time = std::chrono::high_resolution_clock::now();
  e.startCounters();
  std::size_t pos;
  const uint64_t num1 = std::stoll(argv[1], &pos);
  const uint64_t num2 = std::stoll(argv[2], &pos);

  uint64_t sum1, sum2;
  sum1 = sum2 = 0;

  uint64_t in1 = num1;
  constexpr uint64_t in2 = 1;
  constexpr uint64_t max = std::numeric_limits<uint64_t>::max();
  uint64_t counter = num2;
  ASM_MULTIPLY(&in1, sum1, &in2, sum2, max, counter);
  // ASM_MULTIPLY2(&in1, sum1, &in2, sum2, max, counter, tmp1, tmp2);

  if (counter != 0) {
    std::cerr << "multiplication overflowed" << std::endl;
    return 1;
  }

  printf("%ld * %ld = %ld\n", num1, num2, sum1);
  e.stopCounters();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
  std::cout << "Execution time: " << duration << " us" << std::endl;
  e.printReport(std::cout, 1);

  return 0;
}