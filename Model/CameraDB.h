//////////////////////////////////////////////////////////////////////
// CameraDB.h - Represent a Camera and a list of Cameras
//
// Copyright David K. McAllister, July 1999.

#pragma once

#include "Math/Matrix44.h"
#include "Util/Utils.h"

#include <vector>

class CameraInfo {
    Matrix44<typename f3Vector::ElType> WorldToEye; // This matrix, usually orthonormal, defines the camera pose.

public:
#define DMC_CAM_ORTHO 0
#define DMC_CAM_PERSP 1
    const char* CameraName;
    int CameraID;   // This is the OpenGL Camera object ID.
    int CameraType; // 0 = Orthographic, 1 = Perspective.

    // XXX These are only used for loading from VRML.
    f3Vector Position;
    f3Vector Orientation;
    f3Vector::ElType OrientationTh;
    f3Vector::ElType FocalDistance;
    f3Vector::ElType HeightValue;

    // The projection matrix
    f3Vector::ElType wL, wR, wB, wT; // WINDOW EXTENTS DEFINED AS A RECT ON NearPlane
    f3Vector::ElType Near, Far;      // DISTANCES TO NEAR AND FAR PLANES (IN VIEWING DIR)

    bool ProjectionValid, ViewValid; // Tells whether the camera data is valid.

    DMC_DECL CameraInfo()
    {
        CameraName = "NoName";
        CameraID = -1;
        CameraType = -1;
        Position = f3Vector(0, 0, 1);
        Orientation = f3Vector(0, 0, 1);
        OrientationTh = 0;
        FocalDistance = 5;
        HeightValue = 0.785398; // Default is 2 for Orthographic.
        ProjectionValid = ViewValid = false;
        ComputePerspective(dmcm::DtoR(45.), 1, 1, 1000);
    }

    Matrix44<typename f3Vector::ElType> GetProjectionMatrix();
    Matrix44<typename f3Vector::ElType> GetViewMatrix() { return WorldToEye; }

    Matrix44<typename f3Vector::ElType>& ViewMat() { return WorldToEye; }

    f3Vector GetX();
    f3Vector GetY();
    f3Vector GetZ();
    f3Vector GetPos();
    void GetFrame(f3Vector& XCol, f3Vector& YCol, f3Vector& ZCol, f3Vector& OCol);
    f3Vector::ElType GetFOVX(); // Returns vertical FOV in radians.
    f3Vector::ElType GetFOVY(); // Returns vertical FOV in radians.

    // Switch to a Perspective camera and set its params
    void ComputePerspective(f3Vector::ElType fovyRad, f3Vector::ElType aspect, f3Vector::ElType znear, f3Vector::ElType zfar);
    // Switch to an Ortho camera and set its params
    void SetOrtho(f3Vector::ElType Wid, f3Vector::ElType Hgt, f3Vector::ElType znear, f3Vector::ElType zfar);

    // Compute the eight corners of the view frustum
    void ComputeCamPoints(f3Vector V[8]);

    // Calculate the six planes for a camera. User must have prealloced an array
    // of 24 floats (6 planes * 4 coeffs each).
    void ComputeCamPlanes(const f3Vector V[8], f3Vector::ElType P[][4]);

    void Xform(const Matrix44<typename f3Vector::ElType>& M);

    void VRMLtoMatrix();

    void WriteToFile(FILE* fp, int FrameNum);
    int ReadFromFile(FILE* fp, int& FrameNum); // RETURNS "1" IF SUCCESSFUL, "0" IF EOF

    DMC_DECL void Dump()
    {
        std::cerr << CameraName << " " << CameraID << " " << CameraType << " " << Position << " " << Orientation << " " << OrientationTh << " " << FocalDistance
                  << " " << HeightValue << std::endl;
    }
};

class CameraDB {
public:
    std::vector<CameraInfo*> CameraList;

    // Returns -1 if not found.
    DMC_DECL CameraInfo* FindByName(const char* name)
    {
        ASSERT_R(name);
        for (int tind = 0; tind < (int)CameraList.size(); tind++) {
            if (!strcmp(CameraList[tind]->CameraName, name)) return CameraList[tind];
        }

        return NULL;
    }

    DMC_DECL CameraInfo* Add(const char* name = NULL)
    {
        CameraInfo* x = new CameraInfo();
        x->CameraName = name;

        CameraList.push_back(x);

        return x;
    }

    // Adds it if not found.
    DMC_DECL CameraInfo* FindByNameOrAdd(const char* name)
    {
        ASSERT_R(name);
        CameraInfo* x = FindByName(name);
        if (x == NULL) { x = Add(); }

        return x;
    }

    DMC_DECL void Dump()
    {
        for (int i = 0; i < (int)CameraList.size(); i++) {
            std::cerr << i << ": ";
            CameraList[i]->Dump();
        }
    }
};
