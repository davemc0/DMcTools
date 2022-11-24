# DMcTools Library

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(DMcTools)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    message( FATAL_ERROR "Don't build directly from here." )
endif()

set(IMAGE_SOURCES
    Image/Bmp.cpp
    Image/ColorMap.cpp
    Image/ColorMap.h
    Image/Gif.cpp
    Image/ImageAlgorithms.cpp
    Image/ImageAlgorithms.h
    Image/ImageLoadSave.cpp
    Image/ImageLoadSave.h
    Image/ImageSampling.cpp
    Image/LoadSaveParams.h
    Image/PullPush.h
    Image/Quant.cpp
    Image/Quant.h
    Image/QuantParams.h
    Image/RGBE.h
    Image/RGBEio.cpp
    Image/RGBEio.h
    Image/Targa.cpp
    Image/VCD.cpp
    Image/tImage.h
    Image/tLoadSave.cpp
    Image/tPixel.h
    deps/stb/stb_image.h
    deps/stb/stb_image_write.h
)

set(MATH_SOURCES
    Math/AABB.cpp
    Math/AABB.h
    Math/Auint.h
    Math/BinaryRep.cpp
    Math/BinaryRep.h
    Math/CatmullRomSpline.h
    Math/DownSimplex.cpp
    Math/DownSimplex.h
    Math/HMatrix.h
    Math/HVector.cpp
    Math/HVector.h
    Math/Halton.h
    Math/HermiteSpline.h
    Math/Intersect.cpp
    Math/Intersect.h
    Math/KDBVector.h
    Math/KDBoxTree.h
    Math/Mappings.h
    Math/Matrix44.cpp
    Math/Matrix44.h
    Math/MiscMath.h
    Math/Perlin.cpp
    Math/Perlin.h
    Math/PerlinImproved.cpp
    Math/Quadric.cpp
    Math/Quadric.h
    Math/Random.h
    Math/SpaceFillCurve.h
    Math/Triangle.cpp
    Math/Triangle.h
    Math/Vector.cpp
    Math/Vector.h
    Math/VectorField.h
    Math/VectorUtils.h
)

set(HALF_SOURCES
    deps/Half/eLut.h
    deps/Half/half.cpp
    deps/Half/half.h
    deps/Half/halfFunction.h
    deps/Half/halfLimits.h
    deps/Half/toFloat.h
)

set(MODEL_SOURCES
    Model/AElements.h
    Model/BVH.cpp
    Model/BVH.h
    Model/BaseObject.h
    Model/CameraDB.cpp
    Model/CameraDB.h
    Model/KDVertex.h
    Model/LBVH.cpp
    Model/LBVH.h
    Model/LightDB.h
    Model/LoadOBJ.cpp
    Model/Mesh.cpp
    Model/Mesh.h
    Model/MeshElements.h
    Model/Model.cpp
    Model/Model.h
    Model/RenderObject.cpp
    Model/RenderObject.h
    Model/SaveOBJ.cpp
    Model/TextureDB.cpp
    Model/TextureDB.h
    Model/Traverser.cpp
    Model/Traverser.h
    Model/TriObject.cpp
    Model/TriObject.h
)

set(UTIL_SOURCES
    Util/Assert.h
    Util/Counters.h
    Util/Counters.cpp
    Util/PerThread.h
    Util/StatTimer.cpp
    Util/StatTimer.h
    Util/Timer.cpp
    Util/Timer.h
    Util/Utils.cpp
    Util/Utils.h
    Util/ToolConfig.h
)

set(SOURCES
    ${HALF_SOURCES}
    ${IMAGE_SOURCES}
    ${MATH_SOURCES}
    ${MODEL_SOURCES}
    ${UTIL_SOURCES}
)

source_group("Half" FILES ${HALF_SOURCES})
source_group("Image" FILES ${IMAGE_SOURCES})
source_group("Math" FILES ${MATH_SOURCES})
source_group("Model" FILES ${MODEL_SOURCES})
source_group("Utils" FILES ${UTIL_SOURCES})

add_library(DMcTools STATIC ${SOURCES})

# Push good flags out to library users: build in parallel, warning level, all warnings as errors, optimization
target_compile_options(DMcTools PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:/MP>
    $<$<COMPILE_LANGUAGE:CXX>:/W3>
    $<$<COMPILE_LANGUAGE:CXX>:/WX>
    $<$<COMPILE_LANGUAGE:CXX>:/GF>
    $<$<COMPILE_LANGUAGE:CXX>:/GL>
    $<$<COMPILE_LANGUAGE:CXX>:/Gy>
    $<$<COMPILE_LANGUAGE:CXX>:/Oi>
    $<$<COMPILE_LANGUAGE:CXX>:/Ot>
    $<$<COMPILE_LANGUAGE:CXX>:/Oy>
    $<$<COMPILE_LANGUAGE:CXX>:/Qpar>
#    $<$<COMPILE_LANGUAGE:CXX>:/arch:AVX512> # Causes failures on new laptop in VS2022
    $<$<COMPILE_LANGUAGE:CXX>:/fp:fast>
)
target_link_options(DMcTools INTERFACE $<$<LINK_LANGUAGE:CXX>:/LTCG>)

target_include_directories(DMcTools
    PRIVATE "."
    PRIVATE "./deps"
    PRIVATE "./deps/Half"
    PRIVATE "./deps/stb"
)

# Tell users of the library what they need to include to use it
target_include_directories(DMcTools
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps
)
