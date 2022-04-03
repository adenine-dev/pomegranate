// Equations referenced from
// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

#version 450

layout(location = 0) in vec4 vertexColor;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 worldPos;
layout(location = 4) in vec3 cameraPos;

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

const float pi = 3.1415926535897932384626433832795;

// Normal Distribution Function
// Using GGX/Trowbridge-Reitz
float specularD(vec3 N, vec3 H, float roughness)
{
    const float alphaSquared = pow(roughness, 4); // α = roughness²

    // D(H) = α² / (π ((N • H)² (α² - 1) + 1))²
    return alphaSquared / (pi * pow(pow(max(dot(N, H), 0.0), 2) * (alphaSquared - 1.0) + 1.0, 2));
}

// Specular Geometric Attenuation
// Using the Schlick model approximating the smith model.
float G1(vec3 N, vec3 V, float roughness)
{
    const float k
        = pow(roughness + 1, 2) / 8.0; // k = (Roughness + 1)² / 8 // NOTE: roughness shouldn't be +1ed with IBL
    const float NdotV = max(dot(N, V), 0.0);

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
    const float VdotH = clamp(dot(H, V), 0.0, 1.0);

    // F(V, H) = F₀ + (1 − F₀) 2^((-5.55473(V · H) - 6.98316(V · H))
    return F0 + (1 - F0) * pow(2, -5.55473 * VdotH - 6.98316 * VdotH);
}

void main()
{
    const vec3 N = normalize(vertexNormal); // TODO: texture mapped normals
    const vec3 V = normalize(cameraPos - worldPos);

    const vec3 F0 = mix(vec3(0.04), material.albedo, material.metallic);
    ;

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 3; ++i) {
        const vec3 L = normalize(light.positions[i].xyz - worldPos);
        const vec3 H = normalize(V + L);

        const vec3 Li = light.colors[i].xyz / pow(length(light.positions[i].xyz - worldPos), 2);

        // Cook-Torrance
        // f(L, V) = D(H)F(V, H)G(L, V, H) / 4(N · L)(N · V)
        const float specular = specularD(N, H, material.roughness)
                                   * specularG(N, V, L, material.roughness) // NOTE: no F yet
                                   / 4 * max(dot(N, L), 0.0) * max(dot(N, V), 0.0)
                               + 0.0001; // prevent divide by zero

        const vec3 kS = specularF(V, H, F0); // kS = F

        const vec3 kD = (vec3(1.0) - kS) * (1 - material.metallic);

        // fᵣ(p, ωᵢ, ωₒ) L(p, ωᵢ) n·ωᵢ
        Lo += (kD * material.albedo / pi + kS * specular) * Li * max(dot(N, L), 0.0); // NOTE: F (=kS) is added here.
    }

    const vec3 ambient = vec3(0.03) * material.albedo; // TODO: ibl, this is temp
    vec3 color = ambient + Lo;

    // gamma correction.
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, 1.0);
}
