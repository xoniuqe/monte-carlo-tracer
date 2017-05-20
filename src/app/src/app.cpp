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

#include "Camera.h"
#include "MonteCarloPathtracer.h"
#include "Scene.h"
#include "shader/LoadShader.h"
#include "util/Light.h"

int setupWindow(int width, int height);
void setupScene();
void setupScreenTexture();
void setupCamera();

void startRaytracer();
void displayScreen();
void display();
//void loop();
void input();
void testMesh();
void loadTeapot(const aiScene* scene);

SDL_Window* window;
SDL_GLContext context;

bool quit = false;
Scene *scene;

GLuint screenVBO, programId, tracerTexture;
GLuint matrixId, locM, locV, locLight;
glm::mat4 MVP, Projection, Model, View;
glm::vec3 LightPosition;
Camera* camera;
MonteCarloPathtracer* mcp;
int renderMode = 1;
int WIDTH = 200;
int HEIGHT = 150;
int main(int argc, char * argv[]) {

  if(!setupWindow(800, 600)){
    return 1;
  }
  glewInit();
  std::cout << "after window setup\n";
  setupScene();
  std::cout << "after scene setup\n";
  setupCamera();
  std::cout << "after camera setup\n";
  
  programId = LoadShaders("../shader/vert.glsl", "../shader/frag.glsl");
  glUseProgram(programId);
  matrixId = glGetUniformLocation(programId, "MVP");
  locM = glGetUniformLocation(programId, "V");
  locV= glGetUniformLocation(programId, "M");
  locLight= glGetUniformLocation(programId, "lightPosition");

  glUseProgram(0);


  mcp = new MonteCarloPathtracer(scene, camera);
  mcp->startPathtracing();

  setupScreenTexture();
 
  
  
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
void display() {
  if(renderMode == 1) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDisable(GL_TEXTURE_2D);
  } else {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
  }
  glClearColor(0.0,0.0,1.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  if(renderMode == 1) {
    glUseProgram(programId);
    glUniformMatrix4fv(matrixId, 1 , GL_FALSE, &MVP[0][0]);

    glUniform3f(locLight, LightPosition.x, LightPosition.y, LightPosition.z);
    glUniformMatrix4fv(locM, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(locV, 1, GL_FALSE, &View[0][0]);
    scene->render();
    glUseProgram(0);
  } else {
    glEnable(GL_TEXTURE_2D); 
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, tracerTexture);
  
    glBegin(GL_QUADS);
    // glColor3f(1.0,0.0,0.0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-1.0,-1.0, 0.0);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(1.0,-1.0,0.0);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(1.0,1.0,0.0);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0, 1.0, 0.0);
    glEnd();
  }
  SDL_GL_SwapWindow(window);
}

void input() {
  SDL_Event event;
  while(SDL_PollEvent(&event) != 0) {
    if(event.type == SDL_QUIT){
      quit = true;
    }
    if(event.type == SDL_KEYUP) {
      if(event.key.keysym.sym == SDLK_1){
        renderMode += 1;
        renderMode %= 2;
      }
    }
  }
}

int setupWindow(int width, int height) {
  // SDL mit dem Grafiksystem initialisieren.
  if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
      // Ups, das hat nicht funktioniert!
      // Wir geben die Fehlermeldung aus.
      std::cerr << "Konnte SDL nicht initialisieren! Fehler: " << SDL_GetError() << std::endl;
      return 0;
    }
  window = SDL_CreateWindow("Monte Carlo Raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width,height, SDL_WINDOW_OPENGL);
	if(window == NULL) {
		//std::cerr << "window is null! << SDl_GetError() << std::endl;
		return 0;
	}
	context = SDL_GL_CreateContext(window);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetSwapInterval(1);
  return 1;
}

void setupScene(){
  Assimp::Importer importer;
  const aiScene* aiScene = importer.ReadFile("../data/test.obj", aiProcess_Triangulate | aiProcess_GenNormals);
 
  scene = new Scene();
  Light* light = new Light();
  light->position = LightPosition;
  light->color = glm::vec3(255,0,255);
  light->power = 5.0f;
  scene->addLight(light);
  Light* light2 = new Light();
  light2->position = glm::vec3(0.5f,0.9f, 0.5f);
  light2->power = 5.f;
  light2->color = glm::vec3(0,255,0);
  scene->addLight(light2);
  if(aiScene) {
    loadTeapot(aiScene);
  } else {
    std::cerr << std::string(importer.GetErrorString()) << std::endl;
  }
  testMesh();
}

void setupCamera() {
  camera = new Camera();
  camera->mOrigin = glm::vec3(2,1,1);//4,3,3);
  camera->mCenter = glm::vec3(0,0,0);
  camera->mDirection = glm::normalize(camera->mOrigin - camera->mCenter);
  camera->mFieldOfView = 90.0f;
  camera->mAspectRatio = 4.0f / 3.0f;
  camera->mWidth = WIDTH;
  camera->mHeight = HEIGHT;
  Projection = glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  View = glm::lookAt(camera->mOrigin, glm::vec3(0,0,0), glm::vec3(0,1,0));
  Model = glm::mat4(1.f); 

  MVP = Projection * View * Model;
  camera->mProjection = Projection;
  camera->mView = View;

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, mcp->mImage); 
}

