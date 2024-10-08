#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "module.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <sstream>

using namespace std;


void printShaderLog(GLuint shader) {
    int len = 0;
    int chWrittn = 0;
    char* log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char*)malloc(len);
        glGetShaderInfoLog(shader, len, &chWrittn, log);
        cout << "Shader Info Log:" << log << endl;
        free(log);
    }
}

void printProgramLog(int prog) {
    int len = 0;
    int chWrittn = 0;
    char* log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char*)malloc(len);
        glGetShaderInfoLog(prog, len, &chWrittn, log);
        cout << "Program Info Log:" << log << endl;
        free(log);
    }
}

bool CheckOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        cout << "glError:" << glErr << endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}

string readShaderSource(const char* filePath) {
    string content;
    ifstream fileStream(filePath, ios::in);
    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}



GLuint createShaderProgram() {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;

    //create empty shaders
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    string vertShaderStr = readShaderSource("vertShader.glsl");
    string fragShaderStr = readShaderSource("fragShader.glsl");

    const char* vertShaderSrc = vertShaderStr.c_str();
    const char* fragShaderSrc = fragShaderStr.c_str();

    // //loads the GLSL code from the strings into the empty shader objects
    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glShaderSource(fShader, 1, &fragShaderSrc, NULL);


    glCompileShader(vShader);
    CheckOpenGLError();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != 1) {
        cout << "vertex compilation failed" << endl;
        printShaderLog(vShader);
    }

    glCompileShader(fShader);
    CheckOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != 1) {
        cout << "fragment compilation failed" << endl;
        printShaderLog(fShader);
    }

    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);
    glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        cout << "linking failed" << endl;
        printProgramLog(vfProgram);
    }

    return vfProgram;
}


GLuint loadTexture(const char* texImagePath) {
    GLuint textureID;
    textureID = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    //mipmapping
    glBindTexture(GL_TEXTURE_2D, textureID);
    //GL_LINEAR_MIPMAP_LINEAR gives trilinear filtering
    //GL_LINEAR_MIPMAP_NEAREST gives linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);


    //anisotropic filtering
    if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
        GLfloat anisoSetting = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting);
    }

    if (textureID == 0) cout << "could not find texture file" << texImagePath << endl;
    return textureID;
}
