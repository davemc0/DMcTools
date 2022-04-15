// Test the DMcTools Matrix44 class by comparing it to the OpenGL matrix implementation

#include "Math/Matrix44.h"

#include "GL/glew.h"

// This needs to come after GLEW
#include "GL/freeglut.h"

#include <iostream>

namespace {

template <class T> void getGLMatrix(T* Mat) {}

template <> void getGLMatrix(double* Mat) { glGetDoublev(GL_MODELVIEW_MATRIX, Mat); }
template <> void getGLMatrix(float* Mat) { glGetFloatv(GL_MODELVIEW_MATRIX, Mat); }

template <class Vec_T> void PrMat()
{
    typename Vec_T::ElType Mat[16];
    getGLMatrix(Mat);
    Matrix44<Vec_T> PrtMat(Mat, true);
    std::cerr << PrtMat.string() << '\n';
}
}; // namespace

// TODO: Use gtest for this. Enable automatic checking.
template <class Vec_T> bool Matrix44Testing(int argc, char** argv)
{
    std::cerr << "Matrix44Test<" << typeid(typename Vec_T::ElType).name() << ">: Testing matrix math. Shows OpenGL matrix, followed by Matrix44.\n";

    // Identity
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    PrMat<Vec_T>();

    Matrix44<Vec_T> mymat;
    std::cerr << mymat.string() << std::endl;

    // Translate
    glTranslated(10, 20, -20);
    PrMat<Vec_T>();

    mymat.Translate(Vec_T(10., 20., -20.));
    std::cerr << mymat.string() << std::endl;

    // Axis Rotate
    glLoadIdentity();
    glRotated(11.1, 0, 0, 1);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.Rotate(degToRad(11.1), Vec_T(0., 0., 1.));
    std::cerr << mymat.string() << std::endl;

    // Rotate
    glLoadIdentity();
    glRotated(11.1, 31, 21, 19);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.Rotate(degToRad(11.1), Vec_T(31., 21., 19.));
    std::cerr << mymat.string() << std::endl;

    // LookAt
    glLoadIdentity();
    gluLookAt(4, 1, -2, 2, 0, 0, 0, 1, 0);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.LookAt(Vec_T(4., 1., -2.), Vec_T(2., 0., 0.), Vec_T(0., 1., 0.));
    std::cerr << mymat.string() << std::endl;

    // LookAt + Scale
    glLoadIdentity();
    glTranslated(10, 3, 5);
    gluLookAt(4, 1, -2, 2, 0, 0, 0, 1, 0);
    glScaled(6, 1, .5);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.Translate(Vec_T(10., 3., 5.));
    mymat.LookAt(Vec_T(4., 1., -2.), Vec_T(2., 0., 0.), Vec_T(0., 1., 0.));
    mymat.Scale(Vec_T(6., 1., .5));
    std::cerr << mymat.string() << std::endl;

    // Perspective
    glLoadIdentity();
    glTranslated(10, 3, 5);
    gluPerspective(30.0, 4.0 / 3.0, 10, 1000);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.Translate(Vec_T(10., 3., 5.));
    mymat.Perspective(degToRad(30.0), 4.0 / 3.0, 10, 1000);
    std::cerr << mymat.string() << std::endl;

    glLoadIdentity();
    glScaled(40.0, 5.0, 9.0);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.Scale(Vec_T(40.0, 5.0, 9.0));
    std::cerr << mymat.string() << std::endl;

    glLoadIdentity();
    glScaled(40.0, 5.0, 9.0);
    glRotated(131.0, 0.7071, 0, 0.7071);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.Scale(Vec_T(40.0, 5.0, 9.0));
    mymat.Rotate(degToRad(131.), Vec_T(0.7071, 0, 0.7071));
    std::cerr << mymat.string() << std::endl;

    glLoadIdentity();
    glRotated(131.0, 0.7071, 0, 0.7071);
    glScaled(40.0, 5.0, 9.0);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.Rotate(degToRad(131.), Vec_T(0.7071, 0, 0.7071));
    mymat.Scale(Vec_T(40.0, 5.0, 9.0));
    std::cerr << mymat.string() << std::endl;

    glLoadIdentity();
    glRotated(131.0, 0.7071, 0, 0.7071);
    glTranslated(40.0, 5.0, 9.0);
    PrMat<Vec_T>();

    mymat.LoadIdentity();
    mymat.Rotate(degToRad(131.), Vec_T(0.7071, 0, 0.7071));
    mymat.Translate(Vec_T(40.0, 5.0, 9.0));
    std::cerr << mymat.string() << std::endl;

    return true;
}

bool Matrix44Test(int argc, char** argv)
{
    std::cerr << "Starting Matrix44Test\n";

    glutInit(&argc, argv);
    glutCreateWindow("Matrix44Test");

    Matrix44Testing<d3vec>(argc, argv);
    Matrix44Testing<f3vec>(argc, argv);

    std::cerr << "Ending Matrix44Test\n";

    return true;
}
