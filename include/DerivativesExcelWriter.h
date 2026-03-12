#pragma once

#include "OpenXLSX/OpenXLSX.hpp"
#include "LONGITUDINALSTABILTY.h"
#include "DIRECTIONALSTABILITY.h"
#include "LATERALSTABILITY.h"
#include "VSPAeroGenerator.h"
#include <filesystem>
#include <functional>

using namespace OpenXLSX;

class DerivativeExcelWriter
{

protected:
    VSP::AeroSettings settings;

private:
    std::string fileName = "";
    std::string nameOfAircraft = "";

    // Format settings
    int indexToStartWriteDerivativeTable = 5;

    // =========================================================
    // Helper: check if structs are empty (all values == 0.0)
    // =========================================================

    bool isEmpty(const LONGITUDINAL_STABILITY::LongitudinalStabilityDerivativesToSingleComponent &value) const
    {
        return value.deltaCmDeltaAlphaWing == 0.0 &&
               value.deltaCmDeltaAlphaCanard == 0.0 &&
               value.deltaCmDeltaAlphaHorizontalTail == 0.0 &&
               value.deltaCmDeltaAlphaFuselage == 0.0 &&
               value.deltaCmDeltaAlphaNacelle == 0.0;
    }

    bool isEmpty(const DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent &value) const
    {
        return value.deltaCyDeltaBetaWingContribution == 0.0 &&
               value.deltaCyDeltaBetaCanardContribution == 0.0 &&
               value.deltaCyDeltaBetaHorizontalTailContribution == 0.0 &&
               value.deltaCyDeltaBetaVerticalTailContribution == 0.0 &&
               value.deltaCyDeltaBetaFuselageContribution == 0.0 &&
               value.deltaCyDeltaBetaWindMillingPropeller == 0.0;
    }

    bool isEmpty(const DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesYawToSingleComponent &value) const
    {
        return value.deltaCnDeltaBetaWingContribution == 0.0 &&
               value.deltaCnDeltaBetaCanardContribution == 0.0 &&
               value.deltaCnDeltaBetaHorizontalTailContribution == 0.0 &&
               value.deltaCnDeltaBetaVerticalTailContribution == 0.0 &&
               value.deltaCnDeltaBetaFuselageContribution == 0.0 &&
               value.deltaCnDeltaBetaNacelleContribution == 0.0;
    }

    bool isEmpty(const LATERAL_STABILITY::LateralStabilityDerivativesRollToSingleComponent &value) const
    {
        return value.deltaClDeltaBetaWingFuselageContribution == 0.0 &&
               value.deltaClDeltaBetaHorizontalTailContribution == 0.0 &&
               value.deltaClDeltaBetaVerticalTailContribution == 0.0;
    }

    bool isEmpty(const LONGITUDINAL_STABILITY::LongitudinalStabilityDerivatives &value) const
    {
        return value.deltaCmDeltaAlphaAircraft == 0.0 &&
               value.deltaCmDeltaClAircraft == 0.0 &&
               value.neutralPointAsFractionOfMAC == 0.0;
    }

    bool isEmpty(const LONGITUDINAL_STABILITY::LongitudinalDynamicDerivatives &value) const
    {
        return value.deltaCLdeltaPitchSpeed == 0.0 &&
               value.deltaCmDeltaPitchSpeed == 0.0 &&
               value.deltaCmDeltaAlphaDot == 0.0 &&
               value.deltaCLDeltaAlphaDot == 0.0;
    }

    bool isEmpty(const DIRECTIONAL_STABILITY::DirectionalStabilityDerivatives &value) const
    {
        return value.deltaCyDeltaBetaAircraft == 0.0 &&
               value.deltaCnDeltaBetaAircraft == 0.0 &&
               value.deltaCnDeltaRudderDeflection == 0.0;
    }

    bool isEmpty(const LATERAL_STABILITY::LateralStabilityDerivatives &value) const
    {
        return value.deltaClDeltaBetaAircraft == 0.0 &&
               value.deltaClBetaDeltaAileronsDeflection == 0.0;
    }

    // =========================================================
    // Helper: common lambdas
    // =========================================================

