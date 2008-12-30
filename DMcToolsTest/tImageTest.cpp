// Test the DMcTools tPixel and tImage classes

#include "Image/ImageAlgorithms.h"
#include "Image/tImage.h"

#include <limits>

using namespace std;

template <class TYPE>
void TestPixels1Chan()
{
    cerr << "************************* TestPixels1Chan\n";
    tPixel<TYPE,1> tp0 = 8;
    tPixel<TYPE,1> tp1 = tPixel<TYPE,1>(64);

    tPixel<TYPE,1> tmp0;
    tPixel<TYPE,1> tmp1 = tPixel<TYPE,1>(7);
    tPixel<TYPE,1> tmp2 = tPixel<TYPE,1>(1);
    tPixel<TYPE,1> tmp3(1);
    tmp3 = tmp1 - (TYPE)1;
    tPixel<TYPE,1> tmp4 = tmp1 - (TYPE)1;
    tPixel<TYPE,1> tmp5 = tPixel<TYPE,1>((TYPE)1);
    tPixel<TYPE,1> tmp6 = tPixel<TYPE,1>(1);
    tPixel<TYPE,1> tmp7 = tp0;
    tPixel<TYPE,1> tmp9 = 4;

    tPixel<TYPE,1> tp2 = tmp1 - (TYPE)1;

    cerr << "tmp2 = tmp5\n";
    tmp2 = tmp5;
    tmp1 += 74;
    tmp5 = (tmp1 + tp2) * tmp7;

    // TYPE *typ = tmp1 - 4; // Converts tmp1 to a TYPE *.
    // cerr << *(typ + 1) << endl; 

    cerr << tmp1 << endl;
    cerr << tmp2 << endl;
    cerr << tmp3 << endl;
    cerr << "sum_chan=" << tmp1.sum_chan() << endl;
    cerr << "min max=" << tmp3.min_chan() << ' ' << tmp3.max_chan() << endl;

    tPixel<double,1> a1 = 0.312341234;
    tPixel<unsigned short,1> a2 = a1;
    a2 = a1;
    cerr << a1 << " " << a2 << endl;
}

template <class TYPE>
void TestPixels2Chan()
{
    cerr << "************************* TestPixels2Chan\n";
    tPixel<TYPE,2> tp0 = 8;
    tPixel<TYPE,2> tp1 = tPixel<TYPE,2>(64);

    tPixel<TYPE,2> tmp0;
    tPixel<TYPE,2> tmp1 = tPixel<TYPE,2>(5);
    tPixel<TYPE,2> tmp2 = tPixel<TYPE,2>(2);
    tPixel<TYPE,2> tmp3(1,2,3);
    tPixel<TYPE,2> tmp4 = tmp1 - (TYPE)2;
    tPixel<TYPE,2> tmp5 = tPixel<TYPE,2>((TYPE)2);
    tPixel<TYPE,2> tmp6 = tPixel<TYPE,2>(1,2,3);
    tPixel<TYPE,2> tmp7 = tp0;
    tPixel<TYPE,2> tmp9 = 4;

    tPixel<TYPE,2> tp2 = tmp1 - (TYPE)1;

    cerr << "tmp2 = tmp5\n";
    tmp2 = tmp5;
    tmp1 += 74;

    // TYPE *typ = tmp1 - 4; // Converts tmp1 to a TYPE *.
    // cerr << *(typ + 1) << endl; 

    cerr << tmp1 << endl;
    cerr << tmp2 << endl;
    cerr << tmp2 << endl;
    cerr << tmp1.sum_chan() << endl;
    cerr << tmp3.min_chan() << ' ' << tmp3.max_chan() << endl;
}

