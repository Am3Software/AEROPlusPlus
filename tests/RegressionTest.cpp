#include <iostream>
#include <vector>
#include "include/Interpolant.h"
#include "include/RegressionMethod.h"
#include "ExcelReader.h"
#include "ConvMass.h"
#include "SforzaSweepConversion.h"
#include "ODE45.h"
#include "PLOT.h"

void testInterpolant() {
    std::vector<double> xData = {1.0, 2.0, 3.0};
    std::vector<double> yData = {2.0, 3.0, 5.0};
    Interpolant interpolant(xData, yData, 2, RegressionMethod::POLYNOMIAL);
    
    std::vector<double> coeffs = interpolant.getCoefficients();
    std::cout << "Interpolant Coefficients: ";
    for (const auto& coeff : coeffs) {
        std::cout << coeff << " ";
    }
    std::cout << std::endl;

    double yValue = interpolant.getYValueFromRegression(2.5);
    std::cout << "Interpolated Y value at X=2.5: " << yValue << std::endl;
}

void testExcelReader() {
    ExcelReader excelReader("ExcelFiles", "UNINA_UAV_Statistics_Input.xlsx");
    excelReader.getData("FUSELAGE", 3, 1, 2, 3);
    
    auto xData = excelReader.getXDataFromExcel();
    auto yData = excelReader.getYDataFromExcel();
    
    std::cout << "X Data from Excel: ";
    for (const auto& x : xData) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "Y Data from Excel: ";
    for (const auto& y : yData) {
        std::cout << y << " ";
    }
    std::cout << std::endl;
}

void testODE45() {
    double RC = 0.2;
    double v_tension = 2.0;
    double t0 = 0.0;
    double tf = 1.0;
    double y0 = 0.0;
    double dt = 1.0 / 99.0;

    ODE45 odeSolver45([RC, v_tension](double t, double y) { return 1.0 / RC * (-y + v_tension); }, t0, tf, y0, dt);
    
    auto tValues = odeSolver45.getT();
    auto yValues = odeSolver45.getY();

    std::cout << "ODE45 Results: " << std::endl;
    for (size_t i = 0; i < tValues.size(); ++i) {
        std::cout << "t: " << tValues[i] << ", y: " << yValues[i] << std::endl;
    }
}

int main() {
    std::cout << "Running Tests..." << std::endl;

    testInterpolant();
    testExcelReader();
    testODE45();

    return 0;
}