    void writeRow(XLWorksheet &sheet, int &rowIdx,
                  const std::string &component,
                  const std::string &unit,
                  double value)
    {
        sheet.cell("A" + std::to_string(rowIdx)).value() = component;
        sheet.cell("B" + std::to_string(rowIdx)).value() = unit;
        sheet.cell("C" + std::to_string(rowIdx)).value() = value;
        rowIdx++;
    }

    void setColumnWidth(XLWorksheet &sheet,
                        const std::string &col,
                        const std::vector<std::string> &contents)
    {
        size_t maxLen = 0;
        for (const auto &value : contents)
            maxLen = std::max(maxLen, value.size());
        sheet.column(col).setWidth(static_cast<int>(maxLen) + 2);
    }

    void writeHeader(XLWorksheet &sheet, const std::string &nameOfAircraft, VSP::AeroSettings &settings)
    {
        sheet.cell("A1").value() = "Aircraft Name:";
        sheet.cell("A2").value() = nameOfAircraft;
        sheet.cell("B1").value() = "Mach:";
        sheet.cell("B2").value() = settings.Mach;
        sheet.cell("C1").value() = "Re:";
        sheet.cell("C2").value() = settings.ReCref;
        sheet.cell("D1").value() = "Altitude (m):";
        sheet.cell("D2").value() = settings.altitude;
        sheet.cell("E1").value() = "Xcg(from the nose) (m):";
        sheet.cell("E2").value() = settings.X_cg;
        sheet.cell("F1").value() = "Ycg(from the nose) (m):";
        sheet.cell("F2").value() = settings.Y_cg;
        sheet.cell("G1").value() = "Zcg(from the nose) (m):";
        sheet.cell("G2").value() = settings.Z_cg;
        sheet.cell("H1").value() = "MAC (m):";
        sheet.cell("H2").value() = settings.Cref;
    }

    // =========================================================
    // Sheet writers 
    // =========================================================

    void writeLongitudinalSheet(XLWorksheet &sheet,
                                LONGITUDINAL_STABILITY::LongitudinalStabilityDerivativesToSingleComponent &longComponents,
                                LONGITUDINAL_STABILITY::LongitudinalStabilityDerivatives &longAircraft,
                                LONGITUDINAL_STABILITY::LongitudinalDynamicDerivatives &dynAircraft)
    {
        sheet.setName("LONGITUDINAL_STATIC_DERIVATIVES");
        writeHeader(sheet, nameOfAircraft, settings);

        int rowIdx = indexToStartWriteDerivativeTable;

        sheet.cell("A4").value() = "ID - Alpha Related";
        sheet.cell("B4").value() = "Unit";
        sheet.cell("C4").value() = "Value";

        if (!isEmpty(longComponents))
        {
            writeRow(sheet, rowIdx, "Cm_alpha_wing", "1/deg", longComponents.deltaCmDeltaAlphaWing);
            writeRow(sheet, rowIdx, "Cm_alpha_canard", "1/deg", longComponents.deltaCmDeltaAlphaCanard);
            writeRow(sheet, rowIdx, "Cm_alpha_horizontal_tail", "1/deg", longComponents.deltaCmDeltaAlphaHorizontalTail);
            writeRow(sheet, rowIdx, "Cm_alpha_fuselage", "1/deg", longComponents.deltaCmDeltaAlphaFuselage);
            writeRow(sheet, rowIdx, "Cm_alpha_nacelle", "1/deg", longComponents.deltaCmDeltaAlphaNacelle);
        }

        if (!isEmpty(longAircraft))
        {
            writeRow(sheet, rowIdx, "TOTAL CM_alpha_Aircraft", "1/deg", longAircraft.deltaCmDeltaAlphaAircraft);
            rowIdx += 2;
            sheet.cell("A" + std::to_string(rowIdx - 1)).value() = "ID - SSM Related";
            sheet.cell("B" + std::to_string(rowIdx - 1)).value() = "Unit";
            sheet.cell("C" + std::to_string(rowIdx - 1)).value() = "Value";
            writeRow(sheet, rowIdx, "Static Stability Margin", "-", longAircraft.deltaCmDeltaClAircraft);
            writeRow(sheet, rowIdx, "Neutral point as fraction of MAC", "-", longAircraft.neutralPointAsFractionOfMAC);
        }

        if (!isEmpty(dynAircraft))
        {
            rowIdx += 2;
            sheet.cell("A" + std::to_string(rowIdx - 1)).value() = "ID - Dynamic Related";
            sheet.cell("B" + std::to_string(rowIdx - 1)).value() = "Unit";
            sheet.cell("C" + std::to_string(rowIdx - 1)).value() = "Value";
            writeRow(sheet, rowIdx, "CL_q", "-", dynAircraft.deltaCLdeltaPitchSpeed);
            writeRow(sheet, rowIdx, "CM_q", "-", dynAircraft.deltaCmDeltaPitchSpeed);
            writeRow(sheet, rowIdx, "CM_alpha_dot", "1/deg", dynAircraft.deltaCmDeltaAlphaDot);
            writeRow(sheet, rowIdx, "CL_alpha_dot", "1/deg", dynAircraft.deltaCLDeltaAlphaDot);
        }

        setColumnWidth(sheet, "A", {"ID - Alpha Related", "Cm_alpha_wing", "Cm_alpha_canard",
                                    "Cm_alpha_horizontal_tail", "Cm_alpha_fuselage", "Cm_alpha_nacelle",
                                    "TOTAL CM_alpha_Aircraft", "ID - SSM Related",
                                    "Static Stability Margin", "Neutral point as fraction of MAC",
                                    "ID - Dynamic Related", "CL_q", "CM_q", "CM_alpha_dot", "CL_alpha_dot"});
        setColumnWidth(sheet, "B", {"Unit", "1/deg"});
        setColumnWidth(sheet, "C", {"Value"});
    }

