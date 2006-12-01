//////////////////////////////////////////////////////////////////////
// Matrix44.h - 4 x 4 transformation matrices.
//
// Copyright David K. McAllister, Aug. 1998.

#ifndef _Matrix44_h
#define _Matrix44_h

#include <Math/Vector.h>

class Matrix44
{
    double mat[4][4];
    double imat[4][4];
    double singularity_thresh; // Tells when to fail on computing inverse.
    bool inverse_valid, is_identity;
    
    // Each of these replaces m with the new matrix.
    void build_identity(double[4][4]) const;
    void build_rotate(double m[4][4], double, const Vector&) const;
    void build_scale(double m[4][4], const Vector&) const;
    void build_translate(double m[4][4], const Vector&) const;
    bool build_inverse(double m[4][4]) const;
    void build_transpose(double m[4][4]) const;
    
    void pre_mulmat(double to[4][4], const double from[4][4]);
    void post_mulmat(double to[4][4], const double from[4][4]);
    
    inline double det1(int c3, int c2, int c1)
    {
        for(int i=0; i<4; i++) {
            if(i != c3 && i != c2 && i != c1)
                return mat[3][i];
        }
        
        ASSERT_R(0);
        return 0;
    }
    
    inline double det2(int c3, int c2)
    {
        double cof = 1, det = 0;
        
        for(int i=0; i<4; i++) {
            if(i != c3 && i != c2) {
                det += cof * mat[2][i] * det1(c3, c2, i);
                cof = -cof;
            }
        }
        
        return det;
    }
    
    inline double det3(int c)
    {
        double cof = 1, det = 0;
        
        for(int i=0; i<4; i++) {
            if(i != c)
            {
                det += cof * mat[1][i] * det2(c, i);
                cof = -cof;
            }
        }
        
        return det;
    }
    
    inline void copy_mat(double *to, const double *from) const
    {
        for(int i=0; i<16; i++)
            to[i] = from[i];
    }
    
    inline void compute_inverse()
    {
        copy_mat((double *)imat, (double *)mat);
        inverse_valid = build_inverse(imat);
    }
    
    inline void switch_rows(double m[4][4], int r1, int r2) const
    {
        for(int i=0;i<4;i++){
            double tmp=m[r1][i];
            m[r1][i]=m[r2][i];
            m[r2][i]=tmp;
        }
    }
    inline void sub_rows(double m[4][4], int r1, int r2, double mul) const
    {
        for(int i=0;i<4;i++)
            m[r1][i] -= m[r2][i]*mul;
    }
    
public:
    inline Matrix44()
    {
        singularity_thresh = 0;
        LoadIdentity();
    }
    
    // Create a matrix with the given incoming values.
    // IsTranspose is true if the incoming values are the
    // transpose of the matrix we want to create. From OpenGL, etc.
    inline Matrix44(double *in, bool IsTranspose = false)
    {
        singularity_thresh = 0;
        if(IsTranspose)
            SetTranspose(in);
        else
            Set(in);
    }
    inline Matrix44(const Matrix44& copy)
    {
        copy_mat((double *)mat, (double *)copy.mat);
        copy_mat((double *)imat, (double *)copy.imat);
        singularity_thresh = copy.singularity_thresh;
        inverse_valid = copy.inverse_valid;
        is_identity = copy.is_identity;
    }
    
    inline ~Matrix44() {}
    
    // Copies the given matrix onto me.
    inline Matrix44& operator=(const Matrix44& copy)
    {
        copy_mat((double *)mat, (double *)copy.mat);
        copy_mat((double *)imat, (double *)copy.imat);
        singularity_thresh = copy.singularity_thresh;
        inverse_valid = copy.inverse_valid;
        is_identity = copy.is_identity;
        return *this;
    }
    
    // Returns this * right.
    inline Matrix44 operator*(const Matrix44& right) const
    {
        Matrix44 m(*this);
        m.PostTrans(right);
        return m;
    }
    
    // Returns the inverse of this matrix.
    inline Matrix44 Inverse()
    {
        if(!inverse_valid)
            compute_inverse();
        
        Matrix44 m(*this);
        m.Invert();
        return m;
    }
    
    // PostTrans: this = this * incoming
    inline Matrix44 &operator*=(const Matrix44& right)
    {
        PostTrans(right);
        return *this;
    }
    
    // Project a point, including the homogeneous divide. Uses p.w = 1.
    Vector Project(const Vector& p) const;
    // Project a homogeneous point, no divide. Replaces the incoming values.
    void Project(double &x, double &y, double &z, double &w) const;
    // Project a homogeneous point, does divide. Returns the vector.
    Vector Project(const Vector &p, const double w) const;
    // Project using the upper 3x3. Doesn't translate.
    Vector ProjectDirection(const Vector& p) const;
    // Transform by the inverse. Uses p.w = 1.
    Vector UnProject(const Vector& p);
    
    // Returns this * right. Includes the homogeneous divide. Uses p.w = 1.
    inline Vector operator*(const Vector& right) const
    {
        return Project(right);
    }
    
    inline void LoadIdentity()
    {
        build_identity(mat);
        build_identity(imat);
        inverse_valid = true;
        is_identity = true;
    }
    
    // Replace the current matrix with the new frame.
    // Takes column vectors. Bottom row is 0,0,0,1.
    void LoadFrame(const Vector&, const Vector&, const Vector&);
    void LoadFrame(const Vector&, const Vector&, const Vector&, const Vector&);
    
