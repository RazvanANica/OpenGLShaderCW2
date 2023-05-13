#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ProjectionMatrix;

void getCamSpaceValues(out vec3 normal, out vec4 position){
      normal = normalize(NormalMatrix* VertexNormal);
      position = (ModelViewMatrix * vec4(VertexPosition,1.0));
}
void main()
{
    TexCoord = VertexTexCoord;
    getCamSpaceValues(Normal, Position);   
    gl_Position =MVP*vec4(VertexPosition, 1.0);
   
}
