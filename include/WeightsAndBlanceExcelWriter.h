#pragma once

#include "OpenXLSX/OpenXLSX.hpp"
#include "WEIGHTS.h"
#include "COGCALCULATOR.h"
#include "VSPAeroGenerator.h"
#include <filesystem>
#include <functional>

using namespace OpenXLSX;

class WeightsAndBalanceExcelWriter
{

protected:
    VSP::AeroSettings settings;
    COG::Weights componentWeight;
    COG::COGDATA componentBalance;

private:
    std::string fileName = "";
    std::string nameOfAircraft = "";

    // Format settings
    int indexToStartWriteDerivativeTable = 5;

    // =========================================================
    // Helper: check if structs are empty (all values == 0.0)
    // =========================================================

    /**
     * @brief Returns the list of component weights with non-zero values.
     * @param value Input weights structure.
     * @return Vector of (component name, weight) pairs.
     */
    std::vector<std::pair<std::string, double>> getNonZeroWeights(const COG::Weights &value) const
    {

        std::vector<std::pair<std::string, double>> components = {
            {"Wing", value.wingWeight},
            {"Canard", value.canardWeight},
            {"Horizontal Tail", value.horizontalWeight},
            {"Vertical Tail", value.verticalWeight},
            {"Fuselage", value.fuselageWeight},
            {"Nacelle", value.nacelleWeight},
            {"Boom", value.boomWeight},
            {"EOIR", value.eoirWeight},
            {"Landing Gear", value.landingGearWeight},
            {"Control Surfaces", value.controlSurfacesWeight},
            {"Propulsion Group", value.propulsionGroupWeight},
            {"APU", value.apuWeight},
            {"Instruments", value.instrumentsWeight},
            {"Hydraulic And Pneumatic", value.hydraulicAndPneumaticWeight},
            {"Electrical Group", value.electricalGroupWeight},
            {"Avionic Group", value.avionicGroupWeight},
            {"Furnishings And Equipment", value.furnishingsAndEquipmentWeight},
            {"Air Conditioning And Anti Ice", value.airConditioningAndAntiIceWeight},
            {"Operating Items", value.operatingItemsWeight},
            {"Payload", value.payloadWeight},
            {"Crew", value.crewWeight},
            {"Fuel", value.fuelWeight},
            {"Total Aircraft", value.totalAircraftWeight}};

        std::vector<std::pair<std::string, double>> nonZeroWeights;
        for (const auto &weightOfcomponent : components)
        {
            if (weightOfcomponent.second != 0.0)
            {
                nonZeroWeights.push_back({weightOfcomponent.first, weightOfcomponent.second});
            }
        }

        return nonZeroWeights;
    }


