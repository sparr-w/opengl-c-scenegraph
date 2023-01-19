#version 330
in float light;
in vec2 textureUV;
in float texDelta;

out vec4 finalColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
	vec4 mat = texture(tex0, textureUV) * texDelta + texture(tex1, textureUV) * (1 - texDelta);
	finalColor = vec4(mat.rgb * light, 1);
}