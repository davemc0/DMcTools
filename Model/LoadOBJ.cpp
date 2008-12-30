//////////////////////////////////////////////////////////////////////
// LoadOBJ.cpp - Parse most of Wavefront OBJ file and return it as a Model.
//
// Changes Copyright David K. McAllister, Aug. 1999.
// Partly based on code written by Peter-Pike Sloan, 1997.

#include "Model/Model.h"
#include "Model/TriObject.h"

#include <cstdio>
#include <cstring>

using namespace std;

struct MatInfo
{
    char *Name;
    TexInfo *TexPtr; // This points to the texture database record.
    double Shininess;
    Vector D, S, A, E;
    bool DColorValid, SColorValid, AColorValid, EColorValid, ShininessValid;

    DMC_INLINE MatInfo()
    {
        Name = NULL; TexPtr = NULL; // There is no texture with this material.
        DColorValid = SColorValid = AColorValid = EColorValid = ShininessValid = false;
    }
};

class MaterialDB
{
public:
    vector<MatInfo> MatList;

    // Inserts if not found.
    DMC_INLINE int FindByName(const char *name)
    {
        for(int mind=0; mind<(int)MatList.size(); mind++)
        {
            if(!strcmp(MatList[mind].Name, name))
                return mind;
        }

        return -1;
    }

    DMC_INLINE void Dump()
    {
        for(int i=0; i<(int)MatList.size(); i++)
            cerr << i << ": " << MatList[i].D << " " << MatList[i].Name << endl;
    }
};

static MaterialDB Mats;

DMC_INLINE Vector Get2D(char* buf)
{
    double x, y;
    if (2 != sscanf(buf,"%lf %lf",&x,&y))
    {
        fprintf(stderr, "Error, couldn't parse 2D!\n");
    }

    return Vector(x, y, 0);
}

DMC_INLINE Vector Get3D(char* buf)
{
    double x, y, z;

    if (3 != sscanf(buf,"%lf %lf %lf",&x,&y,&z))
    {
        fprintf(stderr,"Error, couldn't parse 3d!\n");
    }

    return Vector(x, y, z);
}

// Add this file to the global material database.
void LoadMTL(const char *fname)
{
    cerr << fname << endl;
    FILE *f = fopen(fname, "r");
    ASSERT_RM(f, "Error opening material file");
    MatInfo M;
    char TmpBuf[4096];

    while(fgets(TmpBuf,4096,f)) {
        // size_t l = strlen(TmpBuf);
        if(!strncmp(TmpBuf, "newmtl", 6)) {
            if(M.Name)
                Mats.MatList.push_back(M);

            M = MatInfo();
            M.Name = strdup(&TmpBuf[7]);
            *strchr(M.Name, '\n') = '\0';
        } else if(TmpBuf[0]=='K') {
            if(TmpBuf[1]=='a') {
                M.A = Get3D(&TmpBuf[2]);
                M.AColorValid = true;
            } else if(TmpBuf[1]=='d') {
                M.D = Get3D(&TmpBuf[2]);
                M.DColorValid = true;
                // cerr << "Diffuse = " << M.D << endl;
            } else if(TmpBuf[1]=='s') {
                M.S = Get3D(&TmpBuf[2]);
                M.SColorValid = true;
            } else if(TmpBuf[1]=='e') {
                M.E = Get3D(&TmpBuf[2]);
                M.EColorValid = true;
            }
        } else if(!strncmp(TmpBuf, "Ns", 2)) {
            // Ns ranges from 0 to 200.0.
            M.Shininess = Clamp(1.0, atof(&TmpBuf[3]), 200.0) * (127.0 / 200.0);
            M.ShininessValid = true;
            // cerr << "Shininess = " << M.Shininess << endl;
        } else if(!strncmp(TmpBuf, "illum", 5)) {
            // cerr << "Illum should be 2.\n";
        } else if(!strncmp(TmpBuf, "map_Kd", 6)) {
            // XXX Replace this: Just store the name and load all textures later.
            *strchr(&TmpBuf[7], '\n') = '\0';
            char *texFName = strdup(&TmpBuf[7]);
            cerr << "Texture FName = " << texFName << endl;

            M.TexPtr = Model::TexDB.FindByNameOrAdd(texFName);
        }
    }

    if(M.Name)
        Mats.MatList.push_back(M);

    fclose(f);
}

