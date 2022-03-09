//////////////////////////////////////////////////////////////////////
// Matrix44.h - 4 x 4 transformation matrices
//
// Copyright David K. McAllister, Aug. 1998.

#pragma once

#include "Math/MiscMath.h"
#include "Math/Vector.h"
#include "Util/Assert.h"

template <class Vec_T> class Matrix44 {
public:
    typedef typename Vec_T::ElType ElType; // The type of an element of the vector
    static const int Chan = Vec_T::Chan;   // The number of channels (dimensions) in the vector

    Matrix44()
    {
        singularity_thresh = 0;
        LoadIdentity();
    }

    // Create a matrix with the given incoming values.
    // IsTranspose is true if the incoming values are the
    // transpose of the matrix we want to create. From OpenGL, etc.
    Matrix44(ElType* in, bool IsTranspose = false)
    {
        singularity_thresh = 0;
        if (IsTranspose)
            SetTranspose(in);
        else
            Set(in);
    }

    Matrix44(ElType in[4][4], bool IsTranspose = false)
    {
        singularity_thresh = 0;
        if (IsTranspose)
            SetTranspose(reinterpret_cast<ElType*>(in));
        else
            Set(reinterpret_cast<ElType*>(in));
    }

    Matrix44(const Matrix44<Vec_T>& copy)
    {
        copy_mat(mat, copy.mat);
        copy_mat(imat, copy.imat);
        singularity_thresh = copy.singularity_thresh;
        inverse_valid = copy.inverse_valid;
        is_identity = copy.is_identity;
    }

    ~Matrix44() {}

    // Copies the given matrix onto me.
    Matrix44<Vec_T>& operator=(const Matrix44<Vec_T>& copy)
    {
        copy_mat(mat, copy.mat);
        copy_mat(imat, copy.imat);
        singularity_thresh = copy.singularity_thresh;
        inverse_valid = copy.inverse_valid;
        is_identity = copy.is_identity;
        return *this;
    }

    // Returns this * right.
    Matrix44<Vec_T> operator*(const Matrix44<Vec_T>& right) const
    {
        Matrix44<Vec_T> m(*this);
        m.PostTrans(right);
        return m;
    }

    // Returns the inverse of this matrix.
    Matrix44<Vec_T> Inverse()
    {
        if (!inverse_valid) compute_inverse();

        Matrix44<Vec_T> m(*this);
        m.Invert();
        return m;
    }

    // PostTrans: this = this * incoming
    Matrix44<Vec_T>& operator*=(const Matrix44<Vec_T>& right)
    {
        PostTrans(right);
        return *this;
    }

    // Project a point, including the homogeneous divide. Uses p.w = 1.
    Vec_T Project(const Vec_T& p) const
    {
        typename Vec_T::ElType xw = mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2] * p.z + mat[0][3];
        typename Vec_T::ElType yw = mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2] * p.z + mat[1][3];
        typename Vec_T::ElType zw = mat[2][0] * p.x + mat[2][1] * p.y + mat[2][2] * p.z + mat[2][3];

        typename Vec_T::ElType rcpw = (typename Vec_T::ElType)1 / (mat[3][0] * p.x + mat[3][1] * p.y + mat[3][2] * p.z + mat[3][3]);
        return Vec_T(xw * rcpw, yw * rcpw, zw * rcpw);
    }

    // Project a homogeneous point, no divide. Replaces the incoming values.
    void Project(ElType& x, ElType& y, ElType& z, ElType& w) const
    {
        typename Vec_T::ElType x1 = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z + mat[0][3] * w;
        typename Vec_T::ElType y1 = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z + mat[1][3] * w;
        typename Vec_T::ElType z1 = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z + mat[2][3] * w;
        typename Vec_T::ElType w1 = mat[3][0] * x + mat[3][1] * y + mat[3][2] * z + mat[3][3] * w;
        x = x1;
        y = y1;
        z = z1;
        w = w1;
    }

    // Project a homogeneous point, does divide. Returns the vector.
    Vec_T Project(const Vec_T& p, const ElType w) const
    {
        typename Vec_T::ElType xw = mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2] * p.z + mat[0][3] * w;
        typename Vec_T::ElType yw = mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2] * p.z + mat[1][3] * w;
        typename Vec_T::ElType zw = mat[2][0] * p.x + mat[2][1] * p.y + mat[2][2] * p.z + mat[2][3] * w;

        typename Vec_T::ElType rcpw = (typename Vec_T::ElType)1 / (mat[3][0] * p.x + mat[3][1] * p.y + mat[3][2] * p.z + mat[3][3] * w);
        return Vec_T(xw * rcpw, yw * rcpw, zw * rcpw);
    }

    // Project using the upper 3x3. Doesn't translate.
    Vec_T ProjectDirection(const Vec_T& p) const
    {
        return Vec_T(mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2] * p.z, mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2] * p.z,
                     mat[2][0] * p.x + mat[2][1] * p.y + mat[2][2] * p.z);
    }

    // Transform by the inverse. Uses p.w = 1.
    Vec_T UnProject(const Vec_T& p)
    {
        if (!inverse_valid) compute_inverse();

        typename Vec_T::ElType xw = imat[0][0] * p.x + imat[0][1] * p.y + imat[0][2] * p.z + imat[0][3];
        typename Vec_T::ElType yw = imat[1][0] * p.x + imat[1][1] * p.y + imat[1][2] * p.z + imat[1][3];
        typename Vec_T::ElType zw = imat[2][0] * p.x + imat[2][1] * p.y + imat[2][2] * p.z + imat[2][3];

        typename Vec_T::ElType rcpw = (typename Vec_T::ElType)1 / (imat[3][0] * p.x + imat[3][1] * p.y + imat[3][2] * p.z + imat[3][3]);
        return Vec_T(xw * rcpw, yw * rcpw, zw * rcpw);
    }

    // Returns this * right. Includes the homogeneous divide. Uses p.w = 1.
    Vec_T operator*(const Vec_T& right) const { return Project(right); }

    void LoadIdentity()
    {
        build_identity(mat);
        build_identity(imat);
        inverse_valid = true;
        is_identity = true;
    }

    // Replace the current matrix with the new frame.
    // Takes column vectors. Bottom row is 0,0,0,1.
    void LoadFrame(const Vec_T&, const Vec_T&, const Vec_T&);
    void LoadFrame(const Vec_T&, const Vec_T&, const Vec_T&, const Vec_T&);

    // Returns column vectors. You can't get the bottom row this way.
    void GetFrame(Vec_T&, Vec_T&, Vec_T&);
    void GetFrame(Vec_T&, Vec_T&, Vec_T&, Vec_T&);

    // All the following functions modify the current matrix.
    void ChangeBasis(const Matrix44<Vec_T>&);

    void PostTrans(const Matrix44<Vec_T>&); // this = this * incoming.
    void PreTrans(const Matrix44<Vec_T>&);  // this = incoming * this.
    void Transform(const Matrix44<Vec_T>& right) { PostTrans(right); }

    // this = this * Scale, etc.
    void Scale(const Vec_T&);
    // Scales x, y, and z uniformly.
    void Scale(const ElType s) { Scale(Vec_T(s, s, s)); }
    void Rotate(const ElType angRad, const Vec_T& axis); // Axis does NOT need to be normalized. Angle is in RADIANS, unlike OpenGL.
    void Translate(const Vec_T&);

    void Frustum(const ElType left, const ElType right, const ElType bottom, const ElType top, const ElType znear, const ElType zfar);
    void Ortho(const ElType left, const ElType right, const ElType bottom, const ElType top, const ElType znear, const ElType zfar);
    void Perspective(const ElType fovyRad, const ElType aspect, const ElType znear, const ElType zfar);
    void LookAt(const Vec_T& eye, const Vec_T& lookat, const Vec_T& up);

    // Replace this matrix with its inverse.
    // Returns true if invertible, false if not.
    bool Invert();

    // Replace this matrix with its transpose.
    void Transpose();

    // Return the determinant of this 4x4 matrix.
    ElType Determinant();

    // For all the following, set inv to true to get/set the inverse.
    // Since it may need to compute the inverse, these are not const.

    template <class OutFl_T> void Get(OutFl_T* out, bool inv = false)
    {
        OutFl_T* p = out;
        ElType* m = reinterpret_cast<ElType*>(mat);
        if (inv) {
            if (!inverse_valid) compute_inverse();
            m = reinterpret_cast<ElType*>(imat);
        }
        for (int i = 0; i < 16; i++) p[i] = static_cast<OutFl_T>(m[i]);
    }

    // OpenGL stores its matrices column-major, so it needs the transpose.
    template <class OutFl_T> void GetTranspose(OutFl_T* out, bool inv = false) const
    {
        if (inv) {
            if (!inverse_valid) compute_inverse();
            OutFl_T* p = out;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++) *p++ = static_cast<OutFl_T>(imat[j][i]);
        } else {
            OutFl_T* p = out;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++) *p++ = static_cast<OutFl_T>(mat[j][i]);
        }
    }

    template <class InFl_T> void Set(InFl_T* in)
    {
        InFl_T* p = in;
        ElType* m = reinterpret_cast<ElType*>(mat);
        for (int i = 0; i < 16; i++) m[i] = static_cast<ElType>(*p++);
        inverse_valid = false;
        is_identity = false;
    }

    // OpenGL stores its matrices column-major, so it needs the transpose.
    template <class InFl_T> void SetTranspose(InFl_T* in)
    {
        InFl_T* p = in;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) mat[j][i] = static_cast<ElType>(*p++);
        inverse_valid = false;
        is_identity = false;
    }

    std::string string() const;     // Return a string for this matrix
    std::string string_inv() const; // Return a string for this matrix inverse
    bool CheckNaN() const;          // Make sure there are no NaNs.

    // Touch a single element of the matrix.
    ElType& operator()(const int r, const int c) { return mat[r][c]; }

    // Set this to false when you touch a single element of the matrix.
    bool& InverseValid() { return inverse_valid; }

    // Set this to false when you touch a single element of the matrix.
    bool& IsIdentity() { return is_identity; }

    // Set the singularity threshold.
    // When inverting a matrix, there must ba an element of each
    // column with a value at least this large.
    void SetSingularityThreshold(const ElType sing) { singularity_thresh = sing; }

