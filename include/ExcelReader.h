#ifndef ExcelReader_H // Impedisce definizioni duplicate dell’header.
#define ExcelReader_H

#include <string>   // Gestione dei percorsi come stringa.
#include <iostream> // Eventuale diagnostica a console.
#include <vector>
#include <cmath>
#include <numbers>
#include "RegressionMethod.h" // Inclusione dell'enum RegressionMethod che è una enumerazione dei metodi di regressione disponibili
#include <Eigen/Dense>
#include "gnuplot-iostream.h"
#include <utility>
#include "OpenXLSX/OpenXLSX.hpp" // Non <OpenXLSX.hpp>
#include <filesystem>            // Recupero della current working directory e composizione dei path.
#include "ConvMass.h"
#include "ConvLength.h"
#include "ConvPower.h"
#include "ConvForce.h"
#include "ConvAngle.h"
#include "ConvArea.h"
#include "AircraftData.h"
#include "Interpolant.h"

/// @brief Constructs an ExcelReader object and opens the specified Excel file.
/// @param folderName The name of the folder containing the Excel file.
/// @param excelFileName The name of the Excel file to open.
class ExcelReader
{
private:
    std::string filePath;            // Percorso completo del file Excel da aprire.
    std::string sheetName;           // Nome del foglio attualmente selezionato.
    std::vector<double> xData;       // Vettore per memorizzare i dati X letti da Excel.
    std::vector<double> yData;       // Vettore per memorizzare i dati Y letti da Excel.
    std::vector<std::string> units;  // Vettore per memorizzare le unità di misura dei dati.
    std::string xLabel;              // Etichetta dell'asse X.
    std::string yLabel;              // Etichetta dell'asse Y.
    RegressionMethod methodLabel;    // Metodo di regressione.
    std::string typeOfRegression;    // Tipo di regressione.
    int degreeOfPolynomial;          // Grado del polinomio per la regressione polinomiale.
    OpenXLSX::XLDocument workbook;   // Oggetto xlnt che rappresenta il file Excel.
    OpenXLSX::XLWorksheet worksheet; // Foglio di lavoro attivo da cui leggere i dati.
    OpenXLSX::XLCell cell;           // Cella attualmente in lettura.
    int startRow;                    // Riga di partenza per la scansione dei dati.
    int endRow;                      // Numero di righe effettivamente popolato.
    int startColX;                   // Colonna contenente i valori X.
    int startColY;                   // Colonna contenente i valori Y.
    int startColMethod;              // Colonna con il metodo di regressione.
    std::string unitToPassX = "";     // Unità di misura da convertire per X o unità di misura finale di X.
    std::string unitToPassY = "";     // Unità di misura da convertire per Y o unità di misura finale di Y.
    std::string flagChart = "";      // Flag per abilitare il grafico della regressione.

    int getLengthOfRow() // Conta quante righe sono popolate a partire da startRow.
    {
        worksheet = workbook.workbook().worksheet(sheetName); // Aggiorna il worksheet in base al nome impostato.
        bool checkFlag = true;                                // Flag di controllo per il while.
        endRow = 0;                                           // Si parte assumendo almeno una riga di dati.

        int indexToRow = startRow;

        while (checkFlag)
        {
            cell = worksheet.cell(OpenXLSX::XLCellReference(indexToRow, startColX)); // Recupera la cella corrente.

            // Se nella cella non leggo o un numero o un stringa vuota, esco dal ciclo;
            if (cell.value().type() == OpenXLSX::XLValueType::Empty) // Se la cella è vuota (nessun valore o stringa vuota) termina il conteggio.
            {
                checkFlag = false;
                break;
            }
            else
            {
                endRow += 1;     // Incrementa il numero di righe valide trovate.
                indexToRow += 1; // Avanza alla riga successiva per il prossimo ciclo.
            }
        }

        return endRow; // Restituisce il numero totale di righe popolate.
    }

