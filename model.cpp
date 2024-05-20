#include <iostream>
#include <fstream>
#include <sstream>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), norms_(), uv_(), diffusemap_(), roughnessmap_(), metalnessmap_() { //, diffusemap_(), normalmap_(), specularmap_()
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            // for (int i=0;i<3;i++) iss >> v[i];
            iss >> v.x >> v.y >> v.z;
            verts_.push_back(v);
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f n;
            // for (int i=0;i<3;i++) iss >> n[i];
            iss >> n.x >> n.y >> n.z;
            norms_.push_back(n);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f uv;
            // for (int i=0;i<2;i++) iss >> uv[i];
            iss >> uv.x >> uv.y;
            uv_.push_back(uv);
        }  else if (!line.compare(0, 2, "f ")) {
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;
            while (iss >> tmp.x >> trash >> tmp.y >> trash >> tmp.z) {
                // for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                tmp.x--; tmp.y--; tmp.z--;
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap_);
    // load_texture(filename, "_nm_tangent.tga",      normalmap_);
    // load_texture(filename, "_spec.tga",    specularmap_);
    load_texture(filename, "_roughness.tga", roughnessmap_);
    load_texture(filename, "_metalness.tga", metalnessmap_);
}

Model::~Model() {}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

Vec3f Model::vert(int iface, int nthvert) {
    return verts_[faces_[iface][nthvert][0]];
}

Vec2f Model::uv(int iface, int nthvert) {
    return uv_[faces_[iface][nthvert][1]];
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

Vec3f Model::fromTGAColor(TGAColor& color) {
    float b = static_cast<float>(color.bgra[0]) / 255.0f;
    float g = static_cast<float>(color.bgra[1]) / 255.0f;
    float r = static_cast<float>(color.bgra[2]) / 255.0f;
    float a = static_cast<float>(color.bgra[3]) / 255.0f;

    return Vec3f(r, g, b);  // a may be 0
}

Vec3f Model::diffuse(Vec2f uvf) {
    Vec2i uv(uvf[0]*diffusemap_.get_width(), uvf[1]*diffusemap_.get_height());
    TGAColor c = diffusemap_.get(uv[0], uv[1]);
    return Model::fromTGAColor(c);
}

float Model::get_width_diffuse() {
    return diffusemap_.get_width();
}

float Model::get_height_diffuse() {
    return diffusemap_.get_height();
}


Vec3f Model::normal(int iface, int nthvert) {
    int idx = faces_[iface][nthvert][2];
    return norms_[idx].normalize();
}

float Model::roughness(Vec2f uvf) {
    Vec2i uv(uvf[0] * roughnessmap_.get_width(), uvf[1] * roughnessmap_.get_height());
    return roughnessmap_.get(uv[0], uv[1])[0] / 255.f;
}

float Model::metalness(Vec2f uvf) {
    Vec2i uv(uvf[0] * metalnessmap_.get_width(), uvf[1] * metalnessmap_.get_height());
    return metalnessmap_.get(uv[0], uv[1])[0] / 255.f;
}
