//////////////////////////////////////////////////////////////////////
// Matrix44.cpp - 4 x 4 transformation matrices
//
// Copyright David K. McAllister, Aug. 1998.

#include "Math/Matrix44.h"

#include "Math/Vector.h"

//////////////////////////////////////////////////////////
// Private member functions.

template <class Fl_T> void Matrix44<Fl_T>::build_identity(Fl_T m[4][4]) const
{
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
    m[0][1] = m[0][2] = m[0][3] = 0.0;
    m[1][0] = m[1][2] = m[1][3] = 0.0;
    m[2][0] = m[2][1] = m[2][3] = 0.0;
    m[3][0] = m[3][1] = m[3][2] = 0.0;
}
template void Matrix44<float>::build_identity(float m[4][4]) const;
template void Matrix44<double>::build_identity(double m[4][4]) const;

template <class Fl_T> void Matrix44<Fl_T>::build_scale(Fl_T m[4][4], const t3Vector<Fl_T>& v) const
{
    build_identity(m);
    m[0][0] = v.x;
    m[1][1] = v.y;
    m[2][2] = v.z;
}
template void Matrix44<float>::build_scale(float m[4][4], const t3Vector<float>& v) const;
template void Matrix44<double>::build_scale(double m[4][4], const t3Vector<double>& v) const;

template <class Fl_T> void Matrix44<Fl_T>::build_translate(Fl_T m[4][4], const t3Vector<Fl_T>& v) const
{
    build_identity(m);
    m[0][3] = v.x;
    m[1][3] = v.y;
    m[2][3] = v.z;
}
template void Matrix44<float>::build_translate(float m[4][4], const t3Vector<float>& v) const;
template void Matrix44<double>::build_translate(double m[4][4], const t3Vector<double>& v) const;

template <class Fl_T> void Matrix44<Fl_T>::build_rotate(Fl_T m[4][4], Fl_T angle, const t3Vector<Fl_T>& naxis) const
{
    t3Vector<Fl_T> axis = naxis.normal(); // Normalize the axis to match glRotate().

    // NOTE: Element 0,1 is wrong in Foley and Van Dam, Pg 227!
    Fl_T sintheta = sin(angle);
    Fl_T costheta = cos(angle);
    Fl_T ux = axis.x;
    Fl_T uy = axis.y;
    Fl_T uz = axis.z;
    m[0][0] = ux * ux + costheta * (1 - ux * ux);
    m[0][1] = ux * uy * (1 - costheta) - uz * sintheta;
    m[0][2] = uz * ux * (1 - costheta) + uy * sintheta;
    m[0][3] = 0;

    m[1][0] = ux * uy * (1 - costheta) + uz * sintheta;
    m[1][1] = uy * uy + costheta * (1 - uy * uy);
    m[1][2] = uy * uz * (1 - costheta) - ux * sintheta;
    m[1][3] = 0;

    m[2][0] = uz * ux * (1 - costheta) - uy * sintheta;
    m[2][1] = uy * uz * (1 - costheta) + ux * sintheta;
    m[2][2] = uz * uz + costheta * (1 - uz * uz);
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
}
template void Matrix44<float>::build_rotate(float m[4][4], float angle, const t3Vector<float>& axis) const;
template void Matrix44<double>::build_rotate(double m[4][4], double angle, const t3Vector<double>& axis) const;

