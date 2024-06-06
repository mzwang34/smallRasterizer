#ifndef __SHADER_H__
#define __SHADER_H__

#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

struct payload_t {
    Matrix4f m_view;
    Matrix4f m_model;
    Matrix4f mvp;
    Matrix4f m_viewport;
	Matrix4f lightmvp;

	Vec3f light;
	Vec3f target;
	Vec3f camera;

    Model* obj;
	Vec3f ndcCoord[3];
};

struct Shader {
    virtual ~Shader();
    payload_t payload;
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual Vec3f fragment(Vec3f bc) = 0;
};

Shader::~Shader() {}

struct normal_shader : public Shader {
    Vec4f n[3]; // *n is wrong!

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f v = payload.m_viewport * payload.mvp * proj4(payload.obj->vert(iface, nthvert));
        n[nthvert] = (payload.m_view * payload.m_model).inv().transpose() * proj4((payload.obj->normal(iface, nthvert))); // view space
        return v;
    }
    virtual Vec3f fragment(Vec3f bc) {
        Vec3f color = proj3(n[0]).normalize() * bc.x + proj3(n[1]).normalize() * bc.y + proj3(n[2]).normalize() * bc.z;
        color = (color + Vec3f(1, 1, 1)) / 2.f;
        return color;
    }
};

struct phong_shader : public Shader {
	Vec4f n[3]; 

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f v = payload.m_viewport * payload.mvp * proj4(payload.obj->vert(iface, nthvert));
		n[nthvert] = (payload.m_view * payload.m_model).inv().transpose() * proj4((payload.obj->normal(iface, nthvert))); // view space
        return v;
	}
	virtual Vec3f fragment(Vec3f bc) {
		Vec3f ka(0.005, 0.005, 0.005);
		Vec3f kd(255, 0, 0);
		Vec3f ks(0.7937, 0.7937, 0.7937);
		Vec3f Ia(10, 10, 10);
		Vec3f I(500, 500, 500);
		int p = 20;

		Vec3f l = (payload.light - payload.target).normalize();
		Vec3f v = (payload.camera - payload.target).normalize();
		float r = l.norm();
		Vec3f nn = (proj3(n[0]).normalize() * bc.x + proj3(n[1]).normalize() * bc.y + proj3(n[2]).normalize() * bc.z).normalize();
		Vec3f h = (v + l).normalize();

		Vec3f ambient = ka * Ia;
		Vec3f diffuse = I / (r * r) * std::max(0.f, dot(nn, l)) * kd;
		Vec3f specular = I / (r * r) * std::pow(std::max(0.f, dot(nn, h)), p);

		return ambient + diffuse + specular;
	}
};

struct texture_shader : public Shader {
	Vec2f uv[3];

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f v = payload.m_viewport * payload.mvp * proj4(payload.obj->vert(iface, nthvert));
		uv[nthvert] = payload.obj->uv(iface, nthvert);
		return v;
	}
	virtual Vec3f fragment(Vec3f bc) {
		float u = 0., v = 0.;
		for (int i = 0; i < 3; i++) {
			u += uv[i].x * bc[i];
			v += uv[i].y * bc[i];
		}
		//std::cout << u << ";" << v << std::endl;
		Vec2f uvf(u, v);
		Vec3f color = payload.obj->diffuse(uvf);
		return color;
	}
};

struct phong_texture_shader : public Shader {
	Vec4f n[3]; 
	Vec2f uv[3];

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f v = payload.m_viewport * payload.mvp * proj4(payload.obj->vert(iface, nthvert));
		n[nthvert] = (payload.m_view * payload.m_model).inv().transpose() * proj4((payload.obj->normal(iface, nthvert))); // view space
		uv[nthvert] = payload.obj->uv(iface, nthvert);
		payload.ndcCoord[nthvert] = proj3(payload.mvp * proj4(payload.obj->vert(iface, nthvert)));
        return v;
	}
	virtual Vec3f fragment(Vec3f bc) {
		float u = 0., v = 0.;
		for (int i = 0; i < 3; i++) {
			u += uv[i].x * bc[i];
			v += uv[i].y * bc[i];
		}
		Vec2f uvf(u, v);
		Vec3f color = payload.obj->diffuse(uvf);

		Vec3f ka(0.005, 0.005, 0.005);
		Vec3f kd = color / 255.f;
		Vec3f ks(0.7937, 0.7937, 0.7937);
		Vec3f Ia(10, 10, 10);
		Vec3f I(500, 500, 500);
		int p = 5000000000000;

		Vec3f l = (payload.light - payload.target).normalize();
		Vec3f vw = (payload.camera - payload.target).normalize();
		float r = l.norm();
		Vec3f nn = (proj3(n[0]).normalize() * bc.x + proj3(n[1]).normalize() * bc.y + proj3(n[2]).normalize() * bc.z).normalize();
		Vec3f h = (vw + l).normalize();

		Vec3f ambient = ka * Ia;
		Vec3f diffuse = I / (r * r) * std::max(0.f, dot(nn, l)) * kd;
		Vec3f specular = I / (r * r) * std::pow(std::max(0.f, dot(nn, h)), p) * ks;

		color = ambient + diffuse + specular;

		return color;
	}
};

