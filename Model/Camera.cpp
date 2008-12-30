//////////////////////////////////////////////////////////////////////
// Camera.cpp - Represent a Camera and a list of Cameras.
//
// Copyright David K. McAllister, Jan. 2002.

#include "Model/CameraDB.h"
#include "Math/MiscMath.h"

using namespace std;

Matrix44 CameraInfo::GetProjectionMatrix()
{
    Matrix44 M;

    if(CameraType == DMC_CAM_PERSP)
        M.Frustum(wL, wR, wB, wT, Near, Far);
    else
        M.Ortho(wL, wR, wB, wT, Near, Far);

    return M;
}

void CameraInfo::GetFrame(Vector &XCol, Vector &YCol, Vector &ZCol, Vector &OCol)
{
    Matrix44 EyeToWorld = WorldToEye.Inverse();

    EyeToWorld.GetFrame(XCol, YCol, ZCol, OCol);
}

Vector CameraInfo::GetX()
{
    Vector XCol, YCol, ZCol, OCol;
    GetFrame(XCol, YCol, ZCol, OCol);
    return XCol;
}

Vector CameraInfo::GetY()
{
    Vector XCol, YCol, ZCol, OCol;
    GetFrame(XCol, YCol, ZCol, OCol);
    return YCol;
}

Vector CameraInfo::GetZ()
{
    Vector XCol, YCol, ZCol, OCol;
    GetFrame(XCol, YCol, ZCol, OCol);
    return ZCol;
}

Vector CameraInfo::GetPos()
{
    Vector XCol, YCol, ZCol, OCol;
    GetFrame(XCol, YCol, ZCol, OCol);
    return OCol;
}

double CameraInfo::GetFOVX()
{
    return 2*atan(wR/Near);
}

double CameraInfo::GetFOVY()
{
    return 2*atan(wT/Near);
}

void CameraInfo::Xform(const Matrix44 &M)
{
    WorldToEye = WorldToEye * M;
}

void CameraInfo::ComputePerspective(double fovyRad, double aspect, double znear, double zfar)
{
    ASSERT_D(znear>0 && zfar>0);
    Near = znear;
    Far = zfar;

    wT = Near * tan(fovyRad * 0.5);
    wB = -wT;
    wL = wB * aspect;
    wR = wT * aspect;

    ProjectionValid = true;
    CameraType = DMC_CAM_PERSP;
}

// These are all world coords.
void CameraInfo::SetOrtho(double Wid, double Hgt, double znear, double zfar)
{
    // ASSERT_D(znear>0 && zfar>0);
    Near = znear;
    Far = zfar;

    wT = Hgt * 0.5;
    wB = -wT;
    wR = Wid * 0.5;
    wL = -wR;

    ProjectionValid = true;
    CameraType = DMC_CAM_ORTHO;
}

void CameraInfo::ComputeCamPoints(f3Vector V[8])
{
    Matrix44 EyeToWorld = WorldToEye.Inverse();

    // WINDOW EXTENTS ARE DEFINED ON THE NEAR PLANE, CALC NEAR PTS (IN CAM COORDS)
    float NearZ = -Near;
    V[0] = f3Vector(wR,wT,NearZ);
    V[1] = f3Vector(wL,wT,NearZ);
    V[2] = f3Vector(wL,wB,NearZ);
    V[3] = f3Vector(wR,wB,NearZ);

    // CALC FAR PTS (IN CAM COORDS)
    float FarZ=-Far, FN=Far/Near;
    float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;
    V[4] = f3Vector(fwR,fwT,FarZ);
    V[5] = f3Vector(fwL,fwT,FarZ);
    V[6] = f3Vector(fwL,fwB,FarZ);
    V[7] = f3Vector(fwR,fwB,FarZ);

    // XFORM FRUSTUM IN CAM COORDS TO WORLD SPACE
    for(int i=0; i<8; i++)
        V[i] = EyeToWorld * V[i];
}

