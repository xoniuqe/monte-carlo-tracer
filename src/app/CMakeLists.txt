add_executable(app
	src/app.cpp
  src/Scene.h
  src/Scene.cpp
  src/Camera.h
  src/Camera.cpp
  src/MonteCarloPathtracer.h
  src/MonteCarloPathtracer.cpp
  src/util/Mesh.h
  src/util/Mesh.cpp
  src/util/Light.h
  src/util/AreaLight.h
  src/util/AreaLight.cpp
  src/util/PointLight.h
  src/util/PointLight.cpp
  src/shader/LoadShader.h
  src/shader/LoadShader.cpp)

#target_link_libraries(app lib)
INCLUDE(FindPkgConfig)

#SET(TBB_FLAGS "-ltbb")
#ADD_DEFINITIONS(${TBB_FLAGS})

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)


#list(APPEND CMAKE_MODULE_PATH /home/pathfinder/programming/c++/hpc/src/app/cmake/modules/)
find_package(tbb)
add_definitions(${TBB_DEFINITIONS}) 

PKG_SEARCH_MODULE(GLM REQUIRED glm)
PKG_SEARCH_MODULE(GLEW REQUIRED glew)
PKG_SEARCH_MODULE(ASSIMP REQUIRED assimp)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${GLM} ${GLEW_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(app ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${GLM_LIBRARIES} ${GLEW_LIBRARIES} ${ASSIMP_LIBRARIES} ${TBB_LIBRARIES} -pthread)
#SET(CMAKE_EXE_LINK_FLAGS "-ltbb")
