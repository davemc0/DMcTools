//////////////////////////////////////////////////////////////////////
// Matrix44.cpp - 4 x 4 transformation matrices
//
// Copyright David K. McAllister, Aug. 1998.

#include "Math/Matrix44.h"

#include "Math/Vector.h"

//////////////////////////////////////////////////////////
// Private member functions.

template <class Vec_T> void Matrix44<Vec_T>::build_identity(typename Vec_T::ElType m[4][4]) const
{
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
    m[0][1] = m[0][2] = m[0][3] = 0.0;
    m[1][0] = m[1][2] = m[1][3] = 0.0;
    m[2][0] = m[2][1] = m[2][3] = 0.0;
    m[3][0] = m[3][1] = m[3][2] = 0.0;
}
template void Matrix44<f3vec>::build_identity(float m[4][4]) const;
template void Matrix44<d3vec>::build_identity(double m[4][4]) const;

template <class Vec_T> void Matrix44<Vec_T>::build_scale(typename Vec_T::ElType m[4][4], const Vec_T& v) const
{
    build_identity(m);
    m[0][0] = v.x;
    m[1][1] = v.y;
    m[2][2] = v.z;
}
template void Matrix44<f3vec>::build_scale(float m[4][4], const f3vec& v) const;
template void Matrix44<d3vec>::build_scale(double m[4][4], const d3vec& v) const;

template <class Vec_T> void Matrix44<Vec_T>::build_translate(typename Vec_T::ElType m[4][4], const Vec_T& v) const
{
    build_identity(m);
    m[0][3] = v.x;
    m[1][3] = v.y;
    m[2][3] = v.z;
}
template void Matrix44<f3vec>::build_translate(float m[4][4], const f3vec& v) const;
template void Matrix44<d3vec>::build_translate(double m[4][4], const d3vec& v) const;

