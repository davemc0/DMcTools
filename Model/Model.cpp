//////////////////////////////////////////////////////////////////////
// Model.cpp - A list of TriObject, Mesh, RenderObject, etc.
//
// Copyright David K. McAllister, July 1999.

#include <Model/Model.h>
#include <Model/Mesh.h>
#include <Model/RenderObject.h>
#include <Model/LightDB.h>
#include <Model/CameraDB.h>

// Define this static dude.
TextureDB Model::TexDB;
LightDB LitDB;
CameraDB CamDB;

// Returns false on success; true on error.
bool Model::Save(const char *fname)
{
    ASSERTERR(fname, "NULL filename");
    bool status = true;

    char *extc = strrchr(fname, '.');
    extc++;
    
    if(strlen(extc) != 3) {
        cerr << "Can't grok filename " << fname << endl;
        return status;
    }
    
	char *extp = strdup(extc);

    extp[0] |= 0x20;
    extp[1] |= 0x20;
    extp[2] |= 0x20;
    
    if(!strcmp(extp, "wrl"))
        status = SaveVRML(fname);
    else if(!strcmp(extp, "obj"))
        status = SaveOBJ(fname);
    else if(!strcmp(extp, "tri"))
        status = SaveTRI(fname);
    else if(!strcmp(extp, "ply"))
        status = SavePLY(fname);
    else {
        cerr << "Can't grok filename " << fname << endl;
        status = true;
    }
    
    cerr << "Model has " << int(Objs.size()) << " objects.\n";

    return status;
}

// Returns false on success; true on error.
bool Model::Load(const char *fname, const unsigned int RequiredAttribs,
        const unsigned int AcceptedAttribs)
{
    ASSERTERR(fname, "NULL filename");
    bool status = true;

    char *extc = strrchr(fname, '.');
    extc++;
    
    if(strlen(extc) != 3) {
        cerr << "Can't grok filename " << fname << endl;
        return status;
    }
    
	char *extp = strdup(extc);

    extp[0] |= 0x20;
    extp[1] |= 0x20;
    extp[2] |= 0x20;
    
    if(!strcmp(extp, "wrl"))
        status = LoadVRML(fname, RequiredAttribs, AcceptedAttribs);
    else if(!strcmp(extp, "obj"))
        status = LoadOBJ(fname, RequiredAttribs, AcceptedAttribs);
    else if(!strcmp(extp, "tri"))
        status = LoadTRI(fname, RequiredAttribs, AcceptedAttribs);
    else if(!strcmp(extp, "ply"))
        status = LoadPLY(fname, RequiredAttribs, AcceptedAttribs);
    else {
        cerr << "Can't grok filename " << fname << endl;
        status = true;
    }
    
    // SaveVRML("foo.wrl");
    // ((Mesh*)Objs[0])->CheckIntegrity();
    // RemoveNormals(); // XXX

    if(!status)
        ModifyAttribs(RequiredAttribs, AcceptedAttribs);
    
    return status;
}

// Converts DMC_MESH_OBJECTs in this Model into DestType objects,
// which is DMC_RENDER_OBJECT or DMC_TRI_OBJECT.
void Model::ObjectConvert(ObjectTypes DestType, unsigned int AcceptedAttribs)
{
    for(int i=0; i<Objs.size(); i++) {
        // cerr << i << endl;
        if(Objs[i]->ObjectType == DMC_MESH_OBJECT) {
            BaseObject *Ob;
            switch(DestType) {
            case DMC_RENDER_OBJECT:
                Ob = new RenderObject;
                ((Mesh *)Objs[i])->ExportRenderObject(*((RenderObject *)Ob), AcceptedAttribs);
                delete (Mesh *)Objs[i];
                Objs[i] = Ob;
                break;
            case DMC_TRI_OBJECT:
                Ob = new TriObject;
                ((Mesh *)Objs[i])->ExportTriObject(*((TriObject *)Ob), AcceptedAttribs);
                delete (Mesh *)Objs[i];
                Objs[i] = Ob;
                break;
            default:
                ASSERT0(0);
            }
        }
    }
}

