#include<iostream>
#include<SDL2/SDL.h>
#include<GL/glew.h>

#include<assimp/scene.h>
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>

#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include "Scene.h"
#include "shader/LoadShader.h"

void display();
//void loop();
void input();
void testMesh();
void loadTeapot(const aiScene* scene);

SDL_Window* window;
bool quit = false;
Scene *scene;

GLuint programId;
GLuint matrixId, locM, locV, locLight;
glm::mat4 MVP, Projection, Model, View;
glm::vec3 LightPosition;

int main(int argc, char * argv[]) {
	std::cout << "Hallo Welt" << std::endl;
	// SDL mit dem Grafiksystem initialisieren.
    	if(SDL_Init(SDL_INIT_VIDEO) == -1)
    	{
        	// Ups, das hat nicht funktioniert!
        	// Wir geben die Fehlermeldung aus.
        	std::cerr << "Konnte SDL nicht initialisieren! Fehler: " << SDL_GetError() << std::endl;
        	return 1;
    	}
	window = SDL_CreateWindow("Monte Carlo Raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	if(window == NULL) {
		//std::cerr << "window is null! << SDl_GetError() << std::endl;
		return 1;
	}
	SDL_GLContext context = SDL_GL_CreateContext(window);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetSwapInterval(1);

  glewInit();

  std::cout << "Error: " << glGetError() << " " << GL_NO_ERROR << std::endl;
  Assimp::Importer importer;
  const aiScene* aiScene = importer.ReadFile("../data/teapot.obj", aiProcess_Triangulate | aiProcess_GenNormals);

  programId = LoadShaders("../shader/vert.glsl", "../shader/frag.glsl");
  glUseProgram(programId);
  matrixId = glGetUniformLocation(programId, "MVP");
  std::cout << "Error: " << glGetError() << " " << GL_NO_ERROR << std::endl;
  locM = glGetUniformLocation(programId, "V");
  std::cout << "Error: " << glGetError() << " " << GL_NO_ERROR << std::endl;
  locV= glGetUniformLocation(programId, "M");
  std::cout << "Error: " << glGetError() << " " << GL_NO_ERROR << std::endl;
  locLight= glGetUniformLocation(programId, "lightPosition");
  std::cout << "Error: " << glGetError() << " " << GL_NO_ERROR << std::endl;

  glUseProgram(0);

  Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  View = glm::lookAt(glm::vec3(4,3,3), glm::vec3(0,0,0), glm::vec3(0,1,0));

  Model = glm::mat4(1.f);

  MVP = Projection * View * Model;

  LightPosition = glm::vec3(6,3,2);

  scene = new Scene();
  if(aiScene) {
    //loadTeapot(aiScene);
  } else {
    std::cerr << std::string(importer.GetErrorString()) << std::endl;
   }
  testMesh();
  //  Vector3 test(1.0,1.0,1.0);
  //  Vector3 test2(0.0, 2.0, 0.0);

  //test += test2;
  //test.normalize().length();
  //test.cross(test2);

  //  std::cout << "x: " << test.x << " y: " << test.y << " z: " << test.z << std::endl;

  std::cout << "Error: " << glGetError() << " " << GL_NO_ERROR << std::endl;
  //nur ein test!
	while(!quit) {
        input();
        display(); 
  }
  

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
	return 0;
}
float rotate = 0.f;
void display() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(programId);
  glUniformMatrix4fv(matrixId, 1 , GL_FALSE, &MVP[0][0]);

  glUniform3f(locLight, LightPosition.x, LightPosition.y, LightPosition.z);
  glUniformMatrix4fv(locM, 1, GL_FALSE, &Model[0][0]);
  glUniformMatrix4fv(locV, 1, GL_FALSE, &View[0][0]);
  scene->render();
  glUseProgram(0);
  SDL_GL_SwapWindow(window);
}

void input() {
  SDL_Event event;
  while(SDL_PollEvent(&event) != 0) {
    if(event.type == SDL_QUIT){
      quit = true;
    }
  }
}

void testMesh() {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  Vertex a,b,c,d;
  a.position = glm::vec3(-1.f, -1.f, -1.f);//-1.f);
  b.position = glm::vec3(1.f, -1.f, -1.f);//-1.f);
  c.position = glm::vec3(1.f, -1.f, 1.f);//-1.f);
  d.position = glm::vec3(-1.f, -1.f, 1.f);//-1.f);

  a.normal = b.normal = c.normal = d.normal = glm::vec3(0.f,1.f,0.f);
  a.color = b.color = c.color = d.color = glm::vec3(0.2,1.0,0.2);

  vertices.push_back(a);
  vertices.push_back(b);
  vertices.push_back(c);
  vertices.push_back(d);

  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  //  indices.push_back(3);
  indices.push_back(0);
  indices.push_back(2);
  indices.push_back(3);
  Vertex e,f,g,h;
  e.position = glm::vec3(-1.f, 1.f, -1.f);//-1.f);
  f.position = glm::vec3(1.f, 1.f, -1.f);//-1.f);
  g.position = glm::vec3(1.f, 1.f, 1.f);//-1.f);
  h.position = glm::vec3(-1.f, 1.f, 1.f);//-1.f);

  e.normal = f.normal = g.normal = h.normal = glm::vec3(0.f,-1.f,0.f);
  a.color = b.color = c.color = d.color = glm::vec3(0.2,1.0,0.2);

  vertices.push_back(e);
  vertices.push_back(f);
  vertices.push_back(g);
  vertices.push_back(h);

  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(4);
  indices.push_back(1);
  indices.push_back(5);
  indices.push_back(4);

  //indices.push_back(0);
  //indices.push_back(4);
  //indices.push_back(5);
  scene->addMesh(new Mesh(std::move(vertices), std::move(indices)));

}
void loadTeapot(const aiScene* aiScene) {

  for(unsigned int i = 0; i < aiScene->mNumMeshes; i++) {
    //std::vector<glm::vec3> vertices;
    //std::vector<glm::vec3> normals;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    aiMesh* mesh = aiScene->mMeshes[i];

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
