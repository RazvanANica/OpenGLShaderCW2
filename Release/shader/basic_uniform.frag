#version 460

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (binding= 0) uniform sampler2D HdrTex;
layout (binding = 1) uniform sampler2D Tex1;
layout (binding = 2) uniform sampler2D Tex2;



layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec3 HdrColor;

//HDR
uniform int Pass;
uniform float AveLum;

//toonShading
const int level = 3;
const float scaleFactor = 1.0/level;

// XYZ/RGB conversion matrices from:
// http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html

uniform mat3 rgb2xyz = mat3(
 0.4124564, 0.2126729, 0.0193339,
 0.3575761, 0.7151522, 0.1191920,
 0.1804375, 0.0721750, 0.9503041 );

uniform mat3 xyz2rgb = mat3(
 3.2404542, -0.9692660, 0.0556434,
 -1.5371385, 1.8760108, -0.2040259,
 -0.4985314, 0.0415560, 1.0572252 );

uniform float Exposure = 0.35;
uniform float White = 0.928;
uniform bool DoToneMap = true;


//contains the variables for light 
uniform struct LightInfo{
   vec4 Position;
   vec3 La; //light ambient intensity
   vec3 Ld; //light diffuse intensity
   vec3 Ls; //light specular intensity
} Lights[3]; //multiple lights



//contains the variables for material 
uniform struct MaterialInfo{
   vec3 Ka; //material ambient reflectivity
   vec3 Kd; //material diffuse reflectivity
   vec3 Ks; //material specular reflectivity
   float Shininess; //material shininess
} Material;


vec3 blinnPhongReflection(int light, vec3 n, vec4 eyePos){

    vec4 Brick = texture(Tex1, TexCoord);
     vec4 Moss = texture(Tex2, TexCoord);
     vec3 texColour = mix(Brick.rgb, Moss.rgb, Moss.a); 

    vec3 ambient = Lights[light].La * Material.Ka * texColour ;

    vec3 s = normalize(vec3(Lights[light].Position-(eyePos*Lights[light].Position.w))); // calulates direction of light (normalized)

    float sDotN = max(dot(s,n),0.0); //calculates the dot product between the direction from the origin of light and the normal vector;
   
   vec3 diffuse = Lights[light].Ld * Material.Kd * sDotN * texColour; // calculates diffuse refelction
   vec3 specular = vec3(0.0);

    if(sDotN > 0.0){ 
       vec3 v = normalize(-eyePos.xyz);
       vec3 h = normalize(v + s);
       specular = Lights[light].Ls * Material.Ks * pow(max(dot(h,n), 0.0), Material.Shininess);
    }

    return ambient + diffuse + specular; //returns phong reflection model 
}

vec3 toonShading(int light, vec3 n, vec4 eyePos){
     
      vec4 Brick = texture(Tex1, TexCoord);
     vec4 Moss = texture(Tex2, TexCoord);
     vec3 texColour = mix(Brick.rgb, Moss.rgb, Moss.a); 

    vec3 ambient = Lights[light].La * Material.Ka * texColour;

    vec3 s = normalize(vec3(Lights[light].Position-(eyePos*Lights[light].Position.w))); // calulates direction of light (normalized)

    float sDotN = max(dot(s,n),0.0); //calculates the dot product between the direction from the origin of light and the normal vector;
   
   vec3 diffuse = Lights[light].Ld * Material.Kd* (floor(sDotN*level)* scaleFactor) * texColour; // calculates diffuse refelction
   vec3 specular = vec3(0.0);

    if(sDotN > 0.0){ 
       vec3 v = normalize(-eyePos.xyz);
       vec3 h = normalize(v + s);
       specular = Lights[light].Ls * Material.Ks * pow(max(dot(h,n), 0.0), Material.Shininess);
    }

    return ambient + diffuse + specular; //returns phong reflection model 
}




void pass1()
{
     vec3 n = normalize(Normal);
     // Compute shading and store result in high-res framebuffer
     HdrColor = vec3(0.0);
     for( int i = 0; i < 3; i++)
     HdrColor += toonShading(i, n,Position );

}


// This pass computes the sum of the luminance of all pixels
void pass2()
{
     // Retrieve high-res color from texture
     vec4 colour = texture( HdrTex, TexCoord );

     // Convert to XYZ
     vec3 xyzCol = rgb2xyz * vec3(colour);
     // Convert to xyY
     float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
     vec3 xyYCol = vec3( xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);
     // Apply the tone mapping operation to the luminance (xyYCol.z or xyzCol.y)
     float L = (Exposure * xyYCol.z) / AveLum;
     L = (L * ( 1 + L / (White * White) )) / ( 1 + L );
     // Using the new luminance, convert back to XYZ
     xyzCol.x = (L * xyYCol.x) / (xyYCol.y);
     xyzCol.y = L;
     xyzCol.z = (L * (1 - xyYCol.x - xyYCol.y))/xyYCol.y;
     // Convert back to RGB and send to output buffer
     if( DoToneMap )
     FragColour = vec4( xyz2rgb * xyzCol, 1.0);
     else
     FragColour = colour;
}





void main() { 
   if( Pass == 1) 
     pass1();
   else if( Pass == 2) 
    pass2();
}