// Returns true if it worked, false if non-invertible.
template <class Fl_T> bool Matrix44<Fl_T>::build_inverse(Fl_T m[4][4]) const
{
    Fl_T p[4][4];
    build_identity(p);

    // Make it upper triangular using Gauss-Jordan with partial pivoting.
    for (int i = 0; i < 4; i++) {
        // Find largest row.
        Fl_T maxv = dmcm::Abs(m[i][i]);
        int row = i;
        for (int j = i + 1; j < 4; j++) {
            if (dmcm::Abs(m[j][i]) > maxv) {
                maxv = dmcm::Abs(m[j][i]);
                row = j;
            }
        }

        // Pivot around largest row.
        if (maxv <= singularity_thresh) return false;

        if (row != i) {
            switch_rows(m, i, row);
            switch_rows(p, i, row);
        }

        // Subtract scaled rows to eliminate column i.
        ASSERT_D(m[i][i] != 0);
        Fl_T denom = 1. / m[i][i];
        for (int j = i + 1; j < 4; j++) {
            Fl_T factor = m[j][i] * denom;
            sub_rows(m, j, i, factor);
            sub_rows(p, j, i, factor);
        }
    }

    // Diagonalize m using Jordan.
    for (int i = 1; i < 4; i++) {
        ASSERT_D(m[i][i] != 0);
        Fl_T denom = 1. / m[i][i];
        for (int j = 0; j < i; j++) {
            Fl_T factor = m[j][i] * denom;
            sub_rows(m, j, i, factor);
            sub_rows(p, j, i, factor);
        }
    }

    // Normalize m to the identity and copy p over m.
    for (int i = 0; i < 4; i++) {
        ASSERT_D(m[i][i] != 0);
        Fl_T factor = 1. / m[i][i];
        for (int j = 0; j < 4; j++) {
            // As if we were doing m[i][j] *= factor
            p[i][j] *= factor;
            m[i][j] = p[i][j];
        }
    }

    return true;
}
template bool Matrix44<float>::build_inverse(float m[4][4]) const;
template bool Matrix44<double>::build_inverse(double m[4][4]) const;

template <class Fl_T> void Matrix44<Fl_T>::build_transpose(Fl_T m[4][4]) const
{
    Fl_T t;
    t = m[0][1];
    m[0][1] = m[1][0];
    m[1][0] = t;
    t = m[0][2];
    m[0][2] = m[2][0];
    m[2][0] = t;
    t = m[0][3];
    m[0][3] = m[3][0];
    m[3][0] = t;
    t = m[1][2];
    m[1][2] = m[2][1];
    m[2][1] = t;
    t = m[1][3];
    m[1][3] = m[3][1];
    m[3][1] = t;
    t = m[2][3];
    m[2][3] = m[3][2];
    m[3][2] = t;
}
template void Matrix44<float>::build_transpose(float m[4][4]) const;
template void Matrix44<double>::build_transpose(double m[4][4]) const;

template <class Fl_T> void Matrix44<Fl_T>::post_mulmat(Fl_T to[4][4], const Fl_T from[4][4])
{
    Fl_T newmat[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            newmat[i][j] = 0.0;
            for (int k = 0; k < 4; k++) { newmat[i][j] += to[i][k] * from[k][j]; }
        }
    }

    copy_mat(to, newmat);
}
template void Matrix44<float>::post_mulmat(float to[4][4], const float from[4][4]);
template void Matrix44<double>::post_mulmat(double to[4][4], const double from[4][4]);

template <class Fl_T> void Matrix44<Fl_T>::pre_mulmat(Fl_T to[4][4], const Fl_T from[4][4])
{
    Fl_T newmat[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            newmat[i][j] = 0.0;
            for (int k = 0; k < 4; k++) { newmat[i][j] += from[i][k] * to[k][j]; }
        }
    }

    copy_mat(to, newmat);
}
template void Matrix44<float>::pre_mulmat(float to[4][4], const float from[4][4]);
template void Matrix44<double>::pre_mulmat(double to[4][4], const double from[4][4]);

//////////////////////////////////////////////////////////
// Public member functions.

template <class Fl_T> std::string Matrix44<Fl_T>::string() const
{
    const int PRDIG = 8;
    char xx[40];
    std::string st;
    for (int i = 0; i < 4; i++) {
        st += std::string("[");
        st += gcvt(mat[i][0], PRDIG, xx) + std::string(", ");
        st += gcvt(mat[i][1], PRDIG, xx) + std::string(", ");
        st += gcvt(mat[i][2], PRDIG, xx) + std::string(", ");
        st += gcvt(mat[i][3], PRDIG, xx) + std::string("]\n");
    }

    return st;
}
template std::string Matrix44<float>::string() const;
template std::string Matrix44<double>::string() const;

