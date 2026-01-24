#ifndef ODE45_H // Impedisce definizioni duplicate dell’header.
#define ODE45_H

#include <string>   // Gestione dei percorsi come stringa.
#include <iostream> // Eventuale diagnostica a console.
#include <vector>
#include <cmath>
#include <numbers>
#include <utility>
#include <functional>            // Per std::function

class ODE45
{

private:
    std::vector<double> tVec; // Vettore che memorizza tutti i valori di t
    std::vector<double> yVec; // Vettore che memorizza tutti i valori di y
    double t0;      
    double tf;      
    double y0;      
    double dt;      

    std::function<double(double, double)> f;


public:
    // MODO CORTO (più efficiente) - Member Initializer List

    /// @brief Constructs an ode45 solver for the given ODE function and parameters.
    /// @param func The function representing the ODE (dy/dt = f(t, y)).
    /// @param t0 The initial time.
    /// @param tf The final time.
    /// @param y0 The initial value of y.
    /// @param dt The time step size (default is 0.01).
    ODE45(std::function<double(double, double)> func, double t0, double tf, double y0, double dt)
        : f(func), t0(t0), tf(tf), y0(y0), dt(dt) {


            solve();

        }

        /// @brief Solves the ODE using the 4th-order Runge-Kutta method.
        /// This method populates the tVec and yVec with computed values.
        void solve() {
        double t = t0;
        double y = y0;
        
        tVec.clear();
        yVec.clear();
        
        for (t = t0; t <= tf; t += dt) {
            tVec.push_back(t);
            yVec.push_back(y);
            
            double k1 = f(t, y);
            double k2 = f(t + dt/2.0, y + dt/2.0 * k1);
            double k3 = f(t + dt/2.0, y + dt/2.0 * k2);
            double k4 = f(t + dt, y + dt * k3);
            
            y = y + (dt/6.0) * (k1 + 2.0*k2 + 2.0*k3 + k4);
        }
    }

    /// @brief Returns the vector of time values.
    /// @return Const reference to the vector of time values.
    const std::vector<double>& getT() const { 
        return tVec; 
    }

    /// @brief Returns the vector of solution values.
    /// @return Const reference to the vector of solution values.
    const std::vector<double>& getY() const { 
        return yVec; 
    }

};

#endif // ODE45_H

// std::function<double(double, double)> f;
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Questo è un "contenitore" per una funzione.
//
// Sintassi: std::function<TipoRitorno(TipoParam1, TipoParam2)>
//
// Nel nostro caso:
//   - double       -> la funzione ritorna un double (cioè dy/dt)
//   - (double, double) -> la funzione accetta due double: (t, y)
//
// Può contenere:
//   1. Una lambda:     f = [](double t, double y) { return -2*y; };
//   2. Una funzione:   f = miaFunzione;
//   3. Un functor:     f = MioOggetto();


//  MODO CORTO (più efficiente) - Member Initializer List
// ode45(std::function<double(double, double)> func, double t0, double tf, double y0, double dt = 0.01)
//     : f(func), t0(t0), tf(tf), y0(y0), dt(dt) { }

// Questo modo di scrivere equivale 

// MODO LUNGO (meno efficiente)
// // ode45(std::function<double(double, double)> func, double t0_, double tf_, double y0_, double dt_ = 0.01) {
// //     this->f = func;
// //     this->t0 = t0_;
// //     this->tf = tf_;
// //     this->y0 = y0_;
// //     this->dt = dt_;
// }

// //ode45(func, t0, tf, y0, dt)
//       │     │   │   │   │
//       │     │   │   │   └──────────────────┐
//       │     │   │   └──────────────────┐   │
//       │     │   └──────────────────┐   │   │
//       │     └──────────────────┐   │   │   │
//       └────────────────────┐   │   │   │   │
//                            ▼   ▼   ▼   ▼   ▼
//                     : f(func), t0(t0), tf(tf), y0(y0), dt(dt)
//                            │   │   │   │   │
//                            ▼   ▼   ▼   ▼   ▼
//                     Variabili membro della classe