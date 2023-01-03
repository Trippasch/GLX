#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

#define M_PI 3.14159265358979323846

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT
{
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} fs_in;

// IBL
layout (binding = 0) uniform samplerCube irradianceMap;
layout (binding = 1) uniform samplerCube prefilterMap;
layout (binding = 2) uniform sampler2D brdfLUT;

layout (binding = 8) uniform sampler2D depthMap;

// material parameters
struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};
uniform Material material;

// directional light
struct DirLight {
    vec3 direction;
    vec3 color;
    bool shadows;
};
uniform DirLight dirLight;

// point lights
#define NR_LIGHTS 2

struct PointLight {
    vec3 position;
    vec3 color;
};
uniform PointLight pointLights[NR_LIGHTS];

uniform vec3 camPos;

const float gamma = 2.2;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DirShadowsCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    // perform perspective division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective
    float closestDepth = texture(depthMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    lightDir = normalize(lightDir - fs_in.FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far plane region of the light's frustum
    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec3 CalcDirLight(vec3 N, vec3 V, vec3 R, vec3 F0)
{
    // reflectance equation
    vec3 Lo = vec3(0.0);
    // calculate per-light radiance
    vec3 L = normalize(-dirLight.direction);
    vec3 H = normalize(V + L);
    vec3 radiance = dirLight.color;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, material.roughness);
    float G = GeometrySmith(N, V, L, material.roughness);
    vec3 FI = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * FI;
    // we add + 0.0001 to prevent division by zero
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specularI = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kSI = FI;
    // for energy conservation, the diffuse and specular light can't 
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kDI = vec3(1.0) - kSI;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure
    // metals have no diffuse light).
    kDI *= 1.0 - material.metallic;

    // scale light by N dot L
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kDI * material.albedo / M_PI + specularI) * radiance * NdotL;
    // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    // ambien lighting (we now use IBL as the ambient term)
    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, material.roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - material.metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * material.albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together
    // as per the split-sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, material.roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), material.roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD * diffuse + specular) * material.ao;

    float shadow = 0.0;
    if (dirLight.shadows)
        shadow = DirShadowsCalculation(fs_in.FragPosLightSpace, dirLight.direction);

    return (1.0 - shadow) * (ambient + Lo);
}

vec3 CalcPointLight(vec3 N, vec3 V, vec3 R, vec3 F0)
{
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < NR_LIGHTS; i++) {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - fs_in.WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - fs_in.WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, material.roughness);
        float G = GeometrySmith(N, V, L, material.roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        // we add + 0.0001 to prevent division by zero
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't 
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure
        // metals have no diffuse light).
        kD *= 1.0 - material.metallic;

        // scale light by N dot L
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * material.albedo / M_PI + specular) * radiance * NdotL;
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambien lighting (we now use IBL as the ambient term)
    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, material.roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - material.metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * material.albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together
    // as per the split-sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, material.roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), material.roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD * diffuse + specular) * material.ao;

    return ambient + Lo;
}

void main()
{
    vec3 N = normalize(fs_in.Normal);
    vec3 V = normalize(camPos - fs_in.WorldPos);
    vec3 R = reflect(-V, N);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.albedo, material.metallic);

    vec3 result = vec3(0.0);
    result += CalcDirLight(N, V, R, F0);
    result += CalcPointLight(N, V, R, F0);

    FragColor = vec4(result, 1.0);

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}