    void writeSideForceSheet(XLWorksheet &sheet,
                             DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent &sideComponents,
                             DIRECTIONAL_STABILITY::DirectionalStabilityDerivatives &dirAircraft)
    {
        sheet.setName("DIR_STATIC_SIDE_DERIVATIVES");
        writeHeader(sheet, nameOfAircraft, settings);

        int rowIdx = indexToStartWriteDerivativeTable;
        sheet.cell("A4").value() = "ID - Beta Related";
        sheet.cell("B4").value() = "Unit";
        sheet.cell("C4").value() = "Value";

        if (!isEmpty(sideComponents))
        {
            writeRow(sheet, rowIdx, "Cy_beta_wing", "1/deg", sideComponents.deltaCyDeltaBetaWingContribution);
            writeRow(sheet, rowIdx, "Cy_beta_canard", "1/deg", sideComponents.deltaCyDeltaBetaCanardContribution);
            writeRow(sheet, rowIdx, "Cy_beta_horizontal_tail", "1/deg", sideComponents.deltaCyDeltaBetaHorizontalTailContribution);
            writeRow(sheet, rowIdx, "Cy_beta_vertical_tail", "1/deg", sideComponents.deltaCyDeltaBetaVerticalTailContribution);
            writeRow(sheet, rowIdx, "Cy_beta_fuselage", "1/deg", sideComponents.deltaCyDeltaBetaFuselageContribution);

            if (settings.IncludePropToAnlysis == "Yes")
                writeRow(sheet, rowIdx, "Cy_beta_propeller_windmilling", "1/deg", sideComponents.deltaCyDeltaBetaWindMillingPropeller);
        }

        if (!isEmpty(dirAircraft))
            writeRow(sheet, rowIdx, "TOTAL CY_beta_Aircraft", "1/deg", dirAircraft.deltaCyDeltaBetaAircraft);

        setColumnWidth(sheet, "A", {"ID - Beta Related", "Cy_beta_wing", "Cy_beta_canard",
                                    "Cy_beta_horizontal_tail", "Cy_beta_vertical_tail",
                                    "Cy_beta_fuselage", "Cy_beta_propeller_windmilling",
                                    "TOTAL CY_beta_Aircraft"});
        setColumnWidth(sheet, "B", {"Unit", "1/deg"});
        setColumnWidth(sheet, "C", {"Value"});
    }

