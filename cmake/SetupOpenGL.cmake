

# Find OpenGL package
find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
	message(STATUS "OpenGL found")
	message(STATUS "OpenGL include directory: ${OPENGL_INCLUDE_DIR}")
	message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
else()
	message(FATAL_ERROR "OpenGL not found")
endif()
