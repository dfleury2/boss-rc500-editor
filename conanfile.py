
from conan import ConanFile
from conan.tools.cmake import cmake_layout


class BossRcEditorRecipe(ConanFile):
    name = "BossRcEditor"
    version = "0.0.11"

    settings = "os", "compiler", "build_type", "arch"

    requires = (
        "inja/3.5.0",
        "pugixml/1.16",
        "qt/6.10.1",
        "yaml-cpp/0.8.0",
        "fmt/12.1.0",
        "spdlog/1.17.0",
        "nlohmann_json/3.12.0",
    )

    generators = (
        "CMakeDeps",
        "CMakeToolchain",
    )

    def requirements(self):
        if self.settings.os == "Linux":
            self.requires("libusb/1.0.29")


    def layout(self):
        cmake_layout(self)
