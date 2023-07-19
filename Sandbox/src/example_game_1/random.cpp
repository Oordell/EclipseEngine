#include "random.h"

std::mt19937 Random::random_engine_;
std::uniform_int_distribution<std::mt19937::result_type> Random::distribution_;