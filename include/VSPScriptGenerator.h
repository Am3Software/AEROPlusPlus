#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace VSP
{

        // ==================== STRUTTURE DATI ====================

        struct Wing
        {
                std::string id;
                std::string type = "WING";

                // Geometria base
                std::vector<double> span;
                std::vector<double> chord;
                std::vector<double> sweep;
                std::vector<double> dihedral;
                std::vector<double> twist;

                // CAMPI PER AIRFOIL
                std::string airfoilType = "XS_FOUR_SERIES"; // "XS_FOUR_SERIES", "XS_FIVE_DIGIT", "XS_SIX_SERIES", "XS_FILE_AIRFOIL"
                std::vector<double> camber;                 // Per NACA 4-digit
                std::vector<double> camberloc;              // Posizione massima camber
                std::vector<double> thickchord;             // Spessore/corda
                std::vector<double> idealcl;                // Per NACA 5 e 6-digit
                std::vector<int> series;                    // Per NACA 6-series (63, 64, 65, etc.)
                std::vector<double> A;                      // Parametro A per NACA 6-series
                std::vector<std::string> affile;            // File airfoil esterni
                std::vector<double> twistloc;               // Posizione twist

                // NUOVI CAMPI PER PANEL (alternativa a chord/sweep/dihedral)
                bool useDetailedPanels = false;  // Se true, usa croot/ctip invece di chord
                std::vector<double> croot;       // Corda root per ogni pannello
                std::vector<double> ctip;        // Corda tip per ogni pannello
                std::vector<double> sweeploc;    // Posizione sweep
                std::vector<double> secsweeploc; // Seconda posizione sweep
                std::vector<int> sectessU;       // Tessellation spanwise per sezione

                // Posizione
                double xloc = 0;
                double yloc = 0;
                double zloc = 0;

                // Rotazione
                double xrot = 0;
                double yrot = 0;
                double zrot = 0;

                // Tessellation globale
                int utess = 20;
                int wtess = 20;

                // Movables
                struct Movable
                {
                        std::vector<char> type;
                        std::vector<double> eta_inner;
                        std::vector<double> eta_outer;
                        std::vector<double> cf_c_inner;
                        std::vector<double> cf_c_outer;
                        std::vector<int> tessellation;
                        std::vector<double> defl;
                } mov;

                // TESSELLATION AGGIUNTIVA
                int captess = 9;
                double lecluster = 0.15;
                double tecluster = 0.15;

                // CAP TYPE
                bool capTreatment = false;
                std::string capType = "Flat";
                double capLength = 1.0;
                double capOffset = 0.0;
                double capStrength = 0.5;

                // BLENDING
                bool blending = false;
                struct BlendParams
                {
                        std::vector<int> InLEMode;
                        std::vector<double> InLESweep;
                        std::vector<double> InLEDihedral;
                        std::vector<double> InLEStrength;

                        std::vector<int> InTEMode;
                        std::vector<double> InTESweep;
                        std::vector<double> InTEDihedral;
                        std::vector<double> InTEStrength;

                        std::vector<int> OutLEMode;
                        std::vector<double> OutLESweep;
                        std::vector<double> OutLEDihedral;
                        std::vector<double> OutLEStrength;

                        std::vector<int> OutTEMode;
                        std::vector<double> OutTESweep;
                        std::vector<double> OutTEDihedral;
                        std::vector<double> OutTEStrength;
                } blend;
        };

        struct Fuselage
        {
                std::string id;
                std::string type;
                double length;
                double diameter;
                int utess;
                int wtess;
                bool customFuselage = false;
                bool advancedFuselage = false;

                double nosemult = 0;
                double aftmult = 0;
                double nosecenter = 0;
                double aftcenter = 0;
                double aftwidth = 0;
                double aftheight = 0;

                // ADVANCED FUSELAGE FIELDS
                std::string fuselagePresetName;
                std::string folderName = "FuselagePreset";
                double width = 0;
                int counterBlendedEngine = 0;

                // UAVDomeFuselagePreset specific
                struct Dome
                {
                        double diameter = 0;
                        double width = 0;
                } dome;

                struct Body
                {
                        double diameter = 0;
                        double width = 0;
                } body;

                struct Tail
                {
                        double diameter = 0;
                } tail;

                struct Nose
                {
                        double zLocPercent = 0;
                } nose;

                struct AttachDomeBody
                {
                        double diameter = 0;
                        double xLocPercent = 0;
                } attachDomeBody;
        };

        struct Nacelle
        {
                std::string id;
                std::string type;
                double width;
                double height;
                double chord;
                double thickchord;
                std::vector<double> xloc;
                std::vector<double> yloc;
                std::vector<double> zloc;
                std::vector<double> yrot;
                int utess;
                int wtess;

                bool advancedNacelle = false;
                std::string nacellePresetName;
                double length = 0;
                double diameter = 0;
        };

        struct Pod
        {
                std::string id;
                std::string type;
                std::vector<double> length;
                std::vector<double> fineratio;
                std::vector<double> xloc;
                std::vector<double> yloc;
                std::vector<double> zloc;
                std::vector<double> yrot;
                int utess;
                int wtess;
        };

        struct Boom
        {
                std::string id;
                std::string type;
                std::vector<double> length;
                std::vector<double> fineratio;
                std::vector<double> xloc;
                std::vector<double> yloc;
                std::vector<double> zloc;
                std::vector<double> yrot;
                int utess;
                int wtess;
        };

        struct EOIR
        {
                std::string id;
                std::string type;
                std::vector<double> length;
                std::vector<double> fineratio;
                std::vector<double> xloc;
                std::vector<double> yloc;
                std::vector<double> zloc;
                std::vector<double> yrot;
                int utess;
                int wtess;
        };

        // ==================== CLASSE PRINCIPALE ====================

        class ScriptGenerator
        {
        private:
                std::ofstream file;
                std::string parentFolder;

                inline void writeComment(const std::string &comment)
                {
                        file << "// " << comment << "\r\n\r\n";
                }

                inline void writeCommand(const std::string &command)
                {
                        file << command << "\r\n";
                }

                inline void writeUpdate()
                {
                        file << "Update();\r\n\r\n";
                }

                // Funzione helper per sostituire backslash con double backslash
                inline std::string replaceBackslash(const std::string &str)
                {
                        std::string result;
                        for (char c : str)
                        {
                                if (c == '\\')
                                {
                                        result += "\\\\";
                                }
                                else
                                {
                                        result += c;
                                }
                        }
                        return result;
                }

                // ============= SHAPE AIRFOIL =============
                inline void shapeAirfoil(const Wing &wing, int sectionIndex)
                {
                        writeComment(wing.id + " airfoil " + std::to_string(sectionIndex));

                        int idx = sectionIndex - 1;

                        if (wing.airfoilType == "XS_FOUR_SERIES")
                        {
                                if (idx < wing.camber.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"Camber\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.camber[idx]) + ");");
                                }
                                if (idx < wing.camberloc.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"CamberLoc\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.camberloc[idx]) + ");");
                                }
                                if (idx < wing.thickchord.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"ThickChord\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.thickchord[idx]) + ");");
                                        file << "\r\n";
                                }
                        }
                        else if (wing.airfoilType == "XS_FIVE_DIGIT")
                        {
                                if (sectionIndex <= 2)
                                {
                                        writeCommand("ChangeXSecShape(GetXSecSurf(" + wing.id + "," +
                                                     std::to_string(idx) + ")," + std::to_string(idx) + ",XS_FIVE_DIGIT);");
                                        writeUpdate();
                                }

                                if (idx < wing.idealcl.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"IdealCl\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.idealcl[idx]) + ");");
                                }
                                if (idx < wing.camberloc.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"CamberLoc\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.camberloc[idx]) + ");");
                                }
                                if (idx < wing.thickchord.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"ThickChord\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.thickchord[idx]) + ");");
                                        file << "\r\n";
                                }
                        }
                        else if (wing.airfoilType == "XS_SIX_SERIES")
                        {
                                writeCommand("ChangeXSecShape(GetXSecSurf(" + wing.id + "," +
                                             std::to_string(idx) + ")," + std::to_string(idx) + ",XS_SIX_SERIES);");
                                writeUpdate();

                                if (idx < wing.series.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"Series\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.series[idx] - 1) + ");");
                                        file << "\r\n";
                                }
                                if (idx < wing.thickchord.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"ThickChord\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.thickchord[idx]) + ");");
                                        file << "\r\n";
                                }
                                if (idx < wing.idealcl.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"IdealCl\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.idealcl[idx]) + ");");
                                }
                                if (idx < wing.A.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"A\",\"XSecCurve_" +
                                                     std::to_string(idx) + "\"," + std::to_string(wing.A[idx]) + ");");
                                }
                        }
                        else if (wing.airfoilType == "XS_FILE_AIRFOIL")
                        {
                                if (idx < wing.affile.size())
                                {
                                        writeCommand("ReadFileAirfoil(GetXSec(GetXSecSurf(" + wing.id +
                                                     ",1)," + std::to_string(idx) + "),\"" + wing.affile[idx] + "\");");

                                        if (idx < wing.thickchord.size())
                                        {
                                                writeCommand("SetParmVal(" + wing.id + ",\"ThickChord\",\"XSecCurve_" +
                                                             std::to_string(idx) + "\"," + std::to_string(wing.thickchord[idx]) + ");");
                                                file << "\r\n";
                                        }
                                }
                        }

                        if (sectionIndex < wing.thickchord.size())
                        {
                                if (sectionIndex - 1 < wing.twist.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"Twist\",\"XSec_" +
                                                     std::to_string(sectionIndex) + "\"," +
                                                     std::to_string(wing.twist[sectionIndex - 1]) + ");");
                                }
                                if (sectionIndex - 1 < wing.twistloc.size())
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"Twist_Location\",\"XSec_" +
                                                     std::to_string(sectionIndex) + "\"," +
                                                     std::to_string(wing.twistloc[sectionIndex - 1]) + ");");
                                }
                                writeUpdate();
                        }

                        writeUpdate();
                }

                // ============= SHAPE PANEL =============
                inline void shapePanel(const Wing &wing, int panelIndex)
                {
                        writeComment(wing.id + " panel " + std::to_string(panelIndex));

                        std::string secID = "XSec_" + std::to_string(panelIndex);

                        writeCommand("SetDriverGroup(" + wing.id + "," + std::to_string(panelIndex) +
                                     ",SPAN_WSECT_DRIVER,ROOTC_WSECT_DRIVER,TIPC_WSECT_DRIVER);");
                        file << "\r\n";

                        int idx = panelIndex - 1;

                        if (idx < wing.croot.size())
                        {
                                writeCommand("SetParmVal(" + wing.id + ",\"Root_Chord\",\"" + secID + "\"," +
                                             std::to_string(wing.croot[idx]) + ");");
                        }
                        if (idx < wing.ctip.size())
                        {
                                writeCommand("SetParmVal(" + wing.id + ",\"Tip_Chord\",\"" + secID + "\"," +
                                             std::to_string(wing.ctip[idx]) + ");");
                        }

                        if (idx < wing.span.size())
                        {
                                writeCommand("SetParmVal(" + wing.id + ",\"Span\",\"" + secID + "\"," +
                                             std::to_string(wing.span[idx]) + ");");
                        }

                        if (idx < wing.sweep.size())
                        {
                                writeCommand("SetParmVal(" + wing.id + ",\"Sweep\",\"" + secID + "\"," +
                                             std::to_string(wing.sweep[idx]) + ");");
                        }
                        if (idx < wing.sweeploc.size())
                        {
                                writeCommand("SetParmVal(" + wing.id + ",\"Sweep_Location\",\"" + secID + "\"," +
                                             std::to_string(wing.sweeploc[idx]) + ");");
                        }
                        if (idx < wing.secsweeploc.size())
                        {
                                writeCommand("SetParmVal(" + wing.id + ",\"Sec_Sweep_Location\",\"" + secID + "\"," +
                                             std::to_string(wing.secsweeploc[idx]) + ");");
                        }

                        if (idx < wing.dihedral.size())
                        {
                                writeCommand("SetParmVal(" + wing.id + ",\"Dihedral\",\"" + secID + "\"," +
                                             std::to_string(wing.dihedral[idx]) + ");");
                                file << "\r\n";
                        }

                        if (idx < wing.sectessU.size())
                        {
                                writeCommand("SetParmVal(" + wing.id + ",\"SectTess_U\",\"" + secID + "\"," +
                                             std::to_string(wing.sectessU[idx]) + ");");
                        }

                        writeUpdate();
                }

        public:
                inline ScriptGenerator(const std::string &filename, const std::string &parentFolderPath = "")
                {
                        file.open(filename);
                        if (!file.is_open())
                        {
                                throw std::runtime_error("Cannot open file: " + filename);
                        }

                        if (parentFolderPath.empty())
                        {
                                parentFolder = std::filesystem::current_path().string();
                        }
                        else
                        {
                                parentFolder = parentFolderPath;
                        }
                        file << "void main(){\r\n";
                }

                inline ~ScriptGenerator()
                {
                }

                // ============= MAKE ADVANCED FUSELAGE =============
                inline void makeAdvancedFuselage(const Fuselage &fuselage)
                {
                        if (!fuselage.advancedFuselage)
                        {
                                writeComment("### Advanced Fuselage is not selected... ###");
                                return;
                        }

                        std::string cwd = parentFolder + "\\" + fuselage.folderName;
                        std::string cwdEscaped = replaceBackslash(cwd);

                        if (fuselage.fuselagePresetName == "UAVDomeFuselagePreset")
                        {
                                // LOADING FUSELAGE PRESET
                                writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + 
                                           fuselage.fuselagePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("ReadVSPFile( fnamePreset );");
                                file << "\r\n";

                                writeCommand("string fuselageName = \"fuselage\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids = FindGeomsWithName( fuselageName );");
                                file << "\r\n";

                                writeCommand("string fuselage = geom_ids[0];");
                                file << "\r\n";

                                writeUpdate();

                                // REFERENCE GEOMETRY VALUES - Heights
                                writeCommand("double refFuselageLength = 11.03376;");
                                file << "\r\n";

                                writeCommand("double refNoseDiameter = 0.57000;");
                                file << "\r\n";

                                writeCommand("double refDomeDiameter = 1.24000;");
                                file << "\r\n";

                                writeCommand("double refAttachDomeBodyDiameter = 1.0;");
                                file << "\r\n";

                                writeCommand("double refBodyDiameter = 0.88000;");
                                file << "\r\n";

                                writeCommand("double refTailDiameter = 0.60000;");
                                file << "\r\n";

                                // Reference widths
                                writeCommand("double refNoseWidth = 0.65000;");
                                file << "\r\n";

                                writeCommand("double refDomeWidth = 1.1;");
                                file << "\r\n";

                                writeCommand("double refAttachDomeBodyWidth = 1.1;");
                                file << "\r\n";

                                writeCommand("double refBodyWidth = 1.1;");
                                file << "\r\n";

                                writeCommand("double refTailWidth = 0.65000;");
                                file << "\r\n";

                                // MATLAB INPUT
                                writeCommand("double fuselageLength = " + std::to_string(fuselage.length) + ";");
                                file << "\r\n";

                                writeCommand("double domeDiameter   = " + std::to_string(fuselage.dome.diameter) + ";");
                                file << "\r\n";

                                writeCommand("double bodyDiameter   = " + std::to_string(fuselage.body.diameter) + ";");
                                file << "\r\n";

                                writeCommand("double tailDiameter   = " + std::to_string(fuselage.tail.diameter) + ";");
                                file << "\r\n";

                                writeCommand("double domeWidth      =" + std::to_string(fuselage.dome.width) + ";");
                                file << "\r\n";

                                writeCommand("double bodyWidth      = " + std::to_string(fuselage.body.width) + ";");
                                file << "\r\n";

                                // SCALING FACTORS
                                writeCommand("double  scalingFactorDiameter     = (domeDiameter/refDomeDiameter);");
                                file << "\r\n";

                                writeCommand("double  scalingFactorBodyDiameter = (bodyDiameter/refBodyDiameter);");
                                file << "\r\n";

                                writeCommand("double  scalingFactorWidthDome    = (domeWidth/refDomeWidth);");
                                file << "\r\n";

                                writeCommand("double  scalingFactorWidthBody    = (bodyWidth/refBodyWidth);");
                                file << "\r\n";

                                writeCommand("double  scalingFactorTailDiameter = (tailDiameter/refTailDiameter);");
                                file << "\r\n";

                                // FUSELAGE LENGTH
                                writeCommand("SetParmVal(fuselage,\"Length\",\"Design\",fuselageLength);");
                                file << "\r\n";

                                writeUpdate();

                                // XSec_0
                                writeCommand("SetParmVal(fuselage,\"ZLocPercent\",\"XSec_0\", " + 
                                           std::to_string(fuselage.nose.zLocPercent) + ");");
                                file << "\r\n";

                                // XSec_1
                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_1\",scalingFactorWidthDome*refNoseWidth);");
                                file << "\r\n";

                                writeUpdate();

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_1\",scalingFactorDiameter*refNoseDiameter);");
                                file << "\r\n";

                                writeUpdate();

                                // XSec_2
                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_2\",scalingFactorWidthDome*refDomeWidth);");
                                file << "\r\n";

                                writeUpdate();

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_2\",scalingFactorDiameter*refDomeDiameter);");
                                file << "\r\n";

                                writeUpdate();

                                // XSec_3
                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_3\",scalingFactorWidthBody*refAttachDomeBodyWidth);");
                                file << "\r\n";

                                writeUpdate();

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_3\"," + 
                                           std::to_string(fuselage.attachDomeBody.diameter) + ");");
                                file << "\r\n";

                                writeUpdate();

                                writeCommand("SetParmVal(fuselage,\"XLocPercent\",\"XSec_3\", " + 
                                           std::to_string(fuselage.attachDomeBody.xLocPercent) + ");");
                                file << "\r\n";

                                // XSec_4
                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_4\",scalingFactorWidthBody*refBodyWidth);");
                                file << "\r\n";

                                writeUpdate();

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_4\",scalingFactorBodyDiameter*refBodyDiameter);");
                                file << "\r\n";

                                writeUpdate();

                                // XSec_5
                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_5\",scalingFactorWidthBody*refBodyWidth);");
                                file << "\r\n";

                                writeUpdate();

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_5\",scalingFactorBodyDiameter*refBodyDiameter);");
                                file << "\r\n";

                                writeUpdate();

                                // XSec_6
                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_6\",scalingFactorWidthBody*refTailWidth);");
                                file << "\r\n";

                                writeUpdate();

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_6\",scalingFactorTailDiameter*refTailDiameter);");
                                file << "\r\n";
                        }
                        else if (fuselage.fuselagePresetName == "P2006T_Fuselage_Skin")
                        {
                                writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + 
                                           fuselage.fuselagePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("ReadVSPFile( fnamePreset );");
                                file << "\r\n";

                                writeCommand("string fuselageName = \"FuselageGeom\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids = FindGeomsWithName( fuselageName );");
                                file << "\r\n";

                                writeCommand("string fuselage = geom_ids[0];");
                                file << "\r\n";
                        }
                        else if (fuselage.fuselagePresetName == "AirbusGenericTransportJet")
                        {
                                writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + 
                                           fuselage.fuselagePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("InsertVSPFile(fnamePreset , \"\");");
                                file << "\r\n";

                                writeCommand("string fuselageName  = \"fuselage\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids = FindGeomsWithName( fuselageName );");
                                file << "\r\n";

                                writeCommand("string fuselage = geom_ids[0];");
                                file << "\r\n";

                                // REFERENCE GEOMETRY VALUES
                                writeCommand("double refFuselageLength  = 37.57;");
                                file << "\r\n";

                                writeCommand("double refFuselageMaxDiameter = 4.141;");
                                file << "\r\n";

                                writeCommand("double refFuselageMaxWidth  = 3.95010;");
                                file << "\r\n";

                                // FUSELAGE TO CUSTOMIZE
                                writeCommand("double fuselageLength = " + std::to_string(fuselage.length) + ";");
                                file << "\r\n";

                                writeCommand("double fuselageMaxDiameter = " + std::to_string(fuselage.diameter) + ";");
                                file << "\r\n";

                                writeCommand("double fuselageMaxWidth = " + std::to_string(fuselage.width) + ";");
                                file << "\r\n";

                                writeCommand("double fuselageRefFineRatio = refFuselageLength/refFuselageMaxDiameter ;");
                                file << "\r\n";

                                // SCALING FACTORS
                                writeCommand("double  diamterFuselageScalingFactor = fuselageMaxDiameter/refFuselageMaxDiameter;");
                                file << "\r\n";

                                writeCommand("double  widthFuselageScalingFactor = fuselageMaxWidth/fuselageMaxDiameter;");
                                file << "\r\n";

                                writeCommand("double  newFuselageFineRatio = fuselageLength/fuselageMaxDiameter ;");
                                file << "\r\n";

                                writeUpdate();

                                // FUSELAGE RECONSTRUCTION
                                writeCommand("for (int i = 0; i<=27; i++ ) {");
                                file << "\r\n";

                                // Variable declarations
                                writeCommand("double fuselageDiameter = 0; ");
                                file << "\r\n";

                                writeCommand("double fuselageWidth = 0; ");
                                file << "\r\n";

                                writeCommand("double newFuselageDiameter = 0; ");
                                file << "\r\n";

                                writeCommand("double newFuselageWidth = 0; ");
                                file << "\r\n";

                                writeCommand("if (i==0) {");
                                file << "\r\n";

                                writeCommand("fuselageDiameter  = GetParmVal(fuselage,\"Ellipse_Height\",\"XSecCurve_\" + (i));");
                                file << "\r\n";

                                writeCommand("fuselageWidth     = GetParmVal(fuselage,\"Ellipse_Width\",\"XSecCurve_\" + (i));");
                                file << "\r\n";

                                writeCommand(" newFuselageDiameter  = fuselageDiameter*diamterFuselageScalingFactor;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Ellipse_Height\",\"XSecCurve_\" + (i),newFuselageDiameter);");
                                file << "\r\n";

                                writeCommand(" newFuselageWidth  = fuselageWidth*widthFuselageScalingFactor;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Ellipse_Width\",\"XSecCurve_\" + (i),newFuselageWidth);");
                                file << "\r\n";

                                writeCommand("} else if (i>0) {");
                                file << "\r\n";

                                writeCommand(" fuselageDiameter  = GetParmVal(fuselage,\"Height\",\"XSecCurve_\" + (i));");
                                file << "\r\n";

                                writeCommand(" fuselageWidth     = GetParmVal(fuselage,\"Width\",\"XSecCurve_\" + (i));");
                                file << "\r\n";

                                writeCommand("} ");
                                file << "\r\n";

                                writeCommand("if (i>= 1) {");
                                file << "\r\n";

                                writeCommand("double fuselagedeltaX    = GetParmVal(fuselage , \"XDelta\",\"XSec_\" + (i));");
                                file << "\r\n";

                                writeCommand("double newFuselageDeltaX = 0; ");
                                file << "\r\n";

                                // DeltaX
                                writeCommand(" newFuselageDeltaX  = fuselagedeltaX*newFuselageFineRatio/fuselageRefFineRatio;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"XDelta\",\"XSec_\" + (i),newFuselageDeltaX);");
                                file << "\r\n";

                                // Diameter
                                writeCommand(" newFuselageDiameter  = fuselageDiameter*diamterFuselageScalingFactor;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_\" + (i),newFuselageDiameter);");
                                file << "\r\n";

                                // Width
                                writeCommand(" newFuselageWidth  = fuselageWidth*widthFuselageScalingFactor;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_\" + (i),newFuselageWidth);");
                                file << "\r\n";

                                writeCommand("}");
                                file << "\r\n";

                                writeCommand("Update();}");
                                file << "\r\n";

                                writeUpdate();
                        }
                        else if (fuselage.fuselagePresetName == "BoeingGenericTransportJet")
                        {
                                writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + 
                                           fuselage.fuselagePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("InsertVSPFile(fnamePreset , \"\");");
                                file << "\r\n";

                                writeCommand("string fuselageName  = \"fuselage\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids = FindGeomsWithName( fuselageName );");
                                file << "\r\n";

                                writeCommand("string fuselage = geom_ids[0];");
                                file << "\r\n";

                                // REFERENCE GEOMETRY VALUES
                                writeCommand("double refFuselageLength  = 63.73;");
                                file << "\r\n";

                                writeCommand("double refFuselageMaxDiameter = 6.20;");
                                file << "\r\n";

                                writeCommand("double refFuselageMaxWidth  = 6.20;");
                                file << "\r\n";

                                // FUSELAGE TO CUSTOMIZE
                                writeCommand("double fuselageLength = " + std::to_string(fuselage.length) + ";");
                                file << "\r\n";

                                writeCommand("double fuselageMaxDiameter = " + std::to_string(fuselage.diameter) + ";");
                                file << "\r\n";

                                writeCommand("double fuselageMaxWidth = " + std::to_string(fuselage.width) + ";");
                                file << "\r\n";

                                writeCommand("double fuselageRefFineRatio = refFuselageLength/refFuselageMaxDiameter ;");
                                file << "\r\n";

                                // SCALING FACTORS
                                writeCommand("double  diamterFuselageScalingFactor = fuselageMaxDiameter/refFuselageMaxDiameter;");
                                file << "\r\n";

                                writeCommand("double  widthFuselageScalingFactor = fuselageMaxWidth/fuselageMaxDiameter;");
                                file << "\r\n";

                                writeCommand("double  newFuselageFineRatio = fuselageLength/fuselageMaxDiameter ;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Length\",\"Design\",fuselageLength);");
                                file << "\r\n";

                                writeUpdate();

                                // FUSELAGE RECONSTRUCTION
                                writeCommand("for (int i = 0; i<=6; i++ ) {");
                                file << "\r\n";

                                // Variable declarations
                                writeCommand("double fuselageDiameter = 0; ");
                                file << "\r\n";

                                writeCommand("double fuselageWidth = 0; ");
                                file << "\r\n";

                                writeCommand("double newFuselageDiameter = 0; ");
                                file << "\r\n";

                                writeCommand("double newFuselageWidth = 0; ");
                                file << "\r\n";

                                writeCommand("if (i==0) {");
                                file << "\r\n";

                                writeCommand("fuselageDiameter  = GetParmVal(fuselage,\"Height\",\"XSecCurve_\" + (i));");
                                file << "\r\n";

                                writeCommand("fuselageWidth     = GetParmVal(fuselage,\"Width\",\"XSecCurve_\" + (i));");
                                file << "\r\n";

                                writeCommand(" newFuselageDiameter  = fuselageDiameter*diamterFuselageScalingFactor;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_\" + (i),newFuselageDiameter);");
                                file << "\r\n";

                                writeCommand(" newFuselageWidth  = fuselageWidth*widthFuselageScalingFactor;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_\" + (i),newFuselageWidth);");
                                file << "\r\n";

                                writeCommand("} else if (i>0 && i<6) {");
                                file << "\r\n";

                                writeCommand(" fuselageDiameter  = GetParmVal(fuselage,\"Height\",\"XSecCurve_\" + (i));");
                                file << "\r\n";

                                writeCommand(" fuselageWidth     = GetParmVal(fuselage,\"Width\",\"XSecCurve_\" + (i));");
                                file << "\r\n";

                                // Diameter
                                writeCommand(" newFuselageDiameter  = fuselageDiameter*diamterFuselageScalingFactor;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Height\",\"XSecCurve_\" + (i),newFuselageDiameter);");
                                file << "\r\n";

                                // Width
                                writeCommand(" newFuselageWidth  = fuselageWidth*widthFuselageScalingFactor;");
                                file << "\r\n";

                                writeCommand("SetParmVal(fuselage,\"Width\",\"XSecCurve_\" + (i),newFuselageWidth);");
                                file << "\r\n";

                                writeCommand("}");
                                file << "\r\n";

                                writeCommand("Update();}");
                                file << "\r\n";

                                writeUpdate();
                        }
                        else if (fuselage.fuselagePresetName == "MQ_20_FuselageBlendedEngineSkin")
                        {
                                writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + 
                                           fuselage.fuselagePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("InsertVSPFile(fnamePreset , \"\");");
                                file << "\r\n";

                                writeCommand("string fuselageName = \"FuselageGeom\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids = FindGeomsWithName( fuselageName );");
                                file << "\r\n";

                                writeCommand("string fuselage = geom_ids[0];");
                                file << "\r\n";

                                for (int i = 1; i <= fuselage.counterBlendedEngine; i++)
                                {
                                        writeCommand("string blendedEngineName_" + std::to_string(i) + 
                                                   " = \"engine" + std::to_string(i) + "\";");
                                        file << "\r\n";

                                        writeCommand("array< string > @geom_ids_engine_" + std::to_string(i) + 
                                                   " = FindGeomsWithName( blendedEngineName_" + std::to_string(i) + " );");
                                        file << "\r\n";

                                        writeCommand("string blendedEngine_" + std::to_string(i) + 
                                                   " = geom_ids_engine_" + std::to_string(i) + "[0];");
                                        file << "\r\n";
                                }
                        }
                        else if (fuselage.fuselagePresetName == "MQ_20_FuselageSkin")
                        {
                                writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + 
                                           fuselage.fuselagePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("InsertVSPFile(fnamePreset , \"\");");
                                file << "\r\n";

                                writeCommand("string fuselageName = \"FuselageGeom\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids = FindGeomsWithName( fuselageName );");
                                file << "\r\n";

                                writeCommand("string fuselage = geom_ids[0];");
                                file << "\r\n";
                        }
                        else if (fuselage.fuselagePresetName == "McDonnel_Duglas_FuselageSkin_Concept")
                        {
                                writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + 
                                           fuselage.fuselagePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("InsertVSPFile(fnamePreset , \"\");");
                                file << "\r\n";

                                writeCommand("string fuselageName = \"FuselageJ36_Concept\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids = FindGeomsWithName( fuselageName );");
                                file << "\r\n";

                                writeCommand("string fuselage = geom_ids[0];");
                                file << "\r\n";

                                for (int i = 1; i <= fuselage.counterBlendedEngine; i++)
                                {
                                        writeCommand("string blendedEngineName_" + std::to_string(i) + 
                                                   " = \"engine" + std::to_string(i) + "\";");
                                        file << "\r\n";

                                        writeCommand("array< string > @geom_ids_engine_" + std::to_string(i) + 
                                                   " = FindGeomsWithName( blendedEngineName_" + std::to_string(i) + " );");
                                        file << "\r\n";

                                        writeCommand("string blendedEngine_" + std::to_string(i) + 
                                                   " = geom_ids_engine_" + std::to_string(i) + "[0];");
                                        file << "\r\n";
                                }
                        }
                        else if (fuselage.fuselagePresetName == "J36_FuselageSkin_HQ")
                        {
                                writeCommand("string fnamePreset = \"" + cwdEscaped + "\\\\" + 
                                           fuselage.fuselagePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("InsertVSPFile(fnamePreset , \"\");");
                                file << "\r\n";

                                writeCommand("string fuselageName = \"fuselage\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids = FindGeomsWithName( fuselageName );");
                                file << "\r\n";

                                writeCommand("string fuselage = geom_ids[0];");
                                file << "\r\n";

                                for (int i = 1; i <= fuselage.counterBlendedEngine; i++)
                                {
                                        writeCommand("string blendedEngineName_" + std::to_string(i) + 
                                                   " = \"engine" + std::to_string(i) + "\";");
                                        file << "\r\n";

                                        writeCommand("array< string > @geom_ids_engine_" + std::to_string(i) + 
                                                   " = FindGeomsWithName( blendedEngineName_" + std::to_string(i) + " );");
                                        file << "\r\n";

                                        writeCommand("string blendedEngine_" + std::to_string(i) + 
                                                   " = geom_ids_engine_" + std::to_string(i) + "[0];");
                                        file << "\r\n";
                                }
                        }
                }

                // ============= FUSELAGE =============
                inline void makeFuselage(const Fuselage &fuselage)
                {
                        if (fuselage.advancedFuselage)
                        {
                                makeAdvancedFuselage(fuselage);
                                return;
                        }

                        writeComment("fuselage");
                        writeCommand("string " + fuselage.id + " = AddGeom(\"" + fuselage.type + "\");");
                        file << "\r\n";

                        writeCommand("SetGeomDrawType(" + fuselage.id + ", GEOM_DRAW_SHADE);");
                        file << "\r\n";

                        writeCommand("SetParmVal(" + fuselage.id + ",\"Length\",\"Design\"," +
                                     std::to_string(fuselage.length) + ");");
                        writeCommand("SetParmVal(" + fuselage.id + ",\"Diameter\",\"Design\"," +
                                     std::to_string(fuselage.diameter) + ");");
                        file << "\r\n";

                        if (fuselage.customFuselage)
                        {
                                writeCommand("SetParmVal(" + fuselage.id + ",\"NoseMult\",\"Design\"," +
                                             std::to_string(fuselage.nosemult) + ");");
                                file << "\r\n";
                                writeCommand("SetParmVal(" + fuselage.id + ",\"AftMult\",\"Design\"," +
                                             std::to_string(fuselage.aftmult) + ");");
                                file << "\r\n";
                                writeCommand("SetParmVal(" + fuselage.id + ",\"NoseCenter\",\"Design\"," +
                                             std::to_string(fuselage.nosecenter) + ");");
                                file << "\r\n";
                                writeCommand("SetParmVal(" + fuselage.id + ",\"AftCenter\",\"Design\"," +
                                             std::to_string(fuselage.aftcenter) + ");");
                                file << "\r\n";
                                writeCommand("SetParmVal(" + fuselage.id + ",\"AftWidth\",\"Design\"," +
                                             std::to_string(fuselage.aftwidth) + ");");
                                file << "\r\n";
                                writeCommand("SetParmVal(" + fuselage.id + ",\"AftHeight\",\"Design\"," +
                                             std::to_string(fuselage.aftheight) + ");");
                                file << "\r\n";
                        }

                        writeUpdate();

                        writeCommand("SetParmVal(" + fuselage.id + ",\"Tess_U\",\"Shape\"," +
                                     std::to_string(fuselage.utess) + ");");
                        writeCommand("SetParmVal(" + fuselage.id + ",\"Tess_W\",\"Shape\"," +
                                     std::to_string(fuselage.wtess) + ");");
                        file << "\r\n";

                        writeUpdate();
                }

        public:
                // ============= WING (COMPLETA) =============
                inline void makeWing(const Wing &wing, int sym)
                {
                        writeComment(wing.id);
                        std::string wingID = wing.id;
                        writeCommand("string " + wingID + " = AddGeom(\"" + wing.type + "\");");
                        file << "\r\n";

                        writeCommand("SetGeomName(" + wingID + ", \"" + wingID + "\");");
                        file << "\r\n";

                        writeCommand("SetGeomDrawType(" + wingID + ", GEOM_DRAW_SHADE);");
                        file << "\r\n";

                        writeCommand("SetParmVal(" + wingID + ",\"Sym_Planar_Flag\",\"Sym\"," + std::to_string(sym) + ");");
                        file << "\r\n";

                        writeUpdate();

                        writeCommand("SetParmVal(" + wingID + ",\"X_Rel_Location\",\"XForm\"," +
                                     std::to_string(wing.xloc) + ");");
                        writeCommand("SetParmVal(" + wingID + ",\"Y_Rel_Location\",\"XForm\"," +
                                     std::to_string(wing.yloc) + ");");
                        writeCommand("SetParmVal(" + wingID + ",\"Z_Rel_Location\",\"XForm\"," +
                                     std::to_string(wing.zloc) + ");");
                        file << "\r\n";

                        writeCommand("SetParmVal(" + wingID + ",\"X_Rel_Rotation\",\"XForm\"," +
                                     std::to_string(wing.xrot) + ");");
                        writeCommand("SetParmVal(" + wingID + ",\"Y_Rel_Rotation\",\"XForm\"," +
                                     std::to_string(wing.yrot) + ");");
                        writeCommand("SetParmVal(" + wingID + ",\"Z_Rel_Rotation\",\"XForm\"," +
                                     std::to_string(wing.zrot) + ");");
                        file << "\r\n";

                        writeUpdate();

                        int numSections = wing.span.size();

                        if (wing.useDetailedPanels)
                        {
                                for (int i = 1; i <= numSections; i++)
                                {
                                        if (i == 1 && numSections > 1)
                                        {
                                                for (int j = i; j <= numSections-1; j++)
                                                {
                                                        writeCommand("InsertXSec(" + wingID + ",1," + wing.airfoilType + ");");
                                                }
                                        }
                                        shapePanel(wing, i);
                                }
                        }
                        else
                        {
                                for (int i = 0; i < numSections; i++)
                                {
                                        std::string secID = "XSec_" + std::to_string(i + 1);

                                        if (i < wing.span.size())
                                        {
                                                writeCommand("SetParmVal(" + wingID + ",\"Span\",\"" + secID + "\"," +
                                                             std::to_string(wing.span[i]) + ");");
                                        }

                                        if (i < wing.chord.size())
                                        {
                                                if (i == 0)
                                                {
                                                        writeCommand("SetParmVal(" + wingID + ",\"Root_Chord\",\"" + secID + "\"," +
                                                                     std::to_string(wing.chord[i]) + ");");
                                                }
                                                writeCommand("SetParmVal(" + wingID + ",\"Tip_Chord\",\"" + secID + "\"," +
                                                             std::to_string(wing.chord[i]) + ");");
                                        }

                                        if (i < wing.sweep.size())
                                        {
                                                writeCommand("SetParmVal(" + wingID + ",\"Sweep\",\"" + secID + "\"," +
                                                             std::to_string(wing.sweep[i]) + ");");
                                        }

                                        if (i < wing.dihedral.size())
                                        {
                                                writeCommand("SetParmVal(" + wingID + ",\"Dihedral\",\"" + secID + "\"," +
                                                             std::to_string(wing.dihedral[i]) + ");");
                                        }
                                }
                                file << "\r\n";
                                writeUpdate();
                        }

                        for (int i = 1; i <= numSections + 1; i++)
                        {
                                shapeAirfoil(wing, i);
                        }

                        writeCommand("SetParmVal(" + wingID + ",\"Tess_W\",\"Shape\"," +
                                     std::to_string(wing.wtess) + ");");
                        file << "\r\n";

                        writeCommand("SetParmVal(" + wingID + ",\"CapUMinTess\",\"EndCap\"," +
                                     std::to_string(wing.captess) + ");");
                        file << "\r\n";

                        writeCommand("SetParmVal(" + wingID + ",\"LECluster\",\"WingGeom\"," +
                                     std::to_string(wing.lecluster) + ");");
                        file << "\r\n";

                        writeCommand("SetParmVal(" + wingID + ",\"TECluster\",\"WingGeom\"," +
                                     std::to_string(wing.tecluster) + ");");
                        file << "\r\n";

                        writeUpdate();

                        if (wing.capTreatment)
                        {
                                int capOption = 0;

                                if (wing.capType == "None")
                                        capOption = 0;
                                else if (wing.capType == "Flat")
                                        capOption = 1;
                                else if (wing.capType == "Round")
                                        capOption = 2;
                                else if (wing.capType == "Edge")
                                        capOption = 3;
                                else if (wing.capType == "Sharp")
                                        capOption = 4;
                                else if (wing.capType == "Round_Ext_None")
                                        capOption = 5;
                                else if (wing.capType == "Round_Ext_LE")
                                        capOption = 6;
                                else if (wing.capType == "Round_Ext_TE")
                                        capOption = 7;
                                else if (wing.capType == "Round_Ext_Both")
                                        capOption = 8;

                                writeCommand("SetParmVal(" + wingID + ",\"CapUMaxOption\",\"EndCap\"," +
                                             std::to_string(capOption) + ");");
                                file << "\r\n";

                                if (capOption >= 2 && capOption <= 8)
                                {
                                        writeCommand("SetParmVal(" + wingID + ",\"CapUMaxLength\",\"EndCap\"," +
                                                     std::to_string(wing.capLength) + ");");
                                        file << "\r\n";

                                        writeCommand("SetParmVal(" + wingID + ",\"CapUMaxOffset\",\"EndCap\"," +
                                                     std::to_string(wing.capOffset) + ");");
                                        file << "\r\n";
                                }

                                if (capOption == 4)
                                {
                                        writeCommand("SetParmVal(" + wingID + ",\"CapUMaxStrength\",\"EndCap\"," +
                                                     std::to_string(wing.capStrength) + ");");
                                        file << "\r\n";
                                }
                        }

                        if (wing.blending)
                        {
                                writeComment("Blending " + wing.id + " sections");

                                int nsec = numSections + 1;
                                bool dihedralControlSurf = false;

                                for (int i = 1; i < nsec; i++)
                                {
                                        std::string secID = "XSec_" + std::to_string(i - 1);

                                        if (i > 0)
                                        {
                                                writeCommand("SetParmVal(" + wingID + ",\"InLEMode\",\"" + secID + "\"," +
                                                             std::to_string(wing.blend.InLEMode[i]) + ");");

                                                if (wing.blend.InLEMode[i] != 0)
                                                {
                                                        if (wing.blend.InLEMode[i] == 1)
                                                        {
                                                                writeCommand("SetParmVal(" + wingID + ",\"InLESweep\",\"" + secID + "\"," +
                                                                             std::to_string(wing.blend.InLESweep[i]) + ");");
                                                                writeCommand("SetParmVal(" + wingID + ",\"InLEDihedral\",\"" + secID + "\"," +
                                                                             std::to_string(wing.blend.InLEDihedral[i]) + ");");
                                                        }
                                                        writeCommand("SetParmVal(" + wingID + ",\"InLEStrength\",\"" + secID + "\"," +
                                                                     std::to_string(wing.blend.InLEStrength[i]) + ");");
                                                }

                                                writeCommand("SetParmVal(" + wingID + ",\"InTEMode\",\"" + secID + "\"," +
                                                             std::to_string(wing.blend.InTEMode[i]) + ");");

                                                if (wing.blend.InTEMode[i] != 0)
                                                {
                                                        if (wing.blend.InTEMode[i] == 1)
                                                        {
                                                                writeCommand("SetParmVal(" + wingID + ",\"InTESweep\",\"" + secID + "\"," +
                                                                             std::to_string(wing.blend.InTESweep[i]) + ");");
                                                                writeCommand("SetParmVal(" + wingID + ",\"InTEDihedral\",\"" + secID + "\"," +
                                                                             std::to_string(wing.blend.InTEDihedral[i]) + ");");
                                                        }
                                                        writeCommand("SetParmVal(" + wingID + ",\"InTEStrength\",\"" + secID + "\"," +
                                                                     std::to_string(wing.blend.InTEStrength[i]) + ");");
                                                }

                                                if (i < nsec - 1)
                                                {
                                                        writeCommand("SetParmVal(" + wingID + ",\"OutLEMode\",\"" + secID + "\"," +
                                                                     std::to_string(wing.blend.OutLEMode[i]) + ");");

                                                        if (wing.blend.OutLEMode[i] != 0)
                                                        {
                                                                if (wing.blend.OutLEMode[i] == 1)
                                                                {
                                                                        writeCommand("SetParmVal(" + wingID + ",\"OutLESweep\",\"" + secID + "\"," +
                                                                                     std::to_string(wing.blend.OutLESweep[i]) + ");");
                                                                        writeCommand("SetParmVal(" + wingID + ",\"OutLEDihedral\",\"" + secID + "\"," +
                                                                                     std::to_string(wing.blend.OutLEDihedral[i]) + ");");
                                                                }
                                                                writeCommand("SetParmVal(" + wingID + ",\"OutLEStrength\",\"" + secID + "\"," +
                                                                             std::to_string(wing.blend.OutLEStrength[i]) + ");");
                                                        }

                                                        writeCommand("SetParmVal(" + wingID + ",\"OutTEMode\",\"" + secID + "\"," +
                                                                     std::to_string(wing.blend.OutTEMode[i]) + ");");

                                                        if (wing.blend.OutTEMode[i] != 0)
                                                        {
                                                                if (wing.blend.OutTEMode[i] == 1)
                                                                {
                                                                        writeCommand("SetParmVal(" + wingID + ",\"OutTESweep\",\"" + secID + "\"," +
                                                                                     std::to_string(wing.blend.OutTESweep[i]) + ");");
                                                                        writeCommand("SetParmVal(" + wingID + ",\"OutTEDihedral\",\"" + secID + "\"," +
                                                                                     std::to_string(wing.blend.OutTEDihedral[i]) + ");");
                                                                }
                                                                writeCommand("SetParmVal(" + wingID + ",\"OutTEStrength\",\"" + secID + "\"," +
                                                                             std::to_string(wing.blend.OutTEStrength[i]) + ");");
                                                        }
                                                }

                                                file << "\r\n";
                                        }

                                        if (i != nsec - 1)
                                        {
                                                if (i - 1 < wing.dihedral.size() && wing.dihedral[i - 1] != 0)
                                                {
                                                        writeCommand("SetParmVal(" + wingID + ",\"RotateMatchDideralFlag\",\"XSec_" +
                                                                     std::to_string(i - 1) + "\",1);");
                                                }
                                                else
                                                {
                                                        int totalSurf = nsec - 3;
                                                        if (!dihedralControlSurf)
                                                        {
                                                                bool allZero = true;
                                                                for (int k = 0; k < totalSurf && k < wing.dihedral.size(); k++)
                                                                {
                                                                        if (wing.dihedral[k] != 0)
                                                                        {
                                                                                allZero = false;
                                                                                break;
                                                                        }
                                                                }

                                                                if (allZero)
                                                                {
                                                                        writeCommand("SetParmVal(" + wingID + ",\"RotateMatchDideralFlag\",\"XSec_" +
                                                                                     std::to_string(totalSurf) + "\",1);");
                                                                        dihedralControlSurf = true;
                                                                }
                                                        }
                                                }
                                        }

                                        writeUpdate();
                                }
                        }

                        if (!wing.mov.type.empty())
                        {
                                makeMovables(wing);
                        }
                }

                inline void makeEOIR(const EOIR &eoir, int i)
                {
                        writeComment(eoir.id + " " + std::to_string(i));

                        writeCommand("string " + eoir.id + "_" + std::to_string(i) +
                                     " = AddGeom(\"" + eoir.type + "\");");
                        file << "\r\n";

                        writeCommand("SetGeomName(" + eoir.id + "_" + std::to_string(i) +
                                     ", \"" + eoir.id + "\");");
                        file << "\r\n";

                        int idx = static_cast<int>(std::round((i - 1) / 2.0));

                        if (idx >= eoir.length.size())
                                idx = eoir.length.size() - 1;

                        writeCommand("SetParmVal(" + eoir.id + "_" + std::to_string(i) +
                                     ",\"Length\",\"Design\"," + std::to_string(eoir.length[idx]) + ");");
                        writeCommand("SetParmVal(" + eoir.id + "_" + std::to_string(i) +
                                     ",\"FineRatio\",\"Design\"," + std::to_string(eoir.fineratio[idx]) + ");");
                        file << "\r\n";

                        writeUpdate();

                        int sign = (i % 2 == 0) ? 1 : -1;
                        writeCommand("SetParmVal(" + eoir.id + "_" + std::to_string(i) +
                                     ",\"X_Rel_Location\",\"XForm\"," + std::to_string(eoir.xloc[idx]) + ");");
                        writeCommand("SetParmVal(" + eoir.id + "_" + std::to_string(i) +
                                     ",\"Y_Rel_Location\",\"XForm\"," + std::to_string(sign * eoir.yloc[idx]) + ");");
                        writeCommand("SetParmVal(" + eoir.id + "_" + std::to_string(i) +
                                     ",\"Z_Rel_Location\",\"XForm\"," + std::to_string(eoir.zloc[idx]) + ");");
                        writeCommand("SetParmVal(" + eoir.id + "_" + std::to_string(i) +
                                     ",\"Y_Rel_Rotation\",\"XForm\"," + std::to_string(eoir.yrot[idx]) + ");");
                        file << "\r\n";

                        writeUpdate();

                        writeCommand("SetParmVal(" + eoir.id + "_" + std::to_string(i) +
                                     ",\"Tess_U\",\"Shape\"," + std::to_string(eoir.utess) + ");");
                        writeCommand("SetParmVal(" + eoir.id + "_" + std::to_string(i) +
                                     ",\"Tess_W\",\"Shape\"," + std::to_string(eoir.wtess) + ");");
                        file << "\r\n";

                        writeUpdate();
                }

                inline void makePod(const Pod &pod, int i)
                {
                        writeComment(pod.id + " " + std::to_string(i));

                        writeCommand("string " + pod.id + "_" + std::to_string(i) +
                                     " = AddGeom(\"" + pod.type + "\");");
                        file << "\r\n";

                        writeCommand("SetGeomName(" + pod.id + "_" + std::to_string(i) +
                                     ", \"" + pod.id + "_" + std::to_string(i) + "\");");
                        file << "\r\n";

                        int idx = static_cast<int>(std::round((i - 1) / 2.0));
                        if (idx >= pod.length.size())
                                idx = pod.length.size() - 1;

                        writeCommand("SetParmVal(" + pod.id + "_" + std::to_string(i) +
                                     ",\"Length\",\"Design\"," + std::to_string(pod.length[idx]) + ");");
                        writeCommand("SetParmVal(" + pod.id + "_" + std::to_string(i) +
                                     ",\"FineRatio\",\"Design\"," + std::to_string(pod.fineratio[idx]) + ");");
                        file << "\r\n";

                        writeUpdate();

                        int sign = (i % 2 == 0) ? 1 : -1;
                        writeCommand("SetParmVal(" + pod.id + "_" + std::to_string(i) +
                                     ",\"X_Rel_Location\",\"XForm\"," + std::to_string(pod.xloc[idx]) + ");");
                        writeCommand("SetParmVal(" + pod.id + "_" + std::to_string(i) +
                                     ",\"Y_Rel_Location\",\"XForm\"," + std::to_string(sign * pod.yloc[idx]) + ");");
                        writeCommand("SetParmVal(" + pod.id + "_" + std::to_string(i) +
                                     ",\"Z_Rel_Location\",\"XForm\"," + std::to_string(pod.zloc[idx]) + ");");
                        writeCommand("SetParmVal(" + pod.id + "_" + std::to_string(i) +
                                     ",\"Y_Rel_Rotation\",\"XForm\"," + std::to_string(pod.yrot[idx]) + ");");
                        file << "\r\n";

                        writeUpdate();

                        writeCommand("SetParmVal(" + pod.id + "_" + std::to_string(i) +
                                     ",\"Tess_U\",\"Shape\"," + std::to_string(pod.utess) + ");");
                        writeCommand("SetParmVal(" + pod.id + "_" + std::to_string(i) +
                                     ",\"Tess_W\",\"Shape\"," + std::to_string(pod.wtess) + ");");
                        file << "\r\n";

                        writeUpdate();
                }

                inline void makeBoom(const Boom &boom, int i)
                {
                        writeComment(boom.id + " " + std::to_string(i));

                        writeCommand("string " + boom.id + "_" + std::to_string(i) +
                                     " = AddGeom(\"" + boom.type + "\");");
                        file << "\r\n";

                        writeCommand("SetGeomName(" + boom.id + "_" + std::to_string(i) +
                                     ", \"" + boom.id + "_" + std::to_string(i) + "\");");
                        file << "\r\n";

                        int idx = static_cast<int>(std::round((i - 1) / 2.0));
                        if (idx >= boom.length.size())
                                idx = boom.length.size() - 1;

                        writeCommand("SetParmVal(" + boom.id + "_" + std::to_string(i) +
                                     ",\"Length\",\"Design\"," + std::to_string(boom.length[idx]) + ");");
                        writeCommand("SetParmVal(" + boom.id + "_" + std::to_string(i) +
                                     ",\"FineRatio\",\"Design\"," + std::to_string(boom.fineratio[idx]) + ");");
                        file << "\r\n";

                        writeUpdate();

                        int sign = (i % 2 == 0) ? 1 : -1;
                        writeCommand("SetParmVal(" + boom.id + "_" + std::to_string(i) +
                                     ",\"X_Rel_Location\",\"XForm\"," + std::to_string(boom.xloc[idx]) + ");");
                        writeCommand("SetParmVal(" + boom.id + "_" + std::to_string(i) +
                                     ",\"Y_Rel_Location\",\"XForm\"," + std::to_string(sign * boom.yloc[idx]) + ");");
                        writeCommand("SetParmVal(" + boom.id + "_" + std::to_string(i) +
                                     ",\"Z_Rel_Location\",\"XForm\"," + std::to_string(boom.zloc[idx]) + ");");
                        writeCommand("SetParmVal(" + boom.id + "_" + std::to_string(i) +
                                     ",\"Y_Rel_Rotation\",\"XForm\"," + std::to_string(boom.yrot[idx]) + ");");
                        file << "\r\n";

                        writeUpdate();

                        writeCommand("SetParmVal(" + boom.id + "_" + std::to_string(i) +
                                     ",\"Tess_U\",\"Shape\"," + std::to_string(boom.utess) + ");");
                        writeCommand("SetParmVal(" + boom.id + "_" + std::to_string(i) +
                                     ",\"Tess_W\",\"Shape\"," + std::to_string(boom.wtess) + ");");
                        file << "\r\n";

                        writeUpdate();
                }

              // ============= MAKE ADVANCED NACELLE =============
                inline void makeAdvancedNacelle(const Nacelle &nacelle, int i)
                {
                        if (!nacelle.advancedNacelle)
                        {
                                writeComment("### Advanced Nacelle is not selected... ###");
                                return;
                        }

                        // Controllo robusto dell'indice
                        int idx = i - 1;
                        if (idx < 0 || idx >= nacelle.xloc.size())
                        {
                                throw std::runtime_error(
                                    "Nacelle index " + std::to_string(i) + 
                                    " is out of bounds. xloc array has " + 
                                    std::to_string(nacelle.xloc.size()) + " elements. " +
                                    "Valid range: 1 to " + std::to_string(nacelle.xloc.size())
                                );
                        }

                        if (idx >= nacelle.yloc.size())
                        {
                                throw std::runtime_error(
                                    "Nacelle index " + std::to_string(i) + 
                                    " exceeds yloc array size (" + std::to_string(nacelle.yloc.size()) + ")"
                                );
                        }

                        if (idx >= nacelle.zloc.size())
                        {
                                throw std::runtime_error(
                                    "Nacelle index " + std::to_string(i) + 
                                    " exceeds zloc array size (" + std::to_string(nacelle.zloc.size()) + ")"
                                );
                        }

                        std::string folderName = "NacellePreset";
                        std::string cwd = parentFolder + "\\" + folderName;
                        std::string cwdEscaped = replaceBackslash(cwd);

                        if (nacelle.nacellePresetName == "GeneralTurbofan")
                        {
                                // LOADING NACELLE PRESET
                                writeCommand("string fnamePresetNacelle_" + std::to_string(i) + 
                                           " = \"" + cwdEscaped + "\\\\" + nacelle.nacellePresetName + ".vsp3\";");
                                file << "\r\n";

                                writeCommand("InsertVSPFile(fnamePresetNacelle_" + std::to_string(i) + " , \"\");");
                                file << "\r\n";

                                writeCommand("string nacelleName_" + std::to_string(i) + " = \"Nacelle 1\";");
                                file << "\r\n";

                                writeCommand("array< string > @geom_ids_" + std::to_string(i) + 
                                           " = FindGeomsWithName( nacelleName_" + std::to_string(i) + " );");
                                file << "\r\n";

                                // REFERENCE GEOMETRY VALUES
                                writeCommand("double refNacelleLength_" + std::to_string(i) + " = 2.995;");
                                file << "\r\n";

                                writeCommand("double refMaxDiameter_" + std::to_string(i) + " = 2.24147;");
                                file << "\r\n";

                                // NACELLE TO CUSTOMIZE
                                writeCommand("double nacelleLength_" + std::to_string(i) + " = " + 
                                           std::to_string(nacelle.length) + ";");
                                file << "\r\n";

                                writeCommand("double maxDiameter_" + std::to_string(i) + "   = " + 
                                           std::to_string(nacelle.diameter) + ";");
                                file << "\r\n";

                                // SCALING FACTORS
                                writeCommand("double  diamterScalingFactor_" + std::to_string(i) + 
                                           " = maxDiameter_" + std::to_string(i) + "/refMaxDiameter_" + 
                                           std::to_string(i) + ";");
                                file << "\r\n";

                                writeCommand("double  newFineRatio_" + std::to_string(i) + "= nacelleLength_" + 
                                           std::to_string(i) + "/maxDiameter_" + std::to_string(i) + " ;");
                                file << "\r\n";

                                writeCommand("double  refNacelleFineRatio_" + std::to_string(i) + 
                                           "= refNacelleLength_" + std::to_string(i) + "/refMaxDiameter_" + 
                                           std::to_string(i) + " ;");
                                file << "\r\n";

                                writeCommand("string nacelle_" + std::to_string(i) + " = geom_ids_" + 
                                           std::to_string(i) + "[0];");
                                file << "\r\n";

                                writeCommand("SetGeomName(nacelle_" + std::to_string(i) + ",\"nacelle_" + 
                                           std::to_string(i) + "\");");
                                file << "\r\n";

                                writeUpdate();

                                // NACELLE RECONSTRUCTION
                                
                                // For command to manage the deltaXs
                                writeCommand("for (int i = 1; i<=23; i++ ) {");
                                file << "\r\n";

                                writeCommand("double deltaX_" + std::to_string(i) + 
                                           "  = GetParmVal(nacelle_" + std::to_string(i) + 
                                           ", \"XDelta\",\"XSec_\" + (i));");
                                file << "\r\n";

                                writeCommand("double newDeltaX_" + std::to_string(i) + "  = 0; ");
                                file << "\r\n";

                                writeCommand(" newDeltaX_" + std::to_string(i) + " = deltaX_" + std::to_string(i) + 
                                           "*newFineRatio_" + std::to_string(i) + "/refNacelleFineRatio_" + 
                                           std::to_string(i) + ";");
                                file << "\r\n";

                                writeCommand("SetParmVal(nacelle_" + std::to_string(i) + 
                                           ",\"XDelta\",\"XSec_\" + (i),newDeltaX_" + std::to_string(i) + " );");
                                file << "\r\n";

                                writeCommand("Update();}");
                                file << "\r\n";

                                // For command to manage the diameters
                                writeCommand("for (int i = 0; i<=23; i++ ) {");
                                file << "\r\n";

                                writeCommand("double val_" + std::to_string(i) + " = GetParmVal(nacelle_" + 
                                           std::to_string(i) + ", \"Circle_Diameter\", \"XSecCurve_\" + i);");
                                file << "\r\n";

                                writeCommand("double newDiamVal_" + std::to_string(i) + " = 0;");
                                file << "\r\n";

                                writeCommand(" newDiamVal_" + std::to_string(i) + " = diamterScalingFactor_" + 
                                           std::to_string(i) + "* val_" + std::to_string(i) + ";");
                                file << "\r\n";

                                writeCommand("SetParmVal(nacelle_" + std::to_string(i) + 
                                           ",\"Circle_Diameter\", \"XSecCurve_\" + i,newDiamVal_" + 
                                           std::to_string(i) + " );");
                                file << "\r\n";

                                writeCommand("Update();}");
                                file << "\r\n";

                                // X,Y,Z position of the nacelle
                                // Usa direttamente idx (già validato) - MATLAB usa nacelle.xloc(i), quindi C++ usa xloc[i-1]
                                writeCommand("SetParmVal(nacelle_" + std::to_string(i) + 
                                           ",\"X_Rel_Location\", \"XForm\" , " + 
                                           std::to_string(nacelle.xloc[idx]) + " );");
                                file << "\r\n";

                                writeCommand("SetParmVal(nacelle_" + std::to_string(i) + 
                                           ",\"Y_Rel_Location\", \"XForm\" , " + 
                                           std::to_string(nacelle.yloc[idx]) + " );");
                                file << "\r\n";

                                writeCommand("SetParmVal(nacelle_" + std::to_string(i) + 
                                           ",\"Z_Rel_Location\", \"XForm\" , " + 
                                           std::to_string(nacelle.zloc[idx]) + " );");
                                file << "\r\n";
                        }
                        else
                        {
                                writeComment("Nacelle preset not recognized: " + nacelle.nacellePresetName);
                        }
                }

                inline void makeNacelle(const Nacelle &nacelle, int i)
                {
                        // Check if advanced nacelle is requested
                        if (nacelle.advancedNacelle)
                        {
                                makeAdvancedNacelle(nacelle, i);
                                return;
                        }

                        writeComment(nacelle.id + " " + std::to_string(i));

                        writeCommand("string " + nacelle.id + "_" + std::to_string(i) +
                                     " = AddGeom(\"" + nacelle.type + "\");");
                        file << "\r\n";

                        writeCommand("SetGeomName(" + nacelle.id + "_" + std::to_string(i) +
                                     ", \"" + nacelle.id + "_" + std::to_string(i) + "\");");
                        file << "\r\n";

                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"Width\",\"Design\"," + std::to_string(nacelle.width) + ");");
                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"Height\",\"Design\"," + std::to_string(nacelle.height) + ");");
                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"Chord\",\"Design\"," + std::to_string(nacelle.chord) + ");");
                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"ThickChord\",\"Design\"," + std::to_string(nacelle.thickchord) + ");");
                        file << "\r\n";

                        writeUpdate();

                        int idx = static_cast<int>(std::round((i - 1) / 2.0));
                        if (idx >= nacelle.xloc.size())
                                idx = nacelle.xloc.size() - 1;

                        int sign = (i % 2 == 0) ? 1 : -1;

                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"X_Rel_Location\",\"XForm\"," + std::to_string(nacelle.xloc[idx]) + ");");
                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"Y_Rel_Location\",\"XForm\"," + std::to_string(sign * nacelle.yloc[idx]) + ");");
                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"Z_Rel_Location\",\"XForm\"," + std::to_string(nacelle.zloc[idx]) + ");");
                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"Y_Rel_Rotation\",\"XForm\"," + std::to_string(nacelle.yrot[idx]) + ");");
                        file << "\r\n";

                        writeUpdate();

                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"Tess_U\",\"Shape\"," + std::to_string(nacelle.utess) + ");");
                        writeCommand("SetParmVal(" + nacelle.id + "_" + std::to_string(i) +
                                     ",\"Tess_W\",\"Shape\"," + std::to_string(nacelle.wtess) + ");");
                        file << "\r\n";

                        writeUpdate();
                }

                inline void makeMovables(const Wing &wing)
                {
                        int numDetectedSlats = 0;

                        for (size_t w = 0; w < wing.mov.type.size(); w++)
                        {
                                std::string subID;

                                switch (wing.mov.type[w])
                                {
                                case 'f':
                                        if (wing.mov.type[0] == 's')
                                        {
                                                subID = "flap_" + std::to_string((w+1) - numDetectedSlats);
                                        }
                                        else
                                        {
                                                subID = "flap_" + std::to_string(w+1);
                                        }
                                        break;
                                case 'a':
                                        subID = "aileron";
                                        break;
                                case 's':
                                        subID = "slat_" + std::to_string(w+1);
                                        numDetectedSlats++;
                                        break;
                                case 'e':
                                        subID = "elevator";
                                        break;
                                case 'r':
                                        subID = "rudder_" + std::to_string(w+1);
                                        break;
                                case 'c':
                                        subID = "flapCanard";
                                        break;
                                }

                                int npanel = wing.span.size();
                                double eta_inner = wing.mov.eta_inner[w];
                                double eta_outer = wing.mov.eta_outer[w];
                                double cf_c_inner = wing.mov.cf_c_inner[w];
                                int tessellation = wing.mov.tessellation[w];

                                double semispan = 0;
                                for (double s : wing.span)
                                        semispan += s;

                                double y_inner = eta_inner * semispan;
                                double y_outer = eta_outer * semispan;

                                std::vector<double> xvec(npanel + 1);
                                std::vector<double> yvec(npanel + 1);

                                double add = 0;
                                for (int i = 0; i < npanel; i++)
                                {
                                        add += wing.span[i];
                                        xvec[i + 1] = add;
                                }

                                for (int i = 0; i <= npanel; i++)
                                {
                                        yvec[i] = static_cast<double>(i + 1) / (npanel + 2);
                                }

                                auto interp = [](const std::vector<double> &x, const std::vector<double> &y, double xi)
                                {
                                        for (size_t i = 0; i < x.size() - 1; i++)
                                        {
                                                if (xi >= x[i] && xi <= x[i + 1])
                                                {
                                                        double t = (xi - x[i]) / (x[i + 1] - x[i]);
                                                        return y[i] + t * (y[i + 1] - y[i]);
                                                }
                                        }
                                        return y.back();
                                };

                                double eta_vsp_inner = interp(xvec, yvec, y_inner);
                                double eta_vsp_outer = interp(xvec, yvec, y_outer);

                                writeCommand("string " + subID + " = AddSubSurf(" + wing.id + ", SS_CONTROL);");
                                file << "\r\n";

                                writeCommand("SetSubSurfName(" + wing.id + "," + subID + ", \"" + subID + "\");");
                                file << "\r\n";

                                if (subID.find("slat") == 0)
                                {
                                        writeCommand("SetParmVal(" + wing.id + ",\"LE_Flag\",\"SS_Control_" +
                                                     std::to_string(w+1) + "\"," + std::to_string(1) + ");");
                                        file << "\r\n";
                                }

                                writeCommand("SetParmVal(" + wing.id + ", \"UStart\", \"SS_Control_" +
                                             std::to_string(w+1) + "\", " + std::to_string(eta_vsp_inner) + ");");
                                writeCommand("SetParmVal(" + wing.id + ", \"UEnd\", \"SS_Control_" +
                                             std::to_string(w+1) + "\", " + std::to_string(eta_vsp_outer) + ");");

                                writeCommand("SetParmVal(" + wing.id + ", \"Length_C_Start\", \"SS_Control_" +
                                             std::to_string(w+1) + "\", " + std::to_string(cf_c_inner) + ");");

                                writeCommand("SetParmVal(" + wing.id + ", \"Tess_Num\", \"SS_Control_" +
                                             std::to_string(w+1) + "\", " + std::to_string(tessellation) + ");");

                                writeUpdate();
                        }
                }

                inline void makeDegenGeom(const std::string &name, bool loadAircraft = false, bool closeScript = true)
                {
                        if (!loadAircraft)
                        {
                                writeCommand("SetComputationFileName(DEGEN_GEOM_CSV_TYPE,\"" + name + "_DegenGeom.csv\");");
                                writeCommand("ComputeDegenGeom(SET_ALL,DEGEN_GEOM_CSV_TYPE);");
                                file << "\r\n";

                                writeCommand("SetComputationFileName(DEGEN_GEOM_M_TYPE,\"" + name + "_DegenGeom.m\");");
                                writeCommand("ComputeDegenGeom(SET_ALL,DEGEN_GEOM_M_TYPE);");
                                file << "\r\n";
                        }
                        else
                        {
                                writeCommand("SetComputationFileName(DEGEN_GEOM_CSV_TYPE,\"" + name + "_DegenGeom.csv\");");
                                writeCommand("ComputeDegenGeom(SET_SHOWN,DEGEN_GEOM_CSV_TYPE);");
                                file << "\r\n";

                                writeCommand("SetComputationFileName(DEGEN_GEOM_M_TYPE,\"" + name + "_DegenGeom.m\");");
                                writeCommand("ComputeDegenGeom(SET_SHOWN,DEGEN_GEOM_M_TYPE);");
                                file << "\r\n";
                        }

                         // Salvataggio del file .vsp3
                         writeComment("//==== Save Vehicle to File ====//");
                         writeCommand("string fname = \"" + name + ".vsp3\";");
                         writeCommand("WriteVSPFile( fname, SET_ALL );");

                        if (closeScript && file.is_open())
                        {
                                file << "}\n";
                                file.close();
                        }
                }
        };

} // namespace VSP