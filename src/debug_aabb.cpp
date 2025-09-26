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

#include "Scene.h"
#include "Mesh.h"
#include "Job.h"
#include "Paths.h"

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

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

    if (argc == 3)
        jobs.emplace_back(RaytraceJob{argv[1], argv[2]});
    else if ((argc == 2) && argv[1][0] == '0') {
        std::cout << "Using scene folder " << sceneDir.string() << std::endl;
        std::cout << "Saving to results folder " << resultsDir.string() << std::endl;
        for (const auto *name: {
                "mask",
                "toon_faces",
                "office",
                "rings",
                })
        {
            jobs.emplace_back(sceneDir / name / (name + std::string(".sce")),
                              resultsDir / (std::string("debug_aabb_") + name + ".bmp"));
        }
    }
    else {
        std::cerr << "Usage: " << argv[0] << " input.sce output.bmp\n";
        std::cerr << "Or: " << argv[0] << " 0\n";
        std::cerr << std::flush;
        exit(1);
    }

    for (const auto &job : jobs) {
        std::cout << "Read scene " << job.scenePath << "..." << std::flush;
        Scene s(job.scenePath);
        std::cout << "\ndone (" << s.numObjects() << " objects)\n";

        const auto &c = s.getCamera();
        Image img(c.width, c.height);
        std::vector<size_t> numIntersected(c.width * c.height);
        for (int x=0; x<int(c.width); ++x) {
            for (int y=0; y<int(c.height); ++y) {
                Ray ray = c.primary_ray(x,y);

                for (const auto &o: s.getObjects()) {
                    if (auto mesh = dynamic_cast<const Mesh *>(o.get())) {
                        if (mesh->intersect_bounding_box(ray))
                            ++numIntersected[y * c.width + x];
                    }
                }
            }
        }
        size_t maxIntersectionCount = *std::max_element(numIntersected.begin(), numIntersected.end());
        for (int x=0; x<int(c.width); ++x)
            for (int y=0; y<int(c.height); ++y)
                img(x, y) = vec3(numIntersected[y * c.width + x] / float(maxIntersectionCount), 0, 0);

        std::cout << "Writing image to " << job.outPath << std::flush;
        img.write_bmp(job.outPath);
        std::cout << ": done\n";
    }
}
