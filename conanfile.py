from conan import ConanFile

class SAD_CPP_Recipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"

    def requirements(self):
        if self.settings.os != "Emscripten": # Emscripten uses emscripten's fetch instead of libcurl
            self.requires("libcurl/8.21.0")

        self.requires("nlohmann_json/3.12.0")
        self.requires("boost/1.91.0", options={"without_cobalt": True})
        self.requires("sdl/2.32.10")
        self.requires("sdl_image/2.8.12")
        self.requires("sdl_ttf/2.24.0")