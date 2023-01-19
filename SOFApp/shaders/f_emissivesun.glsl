#version 330

uniform vec4 color0;
uniform vec4 color1;
uniform float waveFrequency;
uniform float elapsed;

in float vertYpos;

out vec4 finalColor;

void main() {
	float blend = abs(sin((vertYpos * waveFrequency) + elapsed));
	
	float Rchannel = (color0.x * blend) + (color1.x * (1 - blend));
	float Gchannel = (color0.y * blend) + (color1.y * (1 - blend));
	float Bchannel = (color0.z * blend) + (color1.z * (1 - blend));
	
	vec4 color = vec4(Rchannel, Gchannel, Bchannel, 1.0f);

	finalColor = color;
}