template <class Fl_T> std::string Matrix44<Fl_T>::string_inv() const
{
    const int PRDIG = 8;
    char xx[40];
    std::string st;
    for (int i = 0; i < 4; i++) {
        st += std::string("[");
        for (int j = 0; j < 4; j++) st += gcvt(imat[i][j], PRDIG, xx) + std::string(", ");
        st += std::string("]\n");
    }

    return st;
}
template std::string Matrix44<float>::string_inv() const;
template std::string Matrix44<double>::string_inv() const;

template <class Fl_T> Fl_T Matrix44<Fl_T>::Determinant() { return mat[0][0] * det3(0) - mat[0][1] * det3(1) + mat[0][2] * det3(2) - mat[0][3] * det3(3); }
template float Matrix44<float>::Determinant();
template double Matrix44<double>::Determinant();

template <class Fl_T> void Matrix44<Fl_T>::PostTrans(const Matrix44<Fl_T>& T)
{
    if (T.is_identity) return;
    if (is_identity) {
        // Copying is faster than matrix multiply.
        copy_mat(mat, T.mat);
        copy_mat(imat, T.imat);
        is_identity = false;
        inverse_valid = T.inverse_valid;
        return;
    }

    post_mulmat(mat, T.mat);
    pre_mulmat(imat, T.imat);
    inverse_valid = inverse_valid && T.inverse_valid;
}
template void Matrix44<float>::PostTrans(const Matrix44<float>& T);
template void Matrix44<double>::PostTrans(const Matrix44<double>& T);

template <class Fl_T> void Matrix44<Fl_T>::PreTrans(const Matrix44<Fl_T>& T)
{
    if (T.is_identity) return;
    if (is_identity) {
        // Copying is faster than matrix multiply.
        copy_mat(mat, T.mat);
        copy_mat(imat, T.imat);
        is_identity = false;
        inverse_valid = T.inverse_valid;
        return;
    }

    pre_mulmat(mat, T.mat);
    post_mulmat(imat, T.imat);
    inverse_valid = inverse_valid && T.inverse_valid;
}
template void Matrix44<float>::PreTrans(const Matrix44<float>& T);
template void Matrix44<double>::PreTrans(const Matrix44<double>& T);

// Load these column vectors. Rest of matrix is identity.
// Assumes x,y,z form an orthonormal basis so that the inverse is the transpose.
template <class Fl_T> void Matrix44<Fl_T>::LoadFrame(const t3Vector<Fl_T>& x, const t3Vector<Fl_T>& y, const t3Vector<Fl_T>& z)
{
    mat[3][3] = imat[3][3] = 1.0;
    mat[0][3] = mat[1][3] = mat[2][3] = 0.0;
    imat[0][3] = imat[1][3] = imat[2][3] = 0.0;

    mat[3][0] = mat[3][1] = mat[3][2] = 0.0;
    imat[3][0] = imat[3][1] = imat[3][2] = 0.0;

    mat[0][0] = x.x;
    mat[1][0] = x.y;
    mat[2][0] = x.z;

    mat[0][1] = y.x;
    mat[1][1] = y.y;
    mat[2][1] = y.z;

    mat[0][2] = z.x;
    mat[1][2] = z.y;
    mat[2][2] = z.z;

    imat[0][0] = x.x;
    imat[0][1] = x.y;
    imat[0][2] = x.z;

    imat[1][0] = y.x;
    imat[1][1] = y.y;
    imat[1][2] = y.z;

    imat[2][0] = z.x;
    imat[2][1] = z.y;
    imat[2][2] = z.z;

    inverse_valid = true;
    is_identity = false;
}
template void Matrix44<float>::LoadFrame(const t3Vector<float>& x, const t3Vector<float>& y, const t3Vector<float>& z);
template void Matrix44<double>::LoadFrame(const t3Vector<double>& x, const t3Vector<double>& y, const t3Vector<double>& z);

