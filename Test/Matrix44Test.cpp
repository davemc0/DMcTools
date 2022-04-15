// Test the DMcTools Matrix44 class by comparing it to the OpenGL matrix implementation

#include "Math/Matrix44.h"

#if 0
#include "GL/glut.h"

#include <iostream>

namespace {

    template<class T>
    void getGLMatrix(T* Mat) {}

    template <> void getGLMatrix(double* Mat) { glGetDoublev(GL_MODELVIEW_MATRIX, Mat); }
    template <> void getGLMatrix(float* Mat) { glGetFloatv(GL_MODELVIEW_MATRIX, Mat); }

    template<class T>
    void PrMat()
    {
        T Mat[16];
        getGLMatrix(Mat);
        Matrix44<T> PrtMat(Mat, true);
        std::cerr << PrtMat.string() << '\n';
    }
};

// TODO: Use gtest for this. Enable automatic checking.
template<class T>
bool Matrix44Testing(int argc, char **argv)
{
    std::cerr << "Matrix44Test<" << typeid(T).name() << ">: Testing matrix math. Shows OpenGL matrix, followed by Matrix44.\n";

    // Identity
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    PrMat<T>();

    Matrix44<T> mymat;
    std::cerr << mymat.string() << std::endl;

    // Translate
    glTranslated(10, 20, -20);
    PrMat<T>();

    mymat.Translate(t3Vector<T>(10, 20, -20));
    std::cerr << mymat.string() << std::endl;

    // Axis Rotate
    glLoadIdentity();
    glRotated(11.1, 0, 0, 1);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.Rotate(dmcm::DtoR(11.1), t3Vector<T>(0, 0, 1));
    std::cerr << mymat.string() << std::endl;

    // Rotate
    glLoadIdentity();
    glRotated(11.1, 31, 21, 19);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.Rotate(dmcm::DtoR(11.1), t3Vector<T>(31, 21, 19));
    std::cerr << mymat.string() << std::endl;

    // LookAt
    glLoadIdentity();
    gluLookAt(4, 1, -2, 2, 0, 0, 0, 1, 0);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.LookAt(t3Vector<T>(4, 1, -2), t3Vector<T>(2, 0, 0), t3Vector<T>(0, 1, 0));
    std::cerr << mymat.string() << std::endl;

    // LookAt + Scale
    glLoadIdentity();
    glTranslated(10, 3, 5);
    gluLookAt(4, 1, -2, 2, 0, 0, 0, 1, 0);
    glScaled(6, 1, .5);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.Translate(t3Vector<T>(10, 3, 5));
    mymat.LookAt(t3Vector<T>(4, 1, -2), t3Vector<T>(2, 0, 0), t3Vector<T>(0, 1, 0));
    mymat.Scale(t3Vector<T>(6, 1, .5));
    std::cerr << mymat.string() << std::endl;

    // Perspective
    glLoadIdentity();
    glTranslated(10, 3, 5);
    gluPerspective(30.0, 4.0 / 3.0, 10, 1000);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.Translate(t3Vector<T>(10, 3, 5));
    mymat.Perspective(dmcm::DtoR(30.0), 4.0 / 3.0, 10, 1000);
    std::cerr << mymat.string() << std::endl;

    glLoadIdentity();
    glScaled(40.0, 5.0, 9.0);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.Scale(t3Vector<T>(40.0, 5.0, 9.0));
    std::cerr << mymat.string() << std::endl;

    glLoadIdentity();
    glScaled(40.0, 5.0, 9.0);
    glRotated(131.0, 0.7071, 0, 0.7071);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.Scale(t3Vector<T>(40.0, 5.0, 9.0));
    mymat.Rotate(dmcm::DtoR(131.), t3Vector<T>(0.7071, 0, 0.7071));
    std::cerr << mymat.string() << std::endl;

    glLoadIdentity();
    glRotated(131.0, 0.7071, 0, 0.7071);
    glScaled(40.0, 5.0, 9.0);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.Rotate(dmcm::DtoR(131.), t3Vector<T>(0.7071, 0, 0.7071));
    mymat.Scale(t3Vector<T>(40.0, 5.0, 9.0));
    std::cerr << mymat.string() << std::endl;

    glLoadIdentity();
    glRotated(131.0, 0.7071, 0, 0.7071);
    glTranslated(40.0, 5.0, 9.0);
    PrMat<T>();

    mymat.LoadIdentity();
    mymat.Rotate(dmcm::DtoR(131.), t3Vector<T>(0.7071, 0, 0.7071));
    mymat.Translate(t3Vector<T>(40.0, 5.0, 9.0));
    std::cerr << mymat.string() << std::endl;

    return true;
}
#endif

bool Matrix44Test(int argc, char** argv)
{
    std::cerr << "Starting Matrix44Test\n";

    /// glutInit(&argc, argv);
    ///
    /// //glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    /// //glutInitWindowSize(64, 64);
    /// glutCreateWindow("Matrix44Test");
    ///
    /// Matrix44Testing<double>(argc, argv);
    /// Matrix44Testing<float>(argc, argv);

    std::cerr << "Ending Matrix44Test\n";

    return true;
}
