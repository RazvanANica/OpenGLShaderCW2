#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::mat4;



SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 10.0f, 100, 100) {
    Vinayagar = ObjMesh::load("media/Vinayagar.obj", true); //load first mesh
    Plant = ObjMesh::load("media/Pot.obj", true); //load second mesh
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(0.0f, 4.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 1.0f));
    projection = mat4(1.0f);

    //values for Light uniforms
    float x, z;
    for (int i = 0; i < 3; i++) {
        std::stringstream name;
        name << "lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
    }

    //diffuse
    prog.setUniform("lights[0].Ld", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("lights[1].Ld", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("lights[2].Ld", vec3(0.5f, 0.5f, 0.5f));

    //ambient
    prog.setUniform("lights[0].La", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("lights[1].La", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("lights[2].La", vec3(0.2f, 0.2f, 0.2f));

    //specular
    prog.setUniform("lights[0].Ls", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("lights[1].Ls", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("lights[2].Ls", vec3(0.8f, 0.8f, 0.8f));

    GLuint tex1 = Texture::loadTexture("media/texture/Brick.jpg");
    GLuint tex2 = Texture::loadTexture("media/texture/Wood.jpg");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex2);

}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    

	//update your angle here
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog.setUniform("Material.Kd", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Ks", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Ka", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Shininess", 30.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    Plant->render();


    prog.setUniform("Material.Kd", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Ka", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Shininess", 30.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-1.0f, 1.5f, 0.0f));
    model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    Vinayagar->render();

    prog.setUniform("Material.Kd", 0.4f, 0.5f, 0.3f);
    prog.setUniform("Material.Ks", 0.4f, 0.5f, 0.3f);
    prog.setUniform("Material.Ka", 0.4f, 0.5f, 0.3f);
    prog.setUniform("Material.Shininess", 30.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(1.0f, 1.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    Vinayagar->render();


    prog.setUniform("Material.Kd", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Ka", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Shininess", 50.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    setMatrices();
    plane.render();
}



void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

}

void SceneBasic_Uniform::setMatrices() {
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]),vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv );
    prog.setUniform("ProjectionMatrix", projection);

}
