#ifndef NACA_H
#define NACA_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

class VelocityToImport
{

private:
    vector<double> x_over_C; // Posizione lungo la corda (non usata ma letta dal file)
    vector<double> V_v;      // v/V del profilo simmetrico
    vector<double> deltaVa;  // deltaVa/V del profilo simmetrico
    vector<double> deltaV_v; // deltaV/V della linea media

public:

    // Costruttore che accetta il percorso del file di testo da importare
    VelocityToImport(string pathTofileTxt)
    {
        importData(pathTofileTxt);
    }

    // Metodo che serve ad importare i dati - La funzione che fa l'import in pratica

    void importData(string pathTofileTxt)
    {

        ifstream file(pathTofileTxt);
        if (!file.is_open())
        {
            cerr << "Errore nell'apertura del file: " << pathTofileTxt << endl;
            return;
        }

        string line;
        bool firstLine = true;

        while (getline(file, line))
        {
            if (firstLine) { // Salto la riga d'intestazione

                firstLine = false; 
                continue; 
            
            }

            istringstream iss(line);
            double xC, v_v, dVa, dV_v;

            // Legge i valori dalla linea
            // Se riesce → restituisce true
            // Se fallisce (es. trova una stringa) → restituisce false
            // Il ! inverte il valore:
            //
            //! true = false → NON entra nell'if (tutto ok)
            //! false = true → ENTRA nell'if (c'è un errore)
            // Esempio pratico:
            if (!(iss >> xC >> v_v >> dVa >> dV_v))
            {
                    cerr << "Errore nella lettura della linea: " << line << endl;
                    continue;
                
            }
            x_over_C.push_back(xC);
            V_v.push_back(v_v); // Inserisce i valori spingendoli indietro a mano a mano che li legge. Prima costruisce una copia temporanea e poi la inserisce nel vettore
            deltaVa.push_back(dVa);
            deltaV_v.push_back(dV_v);
            
        }

        file.close();
    }

    // Getter per accedere ai dati
    // Restituisce un riferimento costante al vettore x_over_C, V_v, deltaVa, deltaV_v
    // const vector<double>& = ritorna un riferimento (nessuna copia) ma il chiamante non può modificarlo
    // const finale = questo metodo non modifica l'oggetto (può essere chiamato su oggetti const)


    const vector<double> &getX_over_C() const
    {
        return x_over_C;
    }
    
    const vector<double> &getV_v() const
    {
        return V_v;
    }
    const vector<double> &getDeltaVa() const
    {
        return deltaVa;
    }
    const vector<double> &getDeltaV_v() const
    {
        return deltaV_v;
    }

    // Getter per la dimensione del vettore (numero di righe lette dal file)
    // size_t = tipo unsigned per rappresentare dimensioni (sempre >= 0)
    // Tutti i vettori hanno la stessa dimensione, quindi basta controllarne uno
    size_t getSize() const { 
        return V_v.size(); 
    }
};

#endif // NACA_H