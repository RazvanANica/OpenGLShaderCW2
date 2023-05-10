# Visual Studio 2019
# Windows 11

I was unable to make the video 2K it is HD, I hope that is acceptable

# How does it work

The program loads the scene element and the two object meshes. It sets the position of the camera and uniform values for the light variables. The material uniform values are set, then the model is added and its position is transformed. 

The vertex and fragment shaders take care of the lighting and reflectivity of the scene and objects. The vertex shader calculates the Phong Reflection Model. 

EXE: The executable file has to be opened and it will take the information from the shaders, media, and helper folders. Once everything is loaded the user can see a scene with two elephant-like deities and a plant in the middle.

SLN: The user has to press on Local Windows Debugger and it will run the program. 

#Textures :
https://www.textures.com/download/BrickOldRounded0310/123888 -brick
https://www.textures.com/download/WoodPlanksOld0210/117308 -wood


# Code

Vertex shader:
The code is pretty explicit. The variables are named based on what information they store. There are two structs that store variables intended to use for Light and Material values.
The function phongReflection() calculates the Phong reflection model for a given normal vector and the position of the viewer. It computes the ambient, diffuse, and specular components of the reflection and returns their sum.

Fragment shader:
The fragment shader just takes in the light intensity and outputs it as a vector 4 through FragColor.

Scene Basic:
SceneBasic_Uniform() - The constructor of the class which loads the Vinayagar and Pot models.

initScene() - Initializes the scene by enabling depth testing, setting up the view and projection matrices, and setting values for the light uniforms.

compile() - Compiles the shaders and links the program.

render() - Renders the scene by setting the material properties and model matrix for each model and rendering them one by one using the ObjMesh class's render() function. It sets the material properties and model matrix for the plane and renders it.

# YouTube : https://youtu.be/GgP-e1JDAmo 