    std::vector<std::string> getUnitOfAllData()
    {
        // Svuota il vettore per evitare duplicati se chiamato più volte
        units.clear();
        
        worksheet = workbook.workbook().worksheet("INFO");

        for (int col = 22; col <= 27; col++)
        {
            std::string unit = worksheet.cell(OpenXLSX::XLCellReference(3, col)).value().get<std::string>();
            units.emplace_back(unit);
        }

        return units;
    }

    std::vector<double> getXData()
    {

        if (startColMethod == 0)
        {

            // Nessuna estrazione di dati possibile
            std::cout << "No data extraction possible for YData." << std::endl;
            return {};
        }

        double vectorXlength = getLengthOfRow();

        double indexX = startRow;

        std::vector<std::string> detectedUnits = getUnitOfAllData();
        worksheet = workbook.workbook().worksheet(sheetName);
        unitToPassX = worksheet.cell(OpenXLSX::XLCellReference(8, startColMethod)).value().get<std::string>();

        for (size_t i = 0; i < vectorXlength; i++)
        {
            cell = worksheet.cell(OpenXLSX::XLCellReference(indexX, startColX));

            if (cell.value().type() != OpenXLSX::XLValueType::Empty)
            {

                xData.emplace_back(cell.value().get<double>());
            }

            indexX += 1;
        }

        // Conversions of xData based on unitToPassX

        if (unitToPassX == "lb" && detectedUnits[0] == "kg")
        {

            ConvMass convMass(Mass::KG, Mass::LB, xData);

            xData = convMass.getConvertedValues();
        }
        else if (unitToPassX == "slug" && detectedUnits[0] == "kg")
        {
            ConvMass convMass(Mass::KG, Mass::SLUG, xData);
            xData = convMass.getConvertedValues();
        }

        else if (unitToPassX == "lb" && detectedUnits[0] == "slug")
        {

            ConvMass convMass(Mass::SLUG, Mass::LB, xData);

            xData = convMass.getConvertedValues();
        }
        else if (unitToPassX == "kg" && detectedUnits[0] == "slug")
        {
            ConvMass convMass(Mass::SLUG, Mass::KG, xData);
            xData = convMass.getConvertedValues();
        }

        else if (unitToPassX == "kg" && detectedUnits[0] == "lb")
        {
            ConvMass convMass(Mass::LB, Mass::KG, xData);

            xData = convMass.getConvertedValues();
        }
        else if (unitToPassX == "slug" && detectedUnits[0] == "lb")
        {
            ConvMass convMass(Mass::LB, Mass::SLUG, xData);
            xData = convMass.getConvertedValues();
        }

        else if (unitToPassX == "ft" && detectedUnits[1] == "m")
        {

            ConvLength convLength(Length::M, Length::FT, xData);

            xData = convLength.getConvertedValues();
        }

        else if (unitToPassX == "m" && detectedUnits[1] == "ft")
        {

            ConvLength convLength(Length::FT, Length::M, xData);

            xData = convLength.getConvertedValues();


        }

          else if (unitToPassX == "Pound_force" && detectedUnits[2] == "Newton")
        {

            ConvForce convForce(Force::NEWTON, Force::POUND_FORCE, xData);

            xData = convForce.getConvertedValues();
        }

        else if (unitToPassX == "Newton" && detectedUnits[2] == "Pound_force")
        {

            ConvForce convForce(Force::POUND_FORCE, Force::NEWTON, xData);

            xData = convForce.getConvertedValues();
        }

        else if (unitToPassX == "Hp" && detectedUnits[3] == "Watt")
        {

            ConvPower convPower(Power::WATT, Power::HP, xData);

            xData = convPower.getConvertedValues();
        }

        else if (unitToPassX == "Watt" && detectedUnits[3] == "Hp")
        {

            ConvPower convPower(Power::HP, Power::WATT, xData);

            xData = convPower.getConvertedValues();
        }

    
        else if (unitToPassX == "deg" && detectedUnits[4] == "rad")
        {

            ConvAngle convAngle(Angle::RAD, Angle::DEG, xData);

            xData = convAngle.getConvertedValues();
            
        }

        else if (unitToPassX == "rad" && detectedUnits[4] == "deg")
        {

            ConvAngle convAngle(Angle::DEG, Angle::RAD, xData);

            xData = convAngle.getConvertedValues();
        }

        else if (unitToPassX == "ft^2" && detectedUnits[5] == "m^2")
        {

            ConvArea convArea(Area::SQUARE_METER, Area::SQUARE_FEET, xData);

            xData = convArea.getConvertedValues();
        }

        else if (unitToPassX == "m^2" && detectedUnits[5] == "ft^2")
        {

            ConvArea convArea(Area::SQUARE_FEET, Area::SQUARE_METER, xData);

            xData = convArea.getConvertedValues();
        }

        return xData;
    }

