#pragma once


#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <filesystem>

class CONVERTVSPFILE {

private:

std::string inputfile;
std::string outputfile;



public:


CONVERTVSPFILE () {

    this->inputfile = "";
    this->outputfile = "";


}

void convertToExcelFile (std::string inputfile, std::string outputfile) {
    auto oldPath = std::filesystem::current_path() / inputfile;
    auto newPath = std::filesystem::current_path() / outputfile;

    if (!std::filesystem::exists(oldPath)) {
        throw std::runtime_error("Input file does not exist: " + oldPath.string());
    }

    // ATTENZIONE: questa funzione NON converte il formato in vero .xlsx!
    // Copia solo il file e lo rinomina con estensione .xlsx.
    // Excel proverrà ad aprirlo, ma se non è un file tabellare (CSV, TSV) non sarà leggibile come tabella.
    std::filesystem::copy_file(oldPath, newPath, std::filesystem::copy_options::overwrite_existing);
}

};