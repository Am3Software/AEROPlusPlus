#pragma once

#include <string>
#include <cmath>
#include "BASEAIRCRAFTDATA.h"

class COGCorrectionFactor : public BaseAircraftData
{

private:

    // COG Correction factors (kX, kZ per component — kY always 0 for symmetric aircraft)
    double kXWing                    = 1.0;    double kZWing                    = 1.0;
    double kXCanard                  = 1.0;    double kZCanard                  = 1.0;
    double kXHorizontal              = 1.0;    double kZHorizontal              = 1.0;
    double kXVertical                = 1.0;    double kZVertical                = 1.0;
    double kXVentralFin              = 1.0;    double kZVentralFin              = 1.0;
    double kXFuselage                = 1.0;    double kZFuselage                = 1.0;
    double kXBoom                    = 1.0;    double kZBoom                    = 1.0;
    double kXEoir                    = 1.0;    double kZEoir                    = 1.0;
    double kXUnderCarriage           = 1.0;    double kZUnderCarriage           = 1.0;
    double kXAPU                     = 1.0;    double kZAPU                     = 1.0;
    double kXControlSurfaces         = 1.0;    double kZControlSurfaces         = 1.0;
    double kXPropulsionGroup         = 1.0;    double kZPropulsionGroup         = 1.0;
    double kXInstruments             = 1.0;    double kZInstruments             = 1.0;
    double kXHydraulicAndPneumatic   = 1.0;    double kZHydraulicAndPneumatic   = 1.0;
    double kXElectricalGroup         = 1.0;    double kZElectricalGroup         = 1.0;
    double kXAvionic                 = 1.0;    double kZAvionic                 = 1.0;
    double kXFurnishingAndEquipment  = 1.0;    double kZFurnishingAndEquipment  = 1.0;
    double kXAntiIcingAndConditioning= 1.0;    double kZAntiIcingAndConditioning= 1.0;
    double kXOperatingItems          = 1.0;    double kZOperatingItems          = 1.0;
    double kXCrew                    = 1.0;    double kZCrew                    = 1.0;
    double kXPayload                 = 1.0;    double kZPayload                 = 1.0;
    double kXFuel                    = 1.0;    double kZFuel                    = 1.0;

public:
    COGCorrectionFactor() = default;

    // =========================================================
    // Setters
    // =========================================================

    COGCorrectionFactor &setKXWing(double value)                     { kXWing                     = value; return *this; }
    COGCorrectionFactor &setKZWing(double value)                     { kZWing                     = value; return *this; }
    COGCorrectionFactor &setKXCanard(double value)                   { kXCanard                   = value; return *this; }
    COGCorrectionFactor &setKZCanard(double value)                   { kZCanard                   = value; return *this; }
    COGCorrectionFactor &setKXHorizontal(double value)               { kXHorizontal               = value; return *this; }
    COGCorrectionFactor &setKZHorizontal(double value)               { kZHorizontal               = value; return *this; }
    COGCorrectionFactor &setKXVertical(double value)                 { kXVertical                 = value; return *this; }
    COGCorrectionFactor &setKZVertical(double value)                 { kZVertical                 = value; return *this; }
    COGCorrectionFactor &setKXVentralFin(double value)               { kXVentralFin               = value; return *this; }
    COGCorrectionFactor &setKZVentralFin(double value)               { kZVentralFin               = value; return *this; }
    COGCorrectionFactor &setKXFuselage(double value)                 { kXFuselage                 = value; return *this; }
    COGCorrectionFactor &setKZFuselage(double value)                 { kZFuselage                 = value; return *this; }
    COGCorrectionFactor &setKXBoom(double value)                     { kXBoom                     = value; return *this; }
    COGCorrectionFactor &setKZBoom(double value)                     { kZBoom                     = value; return *this; }
    COGCorrectionFactor &setKXEoir(double value)                     { kXEoir                     = value; return *this; }
    COGCorrectionFactor &setKZEoir(double value)                     { kZEoir                     = value; return *this; }
    COGCorrectionFactor &setKXUnderCarriage(double value)            { kXUnderCarriage            = value; return *this; }
    COGCorrectionFactor &setKZUnderCarriage(double value)            { kZUnderCarriage            = value; return *this; }
    COGCorrectionFactor &setKXAPU(double value)                      { kXAPU                      = value; return *this; }
    COGCorrectionFactor &setKZAPU(double value)                      { kZAPU                      = value; return *this; }
    COGCorrectionFactor &setKXControlSurfaces(double value)          { kXControlSurfaces          = value; return *this; }
    COGCorrectionFactor &setKZControlSurfaces(double value)          { kZControlSurfaces          = value; return *this; }
    COGCorrectionFactor &setKXPropulsionGroup(double value)          { kXPropulsionGroup          = value; return *this; }
    COGCorrectionFactor &setKZPropulsionGroup(double value)          { kZPropulsionGroup          = value; return *this; }
    COGCorrectionFactor &setKXInstruments(double value)              { kXInstruments              = value; return *this; }
    COGCorrectionFactor &setKZInstruments(double value)              { kZInstruments              = value; return *this; }
    COGCorrectionFactor &setKXHydraulicAndPneumatic(double value)    { kXHydraulicAndPneumatic    = value; return *this; }
    COGCorrectionFactor &setKZHydraulicAndPneumatic(double value)    { kZHydraulicAndPneumatic    = value; return *this; }
    COGCorrectionFactor &setKXElectricalGroup(double value)          { kXElectricalGroup          = value; return *this; }
    COGCorrectionFactor &setKZElectricalGroup(double value)          { kZElectricalGroup          = value; return *this; }
    COGCorrectionFactor &setKXAvionic(double value)                  { kXAvionic                  = value; return *this; }
    COGCorrectionFactor &setKZAvionic(double value)                  { kZAvionic                  = value; return *this; }
    COGCorrectionFactor &setKXFurnishingAndEquipment(double value)   { kXFurnishingAndEquipment   = value; return *this; }
    COGCorrectionFactor &setKZFurnishingAndEquipment(double value)   { kZFurnishingAndEquipment   = value; return *this; }
    COGCorrectionFactor &setKXAntiIcingAndConditioning(double value) { kXAntiIcingAndConditioning = value; return *this; }
    COGCorrectionFactor &setKZAntiIcingAndConditioning(double value) { kZAntiIcingAndConditioning = value; return *this; }
    COGCorrectionFactor &setKXOperatingItems(double value)           { kXOperatingItems           = value; return *this; }
    COGCorrectionFactor &setKZOperatingItems(double value)           { kZOperatingItems           = value; return *this; }
    COGCorrectionFactor &setKXCrew(double value)                     { kXCrew                     = value; return *this; }
    COGCorrectionFactor &setKZCrew(double value)                     { kZCrew                     = value; return *this; }
    COGCorrectionFactor &setKXPayload(double value)                  { kXPayload                  = value; return *this; }
    COGCorrectionFactor &setKZPayload(double value)                  { kZPayload                  = value; return *this; }
    COGCorrectionFactor &setKXFuel(double value)                     { kXFuel                     = value; return *this; }
    COGCorrectionFactor &setKZFuel(double value)                     { kZFuel                     = value; return *this; }