DMC_INLINE bool GetFace(char *OBuf, vector<Vector> &tverts, vector<Vector> &tnormals,
                    vector<Vector> &ttexcoords, vector<Vector> &verts, vector<Vector> &normals,
                    vector<Vector> &texcoords)
{
#define MAX_VERTS 128

    char *buf = OBuf;

    int uv[MAX_VERTS], v[MAX_VERTS], n[MAX_VERTS], i=0, j=0;
    bool HasNormals = true, HasTexcoords = true;

    while(3 == sscanf(buf, " %d/%d/%d%n", &v[i], &uv[i], &n[i], &j))
    {
        // cerr << "xx " << v[i] << " " << uv[i] << " " << n[i] << " XXX:" << buf << endl;

        v[i]--; uv[i]--; n[i]--;

        ASSERT_RM(v[i]>=0, "Vertex index underflow.");
        ASSERT_RM(uv[i]>=0, "Texcoord index underflow.");
        ASSERT_RM(n[i]>=0, "Normal index underflow.");

        ASSERT_RM(v[i]<(int)tverts.size(), "Vertex index overflow.");
        ASSERT_RM(uv[i]<(int)ttexcoords.size(), "Texcoord index overflow.");
        ASSERT_RM(n[i]<(int)tnormals.size(), "Normal index overflow.");

        buf += j;
        i++;
    }

    // cerr << i << endl;

    if(!i)
    {
        while(2 == sscanf(buf, " %d//%d%n", &v[i], &n[i], &j))
        {
            // cerr << "xx " << v[i] << " " << n[i] << " XXX:" << buf << endl;

            v[i]--; n[i]--;

            ASSERT_RM(v[i]>=0, "Vertex index underflow.");
            ASSERT_RM(n[i]>=0, "Normal index underflow.");

            ASSERT_RM(v[i]<(int)tverts.size(), "Vertex index overflow.");
            ASSERT_RM(n[i]<(int)tnormals.size(), "Normal index overflow.");

            buf += j;
            HasTexcoords = false;
            i++;
        }
    }

    if(!i)
    {
        while(2 == sscanf(buf, " %d/%d%n", &v[i], &uv[i], &j))
        {
            // cerr << "xx " << v[i] << " " << uv[i] << " XXX:" << buf << endl;

            v[i]--; uv[i]--;

            ASSERT_RM(v[i]>=0, "Vertex index underflow.");
            ASSERT_RM(uv[i]>=0, "Texcoord index underflow.");

            ASSERT_RM(v[i]<(int)tverts.size(), "Vertex index overflow.");
            ASSERT_RM(uv[i]<(int)ttexcoords.size(), "Texcoord index overflow.");

            buf += j;
            HasNormals = false;
            i++;
        }
    }

    if(!i)
    {
        while(1 == sscanf(buf, " %d%n", &v[i], &j))
        {
            // cerr << "xx " << v[i] << " XXX:" << buf << endl;
            v[i]--;

            ASSERT_RM(v[i]>=0, "Vertex index underflow.");
            ASSERT_RM(v[i]<(int)tverts.size(), "Vertex index overflow.");

            if((int)ttexcoords.size() > v[i])
            {
                // It implies a texcoord also.
                uv[i] = v[i];
                ASSERT_RM(uv[i]>=0, "Implied Texcoord index underflow.");
                ASSERT_RM(uv[i]<(int)ttexcoords.size(), "Implied Texcoord index overflow.");
            }
            else
                HasTexcoords = false;

            if((int)tnormals.size() > v[i])
            {
                // It implies a normal also.
                n[i] = v[i];
                ASSERT_RM(n[i]>=0, "Implied Normal index underflow.");
                ASSERT_RM(n[i]<(int)tnormals.size(), "Implied Normal index overflow.");
            }
            else
                HasNormals = false;

            buf += j;
            i++;
        }
    }

    if(!i)
    {
        cerr << "What kind of face is this, anyway? " << OBuf << endl;
        return false;
    }

    // Save the polygon as triangles.
    for(int k=2; k<i; k++)
    {
        // Emit a triangle.
        verts.push_back(tverts[v[0]]);
        verts.push_back(tverts[v[k-1]]);
        verts.push_back(tverts[v[k]]);

        if(HasTexcoords)
        {
            texcoords.push_back(ttexcoords[uv[0]]);
            texcoords.push_back(ttexcoords[uv[k-1]]);
            texcoords.push_back(ttexcoords[uv[k]]);
        }

        if(HasNormals)
        {
            normals.push_back(tnormals[n[0]]);
            normals.push_back(tnormals[n[k-1]]);
            normals.push_back(tnormals[n[k]]);
        }
    }

    return true;
}

