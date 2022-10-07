
set(CTEST_PROJECT_NAME Portfolio)
set(CTEST_CONFIGURATION_TYPE Release)

if (PLATFORM MATCHES osx)
  set(ARGS
    -DRUN=EXE
  )
elseif (PLATFORM MATCHES emscripten)
  set(ARGS
    -DRUN=BROWSER
  )
endif()

include("$ENV{CROSS_PATH}/core/cmake/platforms.cmake")
