cmake_minimum_required(VERSION 3.20)

project(ttLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    # /O1 often results in faster code than /O2 due to CPU caching
    string(REPLACE "/O2" "/O1" cl_optimize ${CMAKE_CXX_FLAGS_RELEASE})
    set(CMAKE_CXX_FLAGS_RELEASE ${cl_optimize} CACHE STRING "C++ Release flags" FORCE)

    # Using /Z7 instead of /Zi to avoid blocking while parallel compilers write to the pdb file.
    # This can considerably speed up build times at the cost of larger object files.
    string(REPLACE "/Zi" "/Z7" z_seven ${CMAKE_CXX_FLAGS_DEBUG})
    set(CMAKE_CXX_FLAGS_DEBUG ${z_seven} CACHE STRING "C++ Debug flags" FORCE)
endif()

get_property(isMultiConfig GLOBAL
  PROPERTY GENERATOR_IS_MULTI_CONFIG
)

if (NOT isMultiConfig)
    message("\nBecause you are using a single target generator, you MUST specify")
    message("    a \"--config [Debug|Release]\" option with the cmake --build command\n")

    set(allowedBuildTypes Debug Release)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowedBuildTypes}")

    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    elseif (NOT CMAKE_BUILD_TYPE IN_LIST allowedBuildTypes)
        message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif()

add_compile_definitions(UNICODE)

# Create the cross-platform library
add_library(ttLib STATIC
    src/ttconsole.cpp    # class that sets/restores console foreground color
    src/ttcstr.cpp       # Class for handling zero-terminated char strings.
    src/ttcvector.cpp    # Vector class for storing ttlib::cstr strings
    src/ttcview.cpp      # string_view functionality on a zero-terminated char string.
    src/ttmultistr.cpp   # ttlib::multistr, ttlib::multiview
    src/ttlibspace.cpp   # ttlib namespace functions
    src/ttparser.cpp     # Command line parser
    src/ttstrings.cpp    # Class for handling zero-terminated char strings.
    src/tttextfile.cpp   # Classes for reading and writing text files.
)

# Also create a Windows-only library
add_library(ttLibWin STATIC
    src/ttconsole.cpp    # class that sets/restores console foreground color
    src/ttcstr.cpp       # Class for handling zero-terminated char strings.
    src/ttcvector.cpp    # Vector class for storing ttlib::cstr strings
    src/ttcview.cpp      # string_view functionality on a zero-terminated char string.
    src/ttmultistr.cpp   # ttlib::multistr, ttlib::multiview
    src/ttlibspace.cpp   # ttlib namespace functions
    src/ttparser.cpp     # Command line parser
    src/ttstrings.cpp    # Class for handling zero-terminated char strings.
    src/tttextfile.cpp   # Classes for reading and writing text files.

# Windows only files

    src/winsrc/ttdib.cpp           # ttCDib class
    src/winsrc/ttdirdlg.cpp        # dialog for selecting a directory
    src/winsrc/ttloadstr.cpp       # Load a language-specific version of a string resource
    src/winsrc/ttmultibtn.cpp      # ttlib::MultiBtn class
    src/winsrc/ttopenfile.cpp      # Wrapper around Windows GetOpenFileName() API
    src/winsrc/ttregistry.cpp      # ttlib::registry -- class for working with Windows registry
    src/winsrc/ttshadebtn.cpp      # ttCShadeBtn class
    src/winsrc/ttthrdpool.cpp      # ttlib::ThrdPool
    src/winsrc/ttwin.cpp           # ttlib::win class

    src/winsrc/ttdebug.cpp         # debug and assertion handling
    src/winsrc/ttwindlg.cpp        # ttlib::dlg -- class for creating a Modal or Modeless dialog box
    src/winsrc/ttwinspace.cpp      # Windows-only ttlib namespace functions
)

if (MSVC)
    # /GL -- combined with the Linker flag /LTCG to perform whole program optimization in Release build
    # /FC -- Full path to source code file in diagnostics

    target_compile_options(ttLib PRIVATE "$<$<CONFIG:Release>:/GL>" "/FC" "/W4" "/Zc:__cplusplus" "/utf-8")
    target_link_options(ttLib PRIVATE "$<$<CONFIG:Release>:/LTCG>")

    target_compile_options(ttLibWin PRIVATE "$<$<CONFIG:Release>:/GL>" "/FC" "/W4" "/Zc:__cplusplus" "/utf-8")
    target_link_options(ttLibWin PRIVATE "$<$<CONFIG:Release>:/LTCG>")

endif()

target_precompile_headers(ttLib PRIVATE "src/precompile/pch.h")
target_precompile_headers(ttLibWin PRIVATE "src/winsrc/precompile/pch.h")

target_include_directories(ttLib PRIVATE
    src/precompile
    include
)

target_include_directories(ttLibWin PRIVATE
    src/winsrc/precompile
    include
)
