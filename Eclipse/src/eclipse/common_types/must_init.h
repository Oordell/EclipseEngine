#pragma once

namespace eclipse {

template <typename T>
class MustInit {
public:
	MustInit(T t) : value_(t) {}

	operator T&() { return value_; }

	operator const T&() const { return value_; }

private:
	T value_;
};

}  // namespace eclipse