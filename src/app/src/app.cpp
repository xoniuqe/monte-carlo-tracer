#include<Windows.h>
#include<iostream>

#include<SDL2/SDL.h>
#include<GL/glew.h>

#include<assimp/scene.h>
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>

#include<string>
#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<thread>
#include<chrono>

#include "boost/config.hpp"
#include "boost/program_options.hpp"

#include "ArcBall.h"
#include "Camera.h"
#include "MonteCarloPathtracer.h"
#include "Scene.h"
#include "shader/LoadShader.h"
#include "util/PointLight.h"
#include "util/AreaLight.h"
#include "util/Material.h"

#include "geometry/geometry.h"
#include "geometry/Octree.h"
#include "geometry/OctreeNode.h"

#include "util/Image.h"

using namespace std::chrono;

int setupWindow(int width, int height);
void setupScreenTexture();
void renderScene();
void fillTexture();
void renderTracerTexture();
void display();
void input();
void loadScene(const aiScene* scene);
int exit();
void start_pathtracer();
void save_tga();


SDL_Window* _window;
SDL_GLContext _context;
bool _quit = false;
Scene* _scene;

std::thread _render_thread;

GLuint _screen_vbo, _program_id, _tracer_texture;
GLuint _matrix_id, _loc_m, _loc_v, _loc_light;
glm::mat4 _mvp, _model;
glm::vec3 _light_position;
Camera* _camera;
MonteCarloPathtracer* _monte_carlo_pathtracer;
int _render_mode = 1;
int _width = 800;
int _height = 600;
int _render_screen_width = 200;
int _render_screen_height = 150;
int _max_recursion = 3;
int _num_samples = 64;
int _antialiasing = 0;
bool _directstart = false;
bool _save_tga = true;
//string _filename;
bool _exit_after_render = false;
ArcBall* _arcball; 
bool _is_tracing = false;
glm::vec3 _mouse_position;

namespace po = boost::program_options;

#ifdef _WIN32
#undef main
#endif



int main(int argc, char * argv[]) {

po::options_description desc("Raytracer Options");
desc.add_options()
("width", po::value<int>()->default_value(1024), "Screen width")
("height", po::value<int>()->default_value(720), "Screen height")
("r-width", po::value<int>()->default_value(1024), "Render texture width")
("r-height", po::value<int>()->default_value(720), "Render texture height")
("max-rec", po::value<int>()->default_value(3), "Maximum recursion depth")
("samples", po::value<int>()->default_value(32), "Number of samples")
("aa", po::value<int>()->default_value(0), "Antiliasing level (not implemented)")
("direct-start", po::value<bool>()->default_value(true), "Direct start of rendering")
("tga", po::value<bool>()->default_value(false), "save as tga")
//("filename", po::value<string>()->default_value("output.tga"), "target filename for tga export (needs .tga)")
("exit", po::value<bool>()->default_value(true), "exit after render?");

po::variables_map vm;
try {
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	_height = vm["height"].as<int>();
	_width = vm["width"].as<int>();
	_render_screen_height = vm["r-height"].as<int>();
	_render_screen_width = vm["r-width"].as<int>();
	_max_recursion = vm["max-rec"].as<int>();
	_num_samples = vm["samples"].as<int>();
	_antialiasing = vm["aa"].as<int>();
	_directstart = vm["direct-start"].as<bool>();
	_save_tga = vm["tga"].as<bool>();
	//_filename = vm["filename"].as<string>();
	_exit_after_render = vm["exit"].as<bool>();
	//std::cout << _filename << std::endl;
	//std::cout << _height << " " << _width << " " << _antialiasing << "\n";
}
catch (po::error& e) {
	std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
	std::cerr << desc << std::endl;
	return 1;
}
std::cout << _render_screen_height << "; " << _render_screen_width << "; " << _max_recursion << ";" << _num_samples << "\n";
if (setupWindow(_width, _height)) {
	return 1;
}
glewInit();


_program_id = LoadShaders("../shader/vert.glsl", "../shader/frag.glsl");
glUseProgram(_program_id);
_matrix_id = glGetUniformLocation(_program_id, "MVP");
_loc_m = glGetUniformLocation(_program_id, "V");
_loc_v = glGetUniformLocation(_program_id, "M");
_loc_light = glGetUniformLocation(_program_id, "lightPosition");

glUseProgram(0);

_camera = new Camera(glm::vec3(0, 0, -2), glm::vec3(0, 0, 0), 45.f, _render_screen_width / _render_screen_height);
_model = glm::mat4(1.f);

float radius = sqrtf(float(_width * _width + _height * _height)) * 0.5f;
_arcball = new ArcBall();
_arcball->place(glm::vec3(0, 0, 0), 3.f);

_mvp = _camera->_projection * _camera->_view * _model;

_light_position = glm::vec3(0.f, 0.85f, 0.f);

_scene = new Scene();

AreaLight* light = new AreaLight(_light_position, 0.89122f, 0.89122f);
light->color = glm::vec3(1.f, 1.f, 1.f);
light->power = 1.f;
_scene->addLight(light);
Assimp::Importer importer;
const aiScene* ai_scene1 = importer.ReadFile("../data/scene-mirror-with-light.obj", aiProcess_Triangulate | aiProcess_GenNormals);
if (ai_scene1) {
	loadScene(ai_scene1);
}
else {
	std::cerr << std::string(importer.GetErrorString()) << std::endl;
}


setupScreenTexture();

_scene->calculateOctree();
_monte_carlo_pathtracer = new MonteCarloPathtracer(_scene, _camera, _render_screen_width, _render_screen_height, _num_samples, _max_recursion, _antialiasing);
while (!_quit) {
	if (_directstart && !_is_tracing) {
		_render_mode = 0;
		start_pathtracer();
	}
	if (!_directstart) {
		input();
	}
	display();
}
return exit();
}

