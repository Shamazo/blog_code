#include <iostream>
#include <string>
#include <sched.h>
#include "PerfEvent.hpp"

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
  uint64_t num1 = std::stoll(argv[1], &pos);
  uint64_t num2 = std::stoll(argv[2], &pos);
  if (num1 > std::numeric_limits<uint64_t>::max() / num2) {
    std::cerr << "Error: multiplication would result in overflow" << std::endl;
    return 1;
  }
  uint64_t result = num1 * num2;

  printf("%ld * %ld = %ld\n", num1, num2, result);
  e.stopCounters();
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
  std::cout << "Execution time: " << duration << " us" << std::endl;
  e.printReport(std::cout, 1);

  return 0;
}