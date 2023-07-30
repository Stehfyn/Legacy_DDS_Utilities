#include <float.h>


#include <windows.h>
#include <dxtlib/dxtlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <io.h>
#include <string>

#include "tga.h"   


#define ALL_MIP_LEVELS 0


/*


    Example how to call the texture compression library, through nvDXTcompressBGRA

    This example reads in .tga file and writes out .dds files. 
        If the image has an alpha channel, a DXT3 file is created, otherwise a DXT1 is created.

    You can either use callback for each MIP level or calls to create a .dds file with 
        WriteDTXnFile.  WriteDTXnFile is called for header infor as well as the image data.


    -Doug Rogers
    NVIDIA Developer Relations
    doug@nvidia.com


 */
       




#include <time.h>
#include <stdlib.h>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <ios>
#include <iosfwd>    
#include <stdlib.h>

using namespace std;



NV_ERROR_CODE nvDXTReadCallback(void *buffer, size_t count, void * userData)
{
    std::istream* s = static_cast<std::istream*>(userData);
    s->read(static_cast<char*>(buffer), (std::streamsize)count);
    return s->good() ? NV_OK : NV_READ_FAILED;
}


 
NV_ERROR_CODE nvDXTWriteCallback(const void *buffer, 
                                 size_t count,  
                                 const MIPMapData * mipMapData, 
                                 void * userData)
{
    if (mipMapData)
    {
        // means a MIP map
    }
    else
    {
        // otherwise file header data
    }

    std::ostream* s = static_cast<std::ostream*>(userData);
    s->write(static_cast<const char*>(buffer), (std::streamsize)count);
    return s->good() ? NV_OK : NV_READ_FAILED;
}




////////////////////////////////////////

              
// read .tga, write out .dds file with MIP maps

void ReadTGA(const char * filename, RGBAImage & image )
{
    TGA tga;

    tga.ReadTGA(filename, image, true);


}

// read image and 
NV_ERROR_CODE ReadTexture(const char* filename, nvImageContainer & image)
{

    std::ifstream file(filename, ios::in|ios::binary);

    return nvDDS::nvDXTdecompress(image, 
        PF_RGBA, ALL_MIP_LEVELS,
        nvDXTReadCallback, &file);


}









NV_ERROR_CODE ScaleTexture(RGBAImage & image, const char* filename)
{

    
    nvCompressionOptions options;
    std::ofstream ofile(filename, ios::out|ios::binary);
    options.user_data = &ofile;
    

    options.textureFormat = k8888;  
    options.PreScaleImage(24.f, 20.f, kMipFilterTriangle);

    

    NV_ERROR_CODE hres = nvDDS::nvDXTcompress(
        image,
        &options,
        nvDXTWriteCallback);

    ofile.flush();

    return hres;
}



NV_ERROR_CODE WriteTexture(RGBAImage & image, nvTextureFormats textureFormat,
                           const char* filename)
{


    
    nvCompressionOptions options;
    std::ofstream ofile(filename, ios::out|ios::binary);
    options.user_data = &ofile;
    

    //options.quality = kQualityHighest;
    options.textureFormat = textureFormat;  



    NV_ERROR_CODE hres = nvDDS::nvDXTcompress(
        image,
        &options,
        nvDXTWriteCallback);

    ofile.flush();

    return hres;
}               




NV_ERROR_CODE WriteCubeMap(const char* filename, nvTextureFormats textureFormat)
{

    
    nvCompressionOptions options;
    std::ofstream ofile(filename, ios::out|ios::binary);


    options.user_data = &ofile;
    
    int width = 256;
    int height = 256;

    RGBAMipMappedCubeMap cubeMap(width, height, 0);

    for(int f = 0; f < 6; f++)
    {

        RGBAMipMappedImage & mipFace = cubeMap[f];

        for(size_t mipLevel = 0; mipLevel < mipFace.numMIPMaps(); mipLevel++)
        {
            RGBAImage & face = mipFace[mipLevel];

            unsigned char r = (unsigned char)(rand() & 0xFF);
            unsigned char g = (unsigned char)(rand() & 0xFF);
            unsigned char b = (unsigned char)(rand() & 0xFF);
            unsigned char a = (unsigned char)(rand() & 0xFF);

            for(size_t i = 0; i < face.height(); i++) 
            {
                for(size_t j = 0; j < face.width(); j++) 
                {
                    rgba_t & p = face(i, j);
                    p.r = r;
                    p.g = g;
                    p.b = b;
                    p.a = a;


                }
            }
        }
    }



    options.SetTextureFormat(kTextureTypeTexture2D, textureFormat );
    options.UseExisitingMIPMaps();


    NV_ERROR_CODE hres = nvDDS::nvDXTcompress(
        cubeMap,
        &options,
        nvDXTWriteCallback);

    ofile.flush();

    return hres;
}