void save_tga() {
	TGAImage *img = new TGAImage(_render_screen_width, _render_screen_height);

	//declare a temporary color variable
	Colour c;

	//Loop through image and set all pixels to red
	//_image[x + (_screen_width * y)]
	for (int x = 0; x < _render_screen_width; x++) {
		for (int y = 0; y < _render_screen_height; y++) {
			glm::u8vec3 color = _monte_carlo_pathtracer->_image[x + (_render_screen_width * y)];
			c.r = color.r;
			c.g = color.g;
			c.b = color.b;
			c.a = 255;
			img->setPixel(c, y, x);
		}
	}	

	//write the image to disk
	string filename =  "output.tga";
	img->WriteImage(filename);

}
void start_pathtracer() {
	if (!_is_tracing) {
		_render_thread = std::thread([=]() {
			//std::cout << "Measurement resolution: " <<
			//	duration_cast<nanoseconds>(steady_clock::duration(1)).count()
			//		<< "ns" << std::endl;
			std::cout << "start\n";
			auto start = steady_clock::now();
			_monte_carlo_pathtracer->startPathtracing();
			auto end = steady_clock::now();
			auto dur = duration_cast<milliseconds>(end - start);
			std::cout << "end\n";

			std::cout << "Measured time: ;" << dur.count() << "ms" << std::endl;
			std::cout << "Delta: ;" <<
				duration_cast<milliseconds>(dur - seconds(1)).count()
				<< "ms" << std::endl;
			if (_save_tga) {
				save_tga();
			}
			if (_exit_after_render) {
				_quit = true;
			}
		});
		_is_tracing = true;
	}
}
int exit() {
	delete _monte_carlo_pathtracer;
	delete _camera;
	delete _scene;
	delete _arcball;
	//delete ai_scene1;
	for (auto mat : Material::materials) {
		delete mat.second;
	}
	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);
	SDL_Quit();
	return 0;
}

float rotate = 0.f;
void renderTracerTexture() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, _tracer_texture);
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
    glUseProgram(_program_id);
    glUniformMatrix4fv(_matrix_id, 1 , GL_FALSE, &_mvp[0][0]);
    
    glUniform3f(_loc_light, _light_position.x, _light_position.y, _light_position.z);
    glUniformMatrix4fv(_loc_m, 1, GL_FALSE, &_model[0][0]);
    glUniformMatrix4fv(_loc_v, 1, GL_FALSE, &_camera->_view[0][0]);
    _scene->render();
    glUseProgram(0);
}

