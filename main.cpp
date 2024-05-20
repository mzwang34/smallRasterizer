#include <iostream>
#include <stdio.h>
#include <limits>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include "shader.h"
#include "transform.h"
#include "pbrShader.h"
#include "shadowShader.h"

const int w = 1024;
const int h = 1024;

int clamp(float x) {
	x = x < 0.f? 0.f : x > 255.f? 255.f : x;
	return static_cast<int>(x);
}

Vec3f correction_gamma(Vec3f c) {
	c.x = pow(c.x, 1.0 / 2.0);
	c.y = pow(c.y, 1.0 / 2.0);
	c.z = pow(c.z, 1.0 / 2.0);
	return c;
}

void writePPM(char* filename, Vec3f *c) {
	FILE *f = fopen(filename, "w");
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
    for (int i = 0; i < w * h; ++i) 
		fprintf(f, "%d %d %d ", clamp(c[i].x), clamp(c[i].y), clamp(c[i].z));
}

bool backCulling(Vec3f v0, Vec3f v1, Vec3f v2) {
	//Vec3f v01 = v1 - v0;
	//Vec3f v02 = v2 - v0;
	//Vec3f view(0, 0, 1);
	//Vec3f normal = cross(v01, v02);
	//return dot(normal, view) > 0;
	
	//return 1;

	float signed_area = v0.x * v1.y - v0.y * v1.x +
						v1.x * v2.y - v1.y * v2.x +
						v2.x * v0.y - v2.y * v0.x;   //|AB AC|
	return signed_area <= 0;
}

Vec3f barycentric(Vec2f v0, Vec2f v1, Vec2f v2, Vec2f p) {
	return Vec3f((p-v1).cross(v2-v1), (p-v2).cross(v0-v2), (p-v0).cross(v1-v0)) * (1.f / (v2-v0).cross(v1-v0));
}

void triangle(Vec4f *v, Shader &shader, Vec3f *&frame, float *&zbuffer) {
	Vec3f v0 = proj3(v[0]);
	Vec3f v1 = proj3(v[1]);
	Vec3f v2 = proj3(v[2]);
	//std::cout << v0.x << ";" << v0.y << ";" << v0.z << std::endl;
	// bounding box
	float bboxmin_x = std::max(0.f, std::min(v0.x, std::min(v1.x, v2.x)));
	float bboxmax_x = std::min(w - 1.f, std::max(v0.x, std::max(v1.x, v2.x)));
	float bboxmin_y = std::max(0.f, std::min(v0.y, std::min(v1.y, v2.y)));
	float bboxmax_y = std::min(h - 1.f, std::max(v0.y, std::max(v1.y, v2.y)));

    Vec3f color;
	for (int x = bboxmin_x; x <= bboxmax_x; x++)
		for (int y = bboxmin_y; y <= bboxmax_y; y++) {
			Vec2f p(x + 0.5, y + 0.5);	// pixel center
			Vec3f bc = barycentric(Vec2f(v0.x, v0.y), Vec2f(v1.x, v1.y), Vec2f(v2.x, v2.y), p);
			//Vec3f bar = bc;
			// perspective correction
			bc.x /= v[0].w;	// w save z in world space
			bc.y /= v[1].w;
			bc.z /= v[2].w;
			float z = 1.f / (bc.x + bc.y + bc.z);
			bc.x *= z;
			bc.y *= z;
			bc.z *= z;
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            color = correction_gamma(shader.fragment(bc)) * 255.f;
			//std::cout << color.x << ";" << color.y << ";" << color.z << std::endl;
			if (z > zbuffer[x + y * w]) {
				zbuffer[x + y * w] = z;
				frame[x + y * w] = color;
			}
		}
}


int main(int argc, char *argv[])
{
    //Model *obj = new Model("D:/Documents/vision/course/smallRasterizer/obj/xier/xierbody.obj");

	const Vec3f camera(1, 0, 4);	// camera position
	Vec3f light(0, 0, 5);
	Vec3f target(0, 0, 0);

	float fov = 60;
	float aspect = 1;
	float near = -0.1, far = -50;
	float angle = 180.f;
	Vec3f up(0, 1, 0);
	
	Matrix4f m_projection = projection(fov, aspect, near, far);
	Matrix4f m_viewport = viewport(w, h);
	Matrix4f m_view = view(camera, up, target);
    Matrix4f m_model = model(angle);
	Matrix4f mvp = m_projection * m_view * m_model;

	Matrix4f m_view_light = view(light, up, target);
	Matrix4f m_ortho_projection = ortho_projection(-2, 2, -2, 2, near, far);
	Matrix4f lightmvp = m_ortho_projection * m_view_light * m_model;

	phong_texture_shader shader;
	shader.payload.mvp = mvp;
	shader.payload.m_model = m_model;
	shader.payload.m_viewport = m_viewport;
	//shader.payload.obj = obj;
	shader.payload.m_view = m_view;
	shader.payload.lightmvp = lightmvp;

	shader.payload.light = light;
	shader.payload.target = target;
	shader.payload.camera = camera;

	Vec3f* frame = new Vec3f[w * h];
	//Vec3f* depth = new Vec3f[w * h];
	float* zbuffer = new float[w * h];
	for (int i = 0; i < w * h; i++) zbuffer[i] = -std::numeric_limits<float>::max();

    /*for (int i = 0; i < obj->nfaces(); i++) {
        Vec4f v[3];
        for (int j = 0; j < 3; j++) {
            v[j] = shader.vertex(i, j);
        }
        triangle(v, shader, frame, zbuffer);
    }*/

	std::vector<Model*> objs;
	objs.push_back(new Model("D:/Documents/vision/course/smallRasterizer/asset/gun/Cerberus.obj"));
	//objs.push_back(new Model("D:/Documents/vision/course/smallRasterizer/obj/fuhua/fuhuahair.obj"));
	//objs.push_back(new Model("D:/Documents/vision/course/smallRasterizer/obj/fuhua/fuhuacloak.obj"));
	//objs.push_back(new Model("D:/Documents/vision/course/smallRasterizer/obj/fuhua/fuhuabody.obj"));
	int cnt = 0;
	for (auto obj : objs) {
		shader.payload.obj = obj;
		for (int i = 0; i < obj->nfaces(); i++) {
			Vec4f v[3];
			for (int j = 0; j < 3; j++) {
				v[j] = shader.vertex(i, j);
			}
			if (!backCulling(shader.payload.ndcCoord[0], shader.payload.ndcCoord[1], shader.payload.ndcCoord[2])) {
				triangle(v, shader, frame, zbuffer);
				cnt++;
			}
		}
	}
	std::cout << cnt << std::endl;

    writePPM(static_cast<char*>("image.ppm"), frame);	// origin at the left top

	//delete[] depth;
	delete[] frame;
	delete[] zbuffer;
}