    void writeYawSheet(XLWorksheet &sheet,
                       DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesYawToSingleComponent &yawComponents,
                       DIRECTIONAL_STABILITY::DirectionalStabilityDerivatives &dirAircraft)
    {
        sheet.setName("DIR_STATIC_YAW_DERIVATIVES");
        writeHeader(sheet, nameOfAircraft, settings);

        int rowIdx = indexToStartWriteDerivativeTable;
        sheet.cell("A4").value() = "ID - Beta Related";
        sheet.cell("B4").value() = "Unit";
        sheet.cell("C4").value() = "Value";

        if (!isEmpty(yawComponents))
        {
            writeRow(sheet, rowIdx, "Cn_beta_wing", "1/deg", yawComponents.deltaCnDeltaBetaWingContribution);
            writeRow(sheet, rowIdx, "Cn_beta_canard", "1/deg", yawComponents.deltaCnDeltaBetaCanardContribution);
            writeRow(sheet, rowIdx, "Cn_beta_horizontal_tail", "1/deg", yawComponents.deltaCnDeltaBetaHorizontalTailContribution);
            writeRow(sheet, rowIdx, "Cn_beta_vertical_tail", "1/deg", yawComponents.deltaCnDeltaBetaVerticalTailContribution);
            writeRow(sheet, rowIdx, "Cn_beta_fuselage", "1/deg", yawComponents.deltaCnDeltaBetaFuselageContribution);
            writeRow(sheet, rowIdx, "Cn_beta_nacelle", "1/deg", yawComponents.deltaCnDeltaBetaNacelleContribution);
        }

        if (!isEmpty(dirAircraft))
        {
            writeRow(sheet, rowIdx, "CN_delta_r", "1/deg", dirAircraft.deltaCnDeltaRudderDeflection);
            writeRow(sheet, rowIdx, "TOTAL CN_beta_Aircraft", "1/deg", dirAircraft.deltaCnDeltaBetaAircraft);
        }

        setColumnWidth(sheet, "A", {"ID - Beta Related", "Cn_beta_wing", "Cn_beta_canard",
                                    "Cn_beta_horizontal_tail", "Cn_beta_vertical_tail",
                                    "Cn_beta_fuselage", "Cn_beta_nacelle",
                                    "CN_delta_r", "TOTAL CN_beta_Aircraft"});
        setColumnWidth(sheet, "B", {"Unit", "1/deg"});
        setColumnWidth(sheet, "C", {"Value"});
    }

    void writeLateralSheet(XLWorksheet &sheet,
                           LATERAL_STABILITY::LateralStabilityDerivativesRollToSingleComponent &latComponents,
                           LATERAL_STABILITY::LateralStabilityDerivatives &latAircraft)
    {
        sheet.setName("DIR_STATIC_ROLL_DERIVATIVES");
        writeHeader(sheet, nameOfAircraft, settings);

        int rowIdx = indexToStartWriteDerivativeTable;
        sheet.cell("A4").value() = "ID - Beta Related";
        sheet.cell("B4").value() = "Unit";
        sheet.cell("C4").value() = "Value";

        if (!isEmpty(latComponents))
        {
            writeRow(sheet, rowIdx, "Cl_beta_wing_fuselage", "1/deg", latComponents.deltaClDeltaBetaWingFuselageContribution);
            writeRow(sheet, rowIdx, "Cl_beta_horizontal_tail", "1/deg", latComponents.deltaClDeltaBetaHorizontalTailContribution);
            writeRow(sheet, rowIdx, "Cl_beta_vertical_tail", "1/deg", latComponents.deltaClDeltaBetaVerticalTailContribution);
        }

        if (!isEmpty(latAircraft))
        {
            writeRow(sheet, rowIdx, "C\u2112_delta_a", "1/deg", latAircraft.deltaClBetaDeltaAileronsDeflection);
            writeRow(sheet, rowIdx, "TOTAL  C\u2112_beta_Aircraft", "1/deg", latAircraft.deltaClDeltaBetaAircraft);
        }

        setColumnWidth(sheet, "A", {"ID - Beta Related", "Cl_beta_wing_fuselage",
                                    "Cl_beta_horizontal_tail", "Cl_beta_vertical_tail",
                                    "C\u2112_delta_a", "TOTAL  C\u2112_beta_Aircraft"});
        setColumnWidth(sheet, "B", {"Unit", "1/deg"});
        setColumnWidth(sheet, "C", {"Value"});
    }

public:
    DerivativeExcelWriter() {}

