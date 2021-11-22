# Boss RC-500 Tools
Boss rc-500 tool to play with the XML configuration... until Boss make our life easier

Using Inja, RapidXml and Nlohmann Json under Conan.

These are some prototypes to learn new things (Inja), and see what can I use later.

## Compilation

CMake + Conan

```
git clone https://github.com/dfleury2/rc500-reader.git
cd rc500-reader
mkdir build && cd build
conan install ..
cmake ..
make
./xml-reader ../templates/MEMORY1.RC0 ../templates/MEMORY.txt > MEMORY1.RC0.NEW
```


## xml-reader

This tool will read a MEMORY1.RC0 file, and will create a Nlohmann Json object to be manipulated later
with an IHM. This in-memory object will be saved using a Inja template.

This is just a prototype for read testing and Inja rendering with a Nlohmann Json object.

The real tool will come with an IHM (provided by someone else maybe)

Happy guitar playing :)

## Tips

Under Linux (Debian), if `Fontconfig error: Cannot load default config file: No such file: (null)` appears

You can add `export FONTCONFIG_PATH=/etc/fonts` before launching the tools

If `Could not create collator: 4`, 
you can set ICU_DATA to the location of icu dat file.

```
export ICU_DATA=~/.conan/data/icu/69.1/_/_/package/3d01f378bd85430e39906eeeaec39c5b63de106c/res/icudt69l.dat
```
