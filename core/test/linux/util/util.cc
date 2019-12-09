#include<string>
#include <vector>
#include <iostream>
#include <lodepng.h>


void encodePixelsToFile(std::string filename, uint8_t *buffer, int width, int height)
{
    //write the pixles to file
    unsigned error = lodepng::encode(filename.c_str(), buffer, width, height);
    if (error)
    {
        std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << filename << std::endl;
    }
}

void decodeFile2Pixels(std::string filename, std::vector<unsigned char> &image)
{
    unsigned width, height;
    //decode image
    unsigned error = lodepng::decode(image, width, height, filename.c_str());
    //if there's an error, display it
    if (error)
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}