float colors[][4] =
{

    {0,0,1,1},
    {0,1,0,1},
    {0,1,1,1},
    {1,0,0,1},
    {1,0,1,1},
    {1,1,0,1},
    {1,1,1,1},
    {0,0,0.5,1},
    {0,0.5,0,1},
    {0,0.5,0.5,1},
    {0.5,0,0,1},
    {0.5,0,0.5,1},
    {0.5,0.5,0,1},
    {0.5,0.5,0.5,1},
    {0.5,0.5,1,1},
    {0.5,1,0.5,1},
    {0.5,1,1,1},
    {1,0.5,0.5,1},
    {1,.50,1,1},
    {1,1,0.5,1},
};


NV_ERROR_CODE WriteVolumeMap(const char* filename, nvTextureFormats textureFormat)
{
    
    nvCompressionOptions options;
    std::ofstream ofile(filename, ios::out|ios::binary);


    options.user_data = &ofile;
    
    int width = 256;
    int height = 256;
    int depth = 8;

    RGBAMipMappedVolumeMap volumeMap(width, height, depth, 0);

    int c = 0;
    for(size_t mipLevel = 0; mipLevel < volumeMap.numMIPMaps(); mipLevel++)
    {

        RGBAImageArray & imageArray = volumeMap[mipLevel];

            c = mipLevel % 20;

            unsigned char r = colors[c][0] * 255;
            unsigned char g = colors[c][1] * 255;
            unsigned char b = colors[c][2] * 255;
            unsigned char a = colors[c][3] * 255;

        for(size_t i = 0; i < imageArray.size(); i++)
        {
            RGBAImage & slice = imageArray[i];


            
            //c++;
            //unsigned char r = (unsigned char)(rand() & 0xFF);
            //unsigned char g = (unsigned char)(rand() & 0xFF);
            //unsigned char b = (unsigned char)(rand() & 0xFF);
            //unsigned char a = (unsigned char)(rand() & 0xFF);

            for(size_t i = 0; i < slice.height(); i++) 
            {
                for(size_t j = 0; j < slice.width(); j++) 
                {
                    rgba_t & p = slice(i, j);
                    p.r = r;
                    p.g = g;
                    p.b = b;
                    p.a = a;


                }
            }
        }
    }


    options.SetTextureFormat(kTextureTypeVolumeMap, textureFormat );

    //options.UseExisitingMIPMaps();
    options.GenerateMIPMaps(ALL_MIP_LEVELS);


    NV_ERROR_CODE hres = nvDDS::nvDXTcompress(
        volumeMap,
        &options,
        nvDXTWriteCallback);

    ofile.flush();

    return hres;
}


int main()
{         


    RGBAImage rgbaImage;

    ReadTGA("argb.tga", rgbaImage);


    WriteTexture(rgbaImage, kDXT1, "textureWriteTest1.dds");
    WriteTexture(rgbaImage, kDXT1a, "textureWriteTest1a.dds");
    WriteTexture(rgbaImage, kDXT3, "textureWriteTest3.dds");
    WriteTexture(rgbaImage, kDXT5, "textureWriteTest5.dds");
    WriteTexture(rgbaImage, kDXT5_NM, "textureWriteTestNM.dds");

    ScaleTexture(rgbaImage, "scaleTextureWriteTest.dds");

    WriteTexture(rgbaImage, kA16B16G16R16F, "textureWriteTestFP.dds");


    nvImageContainer imageContainer;
    ReadTexture("textureWriteTest.dds", imageContainer);


    WriteCubeMap("cubeMapTest.dds", k8888);
    ReadTexture("cubeMapTest.dds", imageContainer);
    
    WriteVolumeMap("volumeMapTest.dds", k8888);
    ReadTexture("volumeMapTest.dds", imageContainer);
   
    return 0;
}