    std::vector<double> getYData()
    {

         if (startColMethod == 0){

            // Nessuna estrazione di dati possibile
            std::cout<<"No data extraction possible for YData."<<std::endl;
            return {};
        }
        double vectorYlength = getLengthOfRow();
        double indexY = startRow;

        std::vector<std::string> detectedUnits = getUnitOfAllData();
        worksheet = workbook.workbook().worksheet(sheetName);
        unitToPassY = worksheet.cell(OpenXLSX::XLCellReference(10, startColMethod)).value().get<std::string>();

        for (size_t i = 0; i < vectorYlength; i++)
        {
            cell = worksheet.cell(OpenXLSX::XLCellReference(indexY, startColY));

            if (cell.value().type() != OpenXLSX::XLValueType::Empty)
            {

                yData.emplace_back(cell.value().get<double>());
            }

            indexY += 1;
        }

        // Conversions of yData based on unitToPassY

        if (unitToPassY == "lb" && detectedUnits[0] == "kg")
        {

            ConvMass convMass(Mass::KG, Mass::LB, yData);

            yData = convMass.getConvertedValues();
        }
        else if (unitToPassY == "slug" && detectedUnits[0] == "kg")
        {
            ConvMass convMass(Mass::KG, Mass::SLUG, yData);
            yData = convMass.getConvertedValues();
        }

        else if (unitToPassY == "lb" && detectedUnits[0] == "slug")
        {
            ConvMass convMass(Mass::SLUG, Mass::LB, yData);

            yData = convMass.getConvertedValues();
        }
        else if (unitToPassY == "kg" && detectedUnits[0] == "slug")
        {
            ConvMass convMass(Mass::SLUG, Mass::KG, yData);
            yData = convMass.getConvertedValues();
        }

        else if (unitToPassY == "kg" && detectedUnits[0] == "lb")
        {
            ConvMass convMass(Mass::LB, Mass::KG, yData);

            yData = convMass.getConvertedValues();
        }
        else if (unitToPassY == "slug" && detectedUnits[0] == "lb")
        {
            ConvMass convMass(Mass::LB, Mass::SLUG, yData);
            yData = convMass.getConvertedValues();
        }

        else if (unitToPassY == "ft" && detectedUnits[1] == "m")
        {

            ConvLength convLength(Length::M, Length::FT, yData);

            yData = convLength.getConvertedValues();
        }

        else if (unitToPassY == "m" && detectedUnits[1] == "ft")
        {

            ConvLength convLength(Length::FT, Length::M, yData);

            yData = convLength.getConvertedValues();
        }

        else if (unitToPassY == "Pound_force" && detectedUnits[2] == "Newton")
        {

            ConvForce convForce(Force::NEWTON, Force::POUND_FORCE, yData);

            yData = convForce.getConvertedValues();
        }

        else if (unitToPassY == "Newton" && detectedUnits[2] == "Pound_force")
        {

            ConvForce convForce(Force::POUND_FORCE, Force::NEWTON, yData);

            yData = convForce.getConvertedValues();
        }

        else if (unitToPassY == "Hp" && detectedUnits[3] == "Watt")
        {

            ConvPower convPower(Power::WATT, Power::HP, yData);

            yData = convPower.getConvertedValues();
        }

        else if (unitToPassY == "Watt" && detectedUnits[3] == "Hp")
        {

            ConvPower convPower(Power::HP, Power::WATT, yData);

            yData = convPower.getConvertedValues();
        }

        else if (unitToPassY == "deg" && detectedUnits[4] == "rad")
        {

            ConvAngle convAngle(Angle::RAD, Angle::DEG, yData);

            yData = convAngle.getConvertedValues();
        }

        else if (unitToPassY == "rad" && detectedUnits[4] == "deg")
        {

            ConvAngle convAngle(Angle::DEG, Angle::RAD, yData);

            yData = convAngle.getConvertedValues();
        }

        else if (unitToPassY == "ft^2" && detectedUnits[5] == "m^2")
        {

            ConvArea convArea(Area::SQUARE_METER, Area::SQUARE_FEET, yData);

            yData = convArea.getConvertedValues();
        }

        else if (unitToPassY == "m^2" && detectedUnits[5] == "ft^2")
        {

            ConvArea convArea(Area::SQUARE_FEET, Area::SQUARE_METER, yData);

            yData = convArea.getConvertedValues();
        }
        

        return yData;
    }

