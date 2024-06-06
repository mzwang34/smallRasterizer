#include "shader.h"
#include "geometry.h"

#define M_PI 3.14159265358979323846 

float DistributionGGX(Vec3f N, Vec3f H, float roughness)
{
    // TODO: To calculate GGX NDF here
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = std::max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;

    return nom / std::max(denom, 0.0001f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    // TODO: To calculate Smith G1 here
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(Vec3f N, Vec3f V, Vec3f L, float roughness)
{
    // TODO: To calculate Smith G here
    float NoV = std::max(dot(N, V), 0.0f);
    float NoL = std::max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NoV, roughness);
    float ggx1 = GeometrySchlickGGX(NoL, roughness);

    return ggx1 * ggx2;
}

Vec3f fresnelSchlick(Vec3f F0, Vec3f V, Vec3f H)
{
    // TODO: To calculate Schlick F here
    float VdotH = std::max(dot(V, H), 0.0f);

    return F0 + (Vec3f(1.f, 1.f, 1.f) - F0) * pow(1.0 - VdotH, 5.0);
}

Vec3f mix(Vec3f a, Vec3f b, float c) {
    return a + (b - a) * c;
}

struct pbr_shader : public Shader {
    Vec4f n[3];
    Vec2f uv[3];
    Vec4f pos[3];

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f v = payload.m_viewport * payload.mvp * proj4(payload.obj->vert(iface, nthvert));
        n[nthvert] = (payload.m_view * payload.m_model).inv().transpose() * proj4((payload.obj->normal(iface, nthvert))); // view space
        uv[nthvert] = payload.obj->uv(iface, nthvert);
        pos[nthvert] = payload.m_model * proj4(payload.obj->vert(iface, nthvert));
        return v;
    }
    virtual Vec3f fragment(Vec3f bc) {
        float u = 0., v = 0.;
        for (int i = 0; i < 3; i++) {
            u += uv[i].x * bc[i];
            v += uv[i].y * bc[i];
        }
        Vec2f uvf(u, v);
        Vec3f albedo = payload.obj->diffuse(uvf);

        float roughness = payload.obj->roughness(uvf);
        float metalness = payload.obj->metalness(uvf);

        Vec3f F0(0.04f, 0.04f, 0.04f);
        F0 = mix(F0, albedo, metalness);

        Vec3f N = proj3(n[0]).normalize() * bc.x + proj3(n[1]).normalize() * bc.y + proj3(n[2]).normalize() * bc.z;
        N = N.normalize();
        Vec3f fragpos = proj3(pos[0]).normalize() * bc.x + proj3(pos[1]).normalize() * bc.y + proj3(pos[2]).normalize() * bc.z;
        Vec3f V = (payload.camera - fragpos).normalize();
        float NdotV = std::max(dot(N, V), 0.0f);

        Vec3f L = (payload.light - fragpos).normalize();
        Vec3f H = (V + L).normalize();
        float NdotL = std::max(dot(N, L), 0.f);

        Vec3f radiance(5.f, 5.f, 5.f);

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        Vec3f F = fresnelSchlick(F0, V, H);

        Vec3f numerator = F * NDF * G;
        float denominator = std::max((4.0f * NdotL * NdotV), 0.001f);
        Vec3f BRDF = numerator / denominator;
        
        Vec3f kd = (Vec3f(1.0, 1.0, 1.0) - F) * (1.f - metalness);
        Vec3f Lo = (albedo * kd / M_PI + BRDF) * radiance * NdotL;  // Cook-Torrance BRDF
        Vec3f color = Lo;

        //color = color / (color + Vec3f(1.f, 1.f, 1.f));
        //color = albedo;
        return color;
    }
};