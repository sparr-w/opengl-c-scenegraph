#version 330
in vec2 vertexUV;

out float light;
out vec2 textureUV;
out vec4 texWeights;

uniform float xzScale;
uniform float yScale;
uniform float deltaU;
uniform float deltaV;
uniform float texRepeat;
uniform mat4 mvp;
uniform sampler2D heightMap;
uniform sampler2D splatMap;

void main() {
	vec3 p[5];
	vec2 uvDelta[5];
	uvDelta[0] = vec2(0,0); // this vertex
	uvDelta[1] = vec2(-deltaU, 0); uvDelta[2] = vec2(deltaU, 0); // either side in u
	uvDelta[3] = vec2(0, -deltaV); uvDelta[4] = vec2(0, deltaV); // either side in v
	for (int i = 0; i < 5; i++) {
		vec2 uv = vertexUV + uvDelta[i];
		float height = texture(heightMap, uv).r * yScale;
		p[i] = vec3(uv.x * xzScale, height, uv.y * xzScale);
	}
	vec3 n = -normalize(cross(p[2] - p[1], p[4] - p[3]));
	light = 0.3 + 0.7 * n.y;
	textureUV = vertexUV * texRepeat;
	
	// calculate texture weights
	//texWeights = texture(splatMap, vertexUV).rgb;
	
	// calculate texture usage
	// dominance: 1 - stone if steep, 2 - sand if below y, 3 - grass & dirt
	
	float stoneSteepness = 0.2f;
	float sandThreshold = 0.2f * yScale;
	
	if (n.y < stoneSteepness) {
		texWeights = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	} else if (p[0].y < sandThreshold) {
		texWeights = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		if (n.y < 0.8f) {
			texWeights = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		} else {
			texWeights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		}
	}
	
	gl_Position = mvp * vec4(p[0], 1);
}