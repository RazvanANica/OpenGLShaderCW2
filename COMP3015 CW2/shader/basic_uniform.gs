#version 460

layout (points) in;
layout (triangle_strip, max_verices = 4) out;

uniform float Size2;

uniform mat4 ProjectionMatrix;

out vec2 TexCoord;

void main(){
	mat4 m = ProjectionMatrix;
	//Vertex 1
	gl_Position = m*(vec4(-Size2, -Size2, 0.0, 0.0)) + gl_in[0].gl_Position);
	TexCoord = vec2(0.0,0.0);
	EmitVertex();

	//Vertex 2
	gl_Position = m*(vec4(-Size2, -Size2, 0.0, 0.0)) + gl_in[0].gl_Position);
	TexCoord = vec2(1.0,0.0);
	EmitVertex();

	//Vertex 3
	gl_Position = m*(vec4(-Size2, -Size2, 0.0, 0.0)) + gl_in[0].gl_Position);
	TexCoord = vec2(0.0,1.0);
	EmitVertex();

	//Vertex 4
	gl_Position = m*(vec4(-Size2, -Size2, 0.0, 0.0)) + gl_in[0].gl_Position);
	TexCoord = vec2(1.0,1.0);
	EmitVertex();

	EndPrimitive();
}