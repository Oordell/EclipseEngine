#pragma once

#include "eclipse/Layer.h"

namespace eclipse {

	class ECLIPSE_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void on_attach() override;
		void on_detach() override;
		void on_update() override;
		void on_event(Event& event) override;

	private:
		float time_ = 0.0F;
	};

}