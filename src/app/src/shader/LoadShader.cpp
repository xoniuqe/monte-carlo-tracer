#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "LoadShader.h"

//Kopiert von opengl-tutorials.org
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {
  GLuint vertexSID = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentSID = glCreateShader(GL_FRAGMENT_SHADER);

  std::string vertexSCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if(VertexShaderStream.is_open()) {
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      vertexSCode += "\n" + Line;
    VertexShaderStream.close();
  } else {
    //error
    return 0;
  }
  std::string fragmentSCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
      fragmentSCode += "\n" + Line;
    FragmentShaderStream.close();
  } else {
    //error
    return 0;
  }
  GLint Result = GL_FALSE;

  char const * vertexSPointer = vertexSCode.c_str();
  glShaderSource(vertexSID, 1, &vertexSPointer, NULL);
  glCompileShader(vertexSID);

  int InfoLogLength;
  glGetShaderiv(vertexSID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(vertexSID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if(InfoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(vertexSID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  char const * fragmentSPointer = fragmentSCode.c_str();
  glShaderSource(fragmentSID, 1, &fragmentSPointer, NULL);
  glCompileShader(fragmentSID);

  glGetShaderiv(fragmentSID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(fragmentSID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if(InfoLogLength > 0) {
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(fragmentSID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  GLuint ProgramId = glCreateProgram();
  glAttachShader(ProgramId, vertexSID);
  glAttachShader(ProgramId, fragmentSID);
  glLinkProgram(ProgramId);

  glDetachShader(ProgramId, vertexSID);
  glDetachShader(ProgramId, fragmentSID);

  glDeleteShader(vertexSID);
  glDeleteShader(fragmentSID);

  return ProgramId;
}
