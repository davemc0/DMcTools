//////////////////////////////////////////////////////////////////////
// Made to do a variety of cheap hacks on an image.

#include <Image/ucImage.h>

void DoFunkyStuffToImage(ucImage *Im)
{
    // Im->Set(Im->wid, Im->hgt, 1);


}

int main(int argc, char **argv)
{
    ucImage MyImage(argv[1], 1);

    DoFunkyStuffToImage(&MyImage);

    MyImage.Save(argv[2]);
}
