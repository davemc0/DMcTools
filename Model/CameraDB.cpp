//////////////////////////////////////////////////////////////////////
// CameraInfo.cpp - Represent a Camera and a list of Cameras
//
// Copyright David K. McAllister, Jan. 2002.

#include "Model/CameraDB.h"

#include "Math/MiscMath.h"

Matrix44<f3vec> CameraInfo::GetProjectionMatrix()
{
    Matrix44<f3vec> M;

    if (CameraType == DMC_CAM_PERSP)
        M.Frustum(wL, wR, wB, wT, Near, Far);
    else
        M.Ortho(wL, wR, wB, wT, Near, Far);

    return M;
}

void CameraInfo::GetFrame(f3vec& XCol, f3vec& YCol, f3vec& ZCol, f3vec& OCol)
{
    Matrix44<f3vec> EyeToWorld = WorldToEye.Inverse();

    EyeToWorld.GetFrame(XCol, YCol, ZCol, OCol);
}

f3vec CameraInfo::GetX()
{
    f3vec XCol, YCol, ZCol, OCol;
    GetFrame(XCol, YCol, ZCol, OCol);
    return XCol;
}

f3vec CameraInfo::GetY()
{
    f3vec XCol, YCol, ZCol, OCol;
    GetFrame(XCol, YCol, ZCol, OCol);
    return YCol;
}

f3vec CameraInfo::GetZ()
{
    f3vec XCol, YCol, ZCol, OCol;
    GetFrame(XCol, YCol, ZCol, OCol);
    return ZCol;
}

f3vec CameraInfo::GetPos()
{
    f3vec XCol, YCol, ZCol, OCol;
    GetFrame(XCol, YCol, ZCol, OCol);
    return OCol;
}

f3vec::ElType CameraInfo::GetFOVX() { return 2 * atan(wR / Near); }

f3vec::ElType CameraInfo::GetFOVY() { return 2 * atan(wT / Near); }

void CameraInfo::Xform(const Matrix44<f3vec>& M) { WorldToEye = WorldToEye * M; }

void CameraInfo::ComputePerspective(f3vec::ElType fovyRad, f3vec::ElType aspect, f3vec::ElType znear, f3vec::ElType zfar)
{
    ASSERT_D(znear > 0 && zfar > 0);
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
void CameraInfo::SetOrtho(f3vec::ElType Wid, f3vec::ElType Hgt, f3vec::ElType znear, f3vec::ElType zfar)
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

void CameraInfo::ComputeCamPoints(f3vec V[8])
{
    Matrix44<f3vec> EyeToWorld = WorldToEye.Inverse();

    // WINDOW EXTENTS ARE DEFINED ON THE NEAR PLANE, CALC NEAR PTS (IN CAM COORDS)
    f3vec::ElType NearZ = -Near;
    V[0] = f3vec(wR, wT, NearZ);
    V[1] = f3vec(wL, wT, NearZ);
    V[2] = f3vec(wL, wB, NearZ);
    V[3] = f3vec(wR, wB, NearZ);

    // CALC FAR PTS (IN CAM COORDS)
    f3vec::ElType FarZ = -Far, FN = Far / Near;
    f3vec::ElType fwL = wL * FN, fwR = wR * FN, fwB = wB * FN, fwT = wT * FN;
    V[4] = f3vec(fwR, fwT, FarZ);
    V[5] = f3vec(fwL, fwT, FarZ);
    V[6] = f3vec(fwL, fwB, FarZ);
    V[7] = f3vec(fwR, fwB, FarZ);

    // XFORM FRUSTUM IN CAM COORDS TO WORLD SPACE
    for (int i = 0; i < 8; i++) V[i] = EyeToWorld * V[i];
}

// Compute a plane from three points
// Points are right-hand wound
DMC_DECL void PlaneFromPoints(f3vec::ElType* ABCD, const f3vec P0, const f3vec P1, const f3vec P2)
{
    f3vec V1 = P1 - P0;
    f3vec V2 = P2 - P0;
    f3vec* ABC = (f3vec*)ABCD;
    *ABC = Cross(V1, V2);
    (*ABC).normalize();
    ABCD[3] = -dot(*ABC, P0); // D
}

// Calculate the six planes for a camera. User must have prealloced an array
// of 24 floats (6 planes * 4 coeffs each).
void CameraInfo::ComputeCamPlanes(const f3vec V[8], f3vec::ElType P[][4])
{
    PlaneFromPoints(P[0], V[2], V[5], V[6]); // LEFT
    PlaneFromPoints(P[1], V[0], V[7], V[4]); // RIGHT
    PlaneFromPoints(P[2], V[3], V[6], V[7]); // BOTTOM
    PlaneFromPoints(P[3], V[1], V[4], V[5]); // TOP
    PlaneFromPoints(P[4], V[1], V[2], V[0]); // NEAR
    PlaneFromPoints(P[5], V[4], V[6], V[5]); // FAR
}

void CameraInfo::WriteToFile(FILE* fp, int FrameNum)
{
    if (fp == NULL) {
        fprintf(stderr, "ERROR WRITING CAM TO FILE!\n");
        return;
    }
    // fprintf(fp,"%f %f %f %f %f %f %f %f %f %f %f %f\n",
    //  X.x,X.y,X.z, Y.x,Y.y,Y.z, Z.x,Z.y,Z.z, Orig.x,Orig.y,Orig.z);

    f3vec Orig, X, Y, Z;
    GetFrame(X, Y, Z, Orig);
    f3vec::ElType fovyDeg = dmcm::RtoD(GetFOVY());

    fprintf(fp, "%f[%f,%f,%f][%f,%f,%f][%f,%f,%f] %d\n", fovyDeg, Orig.x, Orig.y, Orig.z, Z.x, Z.y, Z.z, Y.x, Y.y, Y.z, FrameNum);
}

int CameraInfo::ReadFromFile(FILE* fp, int& FrameNum) // RETURNS "1" IF SUCCESSFUL, "0" IF EOF
{
    if (fp == NULL) {
        fprintf(stderr, "ERROR READING CAM FROM FILE!\n");
        return 0;
    }

    f3vec::ElType FOV;
    f3vec Orig, Y, Z;
    int Cond = fscanf(fp, "%f[%f,%f,%f][%f,%f,%f][%f,%f,%f] %d", &FOV, &Orig.x, &Orig.y, &Orig.z, &Z.x, &Z.y, &Z.z, &Y.x, &Y.y, &Y.z, &FrameNum);
    if (Cond == EOF) {
        fseek(fp, 0, SEEK_SET);
        Cond = fscanf(fp, "%f[%f,%f,%f][%f,%f,%f][%f,%f,%f] %d", &FOV, &Orig.x, &Orig.y, &Orig.z, &Z.x, &Z.y, &Z.z, &Y.x, &Y.y, &Y.z, &FrameNum);
    }

    if (Cond == EOF) { std::cerr << "Bad syntax in path file.\n"; }

    WorldToEye.LoadIdentity();
    ViewValid = true;

    WorldToEye.LookAt(Orig, Orig - Z, Y);
    // std::cerr << DtoR(FOV) << std::endl;
    ComputePerspective(dmcm::DtoR(FOV), wR / wT, Near, Far);

    return (Cond != EOF);
}
