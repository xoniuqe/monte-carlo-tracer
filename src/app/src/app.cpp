#include<iostream>
#include<SDL2/SDL.h>
#include<GL/glew.h>

#include<assimp/scene.h>
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>

#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<thread>

#include "ArcBall.h"
#include "Camera.h"
#include "MonteCarloPathtracer.h"
#include "Scene.h"
#include "shader/LoadShader.h"
#include "util/PointLight.h"
#include "util/AreaLight.h"

#include "geometry/geometry.h"
#include "geometry/Octree.h"
#include "geometry/OctreeNode.h"

int setupWindow(int width, int height);
void setupScreenTexture();
void renderScene();

void fillTexture();
void renderTracerTexture();
void display();
void input();
void loadScene(const aiScene* scene);

SDL_Window* window;
SDL_GLContext context;
bool quit = false;
Scene *scene;

std::thread renderThread;

GLuint screenVBO, programId, tracerTexture;
GLuint matrixId, locM, locV, locLight;
glm::mat4 MVP, Model;
glm::vec3 LightPosition;
Camera* camera;
MonteCarloPathtracer* mcp;
int renderMode = 1;
int WIDTH = 800;
int HEIGHT = 600;
int renderScreenWidth = 300;
int renderScreenHeight = 200;
ArcBall* arcBall;

OctreeNode* test;
std::vector<Triangle*> testTriangles;

int main(int argc, char * argv[]) {
  if(setupWindow(WIDTH,HEIGHT)){
    return 1;
  }
  glewInit();

  
  programId = LoadShaders("../shader/vert.glsl", "../shader/frag.glsl");
  glUseProgram(programId);
  matrixId = glGetUniformLocation(programId, "MVP");
  locM = glGetUniformLocation(programId, "V");
  locV= glGetUniformLocation(programId, "M");
  locLight= glGetUniformLocation(programId, "lightPosition");

  glUseProgram(0);
  
  camera = new Camera(glm::vec3(-1,1,-1), glm::vec3(0,0,0), 90.f, 4.f / 3.f, WIDTH, HEIGHT);
  Model = glm::mat4(1.f);

  float radius = sqrtf(float(WIDTH * WIDTH + HEIGHT * HEIGHT)) * 0.5f;
  arcBall = new ArcBall();
  arcBall->place(glm::vec3(0,0,0), 3.f);

  MVP = camera->mProjection * camera->mView * Model;
  
  LightPosition = glm::vec3(-0.2f,0.9,-0.2f);

  scene = new Scene();

  AreaLight* light = new AreaLight(LightPosition, 0.2f, 0.2f);
  light->color = glm::vec3(1.f,1.f,1.f);
  light->power = 50.f;
  scene->addLight(light);


  Assimp::Importer importer;
  const aiScene* aiScene = importer.ReadFile("../data/scene.obj", aiProcess_Triangulate | aiProcess_GenNormals);
  if(aiScene) {
    loadScene(aiScene);
  } else {
    std::cerr << std::string(importer.GetErrorString()) << std::endl;
   }

  setupScreenTexture();
  
  /*test = new OctreeNode(12, glm::vec3(0,0,0), glm::vec3(1,1,1));//-1,-1,-1), glm::vec3(1,1,1));
  for(auto triangle : testTriangles) {
    test->insert(triangle);
  }
  test->subdivide();*/
  scene->calculateOctree();
  mcp = new MonteCarloPathtracer(scene, camera, renderScreenWidth, renderScreenHeight, 4);
  while(!quit) {
    input();
    display(); 
  }
  
  delete mcp;
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
	return 0;
}
float rotate = 0.f;
void renderTracerTexture() {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, tracerTexture);
  fillTexture();
  glEnable(GL_TEXTURE_2D); 
  glUseProgram(0);
  
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f,0.0f);
  glVertex3f(-1.0,-1.0, 0.0);
  glTexCoord2f(1.0f,0.0f);
  glVertex3f(1.0,-1.0,0.0);
  glTexCoord2f(1.0f,1.0f);
  glVertex3f(1.0,1.0,0.0);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-1.0, 1.0, 0.0);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);
}

void renderScene() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_TEXTURE_2D);
  glUseProgram(programId);
  glUniformMatrix4fv(matrixId, 1 , GL_FALSE, &MVP[0][0]);

  glUniform3f(locLight, LightPosition.x, LightPosition.y, LightPosition.z);
  glUniformMatrix4fv(locM, 1, GL_FALSE, &Model[0][0]);
  glUniformMatrix4fv(locV, 1, GL_FALSE, &camera->mView[0][0]);
  scene->render();
  //test->render();
  glUseProgram(0);
}

