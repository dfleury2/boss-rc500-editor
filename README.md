<div align="center">
  <img alt="GUI Screenshot" src="https://github.com/dfleury2/boss-rc500-editor/raw/master/docs/images/screenshot.png" height="500" />
</div>
<br>


# Boss RC-500 Tools
Boss rc-500 tool to play with the XML configuration... until Boss make our life easier


## User Manual

If you just want to use it without dive into the glory details, you can go the
documentation directly [here](https://dfleury2.github.io/boss-rc500-editor/#/).


## Compilation

You need to install : CMake (https://cmake.org/) + Conan v2 (https://conan.io/)
and a compiler for your system (gcc, ...)

### Linux

```
git clone https://github.com/dfleury2/boss-rc500-editor.git
cd boss-rc500-editor
conan install . -pr:b gcc-13 -pr:h gcc-13 -b missing -of cmake-build-release
cmake . --preset conan-release
cmake --build . --preset conan-release
```

### Windows

You need to launch a Developer command prompt to have your compiler environment accessible by CMake and Conan.

```
git clone https://github.com/dfleury2/boss-rc500-editor.git
cd boss-rc500-editor
conan install . -pr:b vs2022 -pr:h vs2022 -b missing
:: debug only: conan install . -pr:b vs2022 -pr:h vs2022-debug -b missing 
cmake .. --preset conan-default
cmake --build . --config Debug
cd ..
.\cmake-build-debug\bin\Debug\BossRc500.exe
```

or you can open the solution file under the `build` directory. (rc500-tools.sln). Visual Studio will open, which will allow you to compiler under VS directly.
I do not use Visual Studio anymore, but to run the program, you must change the working directory to the project root, where the resources directory is,
else the program could not find the images and others files needed.

If you use CLion, after the conan install command, you can open the project root directory in it.

## Tips

Under Linux (Debian), if `Fontconfig error: Cannot load default config file: No such file: (null)` appears

You can add `export FONTCONFIG_PATH=/etc/fonts` before launching the tools

If `Could not create collator: 4`, 
you can set ICU_DATA to the location of icu dat file.

```
export ICU_DATA=~/.conan/data/icu/69.1/_/_/package/3d01f378bd85430e39906eeeaec39c5b63de106c/res/icudt69l.dat
```
