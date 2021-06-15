#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace pom::platform {

    /// @addtogroup platform
    /// @{

    /// Opens a platform based error window, separate from all other state. Useful for throwing
    /// fatal errors in production, when that is needed.
    void openErrorWindow(const char* title, const char* body);

    /// Reference to a shared object file (.dll or .so depending on platform) that holds all functions loaded by it.
    /// This code is *extremely* not threadsafe.
    /// @warning Do not attempt to load the same shared object file more than once, it will not work.
    class SharedObject {
    public:
        /// Utility class to help in reloading and make syntax easier. This overloads the `operator()` function so that
        /// it can be called in the same way as any other function pointer while still being ref-counted.
        template <typename F> class SOFunction {
        public:
            template <typename... Args> auto operator()(Args... args)
            {
                return ((F)(*(funcptrptr.get())))(std::forward<Args>(args)...);
            }

            /// Returns true if the function is valid, it should only be invalid when the parent @ref
            /// SharedObject is deleted, or if the SharedObject it was created from does not hold the function.
            [[nodiscard]] bool valid()
            {
                return funcptrptr && *funcptrptr;
            }

        private:
            friend class SharedObject;
            SOFunction(std::shared_ptr<void*> func) : funcptrptr(std::move(func))
            {
            }

            std::shared_ptr<void*> funcptrptr;
        };

        /// Creation of this object will copy a shared object file on disk to `{soFilename}.temp`, on destruction it
        /// will attempt to delete the file.
        /// Throws an std::filesystem::filesystem_error exception on file error.
        SharedObject(std::string soFilename);

        SharedObject(SharedObject&& o) noexcept;

        SharedObject& operator=(SharedObject&& o) noexcept;

        ~SharedObject();

        /// Loads and returns a function inside the loaded shared object. Returns a `SOFunction` which will
        /// automatically update its internals when `reload` is called.
        /// checking `SOFunction.valid` is a good decision.
        /// @tparam F the function type.
        template <typename F> SOFunction<F> getFunction(std::string function)
        {
            if (functions.contains(function)) {
                return { functions.find(function)->second };
            }
            void* func = SDL_LoadFunction(handle, function.c_str());
            if (func) {
                std::shared_ptr<void*> f = std::make_shared<void*>(func);
                return { functions.insert({ std::move(function), f }).first->second };
            }
            return { nullptr };
        }

        /// Unloads and reloads the shared object and reloads all associated functions.
        /// Throws an std::filesystem::filesystem_error exception on file error. After running this function its a good
        /// idea to recheck `SOFunction.valid`
        void reload();

    private:
        POM_NOCOPY(SharedObject);

        std::string filename;
        std::string loadedFilename;

        void* handle;
        std::unordered_map<std::string, std::shared_ptr<void*>> functions;
    };

    /// @}

} // namespace pom::platform