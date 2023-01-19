#version 330
in vec2 vertexUV;

out float light;
out vec2 textureUV;
out float texDelta;

uniform float xzScale;
uniform float yScale;
uniform float deltaU;
uniform float deltaV;
uniform float texRepeat;
uniform mat4 mvp;
uniform sampler2D heightMap;
uniform sampler2D heightMapAlt;
uniform float heightDelta;

void main() {
	vec3 p[5];
	vec2 uvDelta[5];
	uvDelta[0] = vec2(0,0); // this vertex
	uvDelta[1] = vec2(-deltaU, 0); uvDelta[2] = vec2(deltaU, 0); // either side in u
	uvDelta[3] = vec2(0, -deltaV); uvDelta[4] = vec2(0, deltaV); // either side in v
	
	for (int i = 0; i < 5; i++) {
		vec2 uv = vertexUV + uvDelta[i];
		
		float height = texture(heightMap, uv).r * yScale;
		float heightAlt = texture(heightMapAlt, uv).r * yScale;
		
		float currentHeight = (height * heightDelta) + (heightAlt * (1 - heightDelta));
		
		p[i] = vec3(uv.x * xzScale, currentHeight, uv.y * xzScale);
	}
	vec3 n = -normalize(cross(p[2] - p[1], p[4] - p[3]));
	light = 0.3 + 0.7 * n.y;
	
	textureUV = vertexUV * texRepeat;
	texDelta = heightDelta;
	
	gl_Position = mvp * vec4(p[0], 1);
}