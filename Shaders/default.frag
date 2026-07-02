#version 460 core
out vec4 FragColor;

uniform float u_TimeFactor;
uniform int u_TriangleID;

vec3 HueToRGB(float hue) {
	vec3 k = vec3(1.0, 2.0 / 3.0, 1.0 / 3.0);
	vec3 p = abs(fract(vec3(hue) + k) * 6.0 - vec3(3.0));
	return clamp(p - vec3(1.0), 0.0, 1.0);
}

void main() {
	float shift = (u_TriangleID == 1) ? 0.5 : 0.0;
	float finalHue = fract(u_TimeFactor + shift);
	vec3 rgbColor = HueToRGB(finalHue);
	FragColor = vec4(rgbColor, 1.0);
}