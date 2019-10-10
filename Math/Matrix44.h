//////////////////////////////////////////////////////////////////////
// Matrix44.h - 4 x 4 transformation matrices
//
// Copyright David K. McAllister, Aug. 1998.

#pragma once

#include "Math/Vector.h"

template <class Fl_T>
class Matrix44
{
public:
    Matrix44()
    {
        singularity_thresh = 0;
        LoadIdentity();
    }

    // Create a matrix with the given incoming values.
    // IsTranspose is true if the incoming values are the
    // transpose of the matrix we want to create. From OpenGL, etc.
    Matrix44(Fl_T *in, bool IsTranspose = false)
    {
        singularity_thresh = 0;
        if(IsTranspose)
            SetTranspose(in);
        else
            Set(in);
    }

    Matrix44(Fl_T in[4][4], bool IsTranspose = false)
    {
        singularity_thresh = 0;
        if(IsTranspose)
            SetTranspose(reinterpret_cast<Fl_T *>(in));
        else
            Set(reinterpret_cast<Fl_T *>(in));
    }

    Matrix44(const Matrix44<Fl_T>& copy)
    {
        copy_mat(mat, copy.mat);
        copy_mat(imat, copy.imat);
        singularity_thresh = copy.singularity_thresh;
        inverse_valid = copy.inverse_valid;
        is_identity = copy.is_identity;
    }

    ~Matrix44() {}

    // Copies the given matrix onto me.
    Matrix44<Fl_T>& operator=(const Matrix44<Fl_T>& copy)
    {
        copy_mat(mat, copy.mat);
        copy_mat(imat, copy.imat);
        singularity_thresh = copy.singularity_thresh;
        inverse_valid = copy.inverse_valid;
        is_identity = copy.is_identity;
        return *this;
    }

    // Returns this * right.
    Matrix44<Fl_T> operator*(const Matrix44<Fl_T>& right) const
    {
        Matrix44<Fl_T> m(*this);
        m.PostTrans(right);
        return m;
    }

    // Returns the inverse of this matrix.
    Matrix44<Fl_T> Inverse()
    {
        if(!inverse_valid)
            compute_inverse();

        Matrix44<Fl_T> m(*this);
        m.Invert();
        return m;
    }

    // PostTrans: this = this * incoming
    Matrix44<Fl_T> &operator*=(const Matrix44<Fl_T>& right)
    {
        PostTrans(right);
        return *this;
    }

    // Project a point, including the homogeneous divide. Uses p.w = 1.
    t3Vector<Fl_T> Project(const t3Vector<Fl_T>& p) const;
    // Project a homogeneous point, no divide. Replaces the incoming values.
    void Project(Fl_T &x, Fl_T &y, Fl_T &z, Fl_T &w) const;
    // Project a homogeneous point, does divide. Returns the vector.
    t3Vector<Fl_T> Project(const t3Vector<Fl_T> &p, const Fl_T w) const;
    // Project using the upper 3x3. Doesn't translate.
    t3Vector<Fl_T> ProjectDirection(const t3Vector<Fl_T>& p) const;
    // Transform by the inverse. Uses p.w = 1.
    t3Vector<Fl_T> UnProject(const t3Vector<Fl_T>& p);

    // Returns this * right. Includes the homogeneous divide. Uses p.w = 1.
    t3Vector<Fl_T> operator*(const t3Vector<Fl_T>& right) const
    {
        return Project(right);
    }

    void LoadIdentity()
    {
        build_identity(mat);
        build_identity(imat);
        inverse_valid = true;
        is_identity = true;
    }

    // Replace the current matrix with the new frame.
    // Takes column vectors. Bottom row is 0,0,0,1.
    void LoadFrame(const t3Vector<Fl_T>&, const t3Vector<Fl_T>&, const t3Vector<Fl_T>&);
    void LoadFrame(const t3Vector<Fl_T>&, const t3Vector<Fl_T>&, const t3Vector<Fl_T>&, const t3Vector<Fl_T>&);

    // Returns column vectors. You can't get the bottom row this way.
    void GetFrame(t3Vector<Fl_T>&, t3Vector<Fl_T>&, t3Vector<Fl_T>&);
    void GetFrame(t3Vector<Fl_T>&, t3Vector<Fl_T>&, t3Vector<Fl_T>&, t3Vector<Fl_T>&);

    // All the following functions modify the current matrix.
    void ChangeBasis(const Matrix44<Fl_T>&);

