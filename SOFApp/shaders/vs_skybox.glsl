#version 420
in vec3 vertexPos;

out vec3 texCoord;

uniform vec3 cameraPos;
uniform mat4 viewProj;

void main () {
	texCoord = normalize(vertexPos);
	gl_Position = viewProj * vec4(vertexPos + cameraPos, 1.0f);
}