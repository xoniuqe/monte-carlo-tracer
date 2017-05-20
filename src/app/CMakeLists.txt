add_executable(app
	src/app.cpp
  src/Scene.h
  src/Scene.cpp
  src/Camera.h
  src/MonteCarloPathtracer.h
  src/MonteCarloPathtracer.cpp
  src/util/Mesh.h
  src/util/Mesh.cpp
  src/shader/LoadShader.h
  src/shader/LoadShader.cpp)

#target_link_libraries(app lib)
INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

#find_package(GLM REQUIRED)
#include_directories(${GLM_INCLUDE_DIRS})
#link_directories(${GLM_INCLUDE_DIRS})
#add_definitions(${GLM_INCLUDE_DIRS})

PKG_SEARCH_MODULE(GLM REQUIRED glm)
PKG_SEARCH_MODULE(GLEW REQUIRED glew)
PKG_SEARCH_MODULE(ASSIMP REQUIRED assimp)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${GLM} ${GLEW_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(app ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${GLM_LIBRARIES} ${GLEW_LIBRARIES} ${ASSIMP_LIBRARIES})
