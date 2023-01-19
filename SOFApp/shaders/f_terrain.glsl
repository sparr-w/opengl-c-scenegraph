#version 330
out vec4 finalColor;
in float light;
in vec2 textureUV;
in vec4 texWeights;
in float textureBlend;

uniform sampler2D tex0; // top soil, upper faces
uniform sampler2D tex1; // upper faces, not too steep dirt
uniform sampler2D tex2; // upper faces, steep stones
uniform sampler2D tex3; // lower faces, near water, sand

void main() {
	vec4 mat = texture(tex0, textureUV) * texWeights.x +
			   texture(tex1, textureUV) * texWeights.y +
			   texture(tex2, textureUV) * texWeights.z +
			   texture(tex3, textureUV) * texWeights.w;
	
	finalColor = vec4(mat.rgb * light, 1);
}