    void PostTrans(const Matrix44<Fl_T>&); // this = this * incoming.
    void PreTrans(const Matrix44<Fl_T>&); // this = incoming * this.
    void Transform(const Matrix44<Fl_T>& right) {PostTrans(right);}

    // this = this * Scale, etc.
    void Scale(const t3Vector<Fl_T>&);
    // Scales x, y, and z uniformly.
    void Scale(const Fl_T s) {Scale(t3Vector<Fl_T>(s,s,s));}
    void Rotate(const Fl_T angRad, const t3Vector<Fl_T>& axis); // axis does NOT need to be normalized. Angle is in RADIANS, unlike OpenGL.
    void Translate(const t3Vector<Fl_T>&);

    void Frustum(const Fl_T left, const Fl_T right, const Fl_T bottom, const Fl_T top, const Fl_T znear, const Fl_T zfar);
    void Ortho(const Fl_T left, const Fl_T right, const Fl_T bottom, const Fl_T top, const Fl_T znear, const Fl_T zfar);
    void Perspective(const Fl_T fovyRad, const Fl_T aspect, const Fl_T znear, const Fl_T zfar);
    void LookAt(const t3Vector<Fl_T>& eye, const t3Vector<Fl_T>& lookat, const t3Vector<Fl_T>& up);

    // Replace this matrix with its inverse.
    // Returns true if invertible, false if not.
    bool Invert();

    // Replace this matrix with its transpose.
    void Transpose();

    // Return the determinant of this 4x4 matrix.
    Fl_T Determinant();

    // For all the following, set inv to true to get/set the inverse.
    // Since it may need to compute the inverse, these are not const.

    template<class OutFl_T>
    void Get(OutFl_T *out, bool inv=false)
    {
        OutFl_T *p = out;
        Fl_T *m = reinterpret_cast<Fl_T *>(mat);
        if(inv) {
            if(!inverse_valid) compute_inverse();
            m = reinterpret_cast<Fl_T *>(imat);
        }
        for(int i=0; i<16; i++)
            p[i] = static_cast<OutFl_T>(m[i]);
    }

    // OpenGL stores its matrices column-major, so it needs the transpose.
    template<class OutFl_T>
    void GetTranspose(OutFl_T *out, bool inv=false) const
    {
        if(inv) {
            if(!inverse_valid) compute_inverse();
            OutFl_T* p = out;
            for(int i=0; i<4; i++)
                for(int j=0; j<4; j++)
                    *p++ = static_cast<OutFl_T>(imat[j][i]);
        } else {
            OutFl_T* p = out;
            for(int i=0; i<4; i++)
                for(int j=0; j<4; j++)
                    *p++ = static_cast<OutFl_T>(mat[j][i]);
        }
    }

    template<class InFl_T>
    void Set(InFl_T *in)
    {
        InFl_T *p = in;
        Fl_T *m = reinterpret_cast<Fl_T *>(mat);
        for(int i=0; i<16; i++)
            m[i] = static_cast<Fl_T>(*p++);
        inverse_valid = false;
        is_identity = false;
    }

    // OpenGL stores its matrices column-major, so it needs the transpose.
    template<class InFl_T>
    void SetTranspose(InFl_T* in)
    {
        InFl_T *p = in;
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                mat[j][i] = static_cast<Fl_T>(*p++);
        inverse_valid = false;
        is_identity = false;
    }

    std::string string() const; // Return a string for this matrix
    std::string string_inv() const; // Return a string for this matrix inverse
    bool CheckNaN() const; // Make sure there are no NaNs.

    // Touch a single element of the matrix.
    Fl_T &operator()(const int r, const int c)
    {
        return mat[r][c];
    }

    // Set this to false when you touch a single element of the matrix.
    bool & InverseValid()
    {
        return inverse_valid;
    }

    // Set this to false when you touch a single element of the matrix.
    bool & IsIdentity()
    {
        return is_identity;
    }

    // Set the singularity threshold.
    // When inverting a matrix, there must ba an element of each
    // column with a value at least this large.
    void SetSingularityThreshold(const Fl_T sing)
    {
        singularity_thresh = sing;
    }

private:
	Fl_T mat[4][4];
	Fl_T imat[4][4];
	Fl_T singularity_thresh; // Tells when to fail on computing inverse.
	bool inverse_valid, is_identity;

	// Each of these replaces m with the new matrix.
	void build_identity(Fl_T[4][4]) const;
	void build_rotate(Fl_T m[4][4], Fl_T, const t3Vector<Fl_T>&) const;
	void build_scale(Fl_T m[4][4], const t3Vector<Fl_T>&) const;
	void build_translate(Fl_T m[4][4], const t3Vector<Fl_T>&) const;
	bool build_inverse(Fl_T m[4][4]) const;
	void build_transpose(Fl_T m[4][4]) const;