// Loads a rotation frame and an offset.
template <class Fl_T> void Matrix44<Fl_T>::LoadFrame(const t3Vector<Fl_T>& x, const t3Vector<Fl_T>& y, const t3Vector<Fl_T>& z, const t3Vector<Fl_T>& t)
{
    LoadFrame(x, y, z);
    inverse_valid = false;

    mat[0][3] = t.x;
    mat[1][3] = t.y;
    mat[2][3] = t.z;
}
template void Matrix44<float>::LoadFrame(const t3Vector<float>& x, const t3Vector<float>& y, const t3Vector<float>& z, const t3Vector<float>& t);
template void Matrix44<double>::LoadFrame(const t3Vector<double>& x, const t3Vector<double>& y, const t3Vector<double>& z, const t3Vector<double>& t);

template <class Fl_T> void Matrix44<Fl_T>::GetFrame(t3Vector<Fl_T>& c0, t3Vector<Fl_T>& c1, t3Vector<Fl_T>& c2)
{
    c0.x = mat[0][0];
    c1.x = mat[0][1];
    c2.x = mat[0][2];
    c0.y = mat[1][0];
    c1.y = mat[1][1];
    c2.y = mat[1][2];
    c0.z = mat[2][0];
    c1.z = mat[2][1];
    c2.z = mat[2][2];
}
template void Matrix44<float>::GetFrame(t3Vector<float>& c0, t3Vector<float>& c1, t3Vector<float>& c2);
template void Matrix44<double>::GetFrame(t3Vector<double>& c0, t3Vector<double>& c1, t3Vector<double>& c2);

template <class Fl_T> void Matrix44<Fl_T>::GetFrame(t3Vector<Fl_T>& c0, t3Vector<Fl_T>& c1, t3Vector<Fl_T>& c2, t3Vector<Fl_T>& c3)
{
    c0.x = mat[0][0];
    c1.x = mat[0][1];
    c2.x = mat[0][2];
    c3.x = mat[0][3];
    c0.y = mat[1][0];
    c1.y = mat[1][1];
    c2.y = mat[1][2];
    c3.y = mat[1][3];
    c0.z = mat[2][0];
    c1.z = mat[2][1];
    c2.z = mat[2][2];
    c3.z = mat[2][3];
}
template void Matrix44<float>::GetFrame(t3Vector<float>& c0, t3Vector<float>& c1, t3Vector<float>& c2, t3Vector<float>& c3);
template void Matrix44<double>::GetFrame(t3Vector<double>& c0, t3Vector<double>& c1, t3Vector<double>& c2, t3Vector<double>& c3);

template <class Fl_T> void Matrix44<Fl_T>::ChangeBasis(const Matrix44<Fl_T>& T)
{
    // If T.imat is invalid it will not only make
    // imat invalid, but mat will be invalid.
    if (!inverse_valid) compute_inverse();

    // XXX Not yet optimized for the identity.
    pre_mulmat(mat, T.imat);
    post_mulmat(mat, T.mat);

    // XXX I need to check this.
    pre_mulmat(imat, T.imat);
    post_mulmat(imat, T.mat);
    is_identity = false;
}
template void Matrix44<float>::ChangeBasis(const Matrix44<float>& T);
template void Matrix44<double>::ChangeBasis(const Matrix44<double>& T);

template <class Fl_T> void Matrix44<Fl_T>::Scale(const t3Vector<Fl_T>& v)
{
    Fl_T m[4][4];
    build_scale(m, v);
    if (is_identity)
        copy_mat(mat, m);
    else
        post_mulmat(mat, m);

    m[0][0] = 1. / m[0][0];
    m[1][1] = 1. / m[1][1];
    m[2][2] = 1. / m[2][2];
    if (is_identity)
        copy_mat(imat, m);
    else
        pre_mulmat(imat, m);
    is_identity = false;
}
template void Matrix44<float>::Scale(const t3Vector<float>& v);
template void Matrix44<double>::Scale(const t3Vector<double>& v);

