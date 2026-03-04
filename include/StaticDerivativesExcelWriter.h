#pragma once

#include "OpenXLSX/OpenXLSX.hpp"
#include "LONGITUDINALSTABILTY.h"
#include "DIRECTIONALSTABILITY.h"
#include "LATERALSTABILITY.h"
#include "VSPAeroGenerator.h"
#include <filesystem>

using namespace OpenXLSX;

class StaticDerivativeExcelWriter
{

protected:
    VSP::AeroSettings settings;

private:
    std::string fileName = "";
    std::string nameOfAircraft = "";

    // Format settings
    int indexToStartWriteDerivativeTableLongitudinal = 5;
    int indexToStartWriteDerivativeTableSideDirectional = 5;
    int indexToStartWriteDerivativeTableYawDirectional = 5;
    int indexToStartWriteDerivativeTableLateral = 5;

public:
    StaticDerivativeExcelWriter()
    {
    }

    inline void writeDerivativesToExcel(const std::string fileName,
                                        const std::string nameOfAircraft,
                                        VSP::AeroSettings settings,
                                        LONGITUDINAL_STABILITY::LongitudinalStabilityDerivativesToSingleComponent longitudinalDerivativesComponents,
                                        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent directionalDerivativesSideForceComponents,
                                        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesYawToSingleComponent directionalDerivativesYawComponents,
                                        LATERAL_STABILITY::LateralStabilityDerivativesRollToSingleComponent lateralDerivativesRollComponents,
                                        LONGITUDINAL_STABILITY::LongitudinalStabilityDerivatives longitudinalDerivativesAircraft,
                                        DIRECTIONAL_STABILITY::DirectionalStabilityDerivatives directionalDerivativesAircraft,
                                        LATERAL_STABILITY::LateralStabilityDerivatives lateralDerivativesAircraft,
                                        std::string pathToSaveExcel = "")
    {

        // Inzialization and lambda function
        this->fileName = fileName;
        this->nameOfAircraft = nameOfAircraft;
        this->settings = settings;

        XLDocument doc;

        // ======================================================
        // LONGITUDINAL SECTION
        // ======================================================

        // Determine the path to save the Excel file
        if (pathToSaveExcel.empty())
        {

            pathToSaveExcel = std::filesystem::current_path().string() + "\\" + "ExcelResults" + "\\" + fileName;
        }

        else
        {

            pathToSaveExcel = pathToSaveExcel + "\\" + fileName;
        }

        // Check if the file already exists, if yes delete it
        if (std::filesystem::exists(pathToSaveExcel))
        {

            std::filesystem::remove(pathToSaveExcel);
        }

        doc.create(pathToSaveExcel, !XLForceOverwrite);

        // Create the woorkbok

        auto workbookSheetLongitudinal = doc.workbook().worksheet("Sheet1");

        // Lambda generalizzate che accettano il foglio come parametro
        auto writeRow = [&](XLWorksheet &sheet, int &rowIdx, const std::string &component, const std::string &unit, double value)
        {
            sheet.cell("A" + std::to_string(rowIdx)).value() = component;
            sheet.cell("B" + std::to_string(rowIdx)).value() = unit;
            sheet.cell("C" + std::to_string(rowIdx)).value() = value;
            rowIdx++;
        };

        auto setColumnWidth = [&](XLWorksheet &sheet, const std::string &col, const std::vector<std::string> &contents)
        {
            size_t maxLen = 0;
            for (const auto &s : contents)
                maxLen = std::max(maxLen, s.size());
            sheet.column(col).setWidth(static_cast<int>(maxLen) + 2);
        };

        workbookSheetLongitudinal.setName("LONGITUDINAL_STATIC_DERIVATIVES");

        // Header creation
        workbookSheetLongitudinal.cell("A1").value() = "Aircraft Name:";
        workbookSheetLongitudinal.cell("A2").value() = nameOfAircraft;

        workbookSheetLongitudinal.cell("B1").value() = "Mach:";
        workbookSheetLongitudinal.cell("B2").value() = settings.Mach;

        workbookSheetLongitudinal.cell("C1").value() = "Re:";
        workbookSheetLongitudinal.cell("C2").value() = settings.ReCref;

        workbookSheetLongitudinal.cell("D1").value() = "Altitude (m):";
        workbookSheetLongitudinal.cell("D2").value() = settings.altitude;

        workbookSheetLongitudinal.cell("E1").value() = "Xcg(from the nose) (m):";
        workbookSheetLongitudinal.cell("E2").value() = settings.X_cg;

        workbookSheetLongitudinal.cell("F1").value() = "Ycg(from the nose) (m):";
        workbookSheetLongitudinal.cell("F2").value() = settings.Y_cg;

        workbookSheetLongitudinal.cell("G1").value() = "Zcg(from the nose) (m):";
        workbookSheetLongitudinal.cell("G2").value() = settings.Z_cg;

        workbookSheetLongitudinal.cell("H1").value() = "MAC (m):";
        workbookSheetLongitudinal.cell("H2").value() = settings.Cref;

        // Write the longitudinal derivatives to the Excel file
        workbookSheetLongitudinal.cell("A4").value() = "ID - Alpha Related";
        workbookSheetLongitudinal.cell("B4").value() = "Unit";
        workbookSheetLongitudinal.cell("C4").value() = "Value";

        // STATIC STABILITY MARGIN
        workbookSheetLongitudinal.cell("A12").value() = "ID - SSM Related";
        workbookSheetLongitudinal.cell("B12").value() = "Unit";
        workbookSheetLongitudinal.cell("C12").value() = "Value";

        writeRow(workbookSheetLongitudinal, indexToStartWriteDerivativeTableLongitudinal, "Cm_alpha_wing", "1/deg",
                 longitudinalDerivativesComponents.deltaCmDeltaAlphaWing);

        writeRow(workbookSheetLongitudinal, indexToStartWriteDerivativeTableLongitudinal, "Cm_alpha_canard", "1/deg",
                 longitudinalDerivativesComponents.deltaCmDeltaAlphaCanard);

        writeRow(workbookSheetLongitudinal, indexToStartWriteDerivativeTableLongitudinal, "Cm_alpha_horizontal_tail", "1/deg",
                 longitudinalDerivativesComponents.deltaCmDeltaAlphaHorizontalTail);

        writeRow(workbookSheetLongitudinal, indexToStartWriteDerivativeTableLongitudinal, "Cm_alpha_fuselage", "1/deg",
                 longitudinalDerivativesComponents.deltaCmDeltaAlphaFuselage);

        writeRow(workbookSheetLongitudinal, indexToStartWriteDerivativeTableLongitudinal, "Cm_alpha_nacelle", "1/deg",
                 longitudinalDerivativesComponents.deltaCmDeltaAlphaNacelle);

        writeRow(workbookSheetLongitudinal, indexToStartWriteDerivativeTableLongitudinal, "TOTAL CM_alpha_Aircraft", "1/deg",
                 longitudinalDerivativesAircraft.deltaCmDeltaAlphaAircraft);

        indexToStartWriteDerivativeTableLongitudinal += 2; // Salto due rige

        writeRow(workbookSheetLongitudinal, indexToStartWriteDerivativeTableLongitudinal, "Static Stability Margin", "-", longitudinalDerivativesAircraft.deltaCmDeltaClAircraft);
        writeRow(workbookSheetLongitudinal, indexToStartWriteDerivativeTableLongitudinal, "Neutral point as fraction of MAC", "-", longitudinalDerivativesAircraft.neutralPointAsFractionOfMAC);

        setColumnWidth(workbookSheetLongitudinal, "A", {
            "ID - Alpha Related", 
            "Cm_alpha_wing", 
            "Cm_alpha_canard", 
            "Cm_alpha_horizontal_tail", 
            "Cm_alpha_fuselage", 
            "Cm_alpha_nacelle", 
            "TOTAL CM_alpha_Aircraft",
            "ID - SSM Related",
            "Static Stability Margin",
            "Neutral point as fraction of MAC"});

        setColumnWidth(workbookSheetLongitudinal, "B", {"Unit", "1/deg"});
        setColumnWidth(workbookSheetLongitudinal, "C", {"Value"});

        // ======================================================
        // DIRECTIONAL SECTION - SIDE FORCE
        // ======================================================

        doc.workbook().addWorksheet("DIR_STATIC_SIDE_DERIVATIVES");
        auto workbookSheetSideDirectional = doc.workbook().worksheet("DIR_STATIC_SIDE_DERIVATIVES");

        // Header creation
        workbookSheetSideDirectional.cell("A1").value() = "Aircraft Name:";
        workbookSheetSideDirectional.cell("A2").value() = nameOfAircraft;

        workbookSheetSideDirectional.cell("B1").value() = "Mach:";
        workbookSheetSideDirectional.cell("B2").value() = settings.Mach;

        workbookSheetSideDirectional.cell("C1").value() = "Re:";
        workbookSheetSideDirectional.cell("C2").value() = settings.ReCref;

        workbookSheetSideDirectional.cell("D1").value() = "Altitude (m):";
        workbookSheetSideDirectional.cell("D2").value() = settings.altitude;

        workbookSheetSideDirectional.cell("E1").value() = "Xcg(from the nose) (m):";
        workbookSheetSideDirectional.cell("E2").value() = settings.X_cg;

        workbookSheetSideDirectional.cell("F1").value() = "Ycg(from the nose) (m):";
        workbookSheetSideDirectional.cell("F2").value() = settings.Y_cg;

        workbookSheetSideDirectional.cell("G1").value() = "Zcg(from the nose) (m):";
        workbookSheetSideDirectional.cell("G2").value() = settings.Z_cg;

        workbookSheetSideDirectional.cell("H1").value() = "MAC (m):";
        workbookSheetSideDirectional.cell("H2").value() = settings.Cref;

        // Write the longitudinal derivatives to the Excel file
        workbookSheetSideDirectional.cell("A4").value() = "ID - Beta Related";
        workbookSheetSideDirectional.cell("B4").value() = "Unit";
        workbookSheetSideDirectional.cell("C4").value() = "Value";

        writeRow(workbookSheetSideDirectional, indexToStartWriteDerivativeTableSideDirectional, "Cy_beta_wing", "1/deg",
                 directionalDerivativesSideForceComponents.deltaCyDeltaBetaWingContribution);

        writeRow(workbookSheetSideDirectional, indexToStartWriteDerivativeTableSideDirectional, "Cy_beta_canard", "1/deg",
                 directionalDerivativesSideForceComponents.deltaCyDeltaBetaCanardContribution);

        writeRow(workbookSheetSideDirectional, indexToStartWriteDerivativeTableSideDirectional, "Cy_beta_horizontal_tail", "1/deg",
                 directionalDerivativesSideForceComponents.deltaCyDeltaBetaHorizontalTailContribution);

        writeRow(workbookSheetSideDirectional, indexToStartWriteDerivativeTableSideDirectional, "Cy_beta_vertical_tail", "1/deg",
                 directionalDerivativesSideForceComponents.deltaCyDeltaBetaVerticalTailContribution);

        writeRow(workbookSheetSideDirectional, indexToStartWriteDerivativeTableSideDirectional, "Cy_beta_fuselage", "1/deg",
                 directionalDerivativesSideForceComponents.deltaCyDeltaBetaFuselageContribution);

        if (settings.IncludePropToAnlysis == "Yes")
        {
            writeRow(workbookSheetSideDirectional, indexToStartWriteDerivativeTableSideDirectional, "Cy_beta_propeller_windmilling", "1/deg",
                     directionalDerivativesSideForceComponents.deltaCyDeltaBetaWindMillingPropeller);
        }

        writeRow(workbookSheetSideDirectional, indexToStartWriteDerivativeTableSideDirectional, "TOTAL CY_beta_Aircraft", "1/deg",
                 directionalDerivativesAircraft.deltaCyDeltaBetaAircraft);

        setColumnWidth(workbookSheetSideDirectional, "A", {"ID - Beta Related", "Cy_beta_wing", "Cy_beta_canard", "Cy_beta_horizontal_tail", "Cy_beta_fuselage", "Cy_beta_propeller_windmilling", "TOTAL CY_beta_Aircraft"});

        setColumnWidth(workbookSheetSideDirectional, "B", {"Unit", "1/deg"});
        setColumnWidth(workbookSheetSideDirectional, "C", {"Value"});

        // ======================================================
        // DIRECTIONAL SECTION - YAW MOMENT
        // ======================================================

        doc.workbook().addWorksheet("DIR_STATIC_YAW_DERIVATIVES");
        auto workbookSheetYawDirectional = doc.workbook().worksheet("DIR_STATIC_YAW_DERIVATIVES");

        // Header creation
        workbookSheetYawDirectional.cell("A1").value() = "Aircraft Name:";
        workbookSheetYawDirectional.cell("A2").value() = nameOfAircraft;

        workbookSheetYawDirectional.cell("B1").value() = "Mach:";
        workbookSheetYawDirectional.cell("B2").value() = settings.Mach;

        workbookSheetYawDirectional.cell("C1").value() = "Re:";
        workbookSheetYawDirectional.cell("C2").value() = settings.ReCref;

        workbookSheetYawDirectional.cell("D1").value() = "Altitude (m):";
        workbookSheetYawDirectional.cell("D2").value() = settings.altitude;

        workbookSheetYawDirectional.cell("E1").value() = "Xcg(from the nose) (m):";
        workbookSheetYawDirectional.cell("E2").value() = settings.X_cg;

        workbookSheetYawDirectional.cell("F1").value() = "Ycg(from the nose) (m):";
        workbookSheetYawDirectional.cell("F2").value() = settings.Y_cg;

        workbookSheetYawDirectional.cell("G1").value() = "Zcg(from the nose) (m):";
        workbookSheetYawDirectional.cell("G2").value() = settings.Z_cg;

        workbookSheetYawDirectional.cell("H1").value() = "MAC (m):";
        workbookSheetYawDirectional.cell("H2").value() = settings.Cref;

        // Write the longitudinal derivatives to the Excel file
        workbookSheetYawDirectional.cell("A4").value() = "ID - Beta Related";
        workbookSheetYawDirectional.cell("B4").value() = "Unit";
        workbookSheetYawDirectional.cell("C4").value() = "Value";

        writeRow(workbookSheetYawDirectional, indexToStartWriteDerivativeTableYawDirectional, "Cn_beta_wing", "1/deg",
                 directionalDerivativesYawComponents.deltaCnDeltaBetaWingContribution);

        writeRow(workbookSheetYawDirectional, indexToStartWriteDerivativeTableYawDirectional, "Cn_beta_canard", "1/deg",
                 directionalDerivativesYawComponents.deltaCnDeltaBetaCanardContribution);

        writeRow(workbookSheetYawDirectional, indexToStartWriteDerivativeTableYawDirectional, "Cn_beta_horizontal_tail", "1/deg",
                 directionalDerivativesYawComponents.deltaCnDeltaBetaHorizontalTailContribution);

        writeRow(workbookSheetYawDirectional, indexToStartWriteDerivativeTableYawDirectional, "Cn_beta_vertical_tail", "1/deg",
                 directionalDerivativesYawComponents.deltaCnDeltaBetaVerticalTailContribution);

        writeRow(workbookSheetYawDirectional, indexToStartWriteDerivativeTableYawDirectional, "Cn_beta_fuselage", "1/deg",
                 directionalDerivativesYawComponents.deltaCnDeltaBetaFuselageContribution);

        writeRow(workbookSheetYawDirectional, indexToStartWriteDerivativeTableYawDirectional, "Cn_beta_nacelle", "1/deg",
                 directionalDerivativesYawComponents.deltaCnDeltaBetaNacelleContribution);

        writeRow(workbookSheetYawDirectional, indexToStartWriteDerivativeTableYawDirectional, "CN_delta_r", "1/deg",
                 directionalDerivativesAircraft.deltaCnDeltaRudderDeflection);

        writeRow(workbookSheetYawDirectional, indexToStartWriteDerivativeTableYawDirectional, "TOTAL CN_beta_Aircraft", "1/deg",
                 directionalDerivativesAircraft.deltaCnDeltaBetaAircraft);

        setColumnWidth(workbookSheetYawDirectional, "A", {
            "ID - Beta Related", 
            "Cn_beta_wing", 
            "Cn_beta_canard", 
            "Cn_beta_horizontal_tail", 
            "Cn_beta_fuselage", 
            "Cn_beta_nacelle", 
            "Cn_delta_r",
            "TOTAL CN_beta_Aircraft"});

        setColumnWidth(workbookSheetYawDirectional, "B", {"Unit", "1/deg"});
        setColumnWidth(workbookSheetYawDirectional, "C", {"Value"});

        // ======================================================
        // LATERAL SECTION
        // ======================================================

        doc.workbook().addWorksheet("DIR_STATIC_ROLL_DERIVATIVES");
        auto workbookSheetRollDirectional = doc.workbook().worksheet("DIR_STATIC_ROLL_DERIVATIVES");

        // Header creation
        workbookSheetRollDirectional.cell("A1").value() = "Aircraft Name:";
        workbookSheetRollDirectional.cell("A2").value() = nameOfAircraft;

        workbookSheetRollDirectional.cell("B1").value() = "Mach:";
        workbookSheetRollDirectional.cell("B2").value() = settings.Mach;

        workbookSheetRollDirectional.cell("C1").value() = "Re:";
        workbookSheetRollDirectional.cell("C2").value() = settings.ReCref;

        workbookSheetRollDirectional.cell("D1").value() = "Altitude (m):";
        workbookSheetRollDirectional.cell("D2").value() = settings.altitude;

        workbookSheetRollDirectional.cell("E1").value() = "Xcg(from the nose) (m):";
        workbookSheetRollDirectional.cell("E2").value() = settings.X_cg;

        workbookSheetRollDirectional.cell("F1").value() = "Ycg(from the nose) (m):";
        workbookSheetRollDirectional.cell("F2").value() = settings.Y_cg;

        workbookSheetRollDirectional.cell("G1").value() = "Zcg(from the nose) (m):";
        workbookSheetRollDirectional.cell("G2").value() = settings.Z_cg;

        workbookSheetRollDirectional.cell("H1").value() = "MAC (m):";
        workbookSheetRollDirectional.cell("H2").value() = settings.Cref;


        // Write the longitudinal derivatives to the Excel file
        workbookSheetRollDirectional.cell("A4").value() = "ID - Beta Related";
        workbookSheetRollDirectional.cell("B4").value() = "Unit";
        workbookSheetRollDirectional.cell("C4").value() = "Value";

        writeRow(workbookSheetRollDirectional, indexToStartWriteDerivativeTableLateral, "Cl_beta_wing_fuselage", "1/deg",
                 lateralDerivativesRollComponents.deltaClDeltaBetaWingFuselageContribution);

        writeRow(workbookSheetRollDirectional, indexToStartWriteDerivativeTableLateral, "Cl_beta_horizontal_tail", "1/deg",
                 lateralDerivativesRollComponents.deltaClDeltaBetaHorizontalTailContribution);

        writeRow(workbookSheetRollDirectional, indexToStartWriteDerivativeTableLateral, "Cl_beta_vertical_tail", "1/deg",
                 lateralDerivativesRollComponents.deltaClDeltaBetaVerticalTailContribution);

        writeRow(workbookSheetRollDirectional, indexToStartWriteDerivativeTableLateral, "C\u2112_delta_a", "1/deg",
                 lateralDerivativesAircraft.deltaClBetaDeltaAileronsDeflection);

        // L write as UNICODE U+2112 --> \u2112
        writeRow(workbookSheetRollDirectional, indexToStartWriteDerivativeTableLateral, "TOTAL  C\u2112_beta_Aircraft", "1/deg",
                 lateralDerivativesAircraft.deltaClDeltaBetaAircraft);

        setColumnWidth(workbookSheetRollDirectional, "A", {"ID - Beta Related", 
            "Cl_beta_wing_fuselage", 
            "Cl_beta_horizontal_tail", 
            "Cl_beta_vertical_tail",  
            "TOTAL  C\u2112_beta_Aircraft"});

        setColumnWidth(workbookSheetRollDirectional, "B", {"Unit", "1/deg"});
        setColumnWidth(workbookSheetRollDirectional, "C", {"Value"});

        doc.save();
        doc.close();
    }
};