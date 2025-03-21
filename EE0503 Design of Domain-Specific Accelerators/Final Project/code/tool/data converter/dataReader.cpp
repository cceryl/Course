// Read the datasets and convert to C arrays

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#define MEAN 0.1307
#define STD  0.3081

void convert_image()
{
    std::ifstream file("./data/t10k-images-idx3-ubyte", std::ios::binary);
    if (!file)
    {
        std::cerr << "Error: Could not open file" << std::endl;
        return;
    }

    std::ofstream out_file("./image.c", std::ios::out);

    // Read the header
    uint32_t magic_number, num_images, num_rows, num_cols;
    file.read(reinterpret_cast<char *>(&magic_number), sizeof(uint32_t));
    file.read(reinterpret_cast<char *>(&num_images), sizeof(uint32_t));
    file.read(reinterpret_cast<char *>(&num_rows), sizeof(uint32_t));
    file.read(reinterpret_cast<char *>(&num_cols), sizeof(uint32_t));
    magic_number = __builtin_bswap32(magic_number);
    num_images   = __builtin_bswap32(num_images);
    num_rows     = __builtin_bswap32(num_rows);
    num_cols     = __builtin_bswap32(num_cols);

    // Read the image data
    std::vector<uint8_t> buffer(num_images * num_rows * num_cols);
    file.read(reinterpret_cast<char *>(buffer.data()), num_images * num_rows * num_cols);

    // Convert header to variables
    out_file << "#include <stdint.h>\n";
    out_file << "\n";
    out_file << "#define NUM_ROWS 28\n";
    out_file << "#define NUM_COLS 28\n";
    out_file << "#define NUM_IMAGES " << num_images << "\n";
    out_file << "\n";

    // Convert data to array
    out_file << "const int image_data[NUM_IMAGES * NUM_ROWS * NUM_COLS] = {\n";
    for (size_t i = 0; i < num_images * num_rows * num_cols; ++i)
    {
        float trans1 = (static_cast<float>(buffer[i]) / 255.0 - MEAN) / STD;
        int   trans2 = trans1 * (1 << 16);
        out_file << trans2;
        if (i != num_images * num_rows * num_cols - 1)
            out_file << ", ";
    }
    out_file << "};\n";
}

void convert_label()
{
    std::ifstream file("./data/t10k-labels-idx1-ubyte", std::ios::binary);
    if (!file)
    {
        std::cerr << "Error: Could not open file" << std::endl;
        return;
    }

    std::ofstream out_file("./label.c", std::ios::out);

    // Read the header
    uint32_t magic_number, num_items;
    file.read(reinterpret_cast<char *>(&magic_number), sizeof(uint32_t));
    file.read(reinterpret_cast<char *>(&num_items), sizeof(uint32_t));
    magic_number = __builtin_bswap32(magic_number);
    num_items    = __builtin_bswap32(num_items);

    // Read the label data
    std::vector<uint8_t> buffer(num_items);
    file.read(reinterpret_cast<char *>(buffer.data()), num_items);

    // Convert header to variables
    out_file << "#include <stdint.h>\n";
    out_file << "\n";
    out_file << "#define NUM_LABELS " << num_items << "\n";
    out_file << "\n";

    // Convert data to array
    out_file << "const uint8_t label_data[NUM_LABELS] = {\n";
    for (size_t i = 0; i < num_items; ++i)
    {
        out_file << static_cast<int>(static_cast<uint8_t>(buffer[i]));
        if (i != num_items - 1)
            out_file << ", ";
    }
    out_file << "};\n";
}

int main()
{
    convert_image();
    convert_label();
    return 0;
}