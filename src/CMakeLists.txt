set(SCENES_PATH  "${CMAKE_SOURCE_DIR}/scenes" CACHE PATH "location of input scenes")

file(TO_NATIVE_PATH ${SCENES_PATH} SCENES_PATH_NATIVE)
STRING(REPLACE "\\" "\\\\" SCENES_PATH_NATIVE "${SCENES_PATH_NATIVE}")  
configure_file("Paths.h.in" "Paths.h" ESCAPE_QUOTES)

# add as object library as not to compile all of these twice:
add_library(common STATIC Cylinder.cpp Mesh.cpp Plane.cpp Scene.cpp Sphere.cpp vec3.cpp Image.cpp)

add_executable(raytrace raytrace.cpp)
add_executable(debug_aabb debug_aabb.cpp)


option(RAYTRACER_ENABLE_OPENMP "Raytracer: enable OpenMP parallelisation" ON)
if (RAYTRACER_ENABLE_OPENMP)
    find_package(OpenMP)
endif()

SET(TARGETS raytrace debug_aabb)

foreach(TARGET common ${TARGETS})
    set_target_properties(${TARGET}
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
        target_include_directories(${TARGET} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
    if(WIN32)
        # windows standard library needs some special treatment
        target_compile_definitions(${TARGET} PRIVATE _USE_MATH_DEFINES NOMINMAX)
    endif()

    if(OpenMP_CXX_FOUND AND RAYTRACER_ENABLE_OPENMP)
        target_link_libraries(${TARGET} PUBLIC OpenMP::OpenMP_CXX)
        target_compile_definitions(${TARGET} PRIVATE "HAVE_OPENMP=1")
    else()
        target_compile_definitions(${TARGET} PRIVATE "HAVE_OPENMP=0")
    endif()
endforeach()

foreach(TARGET ${TARGETS})
    target_link_libraries(${TARGET} PRIVATE common)
endforeach()
