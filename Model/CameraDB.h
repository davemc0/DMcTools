//////////////////////////////////////////////////////////////////////
// CameraDB.h - Represent a Camera and a list of Cameras.
//
// Copyright David K. McAllister, July 1999.

#ifndef dmc_cameradb_h
#define dmc_cameradb_h

#include "Math/Matrix44.h"
#include "Util/Utils.h"

#include <vector>

class CameraInfo
{
    Matrix44 WorldToEye; // This matrix, usually orthonormal, defines the camera pose.

public:
#define DMC_CAM_ORTHO 0
#define DMC_CAM_PERSP 1
    const char *CameraName;
    int CameraID; // This is the OpenGL Camera object ID.
    int CameraType; // 0 = Orthographic, 1 = Perspective.

    // XXX These are only used for loading from VRML.
    Vector Position;
    Vector Orientation;
    float OrientationTh;
    float FocalDistance;
    float HeightValue;

    // The projection matrix
    float wL,wR,wB,wT;        // WINDOW EXTENTS DEFINED AS A RECT ON NearPlane
    float Near,Far;           // DISTANCES TO NEAR AND FAR PLANES (IN VIEWING DIR)

    bool ProjectionValid, ViewValid; // Tells whether the camera data is valid.

    DMC_INLINE CameraInfo()
    {
        CameraName = "NoName";
        CameraID = -1;
        CameraType = -1;
        Position = Vector(0,0,1);
        Orientation = Vector(0,0,1);
        OrientationTh = 0;
        FocalDistance = 5;
        HeightValue = 0.785398; // Default is 2 for Orthographic.
        ProjectionValid = ViewValid = false;
        ComputePerspective(DtoR(45), 1, 1, 1000);
    }

    Matrix44 GetProjectionMatrix();
    Matrix44 GetViewMatrix() {return WorldToEye;}

    Matrix44 &ViewMat() {return WorldToEye;}

    Vector GetX();
    Vector GetY();
    Vector GetZ();
    Vector GetPos();
    void GetFrame(Vector &XCol, Vector &YCol, Vector &ZCol, Vector &OCol);
    double GetFOVX(); // Returns vertical FOV in radians.
    double GetFOVY(); // Returns vertical FOV in radians.

    // Switch to a Perspective camera and set its params
    void ComputePerspective(double fovyRad, double aspect, double znear, double zfar);
    // Switch to an Ortho camera and set its params
    void SetOrtho(double Wid, double Hgt, double znear, double zfar);

    // Compute the eight corners of the view frustum
    void ComputeCamPoints(f3Vector V[8]);

    // Calculate the six planes for a camera. User must have prealloced an array
    // of 24 floats (6 planes * 4 coeffs each).
    void ComputeCamPlanes(const f3Vector V[8], float P[][4]);

    void Xform(const Matrix44 &M);

    void VRMLtoMatrix();

    void WriteToFile(FILE *fp, int FrameNum);
    int ReadFromFile(FILE *fp, int &FrameNum);  // RETURNS "1" IF SUCCESSFUL, "0" IF EOF

    DMC_INLINE void Dump()
    {
        std::cerr << CameraName<<" "<<CameraID<<" "<<CameraType<<" "<<Position<<" "<<Orientation
                  <<" "<<OrientationTh<<" "<<FocalDistance<<" "<<HeightValue<<std::endl;
    }
};

class CameraDB
{
public:
    std::vector<CameraInfo *> CameraList;

    // Returns -1 if not found.
    DMC_INLINE CameraInfo *FindByName(const char *name)
    {
        ASSERT_R(name);
        for(int tind=0; tind<(int)CameraList.size(); tind++) {
            if(!strcmp(CameraList[tind]->CameraName, name))
                return CameraList[tind];
        }

        return NULL;
    }

    DMC_INLINE CameraInfo *Add(const char *name=NULL)
    {
        CameraInfo *x = new CameraInfo();
        x->CameraName = name;

        CameraList.push_back(x);

        return x;
    }

    // Adds it if not found.
    DMC_INLINE CameraInfo *FindByNameOrAdd(const char *name)
    {
        ASSERT_R(name);
        CameraInfo *x = FindByName(name);
        if(x == NULL) {
            x = Add();
        }

        return x;
    }

    DMC_INLINE void Dump()
    {
        for(int i=0; i<(int)CameraList.size(); i++) {
            std::cerr << i << ": ";
            CameraList[i]->Dump();
        }
    }
};

#endif
