#pragma once

#include "eclipse/log.h"
#include "eclipse/core.h"
#include <vector>
#include <string>

namespace eclipse {

enum class ShaderDataType : uint8_t {
	none,
	floatvec1,
	floatvec2,
	floatvec3,
	floatvec4,
	floatmat3,
	floatmat4,
	intvec1,
	intvec2,
	intvec3,
	intvec4,
	boolean,
};

static uint32_t ShaderDataTypeSize(ShaderDataType type) {
	using enum ShaderDataType;
	static const uint32_t float_size = 4;
	switch (type) {
		case floatvec1:
			return float_size;
		case floatvec2:
			return float_size * 2;
		case floatvec3:
			return float_size * 3;
		case floatvec4:
			return float_size * 4;
		case floatmat3:
			return float_size * 3 * 3;
		case floatmat4:
			return float_size * 4 * 4;
		case intvec1:
			return float_size;
		case intvec2:
			return float_size * 2;
		case intvec3:
			return float_size * 3;
		case intvec4:
			return float_size * 4;
		case boolean:
			return 1;
	}

	EC_CORE_ASSERT(false, "Unknown shader data type!");
	return 0;
}

struct BufferElement {
	BufferElement(ShaderDataType type, const std::string& name) : type(type), name(name), size(ShaderDataTypeSize(type)) {}

	uint32_t get_component_count() const {
		using enum ShaderDataType;
		switch (type) {
			case floatvec1:
				return 1;
			case floatvec2:
				return 2;
			case floatvec3:
				return 3;
			case floatvec4:
				return 4;
			case floatmat3:
				return 3 * 3;
			case floatmat4:
				return 4 * 4;
			case intvec1:
				return 1;
			case intvec2:
				return 2;
			case intvec3:
				return 3;
			case intvec4:
				return 4;
			case boolean:
				return 1;
		}
		EC_CORE_ASSERT(false, "Unknown shader data type!");
		return 0;
	}

	ShaderDataType type {ShaderDataType::none};
	std::string name {};
	uint32_t size {0};
	uint32_t offset {0};
	bool normalized {false};
};

class ECLIPSE_API BufferLayout {
public:
	BufferLayout() = default;

	BufferLayout(const std::initializer_list<BufferElement>& elements) : elements_(elements) {
		calculate_offset_and_stride();
	}

	inline const std::vector<BufferElement>& get_elements() const { return elements_; }

	inline uint32_t get_stride() const { return stride_; }

	std::vector<BufferElement>::iterator begin() { return elements_.begin(); }

	std::vector<BufferElement>::iterator end() { return elements_.end(); }

	std::vector<BufferElement>::const_iterator begin() const { return elements_.begin(); }

	std::vector<BufferElement>::const_iterator end() const { return elements_.end(); }

private:
	void calculate_offset_and_stride() {
		uint32_t offset = 0;
		for (auto& element : elements_) {
			element.offset = offset;
			offset += element.size;
			stride_ += element.size;
		}
	}

	std::vector<BufferElement> elements_;
	uint32_t stride_ = 0;
};

}  // namespace eclipse