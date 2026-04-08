#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace VSPPolar
{

    struct PolarPoint
    {

        double Beta;
        double Mach;
        double AoA;
        double Re_1e6;
        double CL;
        double CDo;
        double CDi;
        double CDtot;
        double CDt;
        double CDtot_t;
        double CS;
        double L_D;
        double E;
        double CFx;
        double CFy;
        double CFz;
        double CMx;
        double CMy;
        double CMz;
        double CMl;
        double CMm;
        double CMn;
        double FOpt;

        /**
         * @brief Default constructor.
         */
        PolarPoint() = default;

        /**
         * @brief Constructs a polar data point from all supported columns.
         * @param beta Sideslip angle value.
         * @param mach Mach number value.
         * @param aoa Angle of attack value.
         * @param re_1e6 Reynolds number in millions.
         * @param cl Lift coefficient.
         * @param cdo Zero-lift drag coefficient.
         * @param cdi Induced drag coefficient.
         * @param cdtot Total drag coefficient.
         * @param cdt Additional drag coefficient term.
         * @param cdtot_t Total drag coefficient variant.
         * @param cs Side-force coefficient.
         * @param l_d Lift-to-drag ratio.
         * @param e Efficiency value.
         * @param cfx Force coefficient along X.
         * @param cfy Force coefficient along Y.
         * @param cfz Force coefficient along Z.
         * @param cmx Moment coefficient around X.
         * @param cmy Moment coefficient around Y.
         * @param cmz Moment coefficient around Z.
         * @param cml Rolling moment coefficient.
         * @param cmm Pitching moment coefficient.
         * @param cmn Yawing moment coefficient.
         * @param fopt Optimization-related output value.
         */
        PolarPoint(double beta, double mach, double aoa, double re_1e6,
                   double cl, double cdo, double cdi, double cdtot,
                   double cdt, double cdtot_t, double cs, double l_d,
                   double e, double cfx, double cfy, double cfz,
                   double cmx, double cmy, double cmz,
                   double cml, double cmm, double cmn,
                   double fopt)
            : Beta(beta), Mach(mach), AoA(aoa), Re_1e6(re_1e6),
              CL(cl), CDo(cdo), CDi(cdi), CDtot(cdtot),
              CDt(cdt), CDtot_t(cdtot_t), CS(cs), L_D(l_d),
              E(e), CFx(cfx), CFy(cfy), CFz(cfz),
              CMx(cmx), CMy(cmy), CMz(cmz),
              CMl(cml), CMm(cmm), CMn(cmn),
              FOpt(fopt)
        {
        }
    };

    class PolarReader
    {

    private:
        std::vector<PolarPoint> data;
        std::vector<std::string> headers;

    public:
        /**
         * @brief Reads and parses a polar file.
         * @param filename Path to the input file.
         * @return true when parsing completes.
         * @throws std::runtime_error If the file cannot be opened, is empty, or has an unexpected header size.
         */
        bool readFile(const std::string &filename);

        /**
         * @brief Returns parsed polar points.
         * @return Constant reference to parsed data.
         */
        const std::vector<PolarPoint> &getData() const
        {
            return data;
        }

        /**
         * @brief Returns parsed header labels.
         * @return Constant reference to header names.
         */
        const std::vector<std::string> &getHeaders() const
        {
            return headers;
        }
    };

    bool PolarReader::readFile(const std::string &filename)
    {

        std::ifstream file(filename);

        if (!file.is_open())
        {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::string line;

        if (!std::getline(file, line))
        {
            throw std::runtime_error("File is empty: " + filename);
        }

        // Separiamo gli header dalle colonne
        std::istringstream headerStream(line);
        std::string header;

        // Ciclo che legge ogni parola dalla prima riga (header) e la aggiunge al vettore headers
        while (headerStream >> header)
        {

            headers.push_back(header);
        }

        // Controllo che ci siano le 23 colonne per gli headers
        if (headers.size() != 23)
        {
            throw std::runtime_error("Unexpected number of columns in header: " + std::to_string(headers.size()));
        }

        // Leggiamo i dati riga per riga
        while (std::getline(file, line))
        {

            // Questo controllo serve a saltare tutte le righe vuote o composte solo da spazi, tabulazioni o caratteri di fine riga.
            // 'line.empty()' verifica se la riga è completamente vuota.
            // 'line.find_first_not_of(" \t\r\n") == std::string::npos' controlla se la riga contiene solo caratteri di spazio, tabulazione o newline.
            // La funzione find_first_not_of("caratteri_specificati") restituisce la posizione del primo carattere che NON è tra quelli specificati.
            // Se la funzione restituisce std::string::npos, significa che TUTTI i caratteri della riga sono tra quelli specificati
            // (spazi, tab, \r, \n), quindi la riga viene considerata "vuota" e saltata.
            // Se invece c'è almeno un carattere diverso, la riga viene considerata valida.
            if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos)
            {
                continue;
            }

            std::istringstream lineStream(line);

            PolarPoint point;

            // Leggi i 23 valori in ordine
            if (!(lineStream >> point.Beta >> point.Mach >>
                  point.AoA >> point.Re_1e6 >> point.CL >> point.CDo >>
                  point.CDi >> point.CDtot >>
                  point.CDt >> point.CDtot_t >>
                  point.CS >> point.L_D >> point.E >> point.CFx >> point.CFy >>
                  point.CFz >> point.CMx >>
                  point.CMy >> point.CMz >>
                  point.CMl >> point.CMm >>
                  point.CMn >> point.FOpt))
            {
                std::cerr << "Errore: lettura dati non valida alla riga" << std::endl;
                continue; // Salta questa riga
            }

            data.push_back(point);
        }

        file.close();
        return true;
    }

}