template <class TYPE>
void TestPixels3Chan()
{
    cerr << "************************* TestPixels3Chan\n";
    tPixel<TYPE,3> tp0 = 8;
    tPixel<TYPE,3> tp1 = tPixel<TYPE,3>(64);

    tPixel<TYPE,3> tmp0;
    tPixel<TYPE,3> tmp1 = tPixel<TYPE,3>(5,6,7);
    tPixel<TYPE,3> tmp2 = 4;
    tPixel<TYPE,3> tmp3(1,2,3);
    tPixel<TYPE,3> tmp4 = tmp1 - (TYPE)2;
    tPixel<TYPE,3> tmp5 = tPixel<TYPE,3>((TYPE)3);
    tPixel<TYPE,3> tmp6 = tPixel<TYPE,3>(1,2,3);
    tPixel<TYPE,3> tmp7 = tp0;
    tPixel<TYPE,3> tmp9 = tPixel<TYPE,3>((TYPE)2);

    tPixel<TYPE,3> tp2 = tmp1 - (TYPE)1;

    cerr << "tmp2 = tmp5\n";
    tmp2 = tmp5;
    tmp1 += 74;

    // TYPE *typ = tmp1 - 4; // Converts tmp1 to a TYPE *.
    // cerr << *(typ + 1) << endl; 
    tmp5 = tp0;
    tmp5 = tp0 + tmp7;
    tmp5 = tmp7 + tp0;
    tp1 = tp0 + tmp7;
    tp1 = tmp7 + tp0;
    tmp5 = (tmp1 + static_cast<TYPE>(53.345)) + tmp7; // Converts tmp1 to a TYPE *.
    //tmp5 = tmp7 * ((-tmp1) + TYPE(53.4));
    // bool b = tmp5 < tmp3; // Doesn't work anymore if Chan>1.

    cerr << tmp1 << endl;
    cerr << tmp2 << endl;
    cerr << tmp3 << endl;
    cerr << tmp1.sum_chan() << endl;
    cerr << tmp3.min_chan() << ' ' << tmp3.max_chan() << endl;
}

template <class TYPE>
void TestPixels4Chan()
{
    cerr << "************************* TestPixels4Chan\n";
    cerr<<"tp0:  "; tPixel<TYPE,4> tp0 = 8; cerr << tp0 << endl;
    cerr<<"tp1:  "; tPixel<TYPE,4> tp1 = tPixel<TYPE,4>(64); cerr << tp1 << endl;

    cerr<<"tmp0: "; tPixel<TYPE,4> tmp0; cerr << tmp0 << endl;
    cerr<<"tmp1: "; tPixel<TYPE,4> tmp1 = 4; cerr << tmp1 << endl;
    cerr<<"tmp2: "; tPixel<TYPE,4> tmp2 = tp0; cerr << tmp2 << endl;
    cerr<<"tmp3: "; tPixel<TYPE,4> tmp3 = tPixel<TYPE,4>(2); cerr << tmp3 << endl;
    cerr<<"tmp4: "; tPixel<TYPE,4> tmp4 = tPixel<TYPE,4>((TYPE)4); cerr << tmp4 << endl;
    cerr<<"tmp5: "; tPixel<TYPE,4> tmp5(1,2,4,8); cerr << tmp5 << endl;
    cerr<<"tmp6: "; tPixel<TYPE,4> tmp6 = tPixel<TYPE,4>(5,6,7,8); cerr << tmp6 << endl;
    cerr<<"tmp7: "; tPixel<TYPE,4> tmp7 = tmp1 - (TYPE)2; cerr << tmp7 << endl;

    cerr<<"tp2:  "; tPixel<TYPE,4> tp2 = tmp1 - (TYPE)1; cerr << tp2 << endl;

    cerr << "tmp2 = tmp5\n";
    tmp2 = tmp5; cerr << tmp2 << endl;
    tmp1 += 74; cerr << tmp1 << endl;

    cerr << tmp1 << endl;
    cerr << tmp2 << endl;
    cerr << tmp1.sum_chan() << endl;
    cerr << tmp3.min_chan() << ' ' << tmp3.max_chan() << endl;
    cerr << tmp3 << endl;
    tmp4 = TYPE(1) / tmp3;
    cerr << tmp4 << endl << endl;
}

