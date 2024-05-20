#ifndef VEC3_H
#define VEC3_H

#include <cmath>

template<typename T>
class Vec2 {
public:
    T x, y;

    Vec2() : x(0), y(0) {}
    Vec2(T x_, T y_) : x(x_), y(y_) {}

    Vec2 operator + (const Vec2 &v) const {
        return Vec2(x + v.x, y + v.y);
    }
    Vec2 operator - (const Vec2 &v) const {
        return Vec2(x - v.x, y - v.y);
    }
    Vec2 operator * (const Vec2 &v) const {
        return Vec2(x * v.x, y * v.y);
    }
    Vec2 operator * (const T &a) const {
        return Vec2(x * a, y * a);
    }
    Vec2 operator / (const T &a) const {
        return Vec2(x / a, y / a);
    }
    T cross(const Vec2 &v) const {
        return x * v.y - y * v.x;
    }
    T operator [] (uint8_t i) {
        return i <= 0? x : y;
    }
};

template<typename T>
class Vec3 {
public:
    T x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator + (const Vec3 &v) const {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }
    Vec3 operator - (const Vec3 &v) const {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }
    Vec3 operator * (const Vec3 &v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }
    Vec3 operator * (const T &a) const {
        return Vec3(x * a, y * a, z * a);
    }
    Vec3 operator / (const T &a) const {
        return Vec3(x / a, y / a, z / a);
    }
    Vec3 operator / (const Vec3& v) const {
        return Vec3(x / v.x, y / v.y, z / v.z);
    }
    T dot(const Vec3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }
    Vec3 cross(const Vec3 &v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    T norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    Vec3 normalize() {
        *this = (*this) * (1 / norm());
        return *this;
        // return Vec3(x * (1/norm()), y * (1/norm()), z * (1/norm()));
    }
    T operator [] (uint8_t i) {
        return i <= 0? x : (i == 1? y : z);
    }
};

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;

template<typename T>
class Vec4 {
public:
    T x, y, z, w;

    Vec4() : x(0), y(0), z(0), w(1) {}
    Vec4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}

    T operator [] (uint8_t i) {
        return i <= 0? x : (i == 1? y : (i == 2? z : w));
    }
};

typedef Vec4<float> Vec4f;

template<typename T>
class Matrix4 {
public:
    T x[4][4] = {{0, 0, 0, 0}, 
                 {0, 0, 0, 0},
                 {0, 0, 0, 0},
                 {0, 0, 0, 0}};
    
    Matrix4() {}
    Matrix4(T a, T b, T c, T d, T e, T f, T g, T h,
           T i, T j, T k, T l, T m, T n, T o, T p) {
            x[0][0] = a;
            x[0][1] = b;
            x[0][2] = c;
            x[0][3] = d;
            
            x[1][0] = e;
            x[1][1] = f;
            x[1][2] = g;
            x[1][3] = h;

            x[2][0] = i;
            x[2][1] = j;
            x[2][2] = k;
            x[2][3] = l;

            x[3][0] = m;
            x[3][1] = n;
            x[3][2] = o;
            x[3][3] = p;
           }
    