    /**
     * @brief Returns the list of component balance entries with non-zero COG coordinates.
     * @param valueCOG Input COG structure.
     * @return Vector of tuples (component name, x, y, z).
     */
    std::vector<std::tuple<std::string, double, double, double>> getNonZeroBalance(const COG::COGDATA &valueCOG) const
    {

        std::vector<std::tuple<std::string, double,double,double>> componentsCOG = {
            {"Wing", valueCOG.xCGWing, valueCOG.yCGWing, valueCOG.zCGWing},
            {"Canard", valueCOG.xCGCanard, valueCOG.yCGCanard, valueCOG.zCGCanard},
            {"Horizontal Tail", valueCOG.xCGHorizontal, valueCOG.yCGHorizontal, valueCOG.zCGHorizontal},
            {"Vertical Tail", valueCOG.xCGVertical, valueCOG.yCGVertical, valueCOG.zCGVertical},
            {"Fuselage", valueCOG.xCGFuselage, valueCOG.yCGFuselage, valueCOG.zCGFuselage},
            {"Nacelle", valueCOG.xCGNacelle, valueCOG.yCGNacelle, valueCOG.zCGNacelle},
            {"Boom", valueCOG.xCGBoom, valueCOG.yCGBoom, valueCOG.zCGBoom},
            {"EOIR", valueCOG.xCGEOIR, valueCOG.yCGEOIR, valueCOG.zCGEOIR},
            {"Landing Gear", valueCOG.xCGLandingGear, valueCOG.yCGLandingGear, valueCOG.zCGLandingGear},
            {"Control Surfaces", valueCOG.xCGControlSurfaces, valueCOG.yCGControlSurfaces, valueCOG.zCGControlSurfaces},
            {"Propulsion Group", valueCOG.xCGPropulsionGroup, valueCOG.yCGPropulsionGroup, valueCOG.zCGPropulsionGroup},
            {"APU", valueCOG.xCGAPU, valueCOG.yCGAPU, valueCOG.zCGAPU},
            {"Instruments", valueCOG.xCGInstruments, valueCOG.yCGInstruments, valueCOG.zCGInstruments},
            {"Hydraulic And Pneumatic", valueCOG.xCGHydraulicAndPneumatic, valueCOG.yCGHydraulicAndPneumatic, valueCOG.zCGHydraulicAndPneumatic},
            {"Electrical Group", valueCOG.xCGElectricalGroup, valueCOG.yCGElectricalGroup, valueCOG.zCGElectricalGroup},
            {"Avionic Group", valueCOG.xCGAvionicGroup, valueCOG.yCGAvionicGroup, valueCOG.zCGAvionicGroup},
            {"Furnishings And Equipment", valueCOG.xCGFurnishingsAndEquipment, valueCOG.yCGFurnishingsAndEquipment, valueCOG.zCGFurnishingsAndEquipment},
            {"Air Conditioning And Anti Ice", valueCOG.xCGAirConditioningAndAntiIce, valueCOG.yCGAirConditioningAndAntiIce, valueCOG.zCGAirConditioningAndAntiIce},
            {"Operating Items", valueCOG.xCGOperatingItems, valueCOG.yCGOperatingItems, valueCOG.zCGOperatingItems},
            {"Payload", valueCOG.xCGPayload, valueCOG.yCGPayload, valueCOG.zCGPayload},
            {"Crew", valueCOG.xCGCrew, valueCOG.yCGCrew, valueCOG.zCGCrew},
            {"Fuel", valueCOG.xCGFuel, valueCOG.yCGFuel, valueCOG.zCGFuel},
            {"Total Aircraft", valueCOG.xCG, valueCOG.yCG, valueCOG.zCG}};

        std::vector<std::tuple<std::string, double, double, double>> nonZeroBalance;
        for (const auto &balanceOfcomponent : componentsCOG)
        {
            if (std::get<1>(balanceOfcomponent) != 0.0 ||
                std::get<2>(balanceOfcomponent) != 0.0 ||
                std::get<3>(balanceOfcomponent) != 0.0)
            {
                nonZeroBalance.push_back({std::get<0>(balanceOfcomponent), std::get<1>(balanceOfcomponent), std::get<2>(balanceOfcomponent), std::get<3>(balanceOfcomponent)});
            }
        }

        return nonZeroBalance;
    }


    // =========================================================
    // Helper: common lambdas
    // =========================================================

