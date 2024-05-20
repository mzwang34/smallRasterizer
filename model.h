#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal
    std::vector<Vec3f> norms_;
    std::vector<Vec2f> uv_;
    TGAImage diffusemap_;
    TGAImage roughnessmap_;
    TGAImage metalnessmap_;

    void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f normal(int iface, int nthvert);
    Vec3f normal(Vec2f uv);
    Vec3f vert(int i);
    Vec3f vert(int iface, int nthvert);
    Vec2f uv(int iface, int nthvert);
    std::vector<int> face(int idx);
    Vec3f diffuse(Vec2f uv);
    float roughness(Vec2f uv);
    float metalness(Vec2f uv);
    Vec3f fromTGAColor(TGAColor& color);
    float get_width_diffuse();
    float get_height_diffuse();
};

#endif //__MODEL_H__