// XXX Need to generalize this.
// Return a duplicate model with a single TriObject.
void Model::Flatten()
{
    ASSERTERR(0, "Not updated.");

    if(Objs.size() < 2)
        return;
    
    TriObject *NOb = (TriObject *) Objs[0];
    
    for(int i=1; i<Objs.size(); i++) {
        const TriObject *O = (TriObject *) Objs[i];
        
        if(O->verts.size() < 1)
            continue;
        
        if(O->dcolors.size() > 1)
            ASSERTERR(O->dcolors.size() == O->verts.size(), "TriObject Bad dcolors.size()");
        
        if(O->normals.size() > 1)
            ASSERTERR(O->normals.size() == O->verts.size(), "TriObject Bad normals.size()");
        
        if(O->texcoords.size() > 1)
            ASSERTERR(O->texcoords.size() == O->verts.size(), "TriObject Bad texcoords.size()");
        
        if(NOb->dcolors.size() > 1)
            ASSERTERR(NOb->dcolors.size() == NOb->verts.size(), "List Bad dcolors.size()");
        
        if(NOb->normals.size() > 1)
            ASSERTERR(NOb->normals.size() == NOb->verts.size(), "List Bad normals.size()");
        
        if(NOb->texcoords.size() > 1)
            ASSERTERR(NOb->texcoords.size() == NOb->verts.size(), "List Bad texcoords.size()");
        
        if(NOb->dcolors.size() == 1 && ((O->dcolors.size() == 1 && O->dcolors[0] != NOb->dcolors[0])
            || O->dcolors.size() > 1)) {
            // Expand my colors.
            Vector col = NOb->dcolors[0];
            NOb->dcolors.clear();
            NOb->dcolors.insert(NOb->dcolors.begin(), NOb->verts.size(), col);
        }
        
        if(NOb->dcolors.size() != 1) {
            if(O->dcolors.size() == 1) {
                // Expand its colors.
                Vector col = O->dcolors[0];
                NOb->dcolors.insert(NOb->dcolors.end(), O->verts.size(), col);
            } else if(O->dcolors.size() == 0) {
                if(NOb->dcolors.size()) {
                    // Have to synthesize a bunch of them.
                    Vector col(0,1,0);
                    NOb->dcolors.insert(NOb->dcolors.end(), O->verts.size(), col);
                }
            } else {
                // Copy its list to my list.
                NOb->dcolors.insert(NOb->dcolors.end(), O->dcolors.begin(), O->dcolors.end());
            }
        }
        
        if(NOb->normals.size() == 1 && ((O->normals.size() == 1 && O->normals[0] != NOb->normals[0])
            || O->normals.size() > 1)) {
            // Expand my normals.
            Vector col = NOb->normals[0];
            NOb->normals.clear();
            NOb->normals.insert(NOb->normals.begin(), NOb->verts.size(), col);
        }
        
        if(NOb->normals.size() != 1) {
            if(O->normals.size() == 1) {
                // Expand its normals.
                Vector col = O->normals[0];
                NOb->normals.insert(NOb->normals.end(), O->verts.size(), col);
            } else if(O->normals.size() == 0) {
                if(NOb->normals.size()) {
                    // Have to synthesize a bunch of them.
                    Vector col(0,1,0);
                    NOb->normals.insert(NOb->normals.end(), O->verts.size(), col);
                }
            } else {
                // Copy its list to my list.
                NOb->normals.insert(NOb->normals.end(), O->normals.begin(), O->normals.end());
            }
        }
        
        if(NOb->texcoords.size() == 1 && ((O->texcoords.size() == 1
            && O->texcoords[0] != NOb->texcoords[0]) || O->texcoords.size() > 1)) {
            // Expand my texcoords.
            Vector col = NOb->texcoords[0];
            NOb->texcoords.clear();
            NOb->texcoords.insert(NOb->texcoords.begin(), NOb->verts.size(), col);
        }
        
        if(NOb->texcoords.size() != 1) {
            if(O->texcoords.size() == 1) {
                // Expand its texcoords.
                Vector col = O->texcoords[0];
                NOb->texcoords.insert(NOb->texcoords.end(), O->verts.size(), col);
            } else if(O->texcoords.size() == 0) {
                if(NOb->texcoords.size()) {
                    // Have to synthesize a bunch of them.
                    Vector col(0,1,0);
                    NOb->texcoords.insert(NOb->texcoords.end(), O->verts.size(), col);
                }
            } else {
                // Copy its list to my list.
                NOb->texcoords.insert(NOb->texcoords.end(), O->texcoords.begin(), O->texcoords.end());
            }
        }
        
        NOb->verts.insert(NOb->verts.end(), O->verts.begin(), O->verts.end());
    }
    
    if(Objs.size() > 1) {
        for(int i=1; i<Objs.size(); i++) {
            delete Objs[i]; // XXX Do we need to detect and cast the type?
        }
        Objs.erase(Objs.begin()+1, Objs.end());
    }

    TriObject *O0 = (TriObject *) Objs[0];

    ASSERTERR(O0->verts.size() % 3 == 0, "Must have a multiple of three vertices in TriObject.");
    
    if(O0->dcolors.size() > 1)
        ASSERTERR(O0->dcolors.size() == O0->verts.size(), "Bad dcolors.size()");
    
    if(O0->normals.size() > 1)
        ASSERTERR(O0->normals.size() == O0->verts.size(), "Bad normals.size()");
    
    if(O0->texcoords.size() > 1)
        ASSERTERR(O0->texcoords.size() == O0->verts.size(), "Bad texcoords.size()");
}

