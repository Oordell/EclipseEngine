#include "ecpch.h"
#include "eclipse/utils/random.h"

namespace eclipse::utils {

std::mt19937 Random::random_engine_;
std::uniform_int_distribution<std::mt19937::result_type> Random::distribution_;

}  // namespace eclipse::utils