    // Returns column vectors. You can't get the bottom row this way.
    void GetFrame(Vector&, Vector&, Vector&);
    void GetFrame(Vector&, Vector&, Vector&, Vector&);
    
    // All the following functions modify the current matrix.
    void ChangeBasis(const Matrix44&);
    
    void PostTrans(const Matrix44&); // this = this * incoming.
    void PreTrans(const Matrix44&); // this = incoming * this.
    inline void Transform(const Matrix44& right) {PostTrans(right);}
    
    // this = this * Scale, etc.
    void Scale(const Vector&);
    // Scales x, y, and z uniformly.
    inline void Scale(const double s) {Scale(Vector(s,s,s));}
    void Rotate(const double angRad, const Vector& axis);
    void Translate(const Vector&);
    
    void Frustum(const double left, const double right, const double bottom, const double top, const double znear, const double zfar);
    void Ortho(const double left, const double right, const double bottom, const double top, const double znear, const double zfar);
    void Perspective(const double fovyRad, const double aspect, const double znear, const double zfar);
    void LookAt(const Vector& eye, const Vector& lookat, const Vector& up);
    
    // Replace this matrix with its inverse.
    // Returns true if invertible, false if not.
    bool Invert();
    
    // Replace this matrix with its transpose.
    void Transpose();
    
    // Return the determinant of this 4x4 matrix.
    double Determinant();
    
    // For all the following, set inv to true to get/set the inverse.
    // Since it may need to compute the inverse, these are not const.
    inline void Get(double *out, bool inv=false)
    {
        double *p = out;
        double *m = (double *)mat;
        if(inv) {
            if(!inverse_valid) compute_inverse();
            m = (double *)imat;
        }
        for(int i=0; i<16; i++)
            *p++ = m[i];
    }
    
    // GL stores its matrices column-major.
    // Need to take the transpose.
    inline void GetTranspose(double *out) const
    {
        double* p = out;
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                *p++ = mat[j][i];
    }
    
    inline void Getf(float *out, bool inv=false)
    {
        float *p = out;
        double *m = (double *)mat;
        if(inv) {
            if(!inverse_valid) compute_inverse();
            m = (double *)imat;
        }
        for(int i=0; i<16; i++)
            *p++ = float(m[i]);
    }
    
    // GL stores its matrices column-major.
    // Need to take the transpose.
    void GetTransposef(float *out) const
    {
        float* p = out;
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                *p++ = float(mat[j][i]);
    }
    
    void Set(double *in, bool inv=false)
    {
        double *p = in;
        double *m = inv ? (double *)imat : (double *)mat;
        for(int i=0; i<16; i++)
            m[i] = *p++;
        inverse_valid = inv;
        is_identity = false;
    }
    // GL stores its matrices column-major.
    // Take the transpose while reading.
    void SetTranspose(double* in)
    {
        double* p = in;
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                mat[j][i] = *p++;
            inverse_valid = false;
            is_identity = false;
    }
    void Setf(float *in, bool inv=false)
    {
        float *p = in;
        double *m = inv ? (double *)imat : (double *)mat;
        for(int i=0; i<16; i++)
            m[i] = double(*p++);
        inverse_valid = inv;
        is_identity = false;
    }
    // GL stores its matrices column-major.
    // Take the transpose while reading.
    void SetTransposef(float* in)
    {
        float* p = in;
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                mat[j][i] = double(*p++);
            inverse_valid = false;
            is_identity = false;
    }
    
    std::string print() const;
    std::string printInv() const;
    bool CheckNaN() const; // Make sure there are no NaNs.
    
    // Touch a single element of the matrix.
    inline double &operator()(const int r, const int c)
    {
        return mat[r][c];
    }
    
    // Set this to false when you touch a single element of the matrix.
    inline bool & InverseValid()
    {
        return inverse_valid;
    }
    
    // Set this to false when you touch a single element of the matrix.
    inline bool & IsIdentity()
    {
        return is_identity;
    }
    
    // Set the singularity threshold.
    // When inverting a matrix, there must ba an element of each
    // column with a value at least this large.
    inline void SetSingularityThreshold(const double _sing)
    {
        singularity_thresh = _sing;
    }
};

inline Matrix44 MScale(const Vector &V) {Matrix44 M; M.Scale(V); return M;}
inline Matrix44 MScale(const double s) {Matrix44 M; M.Scale(Vector(s,s,s)); return M;}
inline Matrix44 MRotate(const double angRad, const Vector& axis) {Matrix44 M; M.Rotate(angRad, axis); return M;}
inline Matrix44 MTranslate(const Vector &V) {Matrix44 M; M.Translate(V); return M;}
inline Matrix44 MFrustum(double l, double r, double b, double t, double zn, double zf) {Matrix44 M; M.Frustum(l, r, b, t, zn, zf); return M;}
inline Matrix44 MPerspective(double fovyRad, double aspect, double znear, double zfar) {Matrix44 M; M.Perspective(fovyRad, aspect, znear, zfar); return M;}
inline Matrix44 MLookAt(const Vector& eye, const Vector& lookat, const Vector& up) {Matrix44 M; M.LookAt(eye, lookat, up); return M;}

inline std::ostream& operator<<(std::ostream& os, const Matrix44& m)
{
    os << m.print();
    return os;
}

inline std::istream& operator>>(std::istream& is, Matrix44& m)
{
    char st;
    double mat[4][4];
    
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
    
    m.Set((double *)mat);
    
    return is;
}

#endif
