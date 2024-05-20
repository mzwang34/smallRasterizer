#include "geometry.h"

#define M_PI 3.14159265358979323846 

Matrix4f projection(float fov, float aspect, float near, float far) {
	float t = std::abs(near) * std::tan(fov / 2. / 180. * M_PI);
	float r = aspect * t;
	float b = -t, l = -r;

	Matrix4f translate(1, 0, 0, -(t+b)/2.f, 
					   0, 1, 0, -(l+r)/2.f,
					   0, 0, 1, -(near + far)/2.f,
					   0, 0, 0, 1);
	Matrix4f scale(2.f/(r-l), 0, 0, 0,
				   0, 2.f/(t-b), 0, 0,
				   0, 0, 2.f/(near-far), 0,
				   0, 0, 0, 1); 
	Matrix4f persp2ortho(near, 0, 0, 0,
						 0, near, 0, 0, 
						 0, 0, near+far, -near*far,
						 0, 0, 1, 0);
	
	return scale * translate * persp2ortho;
}

Matrix4f ortho_projection(float l, float r, float b, float t, float near, float far) {
	Matrix4f translate(1, 0, 0, -(t+b)/2.f, 
					   0, 1, 0, -(l+r)/2.f,
					   0, 0, 1, -(near + far)/2.f,
					   0, 0, 0, 1);
	Matrix4f scale(2.f/(r-l), 0, 0, 0,
				   0, 2.f/(t-b), 0, 0,
				   0, 0, 2.f/(near-far), 0,
				   0, 0, 0, 1); 
	
	return scale * translate;
}

// raster space origin is at left top
// while NDC space origin is at left bottom
// therefore we need flip y axis when scale
Matrix4f viewport(int w, int h) {
	return Matrix4f(w/2.f, 0, 0, w/2.f,
					0, -h/2.f, 0, h/2.f,
					// 0, 0, 255/2.f, 255/2.f,
					0, 0, 1, 0,
					0, 0, 0, 1);
}

Matrix4f view(Vec3f camera, Vec3f up, Vec3f target) {
	Vec3f z = (camera - target).normalize();	// point to viewer
	Vec3f x = cross(up, z).normalize();	
	Vec3f y = cross(z, x).normalize();

	Matrix4f rotate(x.x, x.y, x.z, 0,
					y.x, y.y, y.z, 0,
					z.x, z.y, z.z, 0,
					0, 0, 0, 1);

	Matrix4f translate(1, 0, 0, -camera.x,
					   0, 1, 0, -camera.y,
					   0, 0, 1, -camera.z,
					   0, 0, 0, 1);

	return rotate * translate;
}

Matrix4f model(float angle) {
	angle = angle / 180.0 * M_PI;
	Matrix4f rotate(cos(angle), 0, sin(angle), 0,
					0, 1, 0, 0,
					-sin(angle), 0, cos(angle), 0,
					0, 0, 0, 1);
	Matrix4f translate = Matrix4f::identity();
	Matrix4f scale = Matrix4f::identity();
	return translate * rotate * scale;
}