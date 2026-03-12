#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "Plotter3D.h"  // Per DegenSurf


// ─────────────────────────────────────────────────────────────────────────────
/// @brief Legge il file nomeAircraft_DegenGeom.csv generato da OpenVSP
///        e lo converte in un vettore di DegenSurf pronti per AircraftPlotter.
///
/// Formato del CSV VSP:
///   LIFTING_SURFACE,nome,...
///   # DegenGeom Type,nXsecs, nPnts/Xsec
///   SURFACE_NODE,61,41
///   # x,y,z,u,w                    ← header ESATTO da cercare
///   6.57, 0.00, 0.77, 1.00, 0.00   ← una riga = un punto (x,y,z,u,w)
///   ...                             ← 61*41 righe totali
///   # nx,ny,nz,area                 ← sezione successiva → stop lettura
///
/// Uso:
/// @code
///     DegenGeomReader reader("P2012_DegenGeom.csv");
///     auto components = reader.read();
///
///     AircraftPlotter plotter("P2012");
///     for (const auto& surf : components)
///         plotter.addComponent(surf);
///     plotter.show();
/// @endcode
// ─────────────────────────────────────────────────────────────────────────────
class DegenGeomReader
{
private:
    std::string filepath;

    static std::vector<std::string> splitCSV(const std::string& line)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            token.erase(std::remove(token.begin(), token.end(), '\r'), token.end());
            token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
            tokens.push_back(token);
        }
        return tokens;
    }

    static double toDouble(const std::string& s)
    {
        try { return std::stod(s); }
        catch (...) { return 0.0; }
    }

    static bool startsWith(const std::string& line, const std::string& prefix)
    {
        return line.rfind(prefix, 0) == 0;
    }

public:
    explicit DegenGeomReader(const std::string& csvPath) : filepath(csvPath) {}

    std::vector<DegenSurf> read() const
    {
        std::ifstream file(filepath);
        if (!file.is_open())
            throw std::runtime_error("Impossibile aprire il file: " + filepath);

        std::vector<DegenSurf> components;
        std::string line;

        DegenSurf currentSurf;
        bool inSurfaceNode = false;  // true SOLO dopo "SURFACE_NODE,..."
        bool inSurfaceData = false;  // true SOLO dopo "# x,y,z,u,w" dentro SURFACE_NODE
        int expectedPoints = 0;
        int readPoints     = 0;

        while (std::getline(file, line))
        {
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            if (line.empty()) continue;

            // ── Nuovo componente ─────────────────────────────────────────────
            if (startsWith(line, "LIFTING_SURFACE,") ||
                startsWith(line, "BODY,")            ||
                startsWith(line, "DISK,"))
            {
                // Salva il precedente se valido
                if (!currentSurf.name.empty() && !currentSurf.x.empty())
                    components.push_back(currentSurf);

                currentSurf    = DegenSurf();
                inSurfaceNode  = false;
                inSurfaceData  = false;
                expectedPoints = 0;
                readPoints     = 0;

                auto tokens = splitCSV(line);
                if (tokens.size() >= 2)
                    currentSurf.name = tokens[1];

                std::cout << "Componente: " << currentSurf.name << "\n";
                continue;
            }

            // ── SURFACE_NODE: memorizza dimensioni, attiva stato ─────────────
            if (startsWith(line, "SURFACE_NODE,"))
            {
                // Se abbiamo già letto una superficie per questo componente, skip
                // (ogni componente ha una sola SURFACE_NODE utile)
                if (currentSurf.x.empty())
                {
                    auto tokens = splitCSV(line);
                    if (tokens.size() >= 3)
                    {
                        currentSurf.nu = std::stoi(tokens[1]);
                        currentSurf.nw = std::stoi(tokens[2]);
                        expectedPoints = currentSurf.nu * currentSurf.nw;
                        currentSurf.x.reserve(expectedPoints);
                        currentSurf.y.reserve(expectedPoints);
                        currentSurf.z.reserve(expectedPoints);
                        std::cout << "  SURFACE_NODE " << currentSurf.nu
                                  << " x " << currentSurf.nw
                                  << " = " << expectedPoints << " punti\n";
                    }
                    inSurfaceNode = true;
                }
                inSurfaceData = false;
                continue;
            }

            // ── Header dati x,y,z,u,w: attiva lettura SOLO se in SURFACE_NODE ─
            // Cerchiamo ESATTAMENTE "# x,y,z,u,w" per non confonderci con
            // "# x,y,z,zCamber,t,..." della sezione PLATE
            if (line == "# x,y,z,u,w" && inSurfaceNode && currentSurf.x.empty())
            {
                inSurfaceData = true;
                readPoints    = 0;
                continue;
            }

            // ── Qualsiasi altra riga # → fine sezione dati ───────────────────
            if (line[0] == '#')
            {
                inSurfaceData = false;
                // NON resettare inSurfaceNode qui: potremmo essere tra header e dati
                continue;
            }

            // ── Sezione non-SURFACE_NODE (PLATE, STICK, ecc.) → disattiva ────
            if (startsWith(line, "PLATE,")    ||
                startsWith(line, "STICK_NODE,") ||
                startsWith(line, "POINT,"))
            {
                inSurfaceNode = false;
                inSurfaceData = false;
                continue;
            }

            // ── Leggi riga dati: x, y, z, u, w ──────────────────────────────
            if (inSurfaceData && readPoints < expectedPoints)
            {
                auto tokens = splitCSV(line);
                if (tokens.size() >= 3)
                {
                    currentSurf.x.push_back(toDouble(tokens[0]));
                    currentSurf.y.push_back(toDouble(tokens[1]));
                    currentSurf.z.push_back(toDouble(tokens[2]));
                    ++readPoints;

                    // Completato: disattiva lettura
                    if (readPoints >= expectedPoints)
                    {
                        inSurfaceData = false;
                        inSurfaceNode = false;
                    }
                }
            }
        }

        // Salva l'ultimo componente
        if (!currentSurf.name.empty() && !currentSurf.x.empty())
            components.push_back(currentSurf);

        std::cout << "\nTotale componenti letti: " << components.size() << "\n";
        for (const auto& c : components)
            std::cout << "  " << c.name
                      << "  [" << c.nu << "x" << c.nw << "]"
                      << "  punti: " << c.x.size()
                      << (c.x.size() == (size_t)(c.nu * c.nw) ? "  OK" : "  !! MISMATCH")
                      << "\n";

        return components;
    }
};