cmake_minimum_required (VERSION 3.2.2)

project(Portfolio)

set(INCLUDE_DIRS
  $ENV{WORKS_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "src/Selector.cpp"
  "$ENV{WORKS_PATH}/common/xf/Font.cpp"
  "$ENV{WORKS_PATH}/common/xf/FontManager.cpp"
  "$ENV{WORKS_PATH}/common/xf/FontSequence.cpp"
  "$ENV{WORKS_PATH}/common/quad/QuadBatch.cpp"
  "$ENV{WORKS_PATH}/common/TextRing.cpp"
  "$ENV{WORKS_PATH}/common/LineProvider.cpp"
)

#
# For Mojave and up
#
add_compile_definitions(
  GL_SILENCE_DEPRECATION
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
