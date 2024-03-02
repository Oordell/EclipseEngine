#pragma once

#include "eclipse/core/core.h"
#include "eclipse/events/event.h"

#include <au.hh>

namespace eclipse {

class ECLIPSE_API Layer {
public:
	Layer(const std::string& debug_name = "Default Layer");
	virtual ~Layer() = default;

	virtual void on_attach() {}

	virtual void on_detach() {}

	virtual void on_update(au::QuantityF<au::Seconds> timestep) {}

	virtual void on_event(Event& event) {}

	virtual void on_imgui_render() {}

	inline const std::string& get_name() const { return debug_name_; }

protected:
	std::string debug_name_;
};
}  // namespace eclipse