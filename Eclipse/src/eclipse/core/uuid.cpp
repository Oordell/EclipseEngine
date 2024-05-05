#include <ecpch.h>
#include "uuid.h"

#include <random>

namespace eclipse {

static std::random_device random_device_;
static std::mt19937_64 engine_(random_device_());
static std::uniform_int_distribution<uint64_t> uniform_distribution_;

UUID::UUID() : uuid_(uniform_distribution_(engine_)) {}

}  // namespace eclipse