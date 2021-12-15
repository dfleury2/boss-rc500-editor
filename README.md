<div align="center">
  <img alt="GUI Screenshot" src="https://github.com/dfleury2/boss-rc500-editor/raw/master/doc/screenshot.png" height="500" />
</div>
<br>


# Boss RC-500 Tools
Boss rc-500 tool to play with the XML configuration... until Boss make our life easier

Using Inja, RapidXml and Nlohmann Json under Conan.

These are some prototypes to learn new things (Inja and Qt), and see what can I use later.


## Compilation

You need to install : CMake (https://cmake.org/) + Conan (https://conan.io/)
and a compiler for your system (gcc, ...)

### Linux

```
git clone https://github.com/dfleury2/boss-rc500-editor.git
cd boss-rc500-editor
mkdir build && cd build
conan install ..
cmake ..
make
```

Personal note: Remove os.version from settings and user default-gcc10 as profile
`CC=gcc-10 CXX=g++-10 conan install .. -pr default-gcc10 -r conan-center`

### Windows

You need to launch a Developper command prompt to have your compiler environment accessible by CMake and Conan.

```
git clone https://github.com/dfleury2/boss-rc500-editor.git
cd boss-rc500-editor
md build
cd build
conan install ..
cmake ..
cmake --build . --config Release
cd ..
.\build\sources\BossRc500\Release\BossRc500.exe
```

or you can open the solution file under the `build` directory. (rc500-tools.sln). Visual Studio will open, which will allow you to compiler under VS directly.
I do not use Visual Studio anymore, but to run the program, you must change the working directory to the project root, where the resources directory is,
else the program could not find the images and others files needed.

If you use CLion, after the conan install command, you can open the project root directory in it.

## xml-reader

This tool will read a MEMORY1.RC0 file, and will create a Nlohmann Json object to be manipulated later
with an IHM. This in-memory object will be saved using a Inja template.

This is just a prototype for read testing and Inja rendering with a Nlohmann Json object.

The real tool will come with an GUI (provided by someone else maybe)

Happy guitar playing :)

## Tips

Under Linux (Debian), if `Fontconfig error: Cannot load default config file: No such file: (null)` appears

You can add `export FONTCONFIG_PATH=/etc/fonts` before launching the tools

If `Could not create collator: 4`, 
you can set ICU_DATA to the location of icu dat file.

```
export ICU_DATA=~/.conan/data/icu/69.1/_/_/package/3d01f378bd85430e39906eeeaec39c5b63de106c/res/icudt69l.dat
```