	void pre_mulmat(Fl_T to[4][4], const Fl_T from[4][4]);
	void post_mulmat(Fl_T to[4][4], const Fl_T from[4][4]);

	Fl_T det1(int c3, int c2, int c1)
	{
		for (int i = 0; i<4; i++) {
			if (i != c3 && i != c2 && i != c1)
				return mat[3][i];
		}

		ASSERT_R(0);
		return 0;
	}

	Fl_T det2(int c3, int c2)
	{
		Fl_T cof = 1, det = 0;

		for (int i = 0; i<4; i++) {
			if (i != c3 && i != c2) {
				det += cof * mat[2][i] * det1(c3, c2, i);
				cof = -cof;
			}
		}

		return det;
	}

	Fl_T det3(int c)
	{
		Fl_T cof = 1, det = 0;

		for (int i = 0; i<4; i++) {
			if (i != c) {
				det += cof * mat[1][i] * det2(c, i);
				cof = -cof;
			}
		}

		return det;
	}

	void copy_mat(Fl_T to[4][4], const Fl_T from[4][4]) const
	{
		for (int i = 0; i<4; i++) {
			for (int j = 0; j<4; j++)
				to[i][j] = from[i][j];
		}
	}

	void compute_inverse()
	{
		copy_mat(imat, mat);
		inverse_valid = build_inverse(imat);
	}

	void switch_rows(Fl_T m[4][4], int r1, int r2) const
	{
		for (int i = 0; i<4; i++) {
			Fl_T tmp = m[r1][i];
			m[r1][i] = m[r2][i];
			m[r2][i] = tmp;
		}
	}
	void sub_rows(Fl_T m[4][4], int r1, int r2, Fl_T mul) const
	{
		for (int i = 0; i<4; i++)
			m[r1][i] -= m[r2][i] * mul;
	}
};

// These functions create and return a matrix,
// whereas the member functions apply the transformation to the current matrix.
template <class Fl_T> DMC_DECL Matrix44<Fl_T> MScale(const t3Vector<Fl_T> &V) {Matrix44<Fl_T> M; M.Scale(V); return M;}
template <class Fl_T> DMC_DECL Matrix44<Fl_T> MScale(const Fl_T s) {Matrix44<Fl_T> M; M.Scale(t3Vector<Fl_T>(s,s,s)); return M;}
template <class Fl_T> DMC_DECL Matrix44<Fl_T> MRotate(const Fl_T angRad, const t3Vector<Fl_T>& axis) {Matrix44<Fl_T> M; M.Rotate(angRad, axis); return M;}
template <class Fl_T> DMC_DECL Matrix44<Fl_T> MTranslate(const t3Vector<Fl_T> &V) {Matrix44<Fl_T> M; M.Translate(V); return M;}
template <class Fl_T> DMC_DECL Matrix44<Fl_T> MFrustum(Fl_T l, Fl_T r, Fl_T b, Fl_T t, Fl_T zn, Fl_T zf) {Matrix44<Fl_T> M; M.Frustum(l, r, b, t, zn, zf); return M;}
template <class Fl_T> DMC_DECL Matrix44<Fl_T> MPerspective(Fl_T fovyRad, Fl_T aspect, Fl_T znear, Fl_T zfar) {Matrix44<Fl_T> M; M.Perspective(fovyRad, aspect, znear, zfar); return M;}
template <class Fl_T> DMC_DECL Matrix44<Fl_T> MLookAt(const t3Vector<Fl_T>& eye, const t3Vector<Fl_T>& lookat, const t3Vector<Fl_T>& up) {Matrix44<Fl_T> M; M.LookAt(eye, lookat, up); return M;}

template <class Fl_T>
DMC_DECL std::ostream& operator<<(std::ostream& os, const Matrix44<Fl_T>& m)
{
    os << m.string();
    return os;
}

template <class Fl_T>
DMC_DECL std::istream& operator>>(std::istream& is, Matrix44<Fl_T>& m)
{
    char st;
    Fl_T mat[4][4];

    // is.eatwhite();
    if(is.peek() == '[') {
        for(int i=0; i<4; i++) {
            is >> st;
            for(int j=0; j<4; j++)
                is >> mat[i][j] >> st;
        }
    } else {
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++)
                is >> mat[i][j];
        }
    }

    m.Set((Fl_T *)mat);

    return is;
}
