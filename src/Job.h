#pragma once

#include <filesystem>
struct RaytraceJob
{
    RaytraceJob(std::filesystem::path _scene_name, std::filesystem::path _out_name)
        : scenePath(_scene_name)
        , outPath(_out_name)
    {
        scenePath.make_preferred();
        outPath.make_preferred();
    }

    std::filesystem::path scenePath, outPath;
};