void Model::Dump() const
{
    cerr << "Dumping Model ObjId: " << ObjID << "\nModel BBox: " << Box << endl
        << "NumObjects: " << int(Objs.size()) << endl << endl;
    for(int i=0; i<Objs.size(); i++) {
        cerr << "Object index: " << i << endl;
        Objs[i]->Dump();
    }
}

void Model::RebuildBBox()
{
    Box.Reset();
    
    for(int i=0; i<Objs.size(); i++) {
        Objs[i]->RebuildBBox();
        Box += Objs[i]->Box;
    }
}

// Also rebuilds the BBox.
void Model::ApplyTransform(Matrix44 &Mat)
{
    Box.Reset();
    
    for(int i=0; i<Objs.size(); i++) {
        Objs[i]->ApplyTransform(Mat);
        Box += Objs[i]->Box;
    }
}

void Model::GenColors()
{
    for(int i=0; i<Objs.size(); i++)
        Objs[i]->GenColors();
}

void Model::GenNormals()
{
    for(int i=0; i<Objs.size(); i++)
        Objs[i]->GenNormals();
}

void Model::GenTexCoords()
{
    for(int i=0; i<Objs.size(); i++)
        Objs[i]->GenTexCoords();
}

void Model::GenTangents()
{
    for(int i=0; i<Objs.size(); i++)
        Objs[i]->GenTangents();
}

void Model::RemoveColors()
{
    for(int i=0; i<Objs.size(); i++)
        Objs[i]->RemoveColors();
}

void Model::RemoveNormals()
{
    for(int i=0; i<Objs.size(); i++)
        Objs[i]->RemoveNormals();
}

void Model::RemoveTexCoords()
{
    for(int i=0; i<Objs.size(); i++)
        Objs[i]->RemoveTexCoords();
}

void Model::RemoveTangents()
{
    for(int i=0; i<Objs.size(); i++)
        Objs[i]->RemoveTangents();
}

void Model::FixFacing()
{
    for(int i=0; i<Objs.size(); i++) {
        if(Objs[i]->ObjectType == DMC_MESH_OBJECT)
            ((Mesh *)Objs[i])->FixFacing();
    }
}
