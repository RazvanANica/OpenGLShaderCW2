#version 460

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (binding = 0) uniform sampler2D Tex1;
layout (binding = 1) uniform sampler2D Tex2;


layout (location = 0) out vec4 FragColor;


//contains the variables for light 
uniform struct LightInfo{
   vec4 Position;
   vec3 La; //light ambient intensity
   vec3 Ld; //light diffuse intensity
   vec3 Ls; //light specular intensity
} lights[3]; //multiple lights


const int level = 3;
const float scaleFactor = 1.0 / level;

//contains the variables for material 
uniform struct MaterialInfo{
   vec3 Ka; //material ambient reflectivity
   vec3 Kd; //material diffuse reflectivity
   vec3 Ks; //material specular reflectivity
   float Shininess; //material shininess
} Material;


vec3 blinnPhongReflection(int light, vec3 n, vec4 eyePos){

    vec4 brickColour = texture(Tex1, TexCoord);
    vec4 woodColour = texture(Tex2, TexCoord);

    vec3 texColour = mix(brickColour.rgb, woodColour.rgb, woodColour.a );

    vec3 ambient = lights[light].La * Material.Ka * texColour;

    vec3 s = normalize(vec3(lights[light].Position-(eyePos*lights[light].Position.w))); // calulates direction of light (normalized)

    float sDotN = max(dot(s,n),0.0); //calculates the dot product between the direction from the origin of light and the normal vector;
   
   vec3 diffuse = lights[light].Ld * Material.Kd * sDotN * texColour; // calculates diffuse refelction
   vec3 specular = vec3(0.0);

    if(sDotN > 0.0){ 
       vec3 v = normalize(-eyePos.xyz);
       vec3 h = normalize(v + s);
       specular = lights[light].Ls * Material.Ks * pow(max(dot(h,n), 0.0), Material.Shininess);
    }

    return ambient + diffuse + specular; //returns phong reflection model 
}

vec3 toonShading(int light, vec3 n, vec4 eyePos){

    vec3 ambient = lights[light].La * Material.Ka;

    vec3 s = normalize(vec3(lights[light].Position-(eyePos*lights[light].Position.w))); // calulates direction of light (normalized)

    float sDotN = max(dot(s,n),0.0); //calculates the dot product between the direction from the origin of light and the normal vector;
   
   vec3 diffuse = lights[light].Ld * Material.Kd* (floor(sDotN * level) *scaleFactor); // calculates diffuse refelction
   vec3 specular = vec3(0.0);

    if(sDotN > 0.0){ 
       vec3 v = normalize(-eyePos.xyz);
       vec3 h = normalize(v + s);
       specular = lights[light].Ls * Material.Ks * pow(max(dot(h,n), 0.0), Material.Shininess);
    }

    return ambient + diffuse + specular; //returns phong reflection model 
}
void main() {
   vec3 Colour = vec3(0.0);
  
  for(int i = 0; i < 3; i++)
       Colour += blinnPhongReflection(i, Normal, Position);
   
   FragColor = vec4(Colour, 2.0);
}
