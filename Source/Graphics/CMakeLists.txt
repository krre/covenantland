add_subdirectory(Light)
add_subdirectory(Render)
add_subdirectory(Voxel)

set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/Color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Color.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Plane.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Plane.h
    PARENT_SCOPE
)