// Compute a plane from three points
// Points are right-hand wound
DMC_INLINE void PlaneFromPoints(float *ABCD, const f3Vector P0, const f3Vector P1, const f3Vector P2)
{
    f3Vector V1 = P1 - P0;
    f3Vector V2 = P2 - P0;
    f3Vector *ABC = (f3Vector *)ABCD;
    *ABC = Cross(V1, V2);
    (*ABC).normalize();
    ABCD[3] = -Dot(*ABC, P0); // D
}

// Calculate the six planes for a camera. User must have prealloced an array
// of 24 floats (6 planes * 4 coeffs each).
void CameraInfo::ComputeCamPlanes(const f3Vector V[8], float P[][4])
{
    PlaneFromPoints(P[0], V[2],V[5],V[6]); // LEFT
    PlaneFromPoints(P[1], V[0],V[7],V[4]); // RIGHT
    PlaneFromPoints(P[2], V[3],V[6],V[7]); // BOTTOM
    PlaneFromPoints(P[3], V[1],V[4],V[5]); // TOP
    PlaneFromPoints(P[4], V[1],V[2],V[0]); // NEAR
    PlaneFromPoints(P[5], V[4],V[6],V[5]); // FAR
}

void CameraInfo::VRMLtoMatrix()
{
    Matrix44 MakeLookVec;
    MakeLookVec.Rotate(OrientationTh, Orientation);
    Vector LookDir= MakeLookVec * Vector(0,0,-1);
    Vector LookPt = Position + LookDir * FocalDistance;

    WorldToEye.LoadIdentity();
    ViewValid = true;
    WorldToEye.LookAt(Position, LookPt, Vector(0,1,0));
}

void CameraInfo::WriteToFile(FILE *fp, int FrameNum)
{
    if (fp==NULL) { fprintf(stderr, "ERROR WRITING CAM TO FILE!\n"); return; }
    // fprintf(fp,"%f %f %f %f %f %f %f %f %f %f %f %f\n",
    //  X.x,X.y,X.z, Y.x,Y.y,Y.z, Z.x,Z.y,Z.z, Orig.x,Orig.y,Orig.z);

    Vector Orig, X, Y, Z;
    GetFrame(X, Y, Z, Orig);
    double fovyDeg = RtoD(GetFOVY());

    fprintf(fp,"%f[%f,%f,%f][%f,%f,%f][%f,%f,%f] %d\n", fovyDeg,
        Orig.x,Orig.y,Orig.z, Z.x,Z.y,Z.z, Y.x,Y.y,Y.z, FrameNum);
}

int CameraInfo::ReadFromFile(FILE *fp, int &FrameNum)  // RETURNS "1" IF SUCCESSFUL, "0" IF EOF
{
    if (fp==NULL) { fprintf(stderr, "ERROR READING CAM FROM FILE!\n"); return 0; }

    float FOV;
    f3Vector Orig, Y, Z;
    int Cond = fscanf(fp,"%f[%f,%f,%f][%f,%f,%f][%f,%f,%f] %d",
        &FOV,
        &Orig.x,&Orig.y,&Orig.z,
        &Z.x,&Z.y,&Z.z,
        &Y.x,&Y.y,&Y.z, &FrameNum);
    if(Cond == EOF) {
        fseek(fp, 0, SEEK_SET);
        Cond = fscanf(fp,"%f[%f,%f,%f][%f,%f,%f][%f,%f,%f] %d",
            &FOV,
            &Orig.x,&Orig.y,&Orig.z,
            &Z.x,&Z.y,&Z.z,
            &Y.x,&Y.y,&Y.z, &FrameNum);
    }

    if(Cond == EOF) {
        cerr << "Bad syntax in path file.\n";
    }

    WorldToEye.LoadIdentity();
    ViewValid = true;

    WorldToEye.LookAt(Orig, Orig - Z, Y);
    // cerr << DtoR(FOV) << endl;
    ComputePerspective(DtoR(FOV), wR/wT, Near, Far);

    return (Cond!=EOF);
}
