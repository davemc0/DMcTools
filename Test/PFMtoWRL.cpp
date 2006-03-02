// Convert a range image into a VRML model

#include <Image/RangeImage.h>
#include <Model/Model.h>

inline float Scl(float Rng, float Scale, float Clmp)
{
  return Clamp(-Clmp, Rng * Scale, Clmp);
}

Model HFtoModel(const RangeImage &Rng, double Scale = 1.0, double Clmp = DMC_MAXFLOAT)
{
  Model M;
  M.Objs.push_back();
  BaseObject &O = M.Objs[0];

  for(int y=0; y<Rng.hgt-1; y++)
    {
      for(int x=0; x<Rng.wid-1; x++)
        {
          Vector Col(DRand(), DRand(), DRand());

          O.dcolors.push_back(Col);
          O.verts.push_back(Vector(x, y, Scl(Rng(x,y), Scale, Clmp)));
          O.dcolors.push_back(Col);
          O.verts.push_back(Vector(x, y+1,  Scl(Rng(x,y+1), Scale, Clmp)));
          O.dcolors.push_back(Col);
          O.verts.push_back(Vector(x+1, y,  Scl(Rng(x+1,y), Scale, Clmp)));

          Col.y = DRand();

          O.dcolors.push_back(Col);
          O.verts.push_back(Vector(x, y+1,  Scl(Rng(x,y+1), Scale, Clmp)));
          O.dcolors.push_back(Col);
          O.verts.push_back(Vector(x+1, y+1,  Scl(Rng(x+1,y+1), Scale, Clmp)));
          O.dcolors.push_back(Col);
          O.verts.push_back(Vector(x+1, y,  Scl(Rng(x+1,y), Scale, Clmp)));
        }
    }

  M.RebuildBBox();

  return M;
}

int main(int argc, char **argv)
{
  ASSERTERR(argc >= 3 && argc <= 5, "Usage: infile.pfm outfile.wrl [Z_scale] [Z_clamp]");

  RangeImage Rng(argv[1]);

  double Scale = 1.0, Clamp = DMC_MAXFLOAT;

  if(argc > 3) Scale = atof(argv[3]);
  if(argc > 4) Clamp = atof(argv[4]);

  Model M = HFtoModel(Rng, Scale, Clamp);

  M.Save(argv[2]);

  return 0;
}