    /**
     * @brief Writes component rows (weights or balance) starting from the provided row index.
     * @param sheet Target worksheet.
     * @param rowIdx In/out row index updated while writing.
     * @param setTypeOfWriteRow Row type selector (0 = weights, non-zero = balance).
     */
    void writeRow(XLWorksheet &sheet, int &rowIdx, int setTypeOfWriteRow = 0)
    {

        std::vector<std::pair<std::string, double>> writeWeightsDifferentFromZero = getNonZeroWeights(componentWeight);
        std::vector<std::tuple<std::string, double, double, double>> writeBalanceDifferentFromZero = getNonZeroBalance(componentBalance);

        if (setTypeOfWriteRow == 0)
        {
            for (const auto &component : writeWeightsDifferentFromZero)
            {

                sheet.cell("A" + std::to_string(rowIdx)).value() = component.first;
                sheet.cell("B" + std::to_string(rowIdx)).value() = component.second;
                rowIdx++;
            }
        }

        else if (setTypeOfWriteRow != 0)
        {

            for (const auto &component : writeBalanceDifferentFromZero)
            {

                sheet.cell("A" + std::to_string(rowIdx)).value() = std::get<0>(component);
                sheet.cell("B" + std::to_string(rowIdx)).value() = std::get<1>(component);
                sheet.cell("C" + std::to_string(rowIdx)).value() = std::get<2>(component);
                sheet.cell("D" + std::to_string(rowIdx)).value() = std::get<3>(component);
                rowIdx++;
            }
        }
    }

    /**
     * @brief Sets worksheet column width based on the longest string among provided contents.
     * @param sheet Target worksheet.
     * @param col Column label (for example "A").
     * @param contents Candidate strings used to compute width.
     */
    void setColumnWidth(XLWorksheet &sheet,
                        const std::string &col,
                        const std::vector<std::string> &contents)
    {
        size_t maxLen = 0;
        for (const auto &value : contents)
            maxLen = std::max(maxLen, value.size());
        sheet.column(col).setWidth(static_cast<int>(maxLen) + 2);
    }

    /**
     * @brief Writes the common sheet header with aircraft name and COG reference values.
     * @param sheet Target worksheet.
     * @param nameOfAircraft Aircraft name.
     * @param settings Aerodynamic settings containing COG reference and MAC.
     */
    void writeHeader(XLWorksheet &sheet, const std::string &nameOfAircraft, VSP::AeroSettings &settings)
    {
        sheet.cell("A1").value() = "Aircraft Name:";
        sheet.cell("A2").value() = nameOfAircraft;
        sheet.cell("B1").value() = "Xcg(from the nose) (m):";
        sheet.cell("B2").value() = settings.X_cg;
        sheet.cell("C1").value() = "Ycg(from the nose) (m):";
        sheet.cell("C2").value() = settings.Y_cg;
        sheet.cell("D1").value() = "Zcg(from the nose) (m):";
        sheet.cell("D2").value() = settings.Z_cg;
        sheet.cell("E1").value() = "MAC (m):";
        sheet.cell("E2").value() = settings.Cref;
    }

    // =========================================================
    // Sheet writers
    // =========================================================

    /**
     * @brief Writes the component-weights worksheet.
     * @param sheet Target worksheet.
     */
    void writeComponentWeight(XLWorksheet &sheet)
    {
        sheet.setName("COMPONENTS_WEIGHTS");
        writeHeader(sheet, nameOfAircraft, settings);

        int rowIdx = indexToStartWriteDerivativeTable;

        sheet.cell("A4").value() = "ID";
        sheet.cell("B4").value() = "Value (kg)";
        // sheet.cell("C4").value() = "Value";

        writeRow(sheet, rowIdx);

        setColumnWidth(sheet, "A", {"Air Conditioning And Anti Ice"});
        setColumnWidth(sheet, "B", {"Value (kg)"});
    }

    /**
     * @brief Writes the component-balance worksheet.
     * @param sheet Target worksheet.
     */
    void writeBalanceSheet(XLWorksheet &sheet)
    {
        sheet.setName("BALANCE");
        writeHeader(sheet, nameOfAircraft, settings);

        int rowIdx = indexToStartWriteDerivativeTable;

        sheet.cell("A4").value() = "ID";
        sheet.cell("B4").value() = "Xcg (%fuselage length)";
        sheet.cell("C4").value() = "Ycg (%semi-span length)";
        sheet.cell("D4").value() = "Zcg (%fuselage diameter)";


        writeRow(sheet, rowIdx,1);

        setColumnWidth(sheet, "A", {"Air Conditioning And Anti Ice"});
        setColumnWidth(sheet, "B", {"Xcg(from the nose) (m)"});
        setColumnWidth(sheet, "C", {"Ycg(from the nose) (m)"});
        setColumnWidth(sheet, "D", {"Zcg(from the nose) (m)"});
        // sheet.cell("C4").value() = "Value";
    }