template <class Fl_T> void Matrix44<Fl_T>::Rotate(Fl_T angle, const t3Vector<Fl_T>& axis)
{
    if (angle == 0) return;

    Fl_T m[4][4];
    build_rotate(m, angle, axis);
    if (is_identity)
        copy_mat(mat, m);
    else
        post_mulmat(mat, m);

    // Make the inverse
    build_transpose(m);
    if (is_identity)
        copy_mat(imat, m);
    else
        pre_mulmat(imat, m);
    is_identity = false;
}
template void Matrix44<float>::Rotate(float angle, const t3Vector<float>& axis);
template void Matrix44<double>::Rotate(double angle, const t3Vector<double>& axis);

template <class Fl_T> void Matrix44<Fl_T>::Translate(const t3Vector<Fl_T>& v)
{
    Fl_T m[4][4];
    build_translate(m, v);
    if (is_identity)
        copy_mat(mat, m);
    else
        post_mulmat(mat, m);

    // Make the inverse
    m[0][3] = -m[0][3];
    m[1][3] = -m[1][3];
    m[2][3] = -m[2][3];

    if (is_identity)
        copy_mat(imat, m);
    else
        pre_mulmat(imat, m);
    is_identity = false;
}
template void Matrix44<float>::Translate(const t3Vector<float>& v);
template void Matrix44<double>::Translate(const t3Vector<double>& v);

template <class Fl_T> bool Matrix44<Fl_T>::Invert()
{
    if (inverse_valid) {
        if (is_identity) return true;

        // Just swap it with its inverse.
        Fl_T temp[4][4];
        copy_mat(temp, mat);
        copy_mat(mat, imat);
        copy_mat(imat, temp);
        return true;
    } else {
        // Copy mat to imat, then invert old mat.
        copy_mat(imat, mat);
        inverse_valid = true;
        if (is_identity) return true;

        return inverse_valid = build_inverse(mat);
    }
}
template bool Matrix44<float>::Invert();
template bool Matrix44<double>::Invert();

template <class Fl_T> void Matrix44<Fl_T>::Transpose()
{
    if (is_identity) return;

    build_transpose(mat);

    // Inverse of the transpose is the transpose of the inverse
    if (inverse_valid) build_transpose(imat);
}
template void Matrix44<float>::Transpose();
template void Matrix44<double>::Transpose();

template <class Fl_T> void Matrix44<Fl_T>::Frustum(Fl_T l, Fl_T r, Fl_T b, Fl_T t, Fl_T n, Fl_T f)
{
    ASSERT_R(n > 0 && f > 0);
    Fl_T m[4][4];

    m[0][0] = (n + n) / (r - l);
    m[0][1] = 0;
    m[0][2] = (r + l) / (r - l);
    m[0][3] = 0;

    m[1][0] = 0;
    m[1][1] = (n + n) / (t - b);
    m[1][2] = (t + b) / (t - b);
    m[1][3] = 0;

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = -(f + n) / (f - n); // On the OpenGL man page, these aren't negative.
    m[2][3] = -2.0 * f * n / (f - n);

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = -1;
    m[3][3] = 0;

    if (is_identity)
        copy_mat(mat, m);
    else
        post_mulmat(mat, m);

    m[0][0] = (r - l) / (n + n);
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = (r + l) / (n + n);

    m[1][0] = 0;
    m[1][1] = (t - b) / (n + n);
    m[1][2] = 0;
    m[1][3] = (t + b) / (n + n);

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 0;
    m[2][3] = -1;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = -(f - n) / (2.0 * f * n);
    m[3][3] = (f + n) / (2.0 * f * n);

    if (is_identity) {
        copy_mat(imat, m);
        inverse_valid = true;
    } else {
        if (inverse_valid) pre_mulmat(imat, m);
    }

    is_identity = false;
}
template void Matrix44<float>::Frustum(float l, float r, float b, float t, float n, float f);
template void Matrix44<double>::Frustum(double l, double r, double b, double t, double n, double f);

