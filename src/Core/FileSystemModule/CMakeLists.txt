set(IEFileSystemModuleSourceFiles  # Gather sources
        File.cpp
        FileSystem.cpp
        Importer.cpp
        )

# Create and define properties for the library
add_library(IEFileSystemModule ${IEFileSystemModuleSourceFiles})
target_link_libraries(IEFileSystemModule PUBLIC INT_src EXT_AssImp)
set_target_properties(IEFileSystemModule PROPERTIES LINKER_LANGUAGE CXX)