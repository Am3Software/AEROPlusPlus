#ifndef AIRCRAFTDATA_H
#define AIRCRAFTDATA_H

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "RegressionMethod.h"
#include "Interpolant.h"

namespace AERO {

// Struttura che rappresenta una variabile interpolabile
struct InterpolableVariable {
    std::string varName;              // Nome della variabile, es: "VAR 1"
    std::string xLabel;               // Etichetta dell'asse X, es: "weightPayload"
    std::string yLabel;               // Etichetta dell'asse Y, es: "bodyLengthToFuselageOverallLength"
    std::string xUnit;                // Unità di misura X, es: "kg"
    std::string yUnit;                // Unità di misura Y, es: "adim", "m", ecc.
    RegressionMethod method;          // Metodo di regressione/interpolazione (POLYNOMIAL, LINEAR, ecc.)
    int polynomialDegree;             // Grado del polinomio per la regressione
    
    std::vector<double> xData;        // Dati X già convertiti
    std::vector<double> yData;        // Dati Y già convertiti
    std::vector<double> coefficients; // Coefficienti dell'interpolazione
    
    // Verifica se la variabile è valida (dati X e Y non vuoti e di uguale dimensione)
    bool isValid() const {
        return !xData.empty() && xData.size() == yData.size();
    }
    
    // Restituisce il numero di punti dati disponibili
    int getNumDataPoints() const {
        return xData.size();
    }
    
    // Metodo per interpolare un nuovo valore dato xValue
    double interpolate(double xValue) const {

        // Crea un oggetto Interpolant con i dati e le impostazioni correnti - L'header sarebbe Interpolant.h che contiene già tutto
        Interpolant interp(xData, yData, polynomialDegree, method);
        return interp.getYValueFromRegression(xValue);
    }
};

// Struttura che rappresenta i dati di un componente dell'aeromobile
// Ogni oggetto ComponentData contiene:
// - Il nome del componente (es: "FUSELAGE", "WING", ecc.)
// - Una mappa 'variables' che associa il nome di una variabile (stringa) a un oggetto InterpolableVariable.
//   Questo permette di gestire dinamicamente più variabili per ciascun componente e di accedervi tramite il loro nome.
//   La mappa consente di aggiungere, cercare e interpolare variabili specifiche di ogni componente.
struct ComponentData {
    std::string componentName;        // Nome del componente, es: "FUSELAGE", "WING", ecc.
    // La mappa 'variables' associa il nome della variabile (stringa) a un oggetto InterpolableVariable.
    // Per costruirla, si inseriscono coppie <nome_variabile, InterpolableVariable>:
    // Esempio:
    // variables["VAR 1"] = InterpolableVariable{...};
    // variables["VAR 2"] = InterpolableVariable{...};
    std::map<std::string, InterpolableVariable> variables; // Mappa delle variabili interpolabili
    
    // Restituisce una variabile dato il suo nome (const)
    const InterpolableVariable& getVariable(const std::string& varName) const {
        return variables.at(varName);
    }
    
    // Verifica se una variabile esiste
    // Questo metodo controlla se nella mappa 'variables' è presente una variabile con nome 'varName'.
    // Restituisce true se la variabile esiste, false altrimenti.
    // Spiegazione: variables.find(varName) cerca la chiave nella mappa.
    // Se la trova, restituisce un iteratore all'elemento trovato.
    // Se NON la trova, restituisce variables.end(), cioè un iteratore che rappresenta "un elemento oltre l'ultimo".
    // Quindi, se il risultato è diverso da end(), la variabile esiste; altrimenti non esiste.
    bool hasVariable(const std::string& varName) const {
        return variables.find(varName) != variables.end();
    }
    
    // Restituisce la lista dei nomi delle variabili disponibili
    std::vector<std::string> getAvailableVariables() const {
        std::vector<std::string> varNames;
        // 'auto' qui deduce automaticamente il tipo della variabile 'pair' come std::pair<const std::string, InterpolableVariable>
        for (const auto& pair : variables) {
            varNames.push_back(pair.first);
        }
        return varNames;
    }
    
    // Restituisce il numero di variabili disponibili
    int getNumVariables() const {
        return variables.size();
    }
    
    // Interpola una variabile specifica dato il nome e il valore X
    double interpolate(const std::string& varName, double xValue) const {
        return getVariable(varName).interpolate(xValue);
    }

    // Genera e mostra il grafico della regressione per una variabile specifica
    void getChartOfVariableRegression(const std::string& varName, std::string enableChart, std::string aircraftName) const {
        const InterpolableVariable& var = getVariable(varName);
        Interpolant interp(var.xData, var.yData, var.polynomialDegree, var.method);
        interp.getChartOfRegression(var.xLabel, var.yLabel, var.xUnit, var.yUnit, enableChart,aircraftName);
    }
};

// Struttura che rappresenta i dati di un intero aeromobile
struct AircraftData {
    ComponentData fuselage;       // Dati della fusoliera
    ComponentData wing;           // Dati dell'ala
    ComponentData horizontal;     // Dati dell'impennaggio orizzontale
    ComponentData vertical;       // Dati dell'impennaggio verticale
    ComponentData undercarriage;  // Dati del carrello
    ComponentData engine;         // Dati del motore
    
    std::string aircraftName;     // Nome dell'aeromobile
    
    // Restituisce un riferimento al componente richiesto (non const)
    // Permette di modificare i dati del componente - se dovesse servire
    ComponentData& getComponent(const std::string& componentName) {
        if (componentName == "FUSELAGE") return fuselage;
        if (componentName == "WING") return wing;
        if (componentName == "HORIZONTAL_TAIL") return horizontal;
        if (componentName == "VERTICAL_TAIL") return vertical;
        if (componentName == "UNDERCARRIAGE") return undercarriage;
        if (componentName == "ENGINES") return engine;
        throw std::runtime_error("Unknown component: " + componentName);
    }
    
    // Restituisce un riferimento const al componente richiesto
    const ComponentData& getComponent(const std::string& componentName) const {
        if (componentName == "FUSELAGE") return fuselage;
        if (componentName == "WING") return wing;
        if (componentName == "HORIZONTAL_TAIL") return horizontal;
        if (componentName == "VERTICAL_TAIL") return vertical;
        if (componentName == "UNDERCARRIAGE") return undercarriage;
        if (componentName == "ENGINES") return engine;
        throw std::runtime_error("Unknown component: " + componentName);
    }

    void setAircraftName(const std::string& name) {
        aircraftName = name;
    }
};

} // namespace AERO

#endif // AIRCRAFTDATA_H