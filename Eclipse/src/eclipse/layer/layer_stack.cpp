#include "ecpch.h"
#include "layer_stack.h"

namespace eclipse {

LayerStack::LayerStack() {}

LayerStack::~LayerStack() {
	for (auto* layer : layers_) {
		layer->on_detach();
		delete layer;
	}
}

void LayerStack::push_layer(Layer* layer) {
	layers_.emplace(layers_.begin() + layer_insert_index_, layer);
	layer_insert_index_++;
}

void LayerStack::push_overlay(Layer* overlay) { layers_.emplace_back(overlay); }

void LayerStack::pop_layer(Layer* layer) {
	auto it = std::find(layers_.begin(), layers_.end(), layer);
	if (it != layers_.end()) {
		layers_.erase(it);
		if (layer_insert_index_ > 0) {
			layer_insert_index_--;
		} else {
			EC_CORE_ERROR(
			    "Tried to decrement layer index, but it's already 0! "
			    "Layer size: {0}, Insert index: {1}",
			    layers_.size(), layer_insert_index_);
		}
	}
}

void LayerStack::pop_overlay(Layer* overlay) {
	auto it = std::find(layers_.begin(), layers_.end(), overlay);
	if (it != layers_.end()) {
		layers_.erase(it);
	}
}
}  // namespace eclipse