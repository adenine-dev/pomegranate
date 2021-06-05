#pragma once

#include <map>
#include <memory>
#include <string>

namespace pom::platform {

    /// @addtogroup platform
    /// @{

    /// Opens a platform based error window, separate from all other state. Useful for throwing
    /// fatal errors in production, when that is needed.
    void openErrorWindow(const char* title, const char* body);

    class SharedObject {
    public:
        SharedObject(const char* soFilename);
        ~SharedObject();

        std::shared_ptr<void*> getFunction(const char* function);

        void reload();

    private:
        std::string filename;
        void* handle;
        std::map<const char*, std::shared_ptr<void*>> functions;
    };

    /// @}

} // namespace pom::platform