void TestPixelConversions()
{
    cerr << "************************* TestPixelsConversions\n";
    float i = numeric_limits<float>::max();
    int j = numeric_limits<unsigned char>::digits;
    cerr << "numeric_limits: " << i << " " << j << endl;

    f3Pixel a(8);
    ui1Pixel b = a;

    cerr << a << b << endl;
    f1Pixel c(3.141);
    unsigned int bob = c;
    float joe = c;
    cerr << bob << joe << endl;

    {
        f4Pixel b4(1,2,3,4);
        f4Pixel a4 = b4;
        f3Pixel a3 = b4;
        f2Pixel a2 = b4;
        f1Pixel a1 = b4;
        cerr << a4<<a3<<a2<<a1<<endl;
    }
    {
        f3Pixel b3(1,2,3,4);
        f4Pixel a4 = b3;
        f3Pixel a3 = b3;
        f2Pixel a2 = b3;
        f1Pixel a1 = b3;
        cerr << a4<<a3<<a2<<a1<<endl;
    }
    {
        f2Pixel b2(1,2,3,4);
        f4Pixel a4 = b2;
        f3Pixel a3 = b2;
        f2Pixel a2 = b2;
        f1Pixel a1 = b2;
        cerr << a4<<a3<<a2<<a1<<endl;
    }
    {
        f1Pixel b1(1,2,3,4);
        f4Pixel a4 = b1;
        f3Pixel a3 = b1;
        f2Pixel a2 = b1;
        f1Pixel a1 = b1;
        cerr << a4<<a3<<a2<<a1<<endl;
    }
}

void TestPixelDefinitions()
{
    cerr << "************************* TestPixelDefinitions\n";
    f3Pixel F(0.5, 0.34243, 0.0);
    F *= 1.32;
    F = F + 0.053f;
    F.r() = 0.122345;
    // F = F * 0.567; Doesn't work with double.
    cerr << F << endl;

    tPixel<unsigned int,3> i2;
    tPixel<unsigned short,3> s2(7,8,9);
    tPixel<unsigned int,3> i3;

    i2 = s2;
    i3 = i2;

    tPixel<double,1> a1 = 0.312341234;
    tPixel<unsigned short,2> a2 = a1;
    tPixel<unsigned int,3> a3(1254334,5345312,6245);
    tPixel<unsigned char,4> a4(12,53,62,250);
    tPixel<float,4> a5 = 1.0;

    cerr << a1 << " " << a2 << " " << a3 << endl << a4 << " " << a5 << endl;
    a1 = a4;
    a2 = a4;
    a3 = a4;
    a4 = a4;
    a5 = a4;
    cerr << a1 << " " << a2 << " " << a3 << endl << a4 << " " << a5 << endl;
    a1 = a3;
    a2 = a3;
    a3 = a3;
    a4 = a3;
    a5 = a3;
    cerr << a1 << " " << a2 << " " << a3 << endl << a4 << " " << a5 << endl;
    a1 = a2;
    a2 = a2;
    a3 = a2;
    a4 = a2;
    a5 = a2;
    cerr << a1 << " " << a2 << " " << a3 << endl << a4 << " " << a5 << endl;
    a1 = a1;
    a2 = a1;
    a3 = a1;
    a4 = a1;
    a5 = a1;
    cerr << a1 << " " << a2 << " " << a3 << endl << a4 << " " << a5 << endl;
}

void TestPixels()
{
    cerr << "************************* TestPixels\n";
    TestPixels1Chan<unsigned char>();
    TestPixels2Chan<unsigned char>();
    TestPixels3Chan<unsigned char>();
    TestPixels4Chan<unsigned char>();
    TestPixels1Chan<unsigned int>();
    TestPixels2Chan<unsigned int>();
    TestPixels3Chan<unsigned int>();
    TestPixels4Chan<unsigned int>();
    TestPixels1Chan<float>();
    TestPixels2Chan<float>();
    TestPixels3Chan<float>();
    TestPixels4Chan<float>();
    TestPixels1Chan<double>();
    TestPixels2Chan<double>();
    TestPixels3Chan<double>();
    TestPixels4Chan<double>();
    TestPixelConversions();
    TestPixelDefinitions();
}