    T* operator [] (uint8_t i) {
        return x[i];
    }
    const T* operator [] (uint8_t i) const {
        return x[i];
    }
    Matrix4 operator * (const Matrix4 &v) const {
        Matrix4 res, a = *this;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                res[i][j] = a[i][0] * v[0][j] + a[i][1] * v[1][j] + 
                            a[i][2] * v[2][j] + a[i][3] * v[3][j];
        return res;
    }
    Vec4f operator * (const Vec4f &v) const {
        Matrix4 a = *this;
        return Vec4f(a[0][0] * v.x + a[0][1] * v.y + a[0][2] * v.z + a[0][3] * v.w,
                     a[1][0] * v.x + a[1][1] * v.y + a[1][2] * v.z + a[1][3] * v.w,
                     a[2][0] * v.x + a[2][1] * v.y + a[2][2] * v.z + a[2][3] * v.w,
                     a[3][0] * v.x + a[3][1] * v.y + a[3][2] * v.z + a[3][3] * v.w);
    }
    Matrix4 operator / (const T &a) {
        Matrix4 A(*this);

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                A[i][j] /= a;
        
        return A;
    }
    static Matrix4 identity() {
        return Matrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    }
    T det() {
        Matrix4 A(*this);
        return
            A[0][3]*A[1][2]*A[2][1]*A[3][0] - A[0][2]*A[1][3]*A[2][1]*A[3][0] -
            A[0][3]*A[1][1]*A[2][2]*A[3][0] + A[0][1]*A[1][3]*A[2][2]*A[3][0] +
            A[0][2]*A[1][1]*A[2][3]*A[3][0] - A[0][1]*A[1][2]*A[2][3]*A[3][0] -
            A[0][3]*A[1][2]*A[2][0]*A[3][1] + A[0][2]*A[1][3]*A[2][0]*A[3][1] +
            A[0][3]*A[1][0]*A[2][2]*A[3][1] - A[0][0]*A[1][3]*A[2][2]*A[3][1] -
            A[0][2]*A[1][0]*A[2][3]*A[3][1] + A[0][0]*A[1][2]*A[2][3]*A[3][1] +
            A[0][3]*A[1][1]*A[2][0]*A[3][2] - A[0][1]*A[1][3]*A[2][0]*A[3][2] -
            A[0][3]*A[1][0]*A[2][1]*A[3][2] + A[0][0]*A[1][3]*A[2][1]*A[3][2] +
            A[0][1]*A[1][0]*A[2][3]*A[3][2] - A[0][0]*A[1][1]*A[2][3]*A[3][2] -
            A[0][2]*A[1][1]*A[2][0]*A[3][3] + A[0][1]*A[1][2]*A[2][0]*A[3][3] +
            A[0][2]*A[1][0]*A[2][1]*A[3][3] - A[0][0]*A[1][2]*A[2][1]*A[3][3] -
            A[0][1]*A[1][0]*A[2][2]*A[3][3] + A[0][0]*A[1][1]*A[2][2]*A[3][3];
    }
    Matrix4 inv() {
        Matrix4 A(*this), B;

        B[0][0] = A[1][2]*A[2][3]*A[3][1] - A[1][3]*A[2][2]*A[3][1] + A[1][3]*A[2][1]*A[3][2] - A[1][1]*A[2][3]*A[3][2] - A[1][2]*A[2][1]*A[3][3] + A[1][1]*A[2][2]*A[3][3];
        B[0][1] = A[0][3]*A[2][2]*A[3][1] - A[0][2]*A[2][3]*A[3][1] - A[0][3]*A[2][1]*A[3][2] + A[0][1]*A[2][3]*A[3][2] + A[0][2]*A[2][1]*A[3][3] - A[0][1]*A[2][2]*A[3][3];
        B[0][2] = A[0][2]*A[1][3]*A[3][1] - A[0][3]*A[1][2]*A[3][1] + A[0][3]*A[1][1]*A[3][2] - A[0][1]*A[1][3]*A[3][2] - A[0][2]*A[1][1]*A[3][3] + A[0][1]*A[1][2]*A[3][3];
        B[0][3] = A[0][3]*A[1][2]*A[2][1] - A[0][2]*A[1][3]*A[2][1] - A[0][3]*A[1][1]*A[2][2] + A[0][1]*A[1][3]*A[2][2] + A[0][2]*A[1][1]*A[2][3] - A[0][1]*A[1][2]*A[2][3];
        B[1][0] = A[1][3]*A[2][2]*A[3][0] - A[1][2]*A[2][3]*A[3][0] - A[1][3]*A[2][0]*A[3][2] + A[1][0]*A[2][3]*A[3][2] + A[1][2]*A[2][0]*A[3][3] - A[1][0]*A[2][2]*A[3][3];
        B[1][1] = A[0][2]*A[2][3]*A[3][0] - A[0][3]*A[2][2]*A[3][0] + A[0][3]*A[2][0]*A[3][2] - A[0][0]*A[2][3]*A[3][2] - A[0][2]*A[2][0]*A[3][3] + A[0][0]*A[2][2]*A[3][3];
        B[1][2] = A[0][3]*A[1][2]*A[3][0] - A[0][2]*A[1][3]*A[3][0] - A[0][3]*A[1][0]*A[3][2] + A[0][0]*A[1][3]*A[3][2] + A[0][2]*A[1][0]*A[3][3] - A[0][0]*A[1][2]*A[3][3];
        B[1][3] = A[0][2]*A[1][3]*A[2][0] - A[0][3]*A[1][2]*A[2][0] + A[0][3]*A[1][0]*A[2][2] - A[0][0]*A[1][3]*A[2][2] - A[0][2]*A[1][0]*A[2][3] + A[0][0]*A[1][2]*A[2][3];
        B[2][0] = A[1][1]*A[2][3]*A[3][0] - A[1][3]*A[2][1]*A[3][0] + A[1][3]*A[2][0]*A[3][1] - A[1][0]*A[2][3]*A[3][1] - A[1][1]*A[2][0]*A[3][3] + A[1][0]*A[2][1]*A[3][3];
        B[2][1] = A[0][3]*A[2][1]*A[3][0] - A[0][1]*A[2][3]*A[3][0] - A[0][3]*A[2][0]*A[3][1] + A[0][0]*A[2][3]*A[3][1] + A[0][1]*A[2][0]*A[3][3] - A[0][0]*A[2][1]*A[3][3];
        B[2][2] = A[0][1]*A[1][3]*A[3][0] - A[0][3]*A[1][1]*A[3][0] + A[0][3]*A[1][0]*A[3][1] - A[0][0]*A[1][3]*A[3][1] - A[0][1]*A[1][0]*A[3][3] + A[0][0]*A[1][1]*A[3][3];
        B[2][3] = A[0][3]*A[1][1]*A[2][0] - A[0][1]*A[1][3]*A[2][0] - A[0][3]*A[1][0]*A[2][1] + A[0][0]*A[1][3]*A[2][1] + A[0][1]*A[1][0]*A[2][3] - A[0][0]*A[1][1]*A[2][3];
        B[3][0] = A[1][2]*A[2][1]*A[3][0] - A[1][1]*A[2][2]*A[3][0] - A[1][2]*A[2][0]*A[3][1] + A[1][0]*A[2][2]*A[3][1] + A[1][1]*A[2][0]*A[3][2] - A[1][0]*A[2][1]*A[3][2];
        B[3][1] = A[0][1]*A[2][2]*A[3][0] - A[0][2]*A[2][1]*A[3][0] + A[0][2]*A[2][0]*A[3][1] - A[0][0]*A[2][2]*A[3][1] - A[0][1]*A[2][0]*A[3][2] + A[0][0]*A[2][1]*A[3][2];
        B[3][2] = A[0][2]*A[1][1]*A[3][0] - A[0][1]*A[1][2]*A[3][0] - A[0][2]*A[1][0]*A[3][1] + A[0][0]*A[1][2]*A[3][1] + A[0][1]*A[1][0]*A[3][2] - A[0][0]*A[1][1]*A[3][2];
        B[3][3] = A[0][1]*A[1][2]*A[2][0] - A[0][2]*A[1][1]*A[2][0] + A[0][2]*A[1][0]*A[2][1] - A[0][0]*A[1][2]*A[2][1] - A[0][1]*A[1][0]*A[2][2] + A[0][0]*A[1][1]*A[2][2];

        return B / det();
    }
    Matrix4 transpose() {
        Matrix4 A(*this), B;

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                B[i][j] = A[j][i];
        
        return B;
    }
};

