#pragma once

#include "IEFile.hpp"
#include "IETempFile.hpp"
#include "IEImporter.hpp"

#include <vector>
#include <string>
#include <unordered_map>

class IEFileSystem {
public:
    std::filesystem::path path;
    IEImporter importer;

    explicit IEFileSystem(const std::filesystem::path& fileSystemPath);

    // Create a new IEFile with the given relative path
    void addFile(const std::string& filePath);

    void createFolder(const std::string& folderPath) const;

    // Export data to an IEFile
    void exportData(const std::string& filePath, const std::vector<char> &data);

    // Delete a file
    void deleteFile(const std::string& filePath);

    // Delete a directory
    void deleteDirectory(const std::string& filePath) const;

    // Delete a directory that has other files in it
    void deleteUsedDirectory(const std::string& filePath);

    // Import a file
    std::string importFile(const std::string& filePath);

private:
    std::unordered_map<std::string, IEFile> files;
    std::unordered_map<std::string, IETempFile> tempFiles;
};