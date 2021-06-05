#include "pch.hpp"

#include "platform.hpp"

namespace pom::platform {
    void openErrorWindow(const char* title, const char* body)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, body, nullptr);
    }

    SharedObject::SharedObject(const char* soFilename) : filename(soFilename)
    {
        std::string tempname = filename + ".temp";
        SDL_RWops* source = SDL_RWFromFile(filename.c_str(), "r");
        SDL_RWops* target = SDL_RWFromFile(tempname.c_str(), "w");

        // Read source into buffer
        Sint64 size = SDL_RWsize(source);

        void* buffer = SDL_calloc(1, size);
        SDL_RWread(source, buffer, size, 1);

        // Write buffer to target
        SDL_RWwrite(target, buffer, size, 1);

        SDL_RWclose(source);
        SDL_RWclose(target);
        SDL_free(buffer);

        handle = SDL_LoadObject(tempname.c_str());
        POM_ASSERT(handle, "unable to load shared object file '", soFilename, "': ", SDL_GetError());
    }

    SharedObject::~SharedObject()
    {
        SDL_UnloadObject(handle);
    }

    std::shared_ptr<void*> SharedObject::getFunction(const char* function)
    {
        if (functions.contains(function)) {
            return functions.find(function)->second;
        }
        void* func = SDL_LoadFunction(handle, function);
        if (func) {
            std::shared_ptr<void*> f = std::make_shared<void*>(func);
            return functions.insert({ function, f }).first->second;
        }
        return {};
    }

    void SharedObject::reload()
    {
        SDL_UnloadObject(handle);

        std::string tempname = filename + ".temp";
        SDL_RWops* source = SDL_RWFromFile(filename.c_str(), "r");
        SDL_RWops* target = SDL_RWFromFile(tempname.c_str(), "w");

        // Read source into buffer
        Sint64 size = SDL_RWsize(source);

        void* buffer = SDL_calloc(1, size);
        SDL_RWread(source, buffer, size, 1);

        // Write buffer to target
        SDL_RWwrite(target, buffer, size, 1);

        SDL_RWclose(source);
        SDL_RWclose(target);
        SDL_free(buffer);

        handle = SDL_LoadObject(tempname.c_str());
        POM_ASSERT(handle, "unable to load shared object file '", filename, "': ", SDL_GetError());

        for (auto [name, funcptr] : functions) {
            POM_LOG_INFO("old: ", *funcptr);
            *funcptr = SDL_LoadFunction(handle, name);
            POM_LOG_INFO("new: ", *funcptr);
        }
    }

} // namespace pom::platform