// n and f are negative if view plane is behind eye.
template <class Fl_T> void Matrix44<Fl_T>::Ortho(Fl_T l, Fl_T r, Fl_T b, Fl_T t, Fl_T n, Fl_T f)
{
    Fl_T m[4][4];

    m[0][0] = 2.0 / (r - l);
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = (r + l) / (r - l);

    m[1][0] = 0;
    m[1][1] = 2.0 / (t - b);
    m[1][2] = 0;
    m[1][3] = (t + b) / (t - b);

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = -2.0 / (f - n);     // The signs of these were a pain.
    m[2][3] = -(f + n) / (f - n); // These signs make it work for OpenGL.

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

    if (is_identity)
        copy_mat(mat, m);
    else
        post_mulmat(mat, m);

    is_identity = false;
    // XXX I don't know what to do to the inverse, so toss it.
    inverse_valid = false;
}
template void Matrix44<float>::Ortho(float l, float r, float b, float t, float n, float f);
template void Matrix44<double>::Ortho(double l, double r, double b, double t, double n, double f);

template <class Fl_T> void Matrix44<Fl_T>::Perspective(Fl_T fovy, Fl_T aspect, Fl_T znear, Fl_T zfar)
{
    ASSERT_D(znear > 0 && zfar > 0);
    Fl_T top = znear * tan(fovy * 0.5);
    Fl_T bottom = -top;
    Fl_T left = bottom * aspect;
    Fl_T right = top * aspect;
    Frustum(left, right, bottom, top, znear, zfar);
}
template void Matrix44<float>::Perspective(float fovy, float aspect, float znear, float zfar);
template void Matrix44<double>::Perspective(double fovy, double aspect, double znear, double zfar);

template <class Fl_T> void Matrix44<Fl_T>::LookAt(const t3Vector<Fl_T>& eye, const t3Vector<Fl_T>& lookat, const t3Vector<Fl_T>& up)
{
    t3Vector<Fl_T> f(lookat - eye);
    f.normalize();

    t3Vector<Fl_T> upn(up);
    upn.normalize();

    t3Vector<Fl_T> s(Cross(f, upn));
    s.normalize();

    t3Vector<Fl_T> u(Cross(s, f));
    // u.normalize(); // This normalize shouldn't be necessary.

    Fl_T m[4][4];

    m[0][0] = s.x;
    m[0][1] = s.y;
    m[0][2] = s.z;
    m[0][3] = 0;

    m[1][0] = u.x;
    m[1][1] = u.y;
    m[1][2] = u.z;
    m[1][3] = 0;

    m[2][0] = -f.x;
    m[2][1] = -f.y;
    m[2][2] = -f.z;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

    if (is_identity)
        copy_mat(mat, m);
    else
        post_mulmat(mat, m);

    build_transpose(m);

    if (is_identity)
        copy_mat(imat, m);
    else
        pre_mulmat(imat, m);

    is_identity = false;

    Translate(-eye);
}
template void Matrix44<float>::LookAt(const t3Vector<float>& eye, const t3Vector<float>& lookat, const t3Vector<float>& up);
template void Matrix44<double>::LookAt(const t3Vector<double>& eye, const t3Vector<double>& lookat, const t3Vector<double>& up);

template <class Fl_T> t3Vector<Fl_T> Matrix44<Fl_T>::Project(const t3Vector<Fl_T>& p) const
{
    // XXX Should I put an optimization here for is_identity?

    Fl_T w1 = mat[3][0] * p.x + mat[3][1] * p.y + mat[3][2] * p.z + mat[3][3];
    w1 = 1. / w1;

    Fl_T xw = mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2] * p.z + mat[0][3];
    Fl_T yw = mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2] * p.z + mat[1][3];
    Fl_T zw = mat[2][0] * p.x + mat[2][1] * p.y + mat[2][2] * p.z + mat[2][3];

    return t3Vector<Fl_T>(xw * w1, yw * w1, zw * w1);
}
template t3Vector<float> Matrix44<float>::Project(const t3Vector<float>& p) const;
template t3Vector<double> Matrix44<double>::Project(const t3Vector<double>& p) const;

