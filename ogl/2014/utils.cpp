#include <fstream>
#include <sstream>
#include <streambuf>
#include <png.h>

#include "utils.hpp"

string OpenTextFile(const string filename) {
    std::ifstream file(filename.c_str());
    if(file.is_open()) {
        string buffer;
        file.seekg(0, std::ios::end);
        buffer.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        buffer.assign((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

        file.close();
        return buffer;
    } else {
        cerr << "ERROR: Could not open file: " << filename << endl;
        return 0;
    }
}

string OpenBMPFile(const string filename, unsigned int &width, unsigned int &height) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if(!file.is_open()) {
        cerr << "ERROR: Could not open BMP file: " << filename << endl;
        return 0;
    }
    string filestr;
    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    filestr.reserve(fileSize);
    file.seekg(0, std::ios::beg);
    filestr.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    string header;
    header.reserve(54);
    header.assign(filestr.begin(), filestr.begin()+54);
    if(header[0] != 'B' || header[1] != 'M') {
        cerr << "ERROR: Not a valid BMP file" << endl;
        return 0;
    }
    unsigned int dataPos = *(int*)&(header[0x0A]),
                 imageSize = *(int*)&(header[0x22]),
                 imageWidth = *(int*)&(header[0x12]),
                 imageHeight = *(int*)&(header[0x16]);

    imageSize = (imageSize == 0) ? imageWidth * imageHeight * 3 : imageSize;
    dataPos = (dataPos == 0) ? 54 : dataPos;

    string imgstr;
    imgstr.reserve(fileSize-54);
    imgstr.assign(filestr.begin()+54, filestr.begin()+imageSize);
    file.close();
    width = imageWidth;
    height = imageHeight;
    return imgstr;
}

void _png_read_data(png_structp, png_bytep, png_size_t);
string OpenPNGFile(const string filename, unsigned int &width, unsigned int &height) {
    std::ifstream file(filename.c_str(), std::ios::binary | std::ios::in);
    if(!file.is_open()) {
        cerr << "ERROR: Could not open PNG file: " << filename << endl;
        return 0;
    }

    const int PNGSIGSIZE = 8;
    {   // Validate png file
        std::cout << "PNG: Validating" << std::endl;
        png_byte pngsig[PNGSIGSIZE];
        int is_png = 0;
        file.read((char*)pngsig, PNGSIGSIZE);
        if(!file.good()) return 0;
        is_png = png_sig_cmp(pngsig, 0, PNGSIGSIZE);
        if(is_png != 0) {
            cerr << "ERROR: Data isn't valid png" << endl;
            return 0;
        }
    }

    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;

    std::cout << "PNG: Creating pointers" << std::endl;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) {
        cerr << "ERROR: PNG ptr was NULL" << endl;
        file.close();
        return 0;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        cerr << "ERROR: PNG info ptr was NULL" << endl;
        png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0); 
        file.close();
        return 0;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
        cerr<< "ERROR: PNG reading error" << endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
        file.close();
        return 0;        
    }

    std::cout << "PNG: setting settings" << std::endl;
    png_set_read_fn(png_ptr, (png_voidp)&file, _png_read_data);

    png_set_sig_bytes(png_ptr, PNGSIGSIZE);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 img_w = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 img_h = png_get_image_height(png_ptr, info_ptr);
    png_uint_32 bitdepth = png_get_bit_depth(png_ptr, info_ptr);
    png_uint_32 channels = png_get_channels(png_ptr, info_ptr);
    png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);
    png_uint_32 row_bytes = png_get_rowbytes(png_ptr, info_ptr);

    switch(color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_ptr);
            channels = 3;
            break;
        case PNG_COLOR_TYPE_GRAY:
            if(bitdepth < 8) {
                png_set_expand_gray_1_2_4_to_8(png_ptr);
            }
            bitdepth = 8;
            break;
    }

    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
        channels += 1;
    }

    if(bitdepth == 16) {
        png_set_strip_16(png_ptr);
    }


    std::cout << "PNG: Reading data" << std::endl;

    string data_s;
    data_s.resize(img_w * img_h * 4, '0');
/*    const unsigned int stride  = img_w * bitdepth * channels / 8;

    for(size_t i = 0; i < img_h; i++) {
        png_uint_32 q = (img_h - i - 1) * stride;
        row_ptrs[i] = (png_bytep)data + q;
    }
*/
    png_bytepp row_pointers = new png_bytep[img_h];
    for(int i = 0; i < img_h; i++) {
        row_pointers[i] = new png_byte[img_w * channels];
    }
    png_read_image(png_ptr, row_pointers);

    for(int y = 0; y < img_h; y++) {
        png_bytep row = row_pointers[y];
        for(int x = 0; x < img_w; x++) {
            data_s[y * img_w * 4 + x*4 + 0] = row[x * channels + 0];
            data_s[y * img_w * 4 + x*4 + 1] = row[x * channels + 1];
            data_s[y * img_w * 4 + x*4 + 2] = row[x * channels + 2];

            data_s[y * img_w + x*4 + 3] = (channels == 4) ? row[x * channels + 3] : 255;
        }
    }
    for(int i = 0; i < img_h; i++) {
        delete[] row_pointers[i];
    }

    delete[] row_pointers;

    std::cout << "PNG: Returning data" << std::endl;
/*
    string data_s = data;
    std::cout << "PNG: Debug info" << std::endl;
    std::cout << "data size: " << img_w * img_h * bitdepth * channels / 8 << " data char 1: " << data[0];
    std::cout << ", data_s size: " << data_s.size() << "; data_s char 1: " << data_s[0] << std::endl;
*/
    width = img_w;
    height = img_h;
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
//    delete[] data;
    file.close();
    return data_s;
}

void serrx(string message) {
    cerr << "ERROR: " << message << endl;
    exit(EXIT_FAILURE);
}

void errx(string message) {
    const GLenum Error = glGetError();
    if(Error != GL_NO_ERROR) {
        std::stringstream messagestr;
        messagestr << "Could not " << message << ": " << Error << " - " << gluErrorString(Error);
        //std::cout << GL_INVALID_VALUE << ", " << GL_INVALID_OPERATION << std::endl;
        serrx(messagestr.str());
    }
}

void _png_read_data(png_structp pngPtr, png_bytep data, png_size_t length) {
    png_voidp a = png_get_io_ptr(pngPtr);
    ((std::istream*)a)->read((char*)data, length);
}