    // std::string trim(const std::string &str)
    // {
    //     auto start = std::find_if_not(str.begin(), str.end(),
    //                                   [](unsigned char c)
    //                                   { return std::isspace(c); });
    //     auto end = std::find_if_not(str.rbegin(), str.rend(),
    //                                 [](unsigned char c)
    //                                 { return std::isspace(c); })
    //                    .base();
    //     return (start < end) ? std::string(start, end) : "";
    // }

    std::string getNameOfXAxis()
    {

        xLabel = worksheet.cell(OpenXLSX::XLCellReference(2, startColX)).value().get<std::string>();

        return xLabel;
    }

    std::string getNameOfYAxis()
    {

        yLabel = worksheet.cell(OpenXLSX::XLCellReference(2, startColY)).value().get<std::string>();

        return yLabel;
    }

    RegressionMethod getTypeOfRegression()
    {

        if (startColMethod == 0)
        {

            // Nessuna estrazione di dati possibile
            throw std::runtime_error("No data extraction possible for Regression Method.");
        }

        std::string methodLabelStr = worksheet.cell(OpenXLSX::XLCellReference(3, startColMethod)).value().get<std::string>();

        if (methodLabelStr == "LINEAR")
        {
            methodLabel = RegressionMethod::LINEAR;
        }
        else if (methodLabelStr == "POLYNOMIAL")
        {
            methodLabel = RegressionMethod::POLYNOMIAL;
        }
        else if (methodLabelStr == "EXPONENTIAL")
        {
            methodLabel = RegressionMethod::EXPONENTIAL;
        }
        else if (methodLabelStr == "POWER")
        {
            methodLabel = RegressionMethod::POWER;
        }
        else if (methodLabelStr == "LOGARITHMIC")
        {
            methodLabel = RegressionMethod::LOGARITHMIC;
        }
        else if (methodLabelStr == "CONSTANT")
        {
            methodLabel = RegressionMethod::CONSTANT;
        }
        else
        {
            std::cerr << "Regression method not found: " << methodLabelStr << std::endl;
            // Gestione dell'errore, ad esempio impostare un valore di default o lanciare un'eccezione
        }
        return methodLabel;
    }

    int getDegreeOfPolynomial()
    {

        if (startColMethod == 0)
        {

            // Nessuna estrazione di dati possibile
            std::cout << "No data extraction possible for YData." << std::endl;
            return 0;
        }
        
        worksheet = workbook.workbook().worksheet(sheetName);
        cell = worksheet.cell(OpenXLSX::XLCellReference(4, startColMethod));
        degreeOfPolynomial = cell.value().get<double>();

        return degreeOfPolynomial;
    }

    std::string getFlagToEnableChart() {


        if (startColMethod == 0)
        {

            // Nessuna estrazione di dati possibile
            std::cout << "No data extraction possible for Flag to Enable Chart." << std::endl;
            return "";
        }

        worksheet = workbook.workbook().worksheet(sheetName);
        flagChart = worksheet.cell(OpenXLSX::XLCellReference(6, startColMethod)).value().get<std::string>();

        return flagChart;
    }

