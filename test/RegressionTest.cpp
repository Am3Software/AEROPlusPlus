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
#include "EnumMASS.h"
#include "ConvMass.h"
#include "SforzaSweepConversion.h"
#include "ODE45.h"
#include "PLOT.h"

using namespace std;

int main()
{

    //============================= Test Interpolant Class ============================
    vector<double> weightPayload = {350, 1633, 1701, 350, 204, 350, 1360, 488};
    vector<double> bodyLengthToFusealgeOverallLength = {0.516417910447761, 0.242246642246642, 0.294835868694956, 0.506867469879518, 0.488485327016713, 0.519072733962958, 0.237324464153732, 0.439407598197038};
    double YValue = 0.0;
    vector<double> xFromExcel;
    vector<double> yFromExcel;
    RegressionMethod methodLabelFromExcel;
    string typeOfRegressionFromExcel;
    int degreeOfPolynomialFromExcel;

    string xLabelFromExcel;
    string yLabelFromExcel;
    string xUnitFromExcel;
    string yUnitFromExcel;
    string enableChartFromExcel;

    double inputKnownSweep = 35.7;
    double taperRatio = 0.28;
    double aspectRatio = 7.5;
    double knwonSweepInPercentofTheChord = 0.0;
    double unknownSweepInPercentofTheChord = 0.25;
    double outputSweep = 0.0;

    //============================= Sforza Sweep Conversion Test ==============================

    ConvSweep convSweep(inputKnownSweep, taperRatio, aspectRatio, knwonSweepInPercentofTheChord, unknownSweepInPercentofTheChord);

    outputSweep = convSweep.getSweepAngle();

    //============================= Excel Reader Test ==============================

    ExcelReader excelReader("ExcelFiles", "Test.xlsx");
    

    excelReader.getData("FUSELAGE", 3, 1, 2, 3);

    // ConvMass convMass(Mass::KG, Mass::SLUG, excelReader.getXDataFromExcel());

    xFromExcel = excelReader.getXDataFromExcel();
    yFromExcel = excelReader.getYDataFromExcel();
    xUnitFromExcel = excelReader.getUnitsOfXY()[0];
    yUnitFromExcel = excelReader.getUnitsOfXY()[1];

    xLabelFromExcel = excelReader.getXLabelFromExcel();
    yLabelFromExcel = excelReader.getYLabelFromExcel();
    methodLabelFromExcel = excelReader.getMethodOfRegressionFromExcel();
    degreeOfPolynomialFromExcel = excelReader.getDegreeOfPolynomialFromExcel();

    enableChartFromExcel = excelReader.getFlagToEnableChartFromExcel();
    //============================= Interpolant Test ==============================
    // RegressionMethod method = RegressionMethod::LINEAR;

    Interpolant interp1(xFromExcel, yFromExcel, degreeOfPolynomialFromExcel, methodLabelFromExcel);

    vector<double> coeffs = interp1.getCoefficients();

    if (methodLabelFromExcel != RegressionMethod::CONSTANT)
    {
        cout << "Coefficients retrieved in main: " << endl;

        for (int i = 0; i < coeffs.size(); i++)
        {
            cout << coeffs[i] << endl;
        }
    }

    YValue = interp1.getYValueFromRegression(1701.0);

    cout << "The Y value retrieved in main is: " << YValue << endl;

    interp1.getChartOfRegression(xLabelFromExcel, yLabelFromExcel, xUnitFromExcel, yUnitFromExcel,enableChartFromExcel,"TestAircraft");


    //============================= ODE45 + Plot Test ==============================

    // Esempio con lambda function, ovvero funzione anonima
    // NOTA: [RC, v_tension] cattura le variabili esterne per valore.
    // Le lambda non vedono automaticamente le variabili dello scope esterno,
    // quindi dobbiamo esplicitamente "catturarle" tra le parentesi quadre.
    // Opzioni: [] = nulla, [=] = tutto per valore, [&] = tutto per riferimento,
    // [var] = specifica variabile per valore, [&var] = per riferimento.

    // double VA = 10.0; // Constraint shear force in kN
    // double q = 10.0; // Distributed load in kN/m
    // double x0 = 0.0; // Initial length
    // double xf = 1.0; // Final length
    // double y0 = -5.0; // Bending moment at root
    // double dx = 1.0 / 99.0; // Step size
    
    
    // ODE45 odeSolver45([VA,q](double x, double y){return VA-q*x;},
    // x0, 
    // xf, 
    // y0, 
    // dx);

    // odeSolver45.getT();
    // odeSolver45.getY();

    // Plot plot(odeSolver45.getT(), odeSolver45.getY(), "Length(m)", 
    // "Bending Moment(KNm)", 
    // "ODE45 - Solver",
    // "Bending Moment",
    // "lines","1","1","","","#FF5733");


      //============================= NEW AIRCRAFT DATA TEST ============================
    
    cout << "\n========== AIRCRAFT DATA TEST ==========\n" << endl;
    
    ExcelReader reader("ExcelFiles", "UNINA_UAV_Statistics_Input.xlsx");
    AERO::AircraftData aircraft = reader.readAllAircraftData("DroneTest");
    
    // Test FUSELAGE
    cout << "\n=== FUSELAGE ===\n" << endl;
    const AERO::ComponentData& fus = aircraft.fuselage;
    cout << "Number of variables: " << fus.getNumVariables() << endl;
    
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