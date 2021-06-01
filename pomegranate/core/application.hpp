#pragma once

#include "base.hpp"

namespace pom {
    class Application {
    public:
        virtual ~Application();

        // This is a singleton, so copying is not good.
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        virtual void update(float dt) = 0;

        [[nodiscard]] inline u64 getFrame() const
        {
            return frame;
        }

    protected:
        Application();

        u64 frame;

        static Application* instance;
    };
} // namespace pom