#include "io.h"

std::string pa::io::readFile(const char* filepath) noexcept {
        FILE* file = fopen(filepath, "rb");
        if (file == nullptr) {
                printf("Failed to open the file: %s\n", filepath);
                return "";
        }
        
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        std::string ret;
        ret.resize_and_overwrite(file_size, [&](char* buf, size_t n) {
                return fread(buf, 1, n, file);
        });
        
        if (ferror(file)) {
                printf("Failed to read the file.\n");
                fclose(file);
                return "";
        }
        
        fclose(file);
        return ret;
}
