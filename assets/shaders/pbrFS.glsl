#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

#define M_PI 3.14159265358979323846

#ifdef TRANSLUCENT
layout (location = 2) out vec4 AccumColor;
layout (location = 3) out float RevealColor;
#else
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
#endif

in VS_OUT
{
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
} fs_in;

// IBL
layout (binding = 0) uniform samplerCube irradianceMap;
layout (binding = 1) uniform samplerCube prefilterMap;
layout (binding = 2) uniform sampler2D brdfLUT;

layout (binding = 3) uniform sampler2D albedoMap;
layout (binding = 4) uniform sampler2D normalMap;
layout (binding = 5) uniform sampler2D metallicMap;
layout (binding = 6) uniform sampler2D roughnessMap;
layout (binding = 7) uniform sampler2D specularMap;
layout (binding = 8) uniform sampler2D aoMap;

layout (binding = 9) uniform sampler2D emissiveMap;

// shadows
layout (binding = 10) uniform sampler2DArray depthMaps[MAX_DIR_LIGHTS];
layout (binding = 20) uniform samplerCube depthCubeMaps[MAX_POINT_LIGHTS];

// directional light
struct DirLight {
    vec3 direction; // 12 bytes + 4 bytes padding
    vec3 color; // 12 bytes
    bool shadows; // 4 bytes
};

// point ligth
struct PointLight {
    vec3 position; // 12 bytes + 4 bytes padding
    vec3 color; // 12 bytes
    bool shadows; // 4 bytes
};

layout (std140, binding = 1) uniform Lights
{
    mat4 lightSpaceMatrices[16 * MAX_DIR_LIGHTS]; // 16 * 16 * 4 * MAX_DIR_LIGHTS
    // float cascadePlaneDistances[16]; // 16 * 16 = 256 bytes
    vec4 cascadePlaneDistances; // 16 bytes
    DirLight dirLights[MAX_DIR_LIGHTS]; // 2 * 16 * MAX_DIR_LIGHTS
    PointLight pointLights[MAX_POINT_LIGHTS]; // 2 * 16 * MAX_POINT_LIGHTS
    int nrDirLights; // 4 bytes
    int nrPointLights; // 4 bytes
    int cascadeCount; // 4 bytes + 4 bytes padding
};

layout (std140, binding = 2) uniform Camera
{
    mat4 camView;
    vec3 camPos;
    float far_plane;
};

// material parameters
struct Material {
    vec4 albedo;
    float metallic;
    float roughness;
    float ao;
    float emissive;
}; // 16 + 16 = 32 bytes

struct ObjectProperties {
    bool useIBL; // 4 bytes
    int objectID; // 4 bytes
};

layout (std140, binding = 3) uniform Object
{
    ObjectProperties properties; // 8 bytes, padded to 16 bytes
    Material materials[MAX_OBJECTS]; // 32 bytes * MAX_OBJECTS
};

