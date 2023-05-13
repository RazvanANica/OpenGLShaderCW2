#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>

#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"

#include <glm/gtc/matrix_transform.hpp>

class SceneBasic_Uniform : public Scene
{
private:
    
    GLSLProgram prog;
    GLuint hdrFBO;
    GLuint quad;
    GLuint hdrTex, avgTex;

    float angle;
   
    float move, up, rotate, moveObj;

    Plane plane;  //plane surface
    std::unique_ptr<ObjMesh> Vinayagar;  //model 1 Diety
    std::unique_ptr<ObjMesh> Plant; //model 2 Plant pot

  

    void compile();
    void setupFBO();
    void pass1();
    void pass2();
    void computeLogAveLuminance();
    void drawScene();


public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void setMatrices();
};

#endif // SCENEBASIC_UNIFORM_H