void display() {
    glClearColor(0.0,0.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    switch(_render_mode) {
    case 0:
        renderTracerTexture();
    break;
    case 1:
        renderScene();
    break;
    }
    SDL_GL_SwapWindow(_window);
}

void input() {
    int x,y;
    SDL_GetMouseState(&x, &y);
    _mouse_position.x = 2.f * ((float) x / _width) -1.f;
    _mouse_position.y = 2.f * ((float) y / _height) - 1.f;
    _mouse_position.z = 0.f;
    _arcball->setMouse(_mouse_position);
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0) {
    if(event.type == SDL_QUIT){
        _quit = true;
    }
    //arcball camera rotation, not used because its bugged
    /* if(event.button.button == SDL_BUTTON_LEFT) {//type == SDL_MOUSEBUTTONDOWN) {
        int x,y;
        _arcball->setMouse(_mouse_position);
        if(event.type == SDL_MOUSEBUTTONDOWN) {
            std::cout << "left click!" << std::endl;
            _arcball->beginDragging();
      }
        else if(event.type == SDL_MOUSEBUTTONUP) {
        std::cout << "left release!" << std::endl;
        _arcball->endDragging();
        }
        _camera->_origin = glm::vec3(glm::inverse(_arcball->getRotationMatrix()) * glm::vec4(_camera->_origin,1));
        _camera->_up = glm::vec3(glm::inverse(_arcball->getRotationMatrix()) * glm::vec4(_camera->_up,0));
        
        _camera->_view = glm::lookAt(_camera->_origin, _camera->_center, _camera->_up);
        _mvp = _camera->_projection * _camera->_view * _model;
        }*/
    if(event.type == SDL_KEYUP) {
        if(event.key.keysym.sym == SDLK_1){
            _render_mode += 1;
            _render_mode %= 2;
        }
        if(event.key.keysym.sym == SDLK_r) {        
			start_pathtracer();
        }
    }
    }
}
int reflect = 0;
int refract = 0;
void loadScene(const aiScene* aiScene) {
    std::map<int, int> material_mapping = {};
    std::vector<glm::vec3> colors;
    for(auto i = 0; i < aiScene->mNumMeshes; i++) {
        int index = aiScene->mMeshes[i]->mMaterialIndex;
        aiMaterial *mtl = aiScene->mMaterials[index];
		aiColor4D diffuse, emitting;
        
        Material *mat = Material::new_material();
        material_mapping[index] = mat->index;
       // mat->shininess = 50.f; //specular shininess
		//mtl->Get(AI_MATKEY_REFLECTI, mat->reflectivity);
       // mtl->Get(AI_MATKEY_REFRACTI, mat->refraction_index);
		mtl->Get(AI_MATKEY_SHININESS, mat->shininess);
		mtl->Get(AI_MATKEY_REFRACTI, mat->reflectivity);
        mtl->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        mtl->Get(AI_MATKEY_COLOR_EMISSIVE, emitting);
		mat->emitting = glm::vec3(emitting.r, emitting.g, emitting.b);
        mat->diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
		//if (mat->index == 4) {
		//	mat->emitting = glm::vec3(1, 1, 1);
		//}
        //if(reflect == 1) { //adding random relfection 
        //    mat->reflectivity = 1.f;
        //}
        //reflect++;        
        if(refract == 0) { //added random refraction, currently not used
            mat->refraction_index = 5.f;
        }
        refract++;
        
        if(aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS) {
            glm::vec3 color(diffuse.r, diffuse.g, diffuse.b);
            colors.push_back(color);
        }
    }
    for(auto i = 0; i < aiScene->mNumMeshes; i++) {
        
        int index = aiScene->mMeshes[i]->mMaterialIndex;
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
            
            vert.color = colors[i];
            vertices.push_back(vert);
            
        }
        for(unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            for(unsigned int k = 0; k < face.mNumIndices; k++) {
                indices.push_back(face.mIndices[k]);
            }
        }
        //std::cout << "added mesh\n";
        auto _mesh = new Mesh(std::move(vertices), std::move(indices));
        _mesh->material(material_mapping[index]);
        _scene->addMesh(_mesh);
    }
    
}

int setupWindow(int width, int height) {
    // SDL mit dem Grafiksystem initialisieren.
  if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
      std::cerr << "Konnte SDL nicht initialisieren! Fehler: " << SDL_GetError() << std::endl;
      return 1;
    }
  _window = SDL_CreateWindow("Monte Carlo Raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
	if(_window == NULL) {
		//std::cerr << "_window is null! << SDl_GetError() << std::endl;
		return 1;
	}
	_context = SDL_GL_CreateContext(_window);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetSwapInterval(1);

  return 0;
}

void setupScreenTexture() {
  glGenTextures(1, &_tracer_texture);
  glBindTexture(GL_TEXTURE_2D, _tracer_texture);

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
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _render_screen_width, _render_screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, _monte_carlo_pathtracer->_image);
 }
