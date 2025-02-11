#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace IE::Core {
class File {
public:
    std::string           name;
    std::filesystem::path path;
    std::fstream          fileIO;
    std::streamsize       size;
    std::string           extension;

    // Constructor
    explicit File(const std::filesystem::path &filePath);

    // Copy constructor and = overload must be defined explicitly because fstream has no defaults for them
    File(const File &file);

    File &operator=(const File &file);

    // Read the entire file
    std::vector<char> read();

    // Read data from the file
    std::vector<char> read(std::streamsize numBytes, std::streamsize startPosition);

    // Write to a file. Clears any old data.
    void write(const std::vector<char> &data);

    // Overwrite a section of the file
    void overwrite(const std::vector<char> &data, std::streamsize startPosition = -1);

private:
    std::streamsize getSize();

    // Open a file for reading and writing
    void open(std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary);

    // Close the file
    void close();
};
}  // namespace IE::Core