#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include "EQUALSIGNORECASE.h"

class SaveFiles {

private:
    std::vector<std::string> fileNames;
    std::filesystem::path baseDirectory = std::filesystem::current_path();
    std::filesystem::file_time_type executionTimestamp;

  

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
                if (equalsIgnoreCase(filename.substr(0, nameOfAircraft.size()), nameOfAircraft) && entry.path().extension() != ".xml") {

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

        // Create temp folder and copy recently modified .vspscript files
        std::filesystem::path tempDirectory = baseDirectory / "temp";
        if (!std::filesystem::exists(tempDirectory))
        {
            std::filesystem::create_directories(tempDirectory);
        }

        for (const auto &vspEntry : std::filesystem::directory_iterator(baseDirectory))
        {
            if (vspEntry.path().extension() == ".vspscript")
            {
                auto lastWriteTime = std::filesystem::last_write_time(vspEntry);

                if (lastWriteTime >= executionTimestamp)
                {
                    // ←Rename the file with the aircraft name as prefix and copy to temp
                    std::string newFilename = nameOfAircraft + "_" + vspEntry.path().filename().string();
                    std::filesystem::path dest = tempDirectory / newFilename;

                    std::filesystem::copy_file(vspEntry.path(), dest,
                                               std::filesystem::copy_options::overwrite_existing);

                    std::cout << "Recent .vspscript copied to temp: " << newFilename << std::endl;

                    // ← rimuovi solo se la copia è andata a buon fine e il file esiste
                    try
                    {
                        if (std::filesystem::exists(dest))
                            std::filesystem::remove(vspEntry.path());
                    }
                    catch (const std::filesystem::filesystem_error &e)
                    {
                        std::cerr << "Failed to remove: "
                                  << vspEntry.path().filename().string()
                                  << " - " << e.what() << std::endl;
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

    /// @brief Captures the current timestamp to be used as reference for recent file detection.
    /// Call this immediately before the execution you want to monitor.
    void captureExecutionTimestamp()
    {
        executionTimestamp = std::filesystem::file_time_type::clock::now();
    }
};