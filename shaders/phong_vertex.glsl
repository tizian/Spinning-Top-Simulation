#version 150

uniform mat4 model, view, proj;		// Uniforms: M, V, P matrices

// In: per vertex information in model space
in vec3 vPosition;	// In: (x,y,z) position
in vec3 vNormal;	// In: (x,y,z) normal values
//in vec2 vTexCoord;	// In: (u,v) texture coordinates

// Out: per fragment information
out vec3 fPosition;		// Out: (x,y,z) position
out vec3 fNormal;		// Out: (x,y,z) normal values
//out vec2 fTexCoord;	// Out: (u,v) texture coordinates

void main() {
	fPosition = vPosition;
	fNormal = vNormal;
	//fTexCoord = vTexCoord;

	gl_Position = proj * view * model * vec4(vPosition, 1);
}