void TestImages1()
{
    cerr << "************************* TestImages1\n";
    for(int i=0; i<3; i++) {
        cerr << "Loading\n";
        f3Image Imf("D:/Documents/Data/DaveMcSBRDF/Content/Images/CarolinaInnEnvMaps/CI4_touched/CI4_NY.hdr");
        Imf.Save("test9.hdr");

        f3Image Im0("D:/Documents/Data/DaveMcSBRDF/Content/Images/Acquired/C1x_7732.jpg");

        cerr << "Loaded: " << Im0.w() << " " << Im0.h() << endl;

        Im0.Save("test10.pfm");
        cerr << "Saved\n";
    }
}

void TestImages2()
{
    cerr << "************************* TestImages2\n";
    tImage<ui1Pixel> Img0(64,64, ui1Pixel(0));
    cerr << Img0.chan() << endl;

    tImage<tPixel<int,1>> Img1(64,64);
    tImage<tPixel<unsigned short,4>> Img2(64,64);
    tImage<tPixel<unsigned char,3>> Img3(64,64);
    tImage<tPixel<float,2>> Img4(64,64);

    cerr << Img0[5] << endl;

    cerr << "now here\n";
    //Img1.Load(NULL);
}

void TestImages3()
{
    cerr << "************************* TestImages3\n";
    f3Image Im0(100,100);

    cerr << "DeSpeckle\n";
    DeSpeckle(Im0);

    cerr << "ToneMapLinear\n";
    uc3Image Toned;
    ToneMapLinear<uc3Image>(Toned, Im0, 255.0f, 1.0f);
    Toned.Save("temp11.png");

    cerr << "Saved.\n";

    f3Image Im1(Im0);
    f3Image Im2 = Im0;
    cerr << "Bad.\n";

    ui3Image Im3(Im0);
    uc3Image Im4 = Im0;
    cerr << "Done\n"; 
}

void TestTIFFLoadSave()
{
    cerr << "************************* TestTIFFLoadSave\n";
    uc4Image I4(123, 456, uc4Pixel(1,2,3,4));

    I4.Save("test4.tif");
    uc4Image T4("test4.tif");
    bool Eq4 = I4 == T4;
    cerr << Eq4 << endl;

    uc3Image I3(I4);
    I3.Save("test3.tif");
    uc3Image T3("test3.tif");
    bool Eq3 = I3 == T3;
    cerr << Eq3 << endl;

    uc2Image I2(I4);
    I2.Save("test2.tif");
    uc2Image T2("test2.tif");
    bool Eq2 = I2 == T2;
    cerr << Eq2 << endl;
    cerr << I2[0] << T2[0] << endl;
    uc4Pixel P4(1,2,3,4);
    uc2Pixel P2(P4);
    cerr << P4<<P2<<endl;

    uc1Image I1(I4);
    I1.Save("test1.tif");
    uc1Image T1("test1.tif");
    bool Eq1 = I1 == T1;
    cerr << Eq1 << endl;
}

void TestImageConversion()
{
    cerr << "************************* TestImageConversion\n";
    uc3Image B1("../../Antique/Cloth/PlaidCloth.jpg");
    uc3Image B3("D:/Pictures/Stuff/By Us/MyArt/WoodFloor1.tif");
    B1.Save("test5.jpg");
    B3.Save("test6.jpg");
    baseImage *C = new uc3Image(B1);
    tImage<uc3Pixel> *D = dynamic_cast<uc3Image *>(&B3); // Could make a virtual casting operator to do this.
    ASSERT_R(D);
    //D->Save("test7.tif");
    tImage<f2Pixel> E(*D);
    //E.Save("test8.hdr");
    cerr << "Done\n";
}

void TestImages()
{
    cerr << "************************* TestImages\n";
    TestImageConversion();
    TestTIFFLoadSave();
    TestImages1();
    TestImages2();
    TestImages3();
}

bool tImageTest(int argc, char **argv)
{
    cerr << "tImageTest\n";
    TestPixels();
    TestImages();

    return true;
}