template <class Fl_T> t3Vector<Fl_T> Matrix44<Fl_T>::Project(const t3Vector<Fl_T>& p, const Fl_T w) const
{
    // XXX Should I put an optimization here for is_identity?

    Fl_T w1 = mat[3][0] * p.x + mat[3][1] * p.y + mat[3][2] * p.z + mat[3][3] * w;
    w1 = 1. / w1;

    Fl_T xw = mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2] * p.z + mat[0][3] * w;
    Fl_T yw = mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2] * p.z + mat[1][3] * w;
    Fl_T zw = mat[2][0] * p.x + mat[2][1] * p.y + mat[2][2] * p.z + mat[2][3] * w;

    return t3Vector<Fl_T>(xw * w1, yw * w1, zw * w1);
}
template t3Vector<float> Matrix44<float>::Project(const t3Vector<float>& p, const float w) const;
template t3Vector<double> Matrix44<double>::Project(const t3Vector<double>& p, const double w) const;

template <class Fl_T> void Matrix44<Fl_T>::Project(Fl_T& x, Fl_T& y, Fl_T& z, Fl_T& w) const
{
    // XXX Should I put an optimization here for is_identity?

    Fl_T x1 = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z + mat[0][3] * w;
    Fl_T y1 = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z + mat[1][3] * w;
    Fl_T z1 = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z + mat[2][3] * w;
    Fl_T w1 = mat[3][0] * x + mat[3][1] * y + mat[3][2] * z + mat[3][3] * w;
    x = x1;
    y = y1;
    z = z1;
    w = w1;
}
template void Matrix44<float>::Project(float& x, float& y, float& z, float& w) const;
template void Matrix44<double>::Project(double& x, double& y, double& z, double& w) const;

template <class Fl_T> t3Vector<Fl_T> Matrix44<Fl_T>::ProjectDirection(const t3Vector<Fl_T>& p) const
{
    // XXX Should I put an optimization here for is_identity?

    return t3Vector<Fl_T>(mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2] * p.z, mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2] * p.z,
                          mat[2][0] * p.x + mat[2][1] * p.y + mat[2][2] * p.z);
}
template t3Vector<float> Matrix44<float>::ProjectDirection(const t3Vector<float>& p) const;
template t3Vector<double> Matrix44<double>::ProjectDirection(const t3Vector<double>& p) const;

template <class Fl_T> t3Vector<Fl_T> Matrix44<Fl_T>::UnProject(const t3Vector<Fl_T>& p)
{
    // XXX Should I put an optimization here for is_identity?

    if (!inverse_valid) compute_inverse();

    Fl_T w1 = imat[3][0] * p.x + imat[3][1] * p.y + imat[3][2] * p.z + imat[3][3];
    w1 = 1. / w1;

    Fl_T xw = imat[0][0] * p.x + imat[0][1] * p.y + imat[0][2] * p.z + imat[0][3];
    Fl_T yw = imat[1][0] * p.x + imat[1][1] * p.y + imat[1][2] * p.z + imat[1][3];
    Fl_T zw = imat[2][0] * p.x + imat[2][1] * p.y + imat[2][2] * p.z + imat[2][3];

    return t3Vector<Fl_T>(xw * w1, yw * w1, zw * w1);
}
template t3Vector<float> Matrix44<float>::UnProject(const t3Vector<float>& p);
template t3Vector<double> Matrix44<double>::UnProject(const t3Vector<double>& p);

template <class Fl_T> bool Matrix44<Fl_T>::CheckNaN() const
{
    Fl_T* m = (Fl_T*)mat;
    for (int i = 0; i < 16; i++) ASSERT_RM(!dmcm::isNaN(m[i]), "Matrix has a NaN");

    return true;
}
template bool Matrix44<float>::CheckNaN() const;
template bool Matrix44<double>::CheckNaN() const;