template <class Vec_T> void Matrix44<Vec_T>::build_rotate(typename Vec_T::ElType m[4][4], typename Vec_T::ElType angle, const Vec_T& naxis) const
{
    Vec_T axis = naxis.normalized(); // Normalize the axis to match glRotate().

    // NOTE: Element 0,1 is wrong in Foley and Van Dam, Pg 227!
    typename Vec_T::ElType sintheta = sin(angle);
    typename Vec_T::ElType costheta = cos(angle);
    typename Vec_T::ElType ux = axis.x;
    typename Vec_T::ElType uy = axis.y;
    typename Vec_T::ElType uz = axis.z;
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
template void Matrix44<f3vec>::build_rotate(float m[4][4], float angle, const f3vec& axis) const;
template void Matrix44<d3vec>::build_rotate(double m[4][4], double angle, const d3vec& axis) const;

// Returns true if it worked, false if non-invertible.
template <class Vec_T> bool Matrix44<Vec_T>::build_inverse(typename Vec_T::ElType m[4][4]) const
{
    typename Vec_T::ElType p[4][4];
    build_identity(p);

    // Make it upper triangular using Gauss-Jordan with partial pivoting.
    for (int i = 0; i < 4; i++) {
        // Find largest row.
        typename Vec_T::ElType maxv = abs(m[i][i]);
        int row = i;
        for (int j = i + 1; j < 4; j++) {
            if (abs(m[j][i]) > maxv) {
                maxv = abs(m[j][i]);
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
        typename Vec_T::ElType denom = 1. / m[i][i];
        for (int j = i + 1; j < 4; j++) {
            typename Vec_T::ElType factor = m[j][i] * denom;
            sub_rows(m, j, i, factor);
            sub_rows(p, j, i, factor);
        }
    }

    // Diagonalize m using Jordan.
    for (int i = 1; i < 4; i++) {
        ASSERT_D(m[i][i] != 0);
        typename Vec_T::ElType denom = 1. / m[i][i];
        for (int j = 0; j < i; j++) {
            typename Vec_T::ElType factor = m[j][i] * denom;
            sub_rows(m, j, i, factor);
            sub_rows(p, j, i, factor);
        }
    }

    // Normalize m to the identity and copy p over m.
    for (int i = 0; i < 4; i++) {
        ASSERT_D(m[i][i] != 0);
        typename Vec_T::ElType factor = 1. / m[i][i];
        for (int j = 0; j < 4; j++) {
            // As if we were doing m[i][j] *= factor
            p[i][j] *= factor;
            m[i][j] = p[i][j];
        }
    }

    return true;
}
template bool Matrix44<f3vec>::build_inverse(float m[4][4]) const;
template bool Matrix44<d3vec>::build_inverse(double m[4][4]) const;

template <class Vec_T> void Matrix44<Vec_T>::build_transpose(typename Vec_T::ElType m[4][4]) const
{
    typename Vec_T::ElType t;
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
template void Matrix44<f3vec>::build_transpose(float m[4][4]) const;
template void Matrix44<d3vec>::build_transpose(double m[4][4]) const;

template <class Vec_T> void Matrix44<Vec_T>::post_mulmat(typename Vec_T::ElType to[4][4], const typename Vec_T::ElType from[4][4])
{
    typename Vec_T::ElType newmat[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            newmat[i][j] = 0.0;
            for (int k = 0; k < 4; k++) { newmat[i][j] += to[i][k] * from[k][j]; }
        }
    }

    copy_mat(to, newmat);
}
template void Matrix44<f3vec>::post_mulmat(float to[4][4], const float from[4][4]);
template void Matrix44<d3vec>::post_mulmat(double to[4][4], const double from[4][4]);

template <class Vec_T> void Matrix44<Vec_T>::pre_mulmat(typename Vec_T::ElType to[4][4], const typename Vec_T::ElType from[4][4])
{
    typename Vec_T::ElType newmat[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            newmat[i][j] = 0.0;
            for (int k = 0; k < 4; k++) { newmat[i][j] += from[i][k] * to[k][j]; }
        }
    }

    copy_mat(to, newmat);
}
template void Matrix44<f3vec>::pre_mulmat(float to[4][4], const float from[4][4]);
template void Matrix44<d3vec>::pre_mulmat(double to[4][4], const double from[4][4]);

//////////////////////////////////////////////////////////
// Public member functions.

template <class Vec_T> std::string Matrix44<Vec_T>::string() const
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
template std::string Matrix44<f3vec>::string() const;
template std::string Matrix44<d3vec>::string() const;

template <class Vec_T> std::string Matrix44<Vec_T>::string_inv() const
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
template std::string Matrix44<f3vec>::string_inv() const;
template std::string Matrix44<d3vec>::string_inv() const;

template <class Vec_T> typename Vec_T::ElType Matrix44<Vec_T>::Determinant()
{
    return mat[0][0] * det3(0) - mat[0][1] * det3(1) + mat[0][2] * det3(2) - mat[0][3] * det3(3);
}
template float Matrix44<f3vec>::Determinant();
template double Matrix44<d3vec>::Determinant();

template <class Vec_T> void Matrix44<Vec_T>::PostTrans(const Matrix44<Vec_T>& T)
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
template void Matrix44<f3vec>::PostTrans(const Matrix44<f3vec>& T);
template void Matrix44<d3vec>::PostTrans(const Matrix44<d3vec>& T);

template <class Vec_T> void Matrix44<Vec_T>::PreTrans(const Matrix44<Vec_T>& T)
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
template void Matrix44<f3vec>::PreTrans(const Matrix44<f3vec>& T);
template void Matrix44<d3vec>::PreTrans(const Matrix44<d3vec>& T);

// Load these column vectors. Rest of matrix is identity.
// Assumes x,y,z form an orthonormal basis so that the inverse is the transpose.
template <class Vec_T> void Matrix44<Vec_T>::LoadFrame(const Vec_T& x, const Vec_T& y, const Vec_T& z)
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
template void Matrix44<f3vec>::LoadFrame(const f3vec& x, const f3vec& y, const f3vec& z);
template void Matrix44<d3vec>::LoadFrame(const d3vec& x, const d3vec& y, const d3vec& z);

// Loads a rotation frame and an offset.
template <class Vec_T> void Matrix44<Vec_T>::LoadFrame(const Vec_T& x, const Vec_T& y, const Vec_T& z, const Vec_T& t)
{
    LoadFrame(x, y, z);
    inverse_valid = false;

    mat[0][3] = t.x;
    mat[1][3] = t.y;
    mat[2][3] = t.z;
}
template void Matrix44<f3vec>::LoadFrame(const f3vec& x, const f3vec& y, const f3vec& z, const f3vec& t);
template void Matrix44<d3vec>::LoadFrame(const d3vec& x, const d3vec& y, const d3vec& z, const d3vec& t);

template <class Vec_T> void Matrix44<Vec_T>::GetFrame(Vec_T& c0, Vec_T& c1, Vec_T& c2)
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
template void Matrix44<f3vec>::GetFrame(f3vec& c0, f3vec& c1, f3vec& c2);
template void Matrix44<d3vec>::GetFrame(d3vec& c0, d3vec& c1, d3vec& c2);

template <class Vec_T> void Matrix44<Vec_T>::GetFrame(Vec_T& c0, Vec_T& c1, Vec_T& c2, Vec_T& c3)
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
template void Matrix44<f3vec>::GetFrame(f3vec& c0, f3vec& c1, f3vec& c2, f3vec& c3);
template void Matrix44<d3vec>::GetFrame(d3vec& c0, d3vec& c1, d3vec& c2, d3vec& c3);

template <class Vec_T> void Matrix44<Vec_T>::ChangeBasis(const Matrix44<Vec_T>& T)
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
template void Matrix44<f3vec>::ChangeBasis(const Matrix44<f3vec>& T);
template void Matrix44<d3vec>::ChangeBasis(const Matrix44<d3vec>& T);

template <class Vec_T> void Matrix44<Vec_T>::Scale(const Vec_T& v)
{
    typename Vec_T::ElType m[4][4];
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
template void Matrix44<f3vec>::Scale(const f3vec& v);
template void Matrix44<d3vec>::Scale(const d3vec& v);

template <class Vec_T> void Matrix44<Vec_T>::Rotate(typename Vec_T::ElType angle, const Vec_T& axis)
{
    if (angle == 0) return;

    typename Vec_T::ElType m[4][4];
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
template void Matrix44<f3vec>::Rotate(float angle, const f3vec& axis);
template void Matrix44<d3vec>::Rotate(double angle, const d3vec& axis);

template <class Vec_T> void Matrix44<Vec_T>::Translate(const Vec_T& v)
{
    typename Vec_T::ElType m[4][4];
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
template void Matrix44<f3vec>::Translate(const f3vec& v);
template void Matrix44<d3vec>::Translate(const d3vec& v);

template <class Vec_T> bool Matrix44<Vec_T>::Invert()
{
    if (inverse_valid) {
        if (is_identity) return true;

        // Just swap it with its inverse.
        typename Vec_T::ElType temp[4][4];
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
template bool Matrix44<f3vec>::Invert();
template bool Matrix44<d3vec>::Invert();

template <class Vec_T> void Matrix44<Vec_T>::Transpose()
{
    if (is_identity) return;

    build_transpose(mat);

    // Inverse of the transpose is the transpose of the inverse
    if (inverse_valid) build_transpose(imat);
}
template void Matrix44<f3vec>::Transpose();
template void Matrix44<d3vec>::Transpose();

template <class Vec_T>
void Matrix44<Vec_T>::Frustum(typename Vec_T::ElType l, typename Vec_T::ElType r, typename Vec_T::ElType b, typename Vec_T::ElType t, typename Vec_T::ElType n,
                              typename Vec_T::ElType f)
{
    ASSERT_R(n > 0 && f > 0);
    typename Vec_T::ElType m[4][4];

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
template void Matrix44<f3vec>::Frustum(float l, float r, float b, float t, float n, float f);
template void Matrix44<d3vec>::Frustum(double l, double r, double b, double t, double n, double f);

// N and f are negative if view plane is behind eye.
template <class Vec_T>
void Matrix44<Vec_T>::Ortho(typename Vec_T::ElType l, typename Vec_T::ElType r, typename Vec_T::ElType b, typename Vec_T::ElType t, typename Vec_T::ElType n,
                            typename Vec_T::ElType f)
{
    typename Vec_T::ElType m[4][4];

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
template void Matrix44<f3vec>::Ortho(float l, float r, float b, float t, float n, float f);
template void Matrix44<d3vec>::Ortho(double l, double r, double b, double t, double n, double f);

template <class Vec_T>
void Matrix44<Vec_T>::Perspective(typename Vec_T::ElType fovy, typename Vec_T::ElType aspect, typename Vec_T::ElType znear, typename Vec_T::ElType zfar)
{
    ASSERT_D(znear > 0 && zfar > 0);
    typename Vec_T::ElType top = znear * tan(fovy * (typename Vec_T::ElType)0.5);
    typename Vec_T::ElType bottom = -top;
    typename Vec_T::ElType left = bottom * aspect;
    typename Vec_T::ElType right = top * aspect;
    Frustum(left, right, bottom, top, znear, zfar);
}
template void Matrix44<f3vec>::Perspective(float fovy, float aspect, float znear, float zfar);
template void Matrix44<d3vec>::Perspective(double fovy, double aspect, double znear, double zfar);

template <class Vec_T> void Matrix44<Vec_T>::LookAt(const Vec_T& eye, const Vec_T& lookat, const Vec_T& up)
{
    Vec_T f(lookat - eye);
    f.normalize();

    Vec_T upn(up);
    upn.normalize();

    Vec_T s(cross(f, upn));
    s.normalize();

    Vec_T u(cross(s, f));
    // u.normalize(); // This normalize shouldn't be necessary.

    typename Vec_T::ElType m[4][4];

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
template void Matrix44<f3vec>::LookAt(const f3vec& eye, const f3vec& lookat, const f3vec& up);
template void Matrix44<d3vec>::LookAt(const d3vec& eye, const d3vec& lookat, const d3vec& up);

template <class Vec_T> bool Matrix44<Vec_T>::CheckNaN() const
{
    typename Vec_T::ElType* m = (typename Vec_T::ElType*)mat;
    for (int i = 0; i < 16; i++) ASSERT_RM(!std::isnan(m[i]), "Matrix has a NaN");

    return true;
}
template bool Matrix44<f3vec>::CheckNaN() const;
template bool Matrix44<d3vec>::CheckNaN() const;
