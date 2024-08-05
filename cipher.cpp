#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>


uint32_t hex_to_uint32(const std::string& hex) {
    uint32_t result;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> result;
    return result;
}


void expand_key(uint32_t kv, std::vector<uint8_t>& key) {
    key.resize(32);
    key[0] = (kv >> 24) - (kv >> 16) + (kv >> 8) + kv;
    for (size_t i = 1; i < key.size(); ++i) {
        key[i] = key[i - 1] + (kv >> 24) + (kv >> 16) - (kv >> 8) + kv;
    }
}


void xor_with_key(std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] ^= key[i];
    }
}


void rotate_key(std::vector<uint8_t>& key) {
    uint8_t last_byte = key.back();
    last_byte += 1; // Increment the last byte
    std::rotate(key.rbegin(), key.rbegin() + 1, key.rend());
    key[0] = last_byte;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file> <key (hex)>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    std::string key_hex = argv[3];

    if (key_hex.length() != 8) {
        std::cerr << "Key must be a 4-byte hex value." << std::endl;
        return 1;
    }

    uint32_t kv = hex_to_uint32(key_hex);
    std::vector<uint8_t> key;
    expand_key(kv, key);

    std::ifstream infile(input_file, std::ios::binary);
    if (!infile) {
        std::cerr << "Failed to open input file." << std::endl;
        return 1;
    }

    std::ofstream outfile(output_file, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to open output file." << std::endl;
        return 1;
    }

    std::vector<uint8_t> buffer(32);
    while (infile.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || infile.gcount()) {
        std::streamsize bytes_read = infile.gcount();
        buffer.resize(bytes_read);
        xor_with_key(buffer, key);
        outfile.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
        rotate_key(key);
        buffer.resize(32);
    }

    infile.close();
    outfile.close();

    return 0;
}