     int countVariables(const std::string& sheetName)
    {
        worksheet = workbook.workbook().worksheet(sheetName);
        
        int count = 0;
        const int MAX_COLUMNS = 300;  // Safety: max 100 variabili (300 colonne)
        
        for (int col = 1; col < MAX_COLUMNS; col += 3) {
            OpenXLSX::XLCell checkCell = worksheet.cell(
                OpenXLSX::XLCellReference(2, col)
            );
            
            // Se la cella è vuota, non ci sono più variabili
            if (checkCell.value().type() == OpenXLSX::XLValueType::Empty) {
                break;
            }
            
            count++;
        }
        return count;
    }

 


public:

    
    ExcelReader(const std::string &folderName, const std::string &excelFileName)
    {
        // Combina la current path con la cartella e il nome del file, salva il percorso risultante e carica il workbook.
        auto baseDir = std::filesystem::current_path() / folderName; // auto deduce std::filesystem::path, ottenendo il path della cartella base.
        auto fullPath = baseDir / excelFileName;                     // auto deduce std::filesystem::path, completando il percorso con il file.
        this->filePath = fullPath.string();                          // Salva il percorso definitivo come std::string per OpenXLSX.
        this->workbook.open(this->filePath);                         // Carica il workbook dal percorso calcolato.
    }

    /// @brief Set the sheet name and data range to read from the Excel file.
    /// @param sheetName Name of the worksheet to read data from.
    /// @param startRow Row index to start reading data (1-based).
    /// @param startColX Column index to start reading X data (1-based).
    /// @param startColY Column index to start reading Y data (1-based).
    /// @param startColMethod Column index to start reading the regression method (1-based).
    void getData(std::string sheetName, int startRow, int startColX, int startColY, int startColMethod)
    {
        this->sheetName = sheetName;
        this->startRow = startRow;
        this->startColX = startColX;
        this->startColY = startColY;
        this->startColMethod = startColMethod;

        xData.clear();
        yData.clear();
        
        int totalRows = getLengthOfRow();
        getXData();
        getYData();
        getNameOfXAxis();
        getNameOfYAxis();
        getTypeOfRegression();
        getDegreeOfPolynomial();
        getUnitOfAllData();
        getFlagToEnableChart();



        std::cout << "Total populated rows: " << totalRows << std::endl;
    }

    /// @brief Returns a const reference to the vector containing X data from the Excel sheet.
    /// @return Const reference to the vector of X data.
    const std::vector<double> &getXDataFromExcel() const
    {
        return xData;
    }

    /// @brief Returns a const reference to the vector containing Y data from the Excel sheet.
    /// @return Const reference to the vector of Y data.
    const std::vector<double> &getYDataFromExcel() const
    {

        return yData;
    }

    /// @brief Returns the label of the X axis from the Excel sheet.
    /// @return Const reference to the X axis label.
    const std::string &getXLabelFromExcel() const
    {
        return xLabel;
    };

    /// @brief Returns the label of the Y axis from the Excel sheet.
    /// @return Const reference to the Y axis label.
    const std::string &getYLabelFromExcel() const
    {
        return yLabel;
    }

    /// @brief Returns the regression method from the Excel sheet.
    /// @return Const reference to the regression method.
    const RegressionMethod &getMethodOfRegressionFromExcel() const
    {
        return methodLabel;
    };

    /// @brief Returns the degree of the polynomial for polynomial regression from the Excel sheet.
    /// @return Const reference to the degree of the polynomial.
    const int &getDegreeOfPolynomialFromExcel() const
    {
        return degreeOfPolynomial;
    };

    /// @brief Returns the units of measurement for X and Y data from the Excel sheet.
    /// @return A vector containing units for X and Y.
    std::vector<std::string> getUnitsOfXY() const
    {
        return {unitToPassX, unitToPassY};
    };