    // void writeSideForceSheet(XLWorksheet &sheet,
    //                          DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent &sideComponents,
    //                          DIRECTIONAL_STABILITY::DirectionalStabilityDerivatives &dirAircraft)
    // {
    //     sheet.setName("DIR_STATIC_SIDE_DERIVATIVES");
    //     writeHeader(sheet, nameOfAircraft, settings);

    //     int rowIdx = indexToStartWriteDerivativeTable;
    //     sheet.cell("A4").value() = "ID - Beta Related";
    //     sheet.cell("B4").value() = "Unit";
    //     sheet.cell("C4").value() = "Value";

    //     if (!isEmpty(sideComponents))
    //     {
    //         writeRow(sheet, rowIdx, "Cy_beta_wing", "1/deg", sideComponents.deltaCyDeltaBetaWingContribution);
    //         writeRow(sheet, rowIdx, "Cy_beta_canard", "1/deg", sideComponents.deltaCyDeltaBetaCanardContribution);
    //         writeRow(sheet, rowIdx, "Cy_beta_horizontal_tail", "1/deg", sideComponents.deltaCyDeltaBetaHorizontalTailContribution);
    //         writeRow(sheet, rowIdx, "Cy_beta_vertical_tail", "1/deg", sideComponents.deltaCyDeltaBetaVerticalTailContribution);
    //         writeRow(sheet, rowIdx, "Cy_beta_fuselage", "1/deg", sideComponents.deltaCyDeltaBetaFuselageContribution);

    //         if (settings.IncludePropToAnlysis == "Yes")
    //             writeRow(sheet, rowIdx, "Cy_beta_propeller_windmilling", "1/deg", sideComponents.deltaCyDeltaBetaWindMillingPropeller);
    //     }

    //     if (!isEmpty(dirAircraft))
    //         writeRow(sheet, rowIdx, "TOTAL CY_beta_Aircraft", "1/deg", dirAircraft.deltaCyDeltaBetaAircraft);

    //     setColumnWidth(sheet, "A", {"ID - Beta Related", "Cy_beta_wing", "Cy_beta_canard",
    //                                 "Cy_beta_horizontal_tail", "Cy_beta_vertical_tail",
    //                                 "Cy_beta_fuselage", "Cy_beta_propeller_windmilling",
    //                                 "TOTAL CY_beta_Aircraft"});
    //     setColumnWidth(sheet, "B", {"Unit", "1/deg"});
    //     setColumnWidth(sheet, "C", {"Value"});
    // }

public:
    /**
     * @brief Default constructor.
     */
    WeightsAndBalanceExcelWriter() {}

