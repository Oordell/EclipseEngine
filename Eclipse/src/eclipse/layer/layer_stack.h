#pragma once

#include "eclipse/core/core.h"
#include "layer.h"

#include <vector>

namespace eclipse {

class ECLIPSE_API LayerStack {
public:
	LayerStack();
	~LayerStack();

	void push_layer(Layer* layer);
	void push_overlay(Layer* overlay);
	void pop_layer(Layer* layer);
	void pop_overlay(Layer* overlay);

	std::vector<Layer*>::iterator begin() { return layers_.begin(); }

	std::vector<Layer*>::iterator end() { return layers_.end(); }

private:
	std::vector<Layer*> layers_;
	unsigned int layer_insert_index_ = 0;
};

}  // namespace eclipse