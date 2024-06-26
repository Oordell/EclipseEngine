#pragma once

namespace eclipse {

class UUID {
public:
	UUID();
	UUID(const UUID&)                   = default;
	UUID(UUID&&)                        = default;
	UUID& operator=(const UUID&)        = default;
	UUID& operator=(UUID&&)             = default;
	~UUID()                             = default;
	auto operator<=>(const UUID&) const = default;

	explicit UUID(uint64_t id) : uuid_(id) {}

	[[nodiscard]] uint64_t value() const { return uuid_; }

private:
	uint64_t uuid_;
};

}  // namespace eclipse

namespace std {

template <>
struct hash<eclipse::UUID> {
	std::size_t operator()(const eclipse::UUID& uuid) const { return static_cast<std::size_t>(uuid.value()); }
};

}  // namespace std