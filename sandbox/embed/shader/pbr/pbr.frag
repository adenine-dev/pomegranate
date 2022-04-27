// Equations referenced from
// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

#version 450

layout(location = 0) in vec4 vertexColor;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec4 vertexTangent;
layout(location = 4) in vec3 worldPos;
layout(location = 5) in vec3 cameraPos;

layout(location = 0) out vec4 fragColor;

layout(set = 1, binding = 1, std140) uniform Material
{
    vec3 albedo;
    float metallic;
    float roughness;
}
material;

layout(set = 0, binding = 1, std140) uniform Light
{
    vec4 positions[3];
    vec4 colors[3];
}
light;

layout(set = 0, binding = 2) uniform samplerCube irradianceMap;
layout(set = 0, binding = 3) uniform samplerCube prefilteredEnvMap;
layout(set = 0, binding = 4) uniform sampler2D brdfLut;

layout(set = 1, binding = 5) uniform sampler2D armMap;
layout(set = 1, binding = 6) uniform sampler2D normalMap;
layout(set = 1, binding = 7) uniform sampler2D albedoMap;


const float pi = 3.1415926535897932384626433832795;

// Normal Distribution Function
// Using GGX/Trowbridge-Reitz
float specularD(vec3 N, vec3 H, float roughness)
{
    const float alphaSquared = pow(roughness, 4); // α = roughness²

    // D(H) = α² / (π ((N • H)² (α² - 1) + 1))²
    return alphaSquared / (pi * pow(pow(max(dot(N, H), 0), 2) * (alphaSquared - 1) + 1, 2));
}

// Specular Geometric Attenuation
// Using the Schlick model approximating the smith model.
float G1(vec3 N, vec3 V, float roughness)
{
    const float k = pow(roughness + 1, 2) / 8; // k = (Roughness + 1)² / 8 // NOTE: roughness shouldn't be +1ed with IBL
    const float NdotV = max(dot(N, V), 0);

    // G₁(V) = (N • V) / ((N • V)(1 - k) + k)
    return NdotV / (NdotV * (1 - k) + k);
}
float specularG(vec3 N, vec3 V, vec3 L, float roughness)
{
    // G(L, V, H) = G₁(V) G₁(L)
    return G1(N, V, roughness) * G1(N, L, roughness);
}

// Fresnel
// Using Schlick's approximation using spherical Gausian approximation.
vec3 specularF(vec3 V, vec3 H, vec3 F0)
{
    const float VdotH = clamp(dot(H, V), 0, 1);

    // F(V, H) = F₀ + (1 − F₀) 2^((-5.55473(V · H) - 6.98316(V · H))
    return F0 + (1.0 - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

vec3 specularFR(vec3 V, vec3 H, vec3 F0, float roughness)
{
    const float VdotH = clamp(dot(H, V), 0, 1);

    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

vec3 getPrefilteredEnvColor(vec3 R, float roughness) {
    const float MAX_REFLECTION_LOD = 5.0;
    const float lod = roughness * MAX_REFLECTION_LOD;
    const float lodf = floor(lod);
    const float lodc = ceil(lod);
    return mix(pow(textureLod(prefilteredEnvMap, R, lodf).rgb, vec3(2.2)), pow(textureLod(prefilteredEnvMap, R, lodc).rgb, vec3(2.2)), lod - lodf);
}

vec3 getNormal() {
    // return vertexNormal;

	vec3 tangentNormal = texture(normalMap, vertexUV).xyz * 2.0 - 1.0;

	vec3 N = normalize(vertexNormal);
	vec3 T = normalize(vertexTangent.xyz);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * tangentNormal);
}


void main()
{
    const vec3 N = getNormal();
    const vec3 V = normalize(cameraPos - worldPos);
    const vec3 R = reflect(-V, N); 

    const vec3 albedo = pow(texture(albedoMap, vertexUV).xyz, vec3(2.2)) * material.albedo;
    const vec3 arm = texture(armMap, vertexUV).xyz * vec3(1, material.roughness, material.metallic);
    const float ao = arm.x;
    const float roughness = arm.y;
    const float metallic = arm.z;

    // const vec3 albedo = material.albedo;
    // const float ao = 1;
    // const float roughness = material.roughness;
    // const float metallic = material.metallic;

    const vec3 F0 = mix(albedo, vec3(0.04), metallic);

    vec3 Lo = vec3(0);
    for (int i = 0; i < 1; ++i) {
        const vec3 L = normalize(light.positions[i].xyz - worldPos);
        const vec3 H = normalize(V + L);

        const vec3 Li = light.colors[i].xyz / pow(length(light.positions[i].xyz - worldPos), 2);
        const vec3 kS = specularF(V, H, F0); // kS = F

        // Cook-Torrance
        // f(L, V) = D(H)F(V, H)G(L, V, H) / 4(N · L)(N · V)
        const vec3 specular = (specularD(N, H, roughness) * kS * specularG(N, V, L, roughness))
                                   / 4 * max(dot(N, L), 0.0) * max(dot(N, V), 0.0)
                               + 0.0001; // prevent divide by zero


        const vec3 kD = (vec3(1) - kS) * (1 - metallic);

        // fᵣ(p, ωᵢ, ωₒ) L(p, ωᵢ) n·ωᵢ
        Lo += (kD * albedo / pi + specular) * Li * max(dot(N, L), 0);
    }
    
    vec3 kS = specularFR(N, V, F0, roughness);
    vec3 kD = (1.0 - kS) * (1.0 - metallic);
    	  
    vec3 irradiance = pow(texture(irradianceMap, N).rgb, vec3(2.2));
    vec3 diffuse = irradiance * albedo;

    const vec2 brdf = texture(brdfLut, vec2(max(dot(N, V), 0.0), roughness)).rg;
    const vec3 specular = getPrefilteredEnvColor(R, roughness) * (kS * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;

    // gamma correction.
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, 1.0);
}
