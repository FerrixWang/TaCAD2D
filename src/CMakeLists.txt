
set(PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/src/Main.cxx)
set(PROJECT_RESOURCES ${CMAKE_SOURCE_DIR}/res/Resource.qrc)

# Recursively find all source and header files
file(GLOB SUB_DIRECTORY_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*/*.hxx"
    "${CMAKE_SOURCE_DIR}/src/*/*.cxx"
    "${CMAKE_SOURCE_DIR}/src/*/*/*.hxx"
    "${CMAKE_SOURCE_DIR}/src/*/*/*.cxx"
    "${CMAKE_SOURCE_DIR}/src/*/*/*/*.hxx"
    "${CMAKE_SOURCE_DIR}/src/*/*/*/*.cxx"
    "${CMAKE_SOURCE_DIR}/src/*/*/*/*/*.hxx"
    "${CMAKE_SOURCE_DIR}/src/*/*/*/*/*.cxx"
)

list(APPEND PROJECT_SOURCES ${SUB_DIRECTORY_SOURCES})

# add executable target
qt_add_executable(${PROJECT_NAME} 
    MANUAL_FINALIZATION
    ${PROJECT_RESOURCES}
    ${PROJECT_SOURCES}
    ${EXTERNAL_SOURCES}
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/Pch.hxx)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCASCADE_INCLUDE_DIR})


if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCASCADE_LIBRARIES})

    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
    )
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
elseif(WIN32)
    # Link OpenCASCADE libraries
    foreach (LIB ${OpenCASCADE_LIBRARIES})
      target_link_libraries(${PROJECT_NAME} PRIVATE debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
      target_link_libraries(${PROJECT_NAME} PRIVATE optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
    endforeach()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

target_link_libraries(${PROJECT_NAME} PRIVATE enumFlags)
target_link_libraries(${PROJECT_NAME} PRIVATE sketchSolve)
target_link_libraries(${PROJECT_NAME} PRIVATE SARibbon)
target_link_libraries(${PROJECT_NAME} PRIVATE cxxopts)
target_link_libraries(${PROJECT_NAME} PRIVATE qtadvanceddocking-qt6)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.

set_target_properties(${PROJECT_NAME}  PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Install the target executable/library
include(GNUInstallDirs)

# Install the executable or runtime (if Windows, RUNTIME is used)
install(TARGETS ${PROJECT_NAME} 
    BUNDLE DESTINATION .                                # For macOS app bundles (if applicable)
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}         # Shared libraries on Linux/macOS
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}         # Executables and Windows DLLs
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
