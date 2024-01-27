#include "ecpch.h"
#include "eclipse/utils/math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace eclipse::utils {

std::optional<Transform> decompose_transform(const glm::mat4& raw_transform) {
	// From glm::decompose in matrix_decompose.inl
	using namespace glm;
	using T = float;

	mat4 Local_matrix(raw_transform);

	// Normalize the matrix.
	if (epsilonEqual(Local_matrix[3][3], static_cast<float>(0), epsilon<T>())) {
		return std::nullopt;
	}

	// First, isolate perspective. This is the messiest.
	if (epsilonNotEqual(Local_matrix[0][3], static_cast<T>(0), epsilon<T>()) ||
	    epsilonNotEqual(Local_matrix[1][3], static_cast<T>(0), epsilon<T>()) ||
	    epsilonNotEqual(Local_matrix[2][3], static_cast<T>(0), epsilon<T>())) {
		// Clear the perspective partition
		Local_matrix[0][3] = static_cast<T>(0);
		Local_matrix[1][3] = static_cast<T>(0);
		Local_matrix[2][3] = static_cast<T>(0);
		Local_matrix[3][3] = static_cast<T>(1);
	}

	// Next take care of translation (easy).
	Transform result {};
	result.translation = vec3(Local_matrix[3]);
	Local_matrix[3]    = vec4(0, 0, 0, Local_matrix[3].w);

	static constexpr uint32 NUM_OF_ROWS = 3;
	vec3 row[NUM_OF_ROWS]               = {{}, {}, {}};
	vec3 p_dum3 {};

	// Now get scale and shear.
	for (length_t i = 0; i < NUM_OF_ROWS; ++i) {
		for (length_t j = 0; j < NUM_OF_ROWS; ++j) {
			row[i][j] = Local_matrix[i][j];
		}
	}

	// Compute X scale factor and normalize first row.
	result.scale.x = length(row[0]);
	row[0]         = detail::scale(row[0], static_cast<T>(1));
	result.scale.y = length(row[1]);
	row[1]         = detail::scale(row[1], static_cast<T>(1));
	result.scale.z = length(row[2]);
	row[2]         = detail::scale(row[2], static_cast<T>(1));

	// At this point, the matrix (in rows[]) is orthonormal. Check for a coordinate system flip.  If the determinant is -1,
	// then negate the matrix and the scaling factors.
#if 0
	p_dum3 = cross(row[1], row[2]);  // v3Cross(row[1], row[2], p_dum3);
	if (dot(row[0], p_dum3) < 0) {
		for (length_t i = 0; i < 3; i++) {
			result.scale[i] *= static_cast<T>(-1);
			row[i] *= static_cast<T>(-1);
		}
	}
#endif

	result.rotation.y = std::asin(-row[0][2]);
	if (std::cos(result.rotation.y) != 0) {
		result.rotation.x = std::atan2(row[1][2], row[2][2]);
		result.rotation.z = std::atan2(row[0][1], row[0][0]);
	} else {
		result.rotation.x = std::atan2(-row[2][0], row[1][1]);
		result.rotation.z = 0;
	}

	return result;
}

}  // namespace eclipse::utils