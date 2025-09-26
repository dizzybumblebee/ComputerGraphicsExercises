//=============================================================================
//
//   Exercise code for the lecture
//   "Introduction to Computer Graphics"
//   by Prof. Dr. Mario Botsch, Bielefeld University
//
//   Copyright (C) Computer Graphics Group, Bielefeld University.
//
//=============================================================================

//== includes =================================================================

#include "StopWatch.h"
#include "Scene.h"
#include "Paths.h"
#include "Job.h"

#include <vector>
#include <iostream>
#include <string>


#ifdef _WIN32
#  include <windows.h>
#  include <stdlib.h>
#  include <errhandlingapi.h>
#endif



/// Program entry point.
int main(int argc, char **argv)
{
#ifdef _WIN32
    // This make crashes very visible - without them, starting the
    // application from cmd.exe or powershell can surprisingly hide
    // any signs of a an application crash!
    SetErrorMode(0);
#endif
    const auto sceneDir = std::filesystem::path(SCENES_PATH, std::filesystem::path::format::native_format);
    const auto resultsDir = sceneDir.parent_path() / "results";
    // Parse input scene file/output path from command line arguments
    std::vector<RaytraceJob> jobs;

    if (argc == 3) {
        jobs.emplace_back(RaytraceJob{argv[1], argv[2]});
    } else if ((argc == 2) && argv[1][0] == '0') {
        std::cout << "Using scene folder " << sceneDir.string() << std::endl;
        std::cout << "Saving to results folder " << resultsDir.string() << std::endl;
        for (const auto *name: {
                "spheres",
                "cylinders",
                "combo",
                "molecule",
                "molecule2",
                "cube",
                "mask",
                "mirror",
                "toon_faces",
                "office",
                "rings",
                })
        {
            jobs.emplace_back(sceneDir / name / (name + std::string(".sce")),
                              resultsDir / (name + std::string(".bmp")));
        }
    }
    else {
        std::cerr << "Usage: " << argv[0] << " path/to/input.sce path/to/output.bmp (to render a single scene)\n";
        std::cerr << "Or:    " << argv[0] << " 0                    (to render all scenes)\n";
        std::cerr << std::flush;
        exit(1);
    }


    for (const auto &job : jobs) {
        std::cout << "Read scene " << job.scenePath << "..." << std::flush;
        Scene s(job.scenePath);
        std::cout << "\ndone (" << s.numObjects() << " objects)\n";

        StopWatch timer;
        std::cout << "Ray tracing..." << std::flush;
        timer.start();
        auto image = s.render();
        timer.stop();
        std::cout << " done (" << timer << ")\n";

        std::cout << "Writing image to " << job.outPath << std::flush;
        image.write_bmp(job.outPath);
        std::cout << "\n\n";
    }
}