void display() {
  glClearColor(0.0,0.0,1.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch(renderMode) {
  case 0:
    renderTracerTexture();
    break;
  case 1:
    renderScene();
    break;
  }
  SDL_GL_SwapWindow(window);
}

bool isTracing = false;
glm::vec3 mousePosition;
void input() {
  int x,y;
  SDL_GetMouseState(&x, &y);
  mousePosition.x = 2.f * ((float) x / WIDTH) -1.f;
  mousePosition.y = 2.f * ((float) y / HEIGHT) - 1.f;
  mousePosition.z = 0.f;
  arcBall->setMouse(mousePosition);
  SDL_Event event;
  while(SDL_PollEvent(&event) != 0) {
    if(event.type == SDL_QUIT){
      quit = true;
    }
    if(event.button.button == SDL_BUTTON_LEFT) {//type == SDL_MOUSEBUTTONDOWN) {
      int x,y;
      arcBall->setMouse(mousePosition);
      if(event.type == SDL_MOUSEBUTTONDOWN) {
        std::cout << "left click!" << std::endl;
        arcBall->beginDragging();
      }
      else if(event.type == SDL_MOUSEBUTTONUP) {
        std::cout << "left release!" << std::endl;
        arcBall->endDragging();
      }
      camera->mOrigin = glm::vec3(glm::inverse(arcBall->getRotationMatrix()) * glm::vec4(camera->mOrigin,1));
      camera->mUp = glm::vec3(glm::inverse(arcBall->getRotationMatrix()) * glm::vec4(camera->mUp,0));
      
      camera->mView = glm::lookAt(camera->mOrigin, camera->mCenter, camera->mUp);
      MVP = camera->mProjection * camera->mView * Model;
    }
    if(event.type == SDL_KEYUP) {
      if(event.key.keysym.sym == SDLK_1){
        renderMode += 1;
        renderMode %= 2;
      }
      if(event.key.keysym.sym == SDLK_r) {        
        if(!isTracing) {
          renderThread = std::thread([=](){mcp->startPathtracing();});
          isTracing = true;
        }
      }
    }
  }
}

void loadScene(const aiScene* aiScene) {

  std::vector<glm::vec3> colors;
  for(auto i = 0; i < aiScene->mNumMeshes; i++) {
    aiMaterial *mtl = aiScene->mMaterials[aiScene->mMeshes[i]->mMaterialIndex];
    aiColor4D diffuse;
    if(aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS) {
      glm::vec3 color(diffuse.r, diffuse.g, diffuse.b);
      colors.push_back(color);
    }
  }
  for(auto i = 0; i < aiScene->mNumMeshes; i++) {

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    aiMesh* mesh = aiScene->mMeshes[i];
    //std::cout << "num verts: " <<  mesh->mNumVertices << std::endl;
    for(unsigned int j = 0; j < mesh->mNumVertices; j++) {
      //glm::vec3 vert, norm;
      Vertex vert;

      vert.position.x = mesh->mVertices[j].x;
      vert.position.y = mesh->mVertices[j].y;
      vert.position.z = mesh->mVertices[j].z;

      vert.normal.x = mesh->mNormals[j].x;
      vert.normal.y = mesh->mNormals[j].y;
      vert.normal.z = mesh->mNormals[j].z;

      vert.color = colors[i];
      vertices.push_back(vert);

    }
    for(unsigned int j = 0; j < mesh->mNumFaces; j++) {
      aiFace face = mesh->mFaces[j];
      Triangle* tmpTriangle = new Triangle();
      tmpTriangle->a = vertices.at(face.mIndices[0]);
      tmpTriangle->b = vertices.at(face.mIndices[1]);
      tmpTriangle->c = vertices.at(face.mIndices[2]);
      testTriangles.push_back(tmpTriangle);
      for(unsigned int k = 0; k < face.mNumIndices; k++) {
        indices.push_back(face.mIndices[k]);
      }
    }
    scene->addMesh(new Mesh(std::move(vertices), std::move(indices)));
  }

}

int setupWindow(int width, int height) {
	// SDL mit dem Grafiksystem initialisieren.
  if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
      std::cerr << "Konnte SDL nicht initialisieren! Fehler: " << SDL_GetError() << std::endl;
      return 1;
    }
  window = SDL_CreateWindow("Monte Carlo Raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800,600, SDL_WINDOW_OPENGL);
	if(window == NULL) {
		//std::cerr << "window is null! << SDl_GetError() << std::endl;
		return 1;
	}
	context = SDL_GL_CreateContext(window);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetSwapInterval(1);

  return 0;
}

void setupScreenTexture() {
  glGenTextures(1, &tracerTexture);
  glBindTexture(GL_TEXTURE_2D, tracerTexture);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);//_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
 
}

 void fillTexture() {
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, renderScreenWidth, renderScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mcp->mImage);
 }
