#include "pch.hpp"

#include "platform.hpp"

#include "core/application.hpp"

#include "terminal.hpp"

namespace pom::platform {
    static char* userPath = nullptr;

    bool init()
    {
        POM_PROFILE_FUNCTION();
        if (SDL_Init(SDL_INIT_VIDEO)) {
            POM_ERROR("Unable to initialize SDL. error: ", SDL_GetError());
            return false;
        }

        if (!terminal::init_terminal()) {
            POM_ERROR("Unable to initialize terminal");
            return false;
        }

        return true;
    }

    void openErrorWindow(const char* title, const char* body)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, body, nullptr);
    }

    void sleep(u32 ms)
    {
        SDL_Delay(ms);
    }

    std::string getUserPath()
    {
        if (!userPath) {
            POM_ASSERT(Application::get(), "Application must be initialized before requesting user path.");
            userPath = SDL_GetPrefPath(Application::getAppCreateInfo().name, Application::getAppCreateInfo().name);
        }

        return userPath;
    }

    SharedObject::SharedObject(std::string soFilename) :
        filename(std::move(soFilename)), loadedFilename(filename + ".temp"), handle(nullptr)
    {
        POM_PROFILE_FUNCTION();
        reload(); // SDL_UnloadObject checks for nullptrs so this is fine.
    }

    SharedObject::SharedObject(SharedObject&& o) noexcept :
        filename(std::move(o.filename)), loadedFilename(std::move(o.loadedFilename)), handle(nullptr),
        functions(std::move(o.functions))
    {
        POM_PROFILE_FUNCTION();
        std::swap(handle, o.handle);
    }

    SharedObject& SharedObject::operator=(SharedObject&& o) noexcept
    {
        POM_PROFILE_FUNCTION();
        filename = std::move(o.filename);
        loadedFilename = std::move(o.loadedFilename);
        handle = o.handle;
        functions = std::move(o.functions);
        return *this;
    }

    SharedObject::~SharedObject()
    {
        POM_PROFILE_FUNCTION();
        SDL_UnloadObject(handle);
        for (auto [name, funcptr] : functions) {
            *funcptr = nullptr;
        }

        if (std::filesystem::exists(loadedFilename)) {
            std::filesystem::remove(loadedFilename);
        }
    }

    void SharedObject::reload()
    {
        POM_PROFILE_FUNCTION();
        SDL_UnloadObject(handle);

// mingw currently has a bug that prevents `std::filesystem::copy_options` from working
#ifdef __MINGW32__
        if (std::filesystem::exists(loadedFilename)) {
            std::filesystem::remove(loadedFilename);
        }
#endif
        // TODO: in release mode this should be disabled in some way because release code shouldn't be hot reloaded,
        // thus there is no need to duplicate it for writing.
        std::filesystem::copy_file(filename, loadedFilename, std::filesystem::copy_options::overwrite_existing);

        handle = SDL_LoadObject(loadedFilename.c_str());
        POM_ASSERT(handle, "unable to load shared object file '", filename, "': ", SDL_GetError());

        for (auto [name, funcptr] : functions) {
            *funcptr = SDL_LoadFunction(handle, name.c_str());
        }
    }

    void SharedObject::setSOFilename(std::string SOFile)
    {
        POM_PROFILE_FUNCTION();
        SDL_UnloadObject(handle);
        if (std::filesystem::exists(loadedFilename)) {
            std::filesystem::remove(loadedFilename);
        }

        filename = std::move(SOFile);
        reload();
    }

} // namespace pom::platform