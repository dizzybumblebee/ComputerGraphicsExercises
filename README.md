Ray Tracer
==========


Building from the commandline
------------------------------

On Linux and MacOS, you can use a regular terminal.
On Windows, this works best inside a WSL (Windows Subsystem for Linux) environment.

You will need [CMake](https://www.cmake.org) for setting up the build environment.

Inside the exercise's top-level directory, execute the following commands:

    cmake -B build .
    cmake --build build

This creates a "build folder" called "build".
The last command compiles the application. Rerun it whenever you have added/changed code in order to recompile.
You can use `make -j8` to compile with 8 cores (choose an appropriate number).

With [Doxygen](https://doxygen.org) installed on your system, you can build web-based code documentation by running

    cmake --build build --target doc

and opening `build/html/index.html` with your favourite browser.

Running the Ray Tracer (commandline)
-------------------------------------

The program expects two command line arguments:
 - a path to an input scene (`*.sce`) and
 - a path to an output image (`*.bmp`).

To render the scene with the three spheres, while inside the `build` directory, type in your shell (Linux / MacOS)

    ./raytrace ../scenes/spheres/spheres.sce output.bmp

or on Windows:

    .\raytrace.exe ../../scenes/spheres/spheres.sce output.bmp

If you have finished all exercise tasks, use

    ./raytrace 0

to render all scenes at once.

You may have to adjust the relative paths if the build folder
containing the `raytrace` or `raytrace.exe` binary is not a
direct subfolder of the project folder.

Make sure (e.g. with `ls`) that you are specifing the correct path to the input file.
The output file will be saved in the current working directory of the program, i.e. the directory you started it from.

Building under Microsoft Windows (Visual Studio)
------------------------------------------------

* Open the CMakeLists.txt as project, or right-click the folder containing it in explorer and "Open with Visual Studio".
* Select target (left of green "play button")
* Click build/run (green "play button")

* you can adjust the provided `launch.vs.json` file to create convenient launch targets.


Building with QtCreator or CLion (supported on Linux/MacOS/Windows)
-------------------------------------------------------------------

It should be sufficient to open the CMakeLists.txt as a project, then using the
build/play button.

Warning: CLion on Windows ships a build environment that does not support OpenMP, which may lead to compile errors.
You can deactivate OpenMP support by setting the CMake setting `RAYTRACER_ENABLE_OPENMP` to `OFF`. This will create
a slow, single-threaded raytracer. We recommend either using a different compiler with CLion, or using another IDE.

Building with XCode (macOS)
---------------------------

If you wish, you can use the CMake build system to generate an XCode project.
Inside the exercise's top-level directory, execute the following commands:

    mkdir xcode
    cd xcode
    cmake -G Xcode ..
    open RayTracing.xcodeproj

Optionally, this project uses OpenMP for parallelization. Unfortunately,
XCode's compiler (based on clang) does not come with OpenMP by default.
You can install it from homebrew using `brew install libomp`.
Then set this environment variable
    export OpenMP_ROOT=/opt/homebrew/opt/libomp/
before running cmake (or delete your build folder and run it again).


Running the Ray Tracer from IDEs
-------------------------------------

To set the command line parameters in IDEs like CLion, QtCreator or Xcode, please refer to the documentation of these programs (or use the command line...).
You'll likely have to set which program should be run (`raytrace`), a working directory, and commandline arguments.
If you set the working directory to the build folder, the above paths should work just the same.



