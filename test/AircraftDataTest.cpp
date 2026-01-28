#define _HAS_STD_BYTE 0 // Risolve conflitto byte tra C++17 e Windows headers
#define UNICODE         // Assicura che Windows usi le versioni wide delle strutture
#define _UNICODE

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include "Interpolant.h"
#include "RegressionMethod.h"
#include "ExcelReader.h"
#include "ODE45.h"
#include "PLOT.h"

using namespace std;


int main() {
      //============================= NEW AIRCRAFT DATA TEST ============================
    
    cout << "\n========== AIRCRAFT DATA TEST ==========\n" << endl;
    
    ExcelReader reader("ExcelFiles", "UNINA_UAV_Statistics_Input.xlsx");
    AERO::AircraftData aircraft = reader.readAllAircraftData("DroneTest");
    
    // Test FUSELAGE
    cout << "=== FUSELAGE ===" << endl;
    const AERO::ComponentData& fus = aircraft.fuselage;
    cout << "Number of variables: " << fus.getNumVariables() << endl;
    
    // Ciclo su tutte le variabili disponibili della fusoliera e stampa le informazioni associate
    // Ciclo range-based for che itera su tutti i nomi delle variabili disponibili nella fusoliera.
    // Per ogni nome di variabile, recupera i dati associati e stampa le informazioni di regressione e unità.
    // La keyword 'auto' permette al compilatore di dedurre automaticamente il tipo di dato di varName.
    for (const auto& varName : fus.getAvailableVariables()) {
        const auto& var = fus.getVariable(varName);
        cout << varName << ": " 
             << var.xLabel << " [" << var.xUnit << "] -> " 
             << var.yLabel << " [" << var.yUnit << "]" << endl;
        cout << "  Method: ";
        if (var.method == RegressionMethod::LINEAR){
            cout << "LINEAR";
        }
        else if (var.method == RegressionMethod::POLYNOMIAL){
            cout << "POLYNOMIAL";
        }
        else if (var.method == RegressionMethod::EXPONENTIAL){
            cout << "EXPONENTIAL";
        }
        else if (var.method == RegressionMethod::POWER){
            cout << "POWER";
        }
        else if (var.method == RegressionMethod::LOGARITHMIC){
            cout << "LOGARITHMIC";
        }
        else if (var.method == RegressionMethod::CONSTANT){
            cout << "CONSTANT";
        }
        cout << ", Degree: " << var.polynomialDegree 
             << ", Points: " << var.xData.size() << endl;
    }
    
    // Test interpolazione VAR 1
    if (fus.hasVariable("VAR 1")) {
        double weightPayload = 1701.0;
        double result = fus.interpolate("VAR 1", weightPayload);
        cout << "\nInterpolation VAR 1 for " << weightPayload << " kg: " 
             << result << endl;
    }

    return 0;
}