const float gamma = 2.2;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.WorldPos);
    vec3 Q2  = dFdy(fs_in.WorldPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

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

float DirShadowsCalculation(int dirLightIndex, vec3 lightDir, sampler2DArray depthMap)
{
    // select cascade layer
    vec4 fragPosViewSpace = camView * vec4(fs_in.WorldPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; i++) {
        if (depthValue < cascadePlaneDistances[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        layer = cascadeCount;
    }
    // normal offset
    vec3 offsetPos = fs_in.WorldPos + normalize(fs_in.Normal) * 0.03;
    vec4 fragPosLightSpace = lightSpaceMatrices[dirLightIndex * 16 + layer] * vec4(offsetPos, 1.0);
    // perform perspective division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // keep the shadow at 0.0 when outside the far plane region of the light's frustum
    if (currentDepth > 1.0) {
        return 0.0;
    }

    // calculate bias (based on depth map resolution and slope)
    // vec3 normal = normalize(fs_in.Normal);
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // const float biasModifier = 0.5;
    // if (layer == cascadeCount) {
    //     bias *= 1 / (far_plane * biasModifier);
    // }
    // else {
    //     bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    // }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(depthMap, 0));
    const int halfkernelSize = 3; // default 1 (bigger value = softer shadows)
    for (int x = -halfkernelSize; x <= halfkernelSize; x++) {
        for (int y = -halfkernelSize; y <= halfkernelSize; y++) {
            float pcfDepth = texture(depthMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= (halfkernelSize * 2 + 1) * (halfkernelSize * 2 + 1);

    return shadow;
}

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

// float PointShadowsCalculation(vec3 lightPos)
// {
//     // get vector between fragment position and light position
//     vec3 fragToLight = fs_in.WorldPos - lightPos;
//     // use the light to fragment vector to sample from the depth map    
//     float closestDepth = texture(depthCubeMap, fragToLight).r;
//     // it is currently in linear range between [0,1]. Re-transform back to original value
//     closestDepth *= far_plane;
//     // now get current linear depth as the length between the fragment and light position
//     float currentDepth = length(fragToLight);
//     // now test for shadows
//     float bias = 0.05; 
//     float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
//     // display closestDepth as debug (to visualize depth cubemap)
//     // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);

//     return shadow;
// }

float PointShadowsCalculation(vec3 lightPos, samplerCube depthCubeMap)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fs_in.WorldPos - lightPos;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(camPos - fs_in.WorldPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthCubeMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec3 CalcDirLight(vec3 N, vec3 V, vec3 R, vec3 F0, vec3 albedo, float metallic, float roughness, float ao)
{
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < nrDirLights; i++) {
        // calculate per-light radiance
        vec3 L = normalize(dirLights[i].direction);
        vec3 H = normalize(V + L);
        vec3 radiance = dirLights[i].color;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
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
        kDI *= 1.0 - metallic;

        // scale light by N dot L
        float NdotL = max(dot(N, L), 0.0);

        float shadow = 0.0;
        if (dirLights[i].shadows)
            shadow = DirShadowsCalculation(i, dirLights[i].direction, depthMaps[i]);

        // add to outgoing radiance Lo
        Lo += (1.0 - shadow) * (kDI * albedo / M_PI + specularI) * radiance * NdotL;
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    vec3 ambient = vec3(0.0);
    if (properties.useIBL) {
        // ambien lighting (we now use IBL as the ambient term)
        vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuse = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together
        // as per the split-sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

        ambient = (kD * diffuse + specular) * ao;
    }
    else {
        ambient = vec3(0.03) * albedo * ao;
    }

    return (ambient + Lo);
}

vec3 CalcPointLight(vec3 N, vec3 V, vec3 R, vec3 F0, vec3 albedo, float metallic, float roughness, float ao)
{
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < nrPointLights; i++) {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - fs_in.WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - fs_in.WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
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
        kD *= 1.0 - metallic;

        // scale light by N dot L
        float NdotL = max(dot(N, L), 0.0);

        float shadow = 0.0;
        if (pointLights[i].shadows)
            shadow = PointShadowsCalculation(pointLights[i].position, depthCubeMaps[i]);

        // add to outgoing radiance Lo
        Lo += (1.0 - shadow) * (kD * albedo / M_PI + specular) * radiance * NdotL;
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    vec3 ambient = vec3(0.0);
    if (properties.useIBL) {
        // ambien lighting (we now use IBL as the ambient term)
        vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuse = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together
        // as per the split-sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

        ambient = (kD * diffuse + specular) * ao;

    }
    else {
        ambient = vec3(0.03) * albedo * ao;
    }

    return (ambient + Lo);
}

void main()
{
    vec3 result = vec3(0.0);
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);

    #ifdef TEXTURED
        vec4 albedoTexture = texture(albedoMap, fs_in.TexCoords);
        float alpha = albedoTexture.a;
        vec3 albedo = pow(albedoTexture.rgb, vec3(gamma)); // gamma corrected for sRGB
        // albedo *= alpha; // pre-multiplied alpha (alpha blending)

        #ifdef GLTF
            float metallic = texture(metallicMap, fs_in.TexCoords).b;
            float roughness = texture(roughnessMap, fs_in.TexCoords).g;
        #else
            float metallic = texture(metallicMap, fs_in.TexCoords).r;
            float roughness = texture(roughnessMap, fs_in.TexCoords).r;
        #endif

        vec3 specular = texture(specularMap, fs_in.TexCoords).rgb;
        metallic += specular.r;
        roughness -= specular.r;

        float ao = texture(aoMap, fs_in.TexCoords).r;
        vec3 N = getNormalFromMap();
        if (!gl_FrontFacing) {
            N = -N;
        }

        albedo = albedo * materials[properties.objectID].albedo.rgb;
        metallic = metallic * materials[properties.objectID].metallic;
        roughness = roughness * materials[properties.objectID].roughness;
        ao = ao * materials[properties.objectID].ao;
    #else
        vec3 albedo = materials[properties.objectID].albedo.rgb;
        float alpha = materials[properties.objectID].albedo.a;
        float metallic = materials[properties.objectID].metallic;
        float roughness = materials[properties.objectID].roughness;
        float ao = materials[properties.objectID].ao;
        vec3 N = normalize(fs_in.Normal);
        if (!gl_FrontFacing) {
            N = -N;
        }
    #endif

    vec3 emissive = texture(emissiveMap, fs_in.TexCoords).rgb;
    emissive = emissive * materials[properties.objectID].emissive;

    vec3 V = normalize(camPos - fs_in.WorldPos);
    vec3 R = reflect(-V, N);

    F0 = mix(F0, albedo, metallic);

    result += CalcDirLight(N, V, R, F0, albedo, metallic, roughness, ao);
    result += CalcPointLight(N, V, R, F0, albedo, metallic, roughness, ao);
    result += emissive;

    #ifdef TRANSLUCENT
        // weight function
        float weight = clamp(pow(min(1.0, alpha * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

        // store pixel color accumulation
        AccumColor = vec4(result * alpha, alpha) * weight;

        // store pixel revealage threshold
        RevealColor = alpha;
    #else
        #ifdef TEXTURED
            if (alpha <= 0.5)
                discard;
        #endif

        FragColor = vec4(result, 1.0);

        // Debug
        // FragColor = vec4(N, 1.0);
        // FragColor = vec4(albedo, 1.0);
        // FragColor = vec4(texture(emissiveMap, fs_in.TexCoords).rgb, 1.0);
        // Debug textures
        // FragColor = vec4(N, 1.0);
        // FragColor = vec4(albedo, 1.0);
        // FragColor = vec4(texture(emissiveMap, fs_in.TexCoords).rgb, 1.0);
        // FragColor = vec4(texture(metallicMap, fs_in.TexCoords).bbb, 1.0);
        // FragColor = vec4(texture(roughnessMap, fs_in.TexCoords).ggg, 1.0);
        // FragColor = vec4(texture(aoMap, fs_in.TexCoords).rrr, 1.0);

        // check whether result is higher than some threshold, if so, output as bloom threshold color
        float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
        if(brightness > 1.0)
            BrightColor = vec4(FragColor.rgb, 1.0);
        else
            BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    #endif
}