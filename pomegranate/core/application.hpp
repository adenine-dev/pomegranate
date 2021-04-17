#pragma once

#include "base.hpp"

namespace pom {
    class Application {
    public:
        virtual ~Application();

        virtual void update(float dt) = 0;

        inline u64 getFrame() const
        {
            return frame;
        }

    protected:
        Application();

    protected:
        u64 frame;

        static Application* instance;
    };
} // namespace pom