    // =========================================================
    // Getters
    // =========================================================

    double getKXWing()                     const { return kXWing;                     }
    double getKZWing()                     const { return kZWing;                     }
    double getKXCanard()                   const { return kXCanard;                   }
    double getKZCanard()                   const { return kZCanard;                   }
    double getKXHorizontal()               const { return kXHorizontal;               }
    double getKZHorizontal()               const { return kZHorizontal;               }
    double getKXVertical()                 const { return kXVertical;                 }
    double getKZVertical()                 const { return kZVertical;                 }
    double getKXVentralFin()               const { return kXVentralFin;               }
    double getKZVentralFin()               const { return kZVentralFin;               }
    double getKXFuselage()                 const { return kXFuselage;                 }
    double getKZFuselage()                 const { return kZFuselage;                 }
    double getKXBoom()                     const { return kXBoom;                     }
    double getKZBoom()                     const { return kZBoom;                     }
    double getKXEoir()                     const { return kXEoir;                     }
    double getKZEoir()                     const { return kZEoir;                     }
    double getKXUnderCarriage()            const { return kXUnderCarriage;            }
    double getKZUnderCarriage()            const { return kZUnderCarriage;            }
    double getKXAPU()                      const { return kXAPU;                      }
    double getKZAPU()                      const { return kZAPU;                      }
    double getKXControlSurfaces()          const { return kXControlSurfaces;          }
    double getKZControlSurfaces()          const { return kZControlSurfaces;          }
    double getKXPropulsionGroup()          const { return kXPropulsionGroup;          }
    double getKZPropulsionGroup()          const { return kZPropulsionGroup;          }
    double getKXInstruments()              const { return kXInstruments;              }
    double getKZInstruments()              const { return kZInstruments;              }
    double getKXHydraulicAndPneumatic()    const { return kXHydraulicAndPneumatic;    }
    double getKZHydraulicAndPneumatic()    const { return kZHydraulicAndPneumatic;    }
    double getKXElectricalGroup()          const { return kXElectricalGroup;          }
    double getKZElectricalGroup()          const { return kZElectricalGroup;          }
    double getKXAvionic()                  const { return kXAvionic;                  }
    double getKZAvionic()                  const { return kZAvionic;                  }
    double getKXFurnishingAndEquipment()   const { return kXFurnishingAndEquipment;   }
    double getKZFurnishingAndEquipment()   const { return kZFurnishingAndEquipment;   }
    double getKXAntiIcingAndConditioning() const { return kXAntiIcingAndConditioning; }
    double getKZAntiIcingAndConditioning() const { return kZAntiIcingAndConditioning; }
    double getKXOperatingItems()           const { return kXOperatingItems;           }
    double getKZOperatingItems()           const { return kZOperatingItems;           }
    double getKXCrew()                     const { return kXCrew;                     }
    double getKZCrew()                     const { return kZCrew;                     }
    double getKXPayload()                  const { return kXPayload;                  }
    double getKZPayload()                  const { return kZPayload;                  }
    double getKXFuel()                     const { return kXFuel;                     }
    double getKZFuel()                     const { return kZFuel;                     }
};