    /// @brief Returns the flag to enable chart generation from the Excel sheet.
    /// @return Const reference to the flag string.
    const std::string& getFlagToEnableChartFromExcel() const
    {
        return flagChart;
    };


    // ===== NUOVI METODI PER AIRCRAFT DATA =====
    
    /// @brief Reads an interpolable variable from the specified Excel sheet and variable index.
    /// @param sheetName Name of the worksheet to read data from.
    /// @param varIndex Index of the variable to read (0-based).
    /// @return An InterpolableVariable object populated with data from the Excel sheet.
    AERO::InterpolableVariable readVariable(const std::string& sheetName, int varIndex)
    {
        AERO::InterpolableVariable var;
        
        // Formula colonne: VAR i → startCol = 1 + (i * 3)
        int colX = 1 + (varIndex * 3);      // 1, 4, 7, 10, ...
        int colY = colX + 1;                // 2, 5, 8, 11, ...
        int colMeta = colY + 1;             // 3, 6, 9, 12, ...
        
        // Usa getData esistente
        getData(sheetName, 3, colX, colY, colMeta);
        
        // Popola la struct
        var.varName = "VAR " + std::to_string(varIndex + 1);
        var.xLabel = getXLabelFromExcel();
        var.yLabel = getYLabelFromExcel();
        
        std::vector<std::string> unitsVec = getUnitsOfXY();
        var.xUnit = unitsVec[0];
        var.yUnit = unitsVec[1];
        
        var.method = getMethodOfRegressionFromExcel();
        var.polynomialDegree = getDegreeOfPolynomialFromExcel();
        
        var.xData = getXDataFromExcel();
        var.yData = getYDataFromExcel();
        
        // Calcola coefficienti
        if (var.method != RegressionMethod::CONSTANT && var.isValid()) {
            Interpolant interp(var.xData, var.yData, 
                              var.polynomialDegree, var.method);
            var.coefficients = interp.getCoefficients();
        }
        
        return var;
    }