// Returns false on success.
bool Model::LoadOBJ(const char *fname, const unsigned int RequiredAttribs,
        const unsigned int AcceptedAttribs)
{
    FILE *f = fopen(fname, "r");
    ASSERT_RM(f, "Error opening input file");

    Objs.clear();
    TriObject *Obj = new TriObject;
    Objs.push_back(Obj);
    strcpy(Obj->Name, "default");

    // These are used over all the groups in the object.
    vector<Vector> ttexcoords, tnormals, tverts;

    char TmpBuf[4096];
    while(fgets(TmpBuf,4096,f)) {
        if(TmpBuf[0] == 'v') {
         // some type of vertex
            switch(TmpBuf[1]) {
            case 't': // texture coordinate
                ttexcoords.push_back(Get2D(&TmpBuf[2]));
                break;
            case 'n': // normal...
                tnormals.push_back(Get3D(&TmpBuf[2]));
                break;
            case ' ': // vertex
            default: // vertex?
                const Vector &V = Get3D(&TmpBuf[2]);
                // cerr << V << endl;
                tverts.push_back(V);
                break;
            }
        } else if(TmpBuf[0] == 'f') {
            // a face
            bool OK = GetFace(&TmpBuf[1], tverts, tnormals, ttexcoords,
                Obj->verts, Obj->normals, Obj->texcoords);
            ASSERT_RM(OK, "Couldn't get the vertex indices.");
        } else if(TmpBuf[0] == 'g') {
            char GName[64];
            char X[64];
            GName[0] = '\0';
            sscanf(TmpBuf, "%s %s", X, GName);
            if(GName[0] == '\0')
                strcpy(GName, "default");
            // cerr << "Group: " << GName << endl;

            // See if the group name exists.
            int ii;
            for(ii=0; ii<(int)Objs.size(); ii++) {
                if(!strcmp(Objs[ii]->Name, GName))
                    break;
            }

            if(ii == (int)Objs.size()) {
                // Group is new. Set it up.
                // cerr << "Another group.\n";
                TriObject *Obj2 = new TriObject;
                Objs.push_back(Obj2);
                strncpy(Obj2->Name, GName, 64);

                // Copy the current material.
                if(Obj->SColorValid) { Obj2->SColorValid = true; Obj2->scolor = Obj->scolor; }
                if(Obj->AColorValid) { Obj2->AColorValid = true; Obj2->acolor = Obj->acolor; }
                if(Obj->EColorValid) { Obj2->EColorValid = true; Obj2->ecolor = Obj->ecolor; }
                if(Obj->ShininessValid) { Obj2->ShininessValid = true; Obj2->shininess = Obj->shininess; }
                if(Obj->dcolors.size()) { Obj2->dcolors.clear(); Obj2->dcolors.push_back(Obj->dcolors[0]); }
                Obj2->TexPtr = Obj->TexPtr;
                Obj2->Name[63] = true;
                Obj = Obj2;
            } else
                Obj = (TriObject *) Objs[ii];
        } else if(!strncmp(TmpBuf, "mtllib", 6)) {
            *strchr(&TmpBuf[7], '\n') = '\0';
            LoadMTL(&TmpBuf[7]);
        } else if(!strncmp(TmpBuf, "usemtl", 6)) {
            char *MatName = &TmpBuf[7];
            *strchr(MatName, '\n') = '\0';
            int mind = Mats.FindByName(MatName);

            if(mind < 0) {
                cerr << "Unknown material '" << MatName << "'" << endl;
                Mats.Dump();
            } else {
                // Set the group material to be this material.
                MatInfo &M = Mats.MatList[mind];
                if(M.SColorValid) { Obj->SColorValid = true; Obj->scolor = M.S; }
                if(M.AColorValid) { Obj->AColorValid = true; Obj->acolor = M.A; }
                if(M.EColorValid) { Obj->EColorValid = true; Obj->ecolor = M.E; }
                if(M.ShininessValid) { Obj->ShininessValid = true; Obj->shininess = M.Shininess; }
                if(M.DColorValid) { Obj->dcolors.clear(); Obj->dcolors.push_back(M.D); }
                Obj->TexPtr = M.TexPtr;
                // cerr << "Got material " << MatName << endl;
            }
        } else if(TmpBuf[0] == 's') {
            // This is a smoothing group. It doesn't really fit our paradigm, except
            // for smoothing = off.
            // Hide the smoothing on/off flag in the name string. Scary!
            Obj->creaseAngle = atoi(&TmpBuf[2]) > 0 ? M_PI : 0;
            // cerr << "Blah " << (Obj->Name[63] ? "Yes" : "No") << endl;
        } else {
            // cerr << "Unknown:" << TmpBuf;
        }
    }

    fclose(f);

#if 1
    ASSERT_R(0);
    // There is a problem with the .erase call in VC2005.
#else
    // We've read it all. Now we need to post-process it.
    for(int j=0; j<(int)Objs.size(); j++) {
        Obj = (TriObject *) Objs[j];

        if((int)Obj->verts.size() < 1) {
            delete Obj;
            std::vector<BaseObject *>::iterator bob(&Objs[j]);
            Objs.erase(bob);
            j--;
            continue;
        }

        // Build the BBox.
        for(int i=0; i<(int)Obj->verts.size(); i++)
            Obj->Box += Obj->verts[i];

        Box += Obj->Box;

        Obj->PrimType = L_TRIANGLES;
    }
#endif
    return false;
}
