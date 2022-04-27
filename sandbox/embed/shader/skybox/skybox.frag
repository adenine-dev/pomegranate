#version 450

layout(location = 0) in vec3 fragCoord;

layout (set = 0, binding = 1) uniform samplerCube skybox;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PC {
    uint mip;
    float exposure;
    float gamma;
} constants;

vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() {
    vec3 color = textureLod(skybox, fragCoord, int(constants.mip)).xyz;
	// Tone mapping
	color = Uncharted2Tonemap(color * 1.2);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / 1));
    outColor = vec4(color, 1);

    gl_FragDepth = 1;
}