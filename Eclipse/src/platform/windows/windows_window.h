#pragma once

#include "eclipse/window.h"

#include <GLFW/glfw3.h>

namespace eclipse {

    class WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow();

        void on_update() override;

        inline unsigned int get_width() const override { return data_.props.window_size.width; }
        inline unsigned int get_height() const override { return data_.props.window_size.height; }

        inline void set_event_callback(const EventCallbackfn& callback) override { data_.event_callback = callback; }
        void set_v_sync(bool enabled) override;
        bool is_v_sync() const override;

        inline void* get_native_window() const override { return window_; }

    private:
        virtual void init(const WindowProps& props);
        virtual void shutdown();
        void set_glfw_callbacks();

        GLFWwindow* window_;

        struct WindowData {
            WindowProps props{};
            bool v_sync{false};

            EventCallbackfn event_callback;
        };

        WindowData data_;
    };


}