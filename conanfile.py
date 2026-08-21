from conan import ConanFile

class SAD_CPP_Recipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"

    def requirements(self):
        self.requires("boost/1.91.0", options={"without_cobalt": True})
        self.requires("sdl/2.32.10")
        self.requires("sdl_image/2.8.12")
        self.requires("sdl_ttf/2.24.0")