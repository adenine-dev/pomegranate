#pragma once

#include "base.hpp"

#include "typesVk.hpp"

#define POM_CHECK_VK(expr, message)                                                                                    \
    {                                                                                                                  \
        VkResult res = expr;                                                                                           \
        if (res != VK_SUCCESS) {                                                                                       \
            POM_FATAL(message, "; failed with error ", ::pom::gfx::fromVkResultToString(res));                         \
        }                                                                                                              \
    }