//struct depth_shader : public Shader {
//    Vec3f vt[3];
//
//    virtual Vec4f vertex(int iface, int nthvert) {
//        Vec4f v = m_viewport * lightmvp * proj4(obj->vert(iface, nthvert));
//		vt[nthvert] = proj3(v);
//
//        return v;
//    }
//    virtual Vec3f fragment(Vec3f bc) {
//		Vec3f color = Vec3f(1, 1, 1) * (bc.x * vt[0].z + bc.y * vt[1].z + bc.z * vt[2].z);
//        return color;
//    }
//};

struct bump_shader : public Shader {
	Vec4f n[3];
	Vec2f uv[3];

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f v = payload.m_viewport * payload.mvp * proj4(payload.obj->vert(iface, nthvert));
		n[nthvert] = (payload.m_view * payload.m_model).inv().transpose() * proj4((payload.obj->normal(iface, nthvert))); // view space
        uv[nthvert] = payload.obj->uv(iface, nthvert);
		// std::cout << v.x << ";" << v.y << ";" << v.z << ";" << v.w << std::endl;
		return v;
    }
    virtual Vec3f fragment(Vec3f bc) {
		// n = normal = (x, y, z)
		// t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
		// b = cross(n, t)
		Vec3f nn = proj3(n[0]).normalize() * bc.x + proj3(n[1]).normalize() * bc.y + proj3(n[2]).normalize() * bc.z;
		float x = nn.x, y = nn.y, z = nn.z;
		Vec3f t(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z), z * y / sqrt(x * x + z * z));
		Vec3f b = cross(nn, t);
		float c1 = 1.5, c2 = 1.5;
		float u = 0., v = 0.;
		for (int i = 0; i < 3; i++) {
			u += uv[i].x * bc[i];
			v += uv[i].y * bc[i];
		}
		float width = payload.obj->get_width_diffuse(), height = payload.obj->get_height_diffuse();
		float dpu = c1 *(payload.obj->diffuse(Vec2f(u + 1.f / width, v)).norm() - payload.obj->diffuse(Vec2f(u, v)).norm());
        float dpv = c2 * (payload.obj->diffuse(Vec2f(u, v + 1.f / height)).norm() - payload.obj->diffuse(Vec2f(u, v)).norm());
		Vec3f normal = Vec3f(-dpu, -dpv, 1.f);
		Matrix3f TBN(t.x, b.x, nn.x,
					 t.y, b.y, nn.y,
					 t.z, b.z, nn.z);
		Vec3f nl = (TBN * normal).normalize();
		Vec3f tex_color = payload.obj->diffuse(Vec2f(u, v));

		Vec3f ka(0.005, 0.005, 0.005);
		Vec3f kd = tex_color / 255.f;
		Vec3f ks(0.7937, 0.7937, 0.7937);
		Vec3f Ia(10, 10, 10);
		Vec3f I(500, 500, 500);
		int p = 500;

		Vec3f l = (payload.light - payload.target).normalize();
		Vec3f vw = (payload.camera - payload.target).normalize();
		float r = l.norm();
		Vec3f h = (vw + l).normalize();

		Vec3f ambient = ka * Ia;
		Vec3f diffuse = I / (r * r) * std::max(0.f, dot(nl, l)) * kd;
		Vec3f specular = I / (r * r) * std::pow(std::max(0.f, dot(nl, h)), p) * ks;

		// Vec3f color = (color + Vec3f(1, 1, 1)) / 2.f * tex_color * 255.f;
		Vec3f color = ambient + diffuse + specular;
		return color;
    }
};

#endif 