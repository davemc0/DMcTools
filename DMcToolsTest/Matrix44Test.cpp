// Test the DMcTools Matrix44 class by comparing it to the OpenGL matrix implementation

#include "Math/Matrix44.h"

#include "GL/glut.h"

#include <iostream>
using namespace std;

namespace {

    void PrMat()
    {
        double Mat[16];

        glGetDoublev(GL_MODELVIEW_MATRIX, Mat);

        cerr << Mat[0] << " " << Mat[4] << " " << Mat[8] << " " << Mat[12] << endl;
        cerr << Mat[1] << " " << Mat[5] << " " << Mat[9] << " " << Mat[13] << endl;
        cerr << Mat[2] << " " << Mat[6] << " " << Mat[10] << " " << Mat[14] << endl;
        cerr << Mat[3] << " " << Mat[7] << " " << Mat[11] << " " << Mat[15] << endl;
        cerr << endl;
    }

};

bool Matrix44Test(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("D");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    PrMat();

    Matrix44 mymat;

    cerr << mymat.string() << endl;

    glTranslatef(10, 20, -20);
    PrMat();

    mymat.Translate(Vector(10, 20, -20));
    cerr << mymat.string() << endl;

    glLoadIdentity();
    glRotated(11.1, 0,0,1);
    PrMat();

    mymat.LoadIdentity();
    mymat.Rotate(11.1, Vector(0,0,1));
    cerr << mymat.string() << endl;

    glLoadIdentity();
    glRotated(11.1, 31,21,19);
    PrMat();

    mymat.LoadIdentity();
    mymat.Rotate(11.1, Vector(31,21,19));
    cerr << mymat.string() << endl;

    glLoadIdentity();
    gluLookAt(4, 1, -2,  2, 0, 0,  0, 1, 0);
    PrMat();

    mymat.LoadIdentity();
    mymat.LookAt(Vector(4, 1, -2), Vector(2, 0, 0),  Vector(0, 1, 0));
    cerr << mymat.string() << endl;

    glLoadIdentity();
    glTranslated(10, 3, 5);
    gluLookAt(4, 1, -2,  2, 0, 0,  0, 1, 0);
    glScaled(6, 1, .5);
    PrMat();

    mymat.LoadIdentity();
    mymat.Translate(Vector(10, 3, 5));
    mymat.LookAt(Vector(4, 1, -2), Vector(2, 0, 0), Vector(0, 1, 0));
    mymat.Scale(Vector(6, 1, .5));
    cerr << mymat.string() << endl;

    glLoadIdentity();
    glTranslated(10, 3, 5);
    gluPerspective(30.0, 4.0/3.0, 10, 1000);
    PrMat();

    mymat.LoadIdentity();
    mymat.Translate(Vector(10, 3, 5));
    mymat.Perspective(30.0, 4.0/3.0, 10, 1000);
    cerr << mymat.string() << endl;

    glLoadIdentity();
    glScaled(40.0, 5.0, 9.0);
    PrMat();

    mymat.LoadIdentity();
    mymat.Scale(Vector(40.0, 5.0, 9.0));
    cerr << mymat.string() << endl;

    glLoadIdentity();
    glScaled(40.0, 5.0, 9.0);
    glRotated(131.0, 0.7071, 0, 0.7071);
    PrMat();

    mymat.LoadIdentity();
    mymat.Scale(Vector(40.0, 5.0, 9.0));
    mymat.Rotate(131, Vector(0.7071, 0, 0.7071));
    cerr << mymat.string() << endl;

    glLoadIdentity();
    glRotated(131.0, 0.7071, 0, 0.7071);
    glScaled(40.0, 5.0, 9.0);
    PrMat();

    mymat.LoadIdentity();
    mymat.Rotate(131, Vector(0.7071, 0, 0.7071));
    mymat.Scale(Vector(40.0, 5.0, 9.0));
    cerr << mymat.string() << endl;

    glLoadIdentity();
    glRotated(131.0, 0.7071, 0, 0.7071);
    glTranslated(40.0, 5.0, 9.0);
    PrMat();

    mymat.LoadIdentity();
    mymat.Rotate(131, Vector(0.7071, 0, 0.7071));
    mymat.Translate(Vector(40.0, 5.0, 9.0));
    cerr << mymat.string() << endl;

    return true;
}
