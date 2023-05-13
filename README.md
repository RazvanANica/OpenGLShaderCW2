# Visual Studio 2019
# Windows 11

# How does it work:
The program loads the scene element and two object meshes, sets the camera's position, and assigns uniform values for the multiple light variables. Afterwards, the material uniform values are set, the model is added, and its position is transformed.

The vertex and fragment shaders take care of the lighting and reflectivity of the scene and objects. The fragment shader calculates the Blinn Phong Reflection Model as well as the Toon Shading Technique, the fragment shader also has HDR implementation and multi-texture. 

SLN: The user has to press on Local Windows Debugger and it will run the program, once everything is loaded the program will display a scene with multiple models and a plane. 

EXE: The executable file has to be opened and it will take the information from the shaders, media, and helper folders. Once everything is loaded the user can see a scene with two elephant-like deities and a plant in the middle. By using WASD, the user is able to move the camera up, down, left and right, the Q and E keys rotate the camera on the y-axis around the centre point of the scene. By pressing O and P the user will get object movement in the scene.

## Keys:
WASD - camera movement
QE - camera rotation
OP - objects movement

# Textures :
https://www.textures.com/download/BrickOldRounded0310/123888 -brick
The moss is provided in the DLE.

# Code:

Vertex shader:
The vertex shader has variables which store information based on the name they have. It also defines the getCamSpaceValues function which computes the camera space values of the vertex normal and position. 

Fragment shader:
The fragment shader takes care of the HDR, toon shading, multi-lights and multi-textures. It has 3 binding layouts for the HDR and the multi-texture. All of the uniforms and constants are named in a way that lets the user know what they store. There are two uniform structs that are defined in the fragment shader. One is storing the values for the multiple lights, such as the position, and the intensity of the ambient, diffuse, and specular light. The other struct is responsible for the material ambient, diffuse, and specular reflectivity, as well as the shininess of the material itself. 

In the fragment shader, there are two matrices responsible for converting the XYZ to RGB and the RGB to XYZ. These two matrices are used for the implementation of the HDR, and they can be found in the pass2() function.
The fragment shader has 4 methods defined besides the main().

blinnPhongReflection() - handles the Blinn Phong Reflection technique by calculating the ambient diffuse and specular components of the model, as well as implementing the multi-texture. This method is not used.

toonShading() - this method calculates the multi-textures and the toon shading model. It does so based on the blinn phong model but the diffuse is calculated using the floor of the dot product between the light direction and the normal vector multiplied by a scale factor. This model does not look as intended because of the way the multi-texture is applied, but it gives a nice light effect on the plane.
 
pass1() - calculates the toon shading for all the 3 lights in the scene, and it returns it as HDR.

pass2() - computes the sum of the luminance of all pixels by retrieving the high-res colour from the texture. Then it converts it to XYZ, it applies the tone mapping, converts it to RGB and sends it to the buffer.

main() - calls the pass 1 and pass 2 functions

Scene Basic:
SceneBasic_Uniform() - The class's constructor loads the Vinayagar and Pot models.

initScene() - Initializes the scene by enabling depth testing, setting up the projection matrice, calling the FBO, and setting the values for the multiple lights uniforms, as well as loading the textures in the scene.

compile() - Compiles the shaders and links the program.

render() - Calls the pass1(), computeLogAveLuminance() and the pass2() functions.

update() - The update method handles the movement commands as well as raising the value of the angle variable which handles the pot rotation into the scene.

isKeyDown() - checks if a key is pressed down, by using the GetAsyncKeyState function.

setupFBO() - Creates an FBO which is used to render the scene with HDR.
pass1() - this is the first rendering pass of the scene. It sets up the view and projection matrices, clears the frame buffer, enables depth testing and it binds the HDR frame buffer. In the end, it calls the drawScene() method.
pass2() - this function renders the final pass of the scene. it sets the Pass uniform to 2, which triggers the shader to apply tone mapping to the HDR colour buffer from pass1. It binds the default frame buffer, clears the colour and the depth buffers, and renders the quad that is used to display the final rendered image. This pass is part of the HDR implementation.
computeLogAveLuminance() - computes the average luminance used for the HDR.
drawScene() - This function sets up the positions of the lights and objects in the scene, as well as the materials.

# YouTube: https://youtu.be/3y2KsHIo7hA