void testMesh() {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  Vertex a,b,c,d,e,f,g,h,i,j,k,l;
  a.position = glm::vec3(0.f, 0.f, 0.f);//-1.f);
  b.position = glm::vec3(0.5f, 0.f, 0.f);//-1.f);
  c.position = glm::vec3(0.f, 0.f, 0.5f);//-1.f);
  d.position = glm::vec3(0.25f, -0.5f, 0.25f);
  e.position = glm::vec3(0.f,0.f,0.f);
  f.position = glm::vec3(0.5f, 0.f, 0.f);//-1.f);
  g.position = glm::vec3(0.5f, 0.f, 0.f);
  h.position = glm::vec3(0.25f, -0.5f, 0.25f);
  i.position = glm::vec3(0.f,0.f,0.5f);
  j.position = glm::vec3(0.f,0.f,0.f);
  k.position = glm::vec3(0.f,0.f,0.5f);
  l.position = glm::vec3(0.25f,-0.5f, 0.25f);

  a.normal = b.normal = c.normal = glm::vec3(0.f,1.f,0.f);
  a.color = b.color = c.color    = glm::vec3(255,0,0);
  
  d.normal = e.normal = f.normal = glm::normalize(glm::vec3(0,-1,-2));
  d.color =e.color = f.color = glm::vec3(0,0,0);

  g.normal = h.normal = i.normal = glm::normalize(glm::vec3(1,0,-1));
  g.color = h.color = i.color = glm::vec3(0,0,0);


  j.normal = k.normal = l.normal = glm::normalize(glm::vec3(-1,0,1));
  j.color = k.color = l.color = glm::vec3(0,0,0);
  vertices.push_back(a);
  vertices.push_back(b);
  vertices.push_back(c);
  vertices.push_back(d);
  vertices.push_back(e);
  vertices.push_back(f);
  vertices.push_back(g);
  vertices.push_back(h);
  vertices.push_back(i);
  vertices.push_back(j);
  vertices.push_back(k);
  vertices.push_back(l);

  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  indices.push_back(3);
  indices.push_back(4);
  indices.push_back(5);
  indices.push_back(6);
  indices.push_back(7);
  indices.push_back(8);
  Mesh* tmp = new Mesh(std::move(vertices), std::move(indices));
  scene->addMesh(tmp);//new Mesh(std::move(vertices), std::move(indices)));

}
void loadTeapot(const aiScene* aiScene) {

  for(unsigned int i = 0; i < aiScene->mNumMeshes; i++) {
    //std::vector<glm::vec3> vertices;
    //std::vector<glm::vec3> normals;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    aiMesh* mesh = aiScene->mMeshes[i];
    std::cout << "num verts: " <<  mesh->mNumVertices << std::endl;
    for(unsigned int j = 0; j < mesh->mNumVertices; j++) {
      //glm::vec3 vert, norm;
      Vertex vert;

      vert.position.x = mesh->mVertices[j].x;
      vert.position.y = mesh->mVertices[j].y;
      vert.position.z = mesh->mVertices[j].z;

      vert.normal.x = mesh->mNormals[j].x;
      vert.normal.y = mesh->mNormals[j].y;
      vert.normal.z = mesh->mNormals[j].z;


      vert.color.x = 0.5f;
      vert.color.y = 0.5f;
      vert.color.z = 0.5f;
      vertices.push_back(vert);
      //normals.push_back(norm);

    }
    for(unsigned int j = 0; j < mesh->mNumFaces; j++) {
      aiFace face = mesh->mFaces[j];

      for(unsigned int k = 0; k < face.mNumIndices; k++) {
        indices.push_back(face.mIndices[k]);
      }
    }
    scene->addMesh(new Mesh(std::move(vertices), std::move(indices)));
  }

}
