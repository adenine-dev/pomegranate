#pragma once

#include "base.hpp"

namespace pom {
    class Application {
    public:
        virtual ~Application();

        virtual void update(float dt) = 0;

        [[nodiscard]] inline u64 getFrame() const
        {
            return frame;
        }

    protected:
        Application();

        POM_NOCOPY(Application)
        POM_NOMOVE(Application)

        u64 frame;

        static Application* instance;
    };
} // namespace pom