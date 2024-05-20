// to do
#include "shader.h"

struct shadow_shader : public Shader {
    float depth[3];

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f v = payload.m_viewport * payload.lightmvp * proj4(payload.obj->vert(iface, nthvert));
        //std::cout << v.x << ";" << v.y << ";" << v.z << ";" << v.w << std::endl;
        depth[nthvert] = proj3(v).z;
        //std::cout << depth[nthvert] << std::endl;
        return v;
    }
    virtual Vec3f fragment(Vec3f bc) {
        float dpt = depth[0] * bc.x + depth[1] * bc.y + depth[2] * bc.z;
        Vec3f color(1.0, 1.0, dpt);
        //std::cout << dpt << std::endl;
        //Vec4f fragColor = pack(dpt);
        //color = (color + Vec3f(1, 1, 1)) / 2.f;
        return color;
    }
};