#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

class SaveFiles {

private:
    std::vector<std::string> fileNames;
    std::filesystem::path baseDirectory = std::filesystem::current_path();

public:

    SaveFiles() {
    };

    /// @brief Saves files that start with the specified aircraft name into a designated folder.
    /// @param nameOfFolder Name of the folder where files will be saved.
    /// @param nameOfAircraft Name of the aircraft to filter files.
    void saveFiles(std::string nameOfFolder, std::string nameOfAircraft) {

        std::filesystem::path saveDirectory = baseDirectory / nameOfFolder;

        if (!std::filesystem::exists(saveDirectory)) {
            std::filesystem::create_directories(saveDirectory);
        }

        // Search all files in the current directory
        for (const auto& entry : std::filesystem::directory_iterator(baseDirectory)) {
            
            // Check if it is a file (not a folder)
            if (entry.is_regular_file()) {
                
                std::string filename = entry.path().filename().string();
                
                // Check if the file name starts with nameOfAircraft and avoid XML files
                if (filename.find(nameOfAircraft) == 0 && entry.path().extension() != ".xml") {

                    // Copy the file to the saveDirectory
                    std::filesystem::path destination = saveDirectory / filename;
                    std::filesystem::copy_file(entry.path(), destination, 
                                              std::filesystem::copy_options::overwrite_existing);
                    
                    // Save the file name in the vector
                    fileNames.push_back(filename);
                    
                    std::cout << "File saved: " << filename << std::endl;

                    // Remove original file only after successful copy
                    try
                    {
                        std::filesystem::remove(entry.path());
                    }
                    catch (const std::filesystem::filesystem_error &e)
                    {
                        std::cerr << "Failed to remove file: " << filename << " - " << e.what() << std::endl;
                    }
                }
            }
        }
        
        std::cout << "Total files saved: " << fileNames.size() << std::endl;
    }

    // Method to get the names of saved files
    /// @brief Returns the names of the saved files.
    /// @return A constant reference to a vector of strings containing the file names.
    const std::vector<std::string>& getFileNames() const {
        return fileNames;
    }
};