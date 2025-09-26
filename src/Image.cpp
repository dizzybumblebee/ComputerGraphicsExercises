#include "Image.h"

bool Image::write_tga(const std::filesystem::path &_filename)
{
    std::ofstream file(_filename, std::fstream::binary);
    if (!file) {
        std::cerr << "ERROR: Failed to open " << _filename.string() << " for writing." << std::endl;
        return false;
    }

    file.put(0); //id length
    file.put(0); //no color map
    file.put(2); //uncompressed image
    file.put(0); //offset color map table
    file.put(0); //
    file.put(0); //number of entries
    file.put(0); //
    file.put(0); //bits per pixel
    file.put(0); //abs coordinate lower left display in x direction
    file.put(0); //
    file.put(0); //abs coordinate lower left display in y direction
    file.put(0); //
    file.put((width_  & 0x00FF)); //width in pixels
    file.put((width_  & 0xFF00)/ 256);
    file.put((height_ & 0x00FF)); //height in pixels
    file.put((height_ & 0xFF00)/ 256);
    file.put(24); //bits per pixel
    file.put(0); //image descriptor

    for (vec3 color: pixels_)
    {
        file.put(static_cast<unsigned char>(255.0 * color[2]));
        file.put(static_cast<unsigned char>(255.0 * color[1]));
        file.put(static_cast<unsigned char>(255.0 * color[0]));
    }

    file.close();
    return true;
}
bool Image::write_bmp(const std::filesystem::path& _filename)
{
    std::ofstream file(_filename, std::fstream::binary);
    if (!file) {
        std::cerr << "ERROR: Failed to open " << _filename.string() << " for writing." << std::endl;
        return false;
    }

    // Helper lambdas for little-endian writing
    auto write16le = [&file](uint16_t v) {
        file.put(static_cast<char>(v & 0xFF));
        file.put(static_cast<char>((v >> 8) & 0xFF));
    };
    auto write32le = [&file](uint32_t v) {
        file.put(static_cast<char>(v & 0xFF));
        file.put(static_cast<char>((v >> 8) & 0xFF));
        file.put(static_cast<char>((v >> 16) & 0xFF));
        file.put(static_cast<char>((v >> 24) & 0xFF));
    };

    // BMP file header (14 bytes)
    unsigned int row_stride = width_ * 3;
    unsigned int padding = (4 - (row_stride % 4)) % 4;
    unsigned int row_size_padded = row_stride + padding;
    unsigned int pixel_data_size = row_size_padded * height_;
    unsigned int file_size = 14 + 40 + pixel_data_size;

    // BITMAPFILEHEADER
    file.put('B');
    file.put('M');
    write32le(file_size); // file size
    write32le(0); // reserved
    write32le(14 + 40); // offset to pixel data

    // BITMAPINFOHEADER (40 bytes)
    write32le(40); // header size
    write32le(static_cast<unsigned int>(width_)); // width
    write32le(static_cast<unsigned int>(height_)); // height
    write16le(1); // planes
    write16le(24); // bits per pixel
    write32le(0); // compression
    write32le(pixel_data_size); // image size
    write32le(2835); // x pixels per meter
    write32le(2835); // y pixels per meter
    write32le(0); // colors used
    write32le(0); // important colors

    /// map double in range [0..1] to byte in range [0..255]
    auto double_to_byte = [](double v){
        return static_cast<unsigned char>(255.0 * std::clamp(v, 0.0, 1.0));
    };
    // Write pixel data (bottom-up, BGR, padded)
    std::vector<unsigned char> row(row_size_padded);
    // BMPs are written bottom-row first, but that's exactly how
    // our image is stored (compare Camera::primary_ray()),
    // so we can iterate starting at y=0 (bottom).
    for (unsigned int y = 0; y < height_; ++y) {
        for (unsigned int x = 0; x < width_; ++x) {
            const vec3& color = (*this)(x, y);
            row[x * 3 + 0] = double_to_byte(color[2]); // Blue
            row[x * 3 + 1] = double_to_byte(color[1]); // Green
            row[x * 3 + 2] = double_to_byte(color[0]); // Red
        }
        // Zero padding
        for (unsigned int p = 0; p < padding; ++p) row[row_stride + p] = 0;
        file.write(reinterpret_cast<const char*>(row.data()), row_size_padded);
    }
    file.close();
    return true;
}
