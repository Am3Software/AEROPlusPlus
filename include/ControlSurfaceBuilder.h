#pragma once

#include "VSPAeroGenerator.h"
#include <string>
#include <vector>
#include <algorithm>

namespace VSP {

struct WingMovables {
    std::vector<char> type;
    std::vector<double> defl;
};

struct Aircraft {
    struct WingData {
        std::string id = "wing";
        WingMovables mov;
    } wing;
    
    struct CanardData {
        std::string id = "canard";
        WingMovables mov;
    } canard;
    
    struct HorizontalData {
        std::string id = "horizontal";
        WingMovables mov;
    } hor;
    
    struct VerticalData {
        std::string id = "vertical";
        WingMovables mov;
        bool isSplittedDeflection = false;
        std::vector<int> activatedRudder;
    } ver;
    
    std::string movables = "none";
    std::string config = "";
};

class ControlSurfaceBuilder {
private:
    static inline void addFlaps(std::vector<ControlSurface>& controls, 
                                const Aircraft& ac, 
                                const std::string& symmetry) {
        ControlSurface flap;
        flap.name = "FLAP";
        
        int numFlaps = 0;
        int numSlats = 0;
        for (char type : ac.wing.mov.type) {
            if (type == 'f') numFlaps++;
            if (type == 's') numSlats++;
        }
        
        if (numFlaps == 0) return;
        
        if (symmetry == "NO") {
            for (int j = 1; j <= numFlaps; j++) {
                int actualIndex = (numSlats > 0) ? j : j;
                flap.surfaces.push_back(ac.wing.id + "_Surf0_flap_" + std::to_string(actualIndex));
                flap.surfaces.push_back(ac.wing.id + "_Surf1_flap_" + std::to_string(actualIndex));
                flap.gains.push_back(1);
                flap.gains.push_back(-1);
            }
        } else {
            for (int j = 1; j <= numFlaps; j++) {
                int actualIndex = (numSlats > 0) ? j : j;
                flap.surfaces.push_back(ac.wing.id + "_Surf0_flap_" + std::to_string(actualIndex));
                flap.gains.push_back(1);
            }
        }
        
        flap.deflection = ac.wing.mov.defl.empty() ? 0.0 : ac.wing.mov.defl[0];
        controls.push_back(flap);
    }
    
    static inline void addAilerons(std::vector<ControlSurface>& controls, 
                                   const Aircraft& ac, 
                                   const std::string& symmetry) {
        ControlSurface aileron;
        aileron.name = "AILERON";
        
        if (symmetry == "NO") {
            aileron.surfaces = {
                ac.wing.id + "_Surf0_aileron",
                ac.wing.id + "_Surf1_aileron"
            };
            aileron.gains = {1, -1};
        } else {
            aileron.surfaces = {ac.wing.id + "_Surf0_aileron"};
            aileron.gains = {1};
        }
        
        for (size_t i = 0; i < ac.wing.mov.type.size(); i++) {
            if (ac.wing.mov.type[i] == 'a') {
                aileron.deflection = (i < ac.wing.mov.defl.size()) ? ac.wing.mov.defl[i] : 0.0;
                break;
            }
        }
        
        controls.push_back(aileron);
    }
    
    static inline void addElevators(std::vector<ControlSurface>& controls, 
                                    const Aircraft& ac, 
                                    const std::string& symmetry) {
        ControlSurface elevator;
        elevator.name = "ELEVATOR";
        
        if (symmetry == "NO") {
            elevator.surfaces = {
                ac.hor.id + "_Surf0_elevator",
                ac.hor.id + "_Surf1_elevator"
            };
            elevator.gains = {1, 1};
        } else {
            elevator.surfaces = {ac.hor.id + "_Surf0_elevator"};
            elevator.gains = {1};
        }
        
        elevator.deflection = ac.hor.mov.defl.empty() ? 0.0 : ac.hor.mov.defl[0];
        controls.push_back(elevator);
    }
    
    static inline void addRudders(std::vector<ControlSurface>& controls, 
                                  const Aircraft& ac, 
                                  const std::string& symmetry) {
        if (ac.ver.isSplittedDeflection) {
            for (size_t j = 0; j < ac.ver.activatedRudder.size(); j++) {
                if (ac.ver.activatedRudder[j] == 1) {
                    ControlSurface rudder;
                    rudder.name = "RUDDER_" + std::to_string(j + 1);
                    rudder.surfaces = {ac.ver.id + "_Surf0_rudder_" + std::to_string(j + 1)};
                    rudder.gains = {-1};
                    rudder.deflection = (j < ac.ver.mov.defl.size()) ? ac.ver.mov.defl[j] : 0.0;
                    controls.push_back(rudder);
                }
            }
        } else {
            ControlSurface rudder;
            rudder.name = "RUDDER";
            
            int numRudders = 0;
            for (char type : ac.ver.mov.type) {
                if (type == 'r') numRudders++;
            }
            
            for (int j = 1; j <= numRudders; j++) {
                rudder.surfaces.push_back(ac.ver.id + "_Surf0_rudder_" + std::to_string(j));
                rudder.gains.push_back(-1);
            }
            
            rudder.deflection = ac.ver.mov.defl.empty() ? 0.0 : ac.ver.mov.defl[0];
            controls.push_back(rudder);
        }
    }
    
    static inline void addCanardFlaps(std::vector<ControlSurface>& controls, 
                                      const Aircraft& ac, 
                                      const std::string& symmetry) {
        ControlSurface canardFlap;
        canardFlap.name = "FLAP_CANARD";
        
        if (symmetry == "NO") {
            canardFlap.surfaces = {
                ac.canard.id + "_Surf0_flapCanard",
                ac.canard.id + "_Surf1_flapCanard"
            };
            canardFlap.gains = {1, -1};
        } else {
            canardFlap.surfaces = {ac.canard.id + "_Surf0_flapCanard"};
            canardFlap.gains = {1};
        }
        
        canardFlap.deflection = ac.canard.mov.defl.empty() ? 0.0 : ac.canard.mov.defl[0];
        controls.push_back(canardFlap);
    }
    
public:
    static inline std::vector<ControlSurface> buildControlSurfaces(
        const Aircraft& ac, 
        const std::string& symmetry) {
        
        std::vector<ControlSurface> controls;
        
        if (ac.movables == "none" || ac.movables.empty()) {
            return controls;
        }
        
        bool hasFlap = (ac.movables.find('f') != std::string::npos);
        bool hasAileron = (ac.movables.find('a') != std::string::npos);
        bool hasElevator = (ac.movables.find('e') != std::string::npos);
        bool hasRudder = (ac.movables.find('r') != std::string::npos);
        bool hasCanard = (ac.movables.find('c') != std::string::npos);
        
        if (hasFlap) {
            addFlaps(controls, ac, symmetry);
        }
        
        if (hasCanard) {
            addCanardFlaps(controls, ac, symmetry);
        }
        
        if (hasAileron) {
            addAilerons(controls, ac, symmetry);
        }
        
        if (hasElevator) {
            addElevators(controls, ac, symmetry);
        }
        
        if (hasRudder) {
            addRudders(controls, ac, symmetry);
        }
        
        return controls;
    }
};

} // namespace VSP