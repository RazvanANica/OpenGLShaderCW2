#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;


//contains the variables for light 
uniform struct LightInfo{
   vec4 Position;
   vec3 La; //light ambient intensity
   vec3 Ld; //light diffuse intensity
   vec3 Ls; //light specular intensity
} Light;


//contains the variables for material 
uniform struct MaterialInfo{
   vec3 Ka; //material ambient reflectivity
   vec3 Kd; //material diffuse reflectivity
   vec3 Ks; //material specular reflectivity
   float Shininess; //material shininess
} Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;


vec3 phongReflection(vec3 n, vec4 eyePos){

    vec3 ambient = Light.La * Material.Ka;

    vec3 s = normalize(vec3(Light.Position-eyePos)); // calulates direction of light (normalized)

    float sDotN = max(dot(s,n),0.0); //calculates the dot product between the direction from the origin of light and the normal vector;
   
   vec3 diffuse = Light.Ld * Material.Kd* sDotN; // calculates diffuse refelction
   vec3 specular = vec3(0.0);

    if(sDotN > 0.0){ 
       vec3 v = normalize(-eyePos.xyz);
       vec3 r = reflect(-s, n);
       specular = Light.Ls * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess);
    }

    return ambient + diffuse + specular; //returns phong reflection model 
}

void getCamSpaceValues(out vec3 normal, out vec3 position){
      normal = normalize(NormalMatrix* VertexNormal);
      position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
}
void main()
{
   vec3 camNormal, camPosition;
   getCamSpaceValues(camNormal, camPosition);

    LightIntensity = phongReflection(camNormal,vec4(camPosition, 1.0));
   
    gl_Position = MVP*vec4(VertexPosition, 1.0);
}