    void writeDerivativesToExcel(
        const std::string fileName,
        const std::string nameOfAircraft,
        VSP::AeroSettings settings,
        LONGITUDINAL_STABILITY::LongitudinalStabilityDerivativesToSingleComponent longitudinalDerivativesComponents = {},
        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesSideForceToSingleComponent directionalDerivativesSideForceComponents = {},
        DIRECTIONAL_STABILITY::DirectionalStabilityDerivativesYawToSingleComponent directionalDerivativesYawComponents = {},
        LATERAL_STABILITY::LateralStabilityDerivativesRollToSingleComponent lateralDerivativesRollComponents = {},
        LONGITUDINAL_STABILITY::LongitudinalStabilityDerivatives longitudinalDerivativesAircraft = {},
        LONGITUDINAL_STABILITY::LongitudinalDynamicDerivatives dynamicDerivativesAircraft = {},
        DIRECTIONAL_STABILITY::DirectionalStabilityDerivatives directionalDerivativesAircraft = {},
        LATERAL_STABILITY::LateralStabilityDerivatives lateralDerivativesAircraft = {},
        std::string pathToSaveExcel = "")
    {
        this->fileName = fileName;
        this->nameOfAircraft = nameOfAircraft;
        this->settings = settings;

        // ── Determina il path ──────────────────────────────────────────
        if (pathToSaveExcel.empty())
            pathToSaveExcel = std::filesystem::current_path().string() + "\\ExcelResults\\" + fileName;
        else
            pathToSaveExcel = pathToSaveExcel + "\\" + fileName;

        if (std::filesystem::exists(pathToSaveExcel))
            std::filesystem::remove(pathToSaveExcel);

        // ── Determina quali sezioni hanno dati ────────────────────────
        // Una sezione è valida se ALMENO una delle sue due struct ha dati
        bool hasLongitudinal = !isEmpty(longitudinalDerivativesComponents) ||
                               !isEmpty(longitudinalDerivativesAircraft)   ||
                               !isEmpty(dynamicDerivativesAircraft);

        bool hasSideForce    = !isEmpty(directionalDerivativesSideForceComponents) ||
                               !isEmpty(directionalDerivativesAircraft);

        bool hasYaw          = !isEmpty(directionalDerivativesYawComponents) ||
                               !isEmpty(directionalDerivativesAircraft);

        bool hasLateral      = !isEmpty(lateralDerivativesRollComponents) ||
                               !isEmpty(lateralDerivativesAircraft);

        // Caso estremo: nessun dato → non creare il file
        if (!hasLongitudinal && !hasSideForce && !hasYaw && !hasLateral)
        {
            std::cerr << "[WARNING] DerivativeExcelWriter: no data to write, file not created." << std::endl;
            return;
        }

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

        // ── Scrittura in ordine, solo sezioni non vuote ───────────────

        if (hasLongitudinal)
        {
            auto sheet = getOrAddSheet("LONGITUDINAL_STATIC_DERIVATIVES");
            writeLongitudinalSheet(sheet,
                                   longitudinalDerivativesComponents,
                                   longitudinalDerivativesAircraft,
                                   dynamicDerivativesAircraft);
        }

        if (hasSideForce)
        {
            auto sheet = getOrAddSheet("DIR_STATIC_SIDE_DERIVATIVES");
            writeSideForceSheet(sheet,
                                directionalDerivativesSideForceComponents,
                                directionalDerivativesAircraft);
        }

        if (hasYaw)
        {
            auto sheet = getOrAddSheet("DIR_STATIC_YAW_DERIVATIVES");
            writeYawSheet(sheet,
                          directionalDerivativesYawComponents,
                          directionalDerivativesAircraft);
        }

        if (hasLateral)
        {
            auto sheet = getOrAddSheet("DIR_STATIC_ROLL_DERIVATIVES");
            writeLateralSheet(sheet,
                              lateralDerivativesRollComponents,
                              lateralDerivativesAircraft);
        }

        doc.save();
        doc.close();
    }
};