    /**
     * @brief Writes weights and balance tables to an Excel file.
     * @param fileName Output Excel file name.
     * @param nameOfAircraft Aircraft name.
     * @param settings Aerodynamic settings.
     * @param componentWeight Component weights data.
     * @param componentBalance Component COG/balance data.
     * @param pathToSaveExcel Output directory path (optional; defaults to current path + "\\ExcelResults").
     */
    void writeWeightsDataAndBalanceToExcel(
        const std::string fileName,
        const std::string nameOfAircraft,
        VSP::AeroSettings settings,
        COG::Weights componentWeight,
        COG::COGDATA componentBalance,
        std::string pathToSaveExcel = "")
    {
        this->fileName = fileName;
        this->nameOfAircraft = nameOfAircraft;
        this->settings = settings;
        this->componentWeight = componentWeight;
        this->componentBalance = componentBalance;


        // ── Determina il path ──────────────────────────────────────────
        if (pathToSaveExcel.empty())
            pathToSaveExcel = std::filesystem::current_path().string() + "\\ExcelResults\\" + fileName;
        else
            pathToSaveExcel = pathToSaveExcel + "\\" + fileName;

        if (std::filesystem::exists(pathToSaveExcel))
            std::filesystem::remove(pathToSaveExcel);

        // // ── Determina quali sezioni hanno dati ────────────────────────
        // // Una sezione è valida se ALMENO una delle sue due struct ha dati
        // bool hasLongitudinal = !isEmpty(longitudinalDerivativesComponents) ||
        //                        !isEmpty(longitudinalDerivativesAircraft)   ||
        //                        !isEmpty(dynamicDerivativesAircraft);

        // bool hasSideForce    = !isEmpty(directionalDerivativesSideForceComponents) ||
        //                        !isEmpty(directionalDerivativesAircraft);

        // bool hasYaw          = !isEmpty(directionalDerivativesYawComponents) ||
        //                        !isEmpty(directionalDerivativesAircraft);

        // bool hasLateral      = !isEmpty(lateralDerivativesRollComponents) ||
        //                        !isEmpty(lateralDerivativesAircraft);

        // // Caso estremo: nessun dato → non creare il file
        // if (!hasLongitudinal && !hasSideForce && !hasYaw && !hasLateral)
        // {
        //     std::cerr << "[WARNING] DerivativeExcelWriter: no data to write, file not created." << std::endl;
        //     return;
        // }

        XLDocument doc;
        doc.create(pathToSaveExcel, !XLForceOverwrite);

        // ── Usa Sheet1 per la prima sezione non vuota ─────────────────
        // OpenXLSX crea sempre Sheet1 di default, quindi lo riusiamo
        // per la prima sezione disponibile invece di cancellarlo.

        bool firstSheetUsed = false;

        auto getOrAddSheet = [&](const std::string &sheetName) -> XLWorksheet
        {
            if (!firstSheetUsed)
            {
                firstSheetUsed = true;
                auto sheet = doc.workbook().worksheet("Sheet1");
                sheet.setName(sheetName);
                return sheet;
            }
            else
            {
                doc.workbook().addWorksheet(sheetName);
                return doc.workbook().worksheet(sheetName);
            }
        };

        auto weightsSheet = getOrAddSheet("COMPONENTS_WEIGHTS");
        writeComponentWeight(weightsSheet);

        auto balanceSheet = getOrAddSheet("BALANCE");
        writeBalanceSheet(balanceSheet);

        // ── Scrittura in ordine, solo sezioni non vuote ───────────────

        // if (hasLongitudinal)
        // {
        //     auto sheet = getOrAddSheet("LONGITUDINAL_STATIC_DERIVATIVES");
        //     writeLongitudinalSheet(sheet,
        //                            longitudinalDerivativesComponents,
        //                            longitudinalDerivativesAircraft,
        //                            dynamicDerivativesAircraft);
        // }

        // if (hasSideForce)
        // {
        //     auto sheet = getOrAddSheet("DIR_STATIC_SIDE_DERIVATIVES");
        //     writeSideForceSheet(sheet,
        //                         directionalDerivativesSideForceComponents,
        //                         directionalDerivativesAircraft);
        // }

        // if (hasYaw)
        // {
        //     auto sheet = getOrAddSheet("DIR_STATIC_YAW_DERIVATIVES");
        //     writeYawSheet(sheet,
        //                   directionalDerivativesYawComponents,
        //                   directionalDerivativesAircraft);
        // }

        // if (hasLateral)
        // {
        //     auto sheet = getOrAddSheet("DIR_STATIC_ROLL_DERIVATIVES");
        //     writeLateralSheet(sheet,
        //                       lateralDerivativesRollComponents,
        //                       lateralDerivativesAircraft);
        // }

        doc.save();
        doc.close();
    }
};