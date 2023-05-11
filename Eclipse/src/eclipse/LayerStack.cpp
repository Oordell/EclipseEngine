#include "ecpch.h"
#include "LayerStack.h"

namespace eclipse {

	LayerStack::LayerStack() {
		layer_insert_ = layers_.begin();
	}

	LayerStack::~LayerStack() {
		for (auto* layer : layers_) {
			delete layer;
		}
	}

	void LayerStack::push_layer(Layer* layer) {
		layer_insert_ = layers_.emplace(layer_insert_, layer);
	}
	
	void LayerStack::push_overlay(Layer* overlay) {
		layers_.emplace_back(overlay);
	}
	
	void LayerStack::pop_layer(Layer* layer) {
		auto it = std::find(layers_.begin(), layers_.end(), layer);
		if (it != layers_.end()) {
			layers_.erase(it);
			layer_insert_--;
		}
	}
	
	void LayerStack::pop_overlay(Layer* overlay) {
		auto it = std::find(layers_.begin(), layers_.end(), overlay);
		if (it != layers_.end()) {
			layers_.erase(it);
		}
	}
}