private:
    ElType mat[4][4];
    ElType imat[4][4];
    ElType singularity_thresh; // Tells when to fail on computing inverse.
    bool inverse_valid, is_identity;

    // Each of these replaces m with the new matrix.
    void build_identity(ElType[4][4]) const;
    void build_rotate(ElType m[4][4], ElType, const Vec_T&) const;
    void build_scale(ElType m[4][4], const Vec_T&) const;
    void build_translate(ElType m[4][4], const Vec_T&) const;
    bool build_inverse(ElType m[4][4]) const;
    void build_transpose(ElType m[4][4]) const;

    void pre_mulmat(ElType to[4][4], const ElType from[4][4]);
    void post_mulmat(ElType to[4][4], const ElType from[4][4]);

    ElType det1(int c3, int c2, int c1)
    {
        for (int i = 0; i < 4; i++) {
            if (i != c3 && i != c2 && i != c1) return mat[3][i];
        }

        ASSERT_R(0);
        return 0;
    }

    ElType det2(int c3, int c2)
    {
        ElType cof = 1, det = 0;

        for (int i = 0; i < 4; i++) {
            if (i != c3 && i != c2) {
                det += cof * mat[2][i] * det1(c3, c2, i);
                cof = -cof;
            }
        }

        return det;
    }

    ElType det3(int c)
    {
        ElType cof = 1, det = 0;

        for (int i = 0; i < 4; i++) {
            if (i != c) {
                det += cof * mat[1][i] * det2(c, i);
                cof = -cof;
            }
        }

        return det;
    }

    void copy_mat(ElType to[4][4], const ElType from[4][4]) const
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) to[i][j] = from[i][j];
        }
    }

    void compute_inverse()
    {
        copy_mat(imat, mat);
        inverse_valid = build_inverse(imat);
    }

    void switch_rows(ElType m[4][4], int r1, int r2) const
    {
        for (int i = 0; i < 4; i++) {
            ElType tmp = m[r1][i];
            m[r1][i] = m[r2][i];
            m[r2][i] = tmp;
        }
    }
    void sub_rows(ElType m[4][4], int r1, int r2, ElType mul) const
    {
        for (int i = 0; i < 4; i++) m[r1][i] -= m[r2][i] * mul;
    }
};

