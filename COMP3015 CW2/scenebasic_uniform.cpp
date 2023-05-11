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
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    projection = mat4(1.0f);
    angle = glm::pi<float>() / 4.0f;

    setupFBO();

    // Array for full-screen quad
    GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Texture coordinates
    glBindVertexArray(0);
    prog.setUniform("EdgeThreshold", 0.2f);


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
    prog.setUniform("lights[0].Ld", vec3(0.2f));
    prog.setUniform("lights[1].Ld", vec3(0.2f));
    prog.setUniform("lights[2].Ld", vec3(0.2f));

    //ambient
    prog.setUniform("lights[0].La", vec3(0.2f));
    prog.setUniform("lights[1].La", vec3(0.2f));
    prog.setUniform("lights[2].La", vec3(0.2f));

    //specular
    prog.setUniform("lights[0].Ls", vec3(0.2f));
    prog.setUniform("lights[1].Ls", vec3(0.2f));
    prog.setUniform("lights[2].Ls", vec3(0.2f));

    float weights[5], sum, sigma2 = 8.0f;
    // Compute and sum the weights
    weights[0] = gauss(0, sigma2);
    sum = weights[0];
    for (int i = 1; i < 5; i++) {
        weights[i] = gauss(float(i), sigma2);
        sum += 2 * weights[i];
    }
    // Normalize the weights and set the uniform
    for (int i = 0; i < 5; i++) {
        std::stringstream uniName;
        uniName << "Weight[" << i << "]";
        float val = weights[i] / sum;
        prog.setUniform(uniName.str().c_str(), val);
        GLuint tex1 = Texture::loadTexture("media/texture/Brick.jpg");
        GLuint tex2 = Texture::loadTexture("media/texture/Wood.jpg");

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex2);

    }
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
    angle = t * 10.0f;
}

void SceneBasic_Uniform::render()
{   
    pass1();
    pass2();
    pass3();
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

void SceneBasic_Uniform::setupFBO() {
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &renderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
    // Create the texture object
    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        renderTex, 0);
    // Create the depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depthBuf);
    // Set the targets for the fragment output variables
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    // Create the texture object
    glGenTextures(1, &intermediateTex);
    glActiveTexture(GL_TEXTURE0); // Use texture unit 0
    glBindTexture(GL_TEXTURE_2D, intermediateTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        intermediateTex, 0);
    // No depth buffer needed for this FBO
    // Set the targets for the fragment output variables
    glDrawBuffers(1, drawBuffers);
    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::pass1()
{
    prog.setUniform("Pass", 1);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = glm::lookAt(vec3(0.0f, 4.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 1.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height,
        0.3f, 100.0f);
    prog.setUniform("Material.Kd", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Ks", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Ka", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    Plant->render();


    prog.setUniform("Material.Kd", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Ka", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-1.0f, 1.5f, 0.0f));
    model = glm::rotate(model, glm::radians(45.0f * angle), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    Vinayagar->render();

    prog.setUniform("Material.Kd", 0.4f, 0.5f, 0.3f);
    prog.setUniform("Material.Ks", 0.4f, 0.5f, 0.3f);
    prog.setUniform("Material.Ka", 0.4f, 0.5f, 0.3f);
    prog.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(1.0f, 1.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-45.0f * angle), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    Vinayagar->render();


    prog.setUniform("Material.Kd", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Ka", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Shininess", 1.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    setMatrices();
    plane.render();
}

void SceneBasic_Uniform::pass2()
{
    prog.setUniform("Pass", 2);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices();
    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::pass3()
{
    prog.setUniform("Pass", 3);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intermediateTex);
    glClear(GL_COLOR_BUFFER_BIT);
    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices();
    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

float SceneBasic_Uniform::gauss(float x, float sigma2)
{
    double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
    double expon = -(x * x) / (2.0 * sigma2);
    return (float)(coeff * exp(expon));
}