    /// @brief Reads a component's data from the specified Excel sheet.
    /// @param sheetName Name of the worksheet to read data from.
    /// @return A ComponentData object populated with data from the Excel sheet.
    AERO::ComponentData readComponent(const std::string &sheetName, const std::string &aircraftName)
    {
        AERO::ComponentData component;
        component.componentName = sheetName;

        // Conta prima quante variabili ci sono
        int numVars = countVariables(sheetName);

        if (numVars == 0)
        {
            std::cerr << "Warning: No variables found in " << sheetName << std::endl;
            return component;
        }

        std::cout << "\n============ " << sheetName << " ============" << std::endl;
        std::cout << "Reading " << numVars << " variable(s) from "
                  << sheetName << "..." << std::endl;

        std::vector<std::string> variablesNameX;
        std::vector<std::string> variablesNameY;
        std::vector<std::string> xDetectedUnits;
        std::vector<std::string> yDetectedUnits;

        // Leggi tutte le variabili trovate
        for (int varIndex = 0; varIndex < numVars; varIndex++)
        {
            try
            {
                AERO::InterpolableVariable var = readVariable(sheetName, varIndex);

                if (var.isValid())
                {

                    // Aggiungi la variabile alla mappa del componente
                    component.variables[var.varName] = var;
                    std::cout << "  [OK] " << var.varName << ": " << var.xLabel
                              << " [->] " << var.yLabel << " ("
                              << var.xData.size() << " points)" << std::endl;

                    // Raccogli i dati per il check di consistenza
                    variablesNameX.push_back(var.xLabel);
                    variablesNameY.push_back(var.yLabel);
                    xDetectedUnits.push_back(var.xUnit);
                    yDetectedUnits.push_back(var.yUnit);

                    // Genera il grafico della regressione se richiesto
                    component.getChartOfVariableRegression(var.varName, getFlagToEnableChartFromExcel(), aircraftName);
                }
                else
                {
                    std::cerr << "  [ERROR] VAR " << (varIndex + 1)
                              << " has invalid data" << std::endl;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "  [ERROR] Error reading VAR " << (varIndex + 1)
                          << ": " << e.what() << std::endl;
            }
        }

        // Check finale sulla consistenza delle unità di misura
        // Eseguito solo dopo aver letto tutte le variabili
        if (!variablesNameX.empty())
        {
            std::cout << "\n[WAIT] Checking unit consistency..." << std::endl;

            // Mappa per le variabili X del tipo xlabel -> unità
            std::map<std::string, std::string> expectedXUnits;

            // Mappa per le variabili Y del tipo ylabel -> unità
            std::map<std::string, std::string> expectedYUnits;

            for (size_t j = 0; j < variablesNameX.size(); j++)
            {
                const std::string &xName = variablesNameX[j];
                const std::string &xUnit = xDetectedUnits[j];
                const std::string &yName = variablesNameY[j];
                const std::string &yUnit = yDetectedUnits[j];

                // Check per X, se la variabile è già stata vista, altrimenti rilascia expectedXUnits.end()
                // aggiunge la variabile alla mappa se non esiste ancora
                if (expectedXUnits.find(xName) != expectedXUnits.end())
                {
                    if (expectedXUnits[xName] != xUnit)
                    {
                        std::cerr << "  [WARNING] Mismatch in X units for: " << xName
                                  << " (expected: " << expectedXUnits[xName]
                                  << ", found: " << xUnit << ")" << std::endl;
                    }
                }
                else
                {
                    expectedXUnits[xName] = xUnit;
                }

                // Check per Y, se la variabile è già stata vista, altrimenti rilascia expectedYUnits.end()
                // aggiunge la variabile alla mappa se non esiste ancora
                if (expectedYUnits.find(yName) != expectedYUnits.end())
                {
                    if (expectedYUnits[yName] != yUnit)
                    {
                        std::cerr << "  [WARNING] Mismatch in Y units for: " << yName
                                  << " (expected: " << expectedYUnits[yName]
                                  << ", found: " << yUnit << ")" << std::endl;
                    }
                }
                else
                {
                    expectedYUnits[yName] = yUnit;
                }
            }

            std::cout << "Unit consistency check completed." << std::endl;
        }

        return component;
    }
    /// @brief Reads all aircraft data from the Excel sheets.
    /// @param aircraftName Name of the aircraft.
    /// @return An AircraftData object populated with data from all relevant Excel sheets.
    AERO::AircraftData readAllAircraftData(const std::string& aircraftName)
    {
        AERO::AircraftData aircraft;
        aircraft.setAircraftName(aircraftName);
        
        aircraft.fuselage = readComponent("FUSELAGE", aircraftName);
        aircraft.wing = readComponent("WING", aircraftName);
        aircraft.horizontal = readComponent("HORIZONTAL_TAIL", aircraftName);
        aircraft.vertical = readComponent("VERTICAL_TAIL", aircraftName);
        aircraft.undercarriage = readComponent("UNDERCARRIAGE", aircraftName);
        aircraft.engine = readComponent("ENGINES", aircraftName);
        
       
        return aircraft;
    }
};

#endif

// Getter const: promette di NON modificare lo stato dell'oggetto.
// Ritorna una referenza const (non una copia) al vettore membro ( il vettore membro è xData o yData )
// - evita copie costose
// - impedisce modifiche dall'esterno grazie a const
// - una referenza (const) a xData, non'è una copia. Vuol dire che chi chiama vede lo stesso vettore interno ma non può modificarlo.

// //std::vector<std::string> getUnitsOfXY() const
//     {
//         return {unitToPassX, unitToPassY};
//     };

// Questo getter dato che crea un nuovo vettore non può essere deferenziato
// perche il vettore che si crea non è un mebro della classe ma una variabile temporanea creata al volo
// quindi non ha senso restituire una referenza a qualcosa che non esiste più dopo la fine della funzione.
// Inoltre, è inutile dichiararlo const perché non accede a membri della classe che potrebbero essere modificati.
// nel senso che è unitile scrivere const std::vector<std::string> getUnitsOfXY() const
// perché il primo const non ha senso in questo contesto.