// These functions create and return a matrix, whereas the member functions apply the transformation to the current matrix.
template <class Vec_T> DMC_DECL Matrix44<Vec_T> MScale(const Vec_T& V)
{
    Matrix44<Vec_T> M;
    M.Scale(V);
    return M;
}
template <class Vec_T> DMC_DECL Matrix44<Vec_T> MScale(const typename Vec_T::ElType s)
{
    Matrix44<Vec_T> M;
    M.Scale(Vec_T(s, s, s));
    return M;
}
template <class Vec_T> DMC_DECL Matrix44<Vec_T> MRotate(const typename Vec_T::ElType angRad, const Vec_T& axis)
{
    Matrix44<Vec_T> M;
    M.Rotate(angRad, axis);
    return M;
}
template <class Vec_T> DMC_DECL Matrix44<Vec_T> MTranslate(const Vec_T& V)
{
    Matrix44<Vec_T> M;
    M.Translate(V);
    return M;
}
template <class Vec_T>
DMC_DECL Matrix44<Vec_T> MFrustum(typename Vec_T::ElType l, typename Vec_T::ElType r, typename Vec_T::ElType b, typename Vec_T::ElType t,
                                  typename Vec_T::ElType zn, typename Vec_T::ElType zf)
{
    Matrix44<Vec_T> M;
    M.Frustum(l, r, b, t, zn, zf);
    return M;
}
template <class Vec_T>
DMC_DECL Matrix44<Vec_T> MPerspective(typename Vec_T::ElType fovyRad, typename Vec_T::ElType aspect, typename Vec_T::ElType znear, typename Vec_T::ElType zfar)
{
    Matrix44<Vec_T> M;
    M.Perspective(fovyRad, aspect, znear, zfar);
    return M;
}
template <class Vec_T> DMC_DECL Matrix44<Vec_T> MLookAt(const Vec_T& eye, const Vec_T& lookat, const Vec_T& up)
{
    Matrix44<Vec_T> M;
    M.LookAt(eye, lookat, up);
    return M;
}

template <class Vec_T> DMC_DECL std::ostream& operator<<(std::ostream& os, const Matrix44<Vec_T>& m)
{
    os << m.string();
    return os;
}

template <class Vec_T> DMC_DECL std::istream& operator>>(std::istream& is, Matrix44<Vec_T>& m)
{
    char st;
    typename Vec_T::ElType mat[4][4];

    // is.eatwhite();
    if (is.peek() == '[') {
        for (int i = 0; i < 4; i++) {
            is >> st;
            for (int j = 0; j < 4; j++) is >> mat[i][j] >> st;
        }
    } else {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) is >> mat[i][j];
        }
    }

    m.Set((typename Vec_T::ElType*)mat);

    return is;
}
