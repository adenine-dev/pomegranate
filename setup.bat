@echo off

cmake --version > nul || (
    echo Could not find CMake, it may not be installed or may be outside of path.
    goto :eof
)

mkdir build
cd build

If "%1" == "ninja" (
    ninja --version > nul || (
        echo Could not find Ninja, it may not be installed or may be outside of path.
        goto :eof
    )

    mkdir ninja
    cd ninja
    cmake ../.. -G"Ninja Multi-Config" -DGENERATOR_NAME=ninja
    cd ..
    
    echo @echo off>build.bat
    echo If Not "%%1" ^=^= "Debug" ^( If Not "%%1" ^=^= "Release" ^( If Not "%%1" ^=^= "RelWithDebInfo" ^(>>build.bat
    echo echo invalid usage: build.bat ^^^<config:Debug^^^|Release^^^|RelWithDebInfo^^^>>>build.bat
    echo goto ^:eof>>build.bat
    echo ^)^)^) >>build.bat
    echo ninja -j20 -Cninja -fbuild-%%1.ninja>>build.bat

    echo @echo off>run.bat
    echo If Not "%%1" ^=^= "Debug" ^( If Not "%%1" ^=^= "Release" ^( If Not "%%1" ^=^= "RelWithDebInfo" ^(>>run.bat
    echo echo invalid usage: run.bat ^^^<config:Debug^^^|Release^^^|RelWithDebInfo^^^>>>run.bat
    echo goto ^:eof>>run.bat
    echo ^)^)^) >>run.bat
    echo ninja -j20 -Cninja -fbuild-%%1.ninja ^&^& "../bin/ninja-%%1/sandbox.exe">>run.bat

) Else ( 
    If "%1" == "msvc" (
        mkdir msvc
        cd msvc
        cmake ../.. -G"Visual Studio 16 2019" -DGENERATOR_NAME=msvc
    ) Else (
        echo invalid usage: setup.bat ^<compiler:ninja^|msvc^>
    )
)

cd ..
