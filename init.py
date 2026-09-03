import subprocess
from pathlib import Path

ROOT = Path(__file__).parent

def RunConan(build_type, profile, output_folder):
    subprocess.run((
        'conan', 'install', '.', 
        '--build', 'missing', 
        f'--output-folder={output_folder}',
        f'--profile:host={profile}',
        '--profile:build=default',
        f'--settings:host=build_type={build_type}',
    ), cwd=ROOT, check=True)

def RunPremake(*args):
    if "--browser" in args:
        command = "call dependencies/emscripten/conanbuild.bat && premake5.exe " + " ".join(args)
        subprocess.run(("cmd", "/d", "/c", command), cwd=ROOT, check=True)

        makefile = ROOT / "project" / "Makefile"
        makefile.write_text(
            makefile.read_text().replace("$(EXE_PATHS) ", ""),
            encoding="ascii"
        )
        return

    subprocess.run((
        'premake5.exe', *args
    ), cwd=ROOT, check=True)

if __name__ == "__main__":
    RunConan("Debug", "profiles/windows-debug", "dependencies")
    RunConan("Release", "profiles/windows-release", "dependencies")
    RunConan("Debug", "profiles/emscripten-debug", "dependencies/emscripten")
    RunConan("Release", "profiles/emscripten-release", "dependencies/emscripten")
    RunPremake("vs2026")
    RunPremake("gmake", "--browser", "--cc=emcc")

# Debug:
# conan install . --build="*" --output-folder=dependencies/emscripten --profile:host=profiles/emscripten-debug --profile:build=default
# cmd /d /c "call dependencies\emscripten\conanbuild.bat && C:\Users\ecard\.conan2\p\makec216fa1c2c18f\p\bin\gnumake.exe -f Makefile config=debug -j1"
# cmd /d /c "call dependencies\emscripten\conanbuild.bat && emrun docs\SAD_CPP.html"

# Release:
# conan install . --build="*" --output-folder=dependencies/emscripten --profile:host=profiles/emscripten-release --profile:build=default
# cmd /d /c "call dependencies\emscripten\conanbuild.bat && C:\Users\ecard\.conan2\p\makec216fa1c2c18f\p\bin\gnumake.exe -f Makefile config=release -j1"
# cmd /d /c "call dependencies\emscripten\conanbuild.bat && emrun docs\SAD_CPP.html"

# cmd /d /c "call dependencies\emscripten\conanbuild.bat && emrun --no_browser docs\SAD_CPP.html"
# cmd /d /c "call dependencies\emscripten\conanbuild.bat && emrun --no_browser docs\SAD_CPP.html"