typedef Matrix4<float> Matrix4f;

template<typename T>
class Matrix3 {
public:
    T x[3][3] = {{0, 0, 0}, 
                 {0, 0, 0},
                 {0, 0, 0}};
    Matrix3() {}
    Matrix3(T a, T b, T c, T d, T e, T f, T g, T h, T i) {
            x[0][0] = a;
            x[0][1] = b;
            x[0][2] = c;
            
            x[1][0] = d;
            x[1][1] = e;
            x[1][2] = f;

            x[2][0] = g;
            x[2][1] = h;
            x[2][2] = i;
    }
    const T* operator [] (uint8_t i) const {
        return x[i];
    }
    Vec3<T> operator *(const Vec3<T> &v) {
        Matrix3 A(*this);
        return Vec3<T>(A[0][0] * v.x + A[0][1] * v.y + A[0][2] * v.z,
                       A[1][0] * v.x + A[1][1] * v.y + A[1][2] * v.z,
                       A[2][0] * v.x + A[2][1] * v.y + A[2][2] * v.z);
    }
};

typedef Matrix3<float> Matrix3f;

template<typename T>
Vec3<T> cross(Vec3<T> &v1, Vec3<T> &v2) {
    return Vec3<T>(v1.y*v2.z-v1.z*v2.y, v1.z*v2.x-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x);
}

template<typename T>
T dot(Vec3<T> &v1, Vec3<T> &v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

//Vec3f proj3(Vec4f v) {
//    return Vec3f(v.x / v.w, v.y / v.w, v.z / v.w);
//}

//Vec4f proj4(Vec3f v) {
//    return Vec4f(v.x, v.y, v.z, 1.f);
//}

template<typename T>
Vec3<T> proj3(Vec4<T> v) {
    return Vec3<T>(v.x / v.w, v.y / v.w, v.z / v.w);
}

template<typename T>
Vec4<T> proj4(Vec3<T> v) {
    return Vec4<T>(v.x, v.y, v.z, 1.f);
}



#endif