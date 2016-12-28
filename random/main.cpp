#include <iostream>
#include <random>
#include <chrono>
#include <stdlib.h>

int main(int argc, char **argv)
{
  char* endptr;
  double mean = strtod(argv[1], &endptr);
  double sigma = strtod(argv[2], &endptr);

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  std::normal_distribution<double> distribution(mean, sigma);

  double number = distribution(generator);
  printf("%f", number);

  return 0;
}