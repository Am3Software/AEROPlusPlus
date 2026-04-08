#ifndef ODE45_H
#define ODE45_H

#include <vector>
#include <functional>
#include <cmath>
#include <stdexcept>
#include <type_traits>   // std::invoke_result_t, std::decay_t

// =============================================================================
//  StateTraits<State>
//  Insegna a ODE45 come fare aritmetica su qualsiasi tipo di stato.
//
//  Specializzazioni built-in:
//    - double              → scalare
//    - std::vector<double> → sistema N-dimensionale
//
//  Per tipi custom (es. Eigen::VectorXd) aggiungi una nuova
//  specializzazione in fondo al file.
// =============================================================================

template <typename State>
struct StateTraits; // dichiarazione base — non istanziabile

// ── Specializzazione SCALARE ─────────────────────────────────────────────────
template <>
struct StateTraits<double> {
    /**
     * @brief Returns the additive zero for scalar states.
     * @param Reference scalar value (unused).
     * @return Scalar zero.
     */
    static double zero (const double&)              { return 0.0;    }

    /**
     * @brief Adds two scalar states.
     * @param a First scalar.
     * @param b Second scalar.
     * @return Sum of a and b.
     */
    static double add  (const double& a, const double& b) { return a + b; }

    /**
     * @brief Scales a scalar state.
     * @param s Scale factor.
     * @param v Scalar state.
     * @return Scaled scalar value.
     */
    static double scale(double s,        const double& v) { return s * v; }
};

// ── Specializzazione VETTORIALE ──────────────────────────────────────────────
template <>
struct StateTraits<std::vector<double>> {
    using State = std::vector<double>;

    /**
     * @brief Returns a zero vector with the same size as the reference.
     * @param ref Reference vector used only for sizing.
     * @return Zero vector.
     */
    static State zero(const State& ref) { return State(ref.size(), 0.0); }

    /**
     * @brief Adds two vectors element-wise.
     * @param a First vector.
     * @param b Second vector.
     * @return Element-wise sum.
     * @throws std::runtime_error If vector sizes do not match.
     */
    static State add(const State& a, const State& b) {
        if (a.size() != b.size())
            throw std::runtime_error("ODE45: dimensioni vettore incompatibili");
        State res(a.size());
        for (size_t i = 0; i < a.size(); i++) res[i] = a[i] + b[i];
        return res;
    }

    /**
     * @brief Multiplies each vector component by a scalar.
     * @param s Scale factor.
     * @param v Input vector.
     * @return Scaled vector.
     */
    static State scale(double s, const State& v) {
        State res(v.size());
        for (size_t i = 0; i < v.size(); i++) res[i] = s * v[i];
        return res;
    }
};


// =============================================================================
//  Helper per le deduction guides
//
//  DeduceState<Func, State>  estrae il tipo di stato in due modi:
//
//    1. Dalla condizione iniziale y0 (via State, parametro esplicito)
//    2. Dal tipo di ritorno della funzione f (via std::invoke_result_t)
//
//  Entrambe devono coincidere — il compilatore lo verifica da solo.
// =============================================================================

// =============================================================================
//  ODE45<State>  —  solutore RK4 generico (C++17)
//
//  Con le deduction guides NON serve specificare <State> esplicitamente:
//
//    // scalare — il compilatore deduce ODE45<double>
//    ODE45 s([](double t, const double& y){ return -2*y; }, 0, 10, 1.0, 0.01);
//
//    // vettoriale — deduce ODE45<std::vector<double>>
//    ODE45 s(eqMoto, 0, 20, std::vector{1.0, 0.0}, 0.01);
// =============================================================================

template <typename State>
class ODE45 {

    using Traits  = StateTraits<State>;
    using OdeFunc = std::function<State(double, const State&)>;

private:
    OdeFunc             f;
    double              t0, tf, dt;
    State               y0;

    std::vector<double> tVec;
    std::vector<State>  yVec;

    // ── aritmetica interna ───────────────────────────────────────────────────
    /**
     * @brief Adds two states through StateTraits.
     * @param a First state.
     * @param b Second state.
     * @return Sum state.
     */
    State add  (const State& a, const State& b) const { return Traits::add(a, b);   }

    /**
     * @brief Scales a state through StateTraits.
     * @param s Scale factor.
     * @param v Input state.
     * @return Scaled state.
     */
    State scale(double s, const State& v)       const { return Traits::scale(s, v); }

public:
    /// @brief Builds the solver from any compatible callable.
    /// @tparam Func  Callable type (lambda, function, functor, std::function)
    /// @param  func  dy/dt = func(t, y)
    /// @param  t0    Initial time
    /// @param  tf    Final time
    /// @param  y0    Initial condition
    /// @param  dt    Integration step (default 0.01)
    template <typename Func>
    ODE45(Func&& func, double t0, double tf, const State& y0, double dt = 0.01)
        : f(std::forward<Func>(func)), t0(t0), tf(tf), y0(y0), dt(dt)
    {
        if (dt <= 0)  throw std::invalid_argument("ODE45: dt deve essere > 0");
        if (tf <= t0) throw std::invalid_argument("ODE45: tf deve essere > t0");
        solve();
    }

    /// @brief Riesegue l'integrazione (es. dopo aver cambiato parametri).
    // void solve() {
    //     tVec.clear();
    //     yVec.clear();

    //     double t = t0;
    //     State  y = y0;

    //     for (; t <= tf; t += dt) {
    //         tVec.push_back(t);
    //         yVec.push_back(y);

    //         State k1 = f(t,        y);
    //         State k2 = f(t + dt/2, add(y, scale(dt/2, k1)));
    //         State k3 = f(t + dt/2, add(y, scale(dt/2, k2)));
    //         State k4 = f(t + dt,   add(y, scale(dt,   k3)));

    //         y = add(y, scale(dt/6.0,
    //                 add(add(k1, scale(2.0, k2)),
    //                     add(scale(2.0, k3), k4))));
    //     }
    // }

    /**
     * @brief Runs RK4 integration from t0 to tf.
     */
    void solve() {
    tVec.clear();
    yVec.clear();

    double t = t0;
    State  y = y0;

    // Salva la condizione iniziale
    tVec.push_back(t);
    yVec.push_back(y);

    while (t < tf - dt * 1e-9) {
        State k1 = f(t,        y);
        State k2 = f(t + dt/2, add(y, scale(dt/2, k1)));
        State k3 = f(t + dt/2, add(y, scale(dt/2, k2)));
        State k4 = f(t + dt,   add(y, scale(dt,   k3)));

        y = add(y, scale(dt/6.0,
                add(add(k1, scale(2.0, k2)),
                    add(scale(2.0, k3), k4))));
        t += dt;

        // Salva DOPO l'aggiornamento
        tVec.push_back(t);
        yVec.push_back(y);  // ← ora getY().back() = y(tf) ✓
    }
}

    /**
     * @brief Returns sampled time values.
     * @return Constant reference to the time vector.
     */
    const std::vector<double>& getT() const { return tVec; }

    /**
     * @brief Returns sampled state values.
     * @return Constant reference to the state vector.
     */
    const std::vector<State>&  getY() const { return yVec; }

    /**
     * @brief Returns the number of stored samples.
     * @return Sample count.
     */
    size_t size()                     const { return tVec.size(); }
};


// =============================================================================
//  DEDUCTION GUIDES  (C++17 — CTAD)
//
//  Permettono di scrivere  ODE45(...)  senza  ODE45<State>(...).
//  Il compilatore deduce State in due passaggi:
//
//  GUIDA 1 — da y0
//  ┌─────────────────────────────────────────────────────────────────┐
//  │  ODE45 s(func, 0, 10, 1.0, 0.01);                              │
//  │                       ^^^                                       │
//  │  y0 è double  →  State = double  →  ODE45<double>              │
//  └─────────────────────────────────────────────────────────────────┘
//
//  GUIDA 2 — dal tipo di ritorno di Func
//  ┌─────────────────────────────────────────────────────────────────┐
//  │  ODE45 s(func, 0, 10, std::vector{1.0, 0.0}, 0.01);            │
//  │          ^^^^                                                   │
//  │  func ritorna vector<double>                                    │
//  │  → State = invoke_result_t<Func, double, const vector<double>&> │
//  │  → ODE45<vector<double>>                                        │
//  └─────────────────────────────────────────────────────────────────┘
//
//  Le due guide devono concordare: se y0 è double ma func ritorna
//  vector<double>, il compilatore emette un errore di deduzione.
// =============================================================================

// GUIDA UNICA: State dedotto dalla condizione iniziale y0
//
//  ODE45 s(func, 0, 10, 1.0, 0.01)        → ODE45<double>
//  ODE45 s(func, 0, 20, std::vector{...}) → ODE45<vector<double>>
//  ODE45 s(func, 0, 20, y0_eigen)         → ODE45<Eigen::VectorXd>
//
//  Una sola guida elimina ogni ambiguità: State è sempre il tipo di y0.
template <typename Func, typename State>
ODE45(Func&&, double, double, const State&, double = 0.01)
    -> ODE45<State>;


// =============================================================================
//  Specializzazione EIGEN  (attiva solo se Eigen è già incluso)
//
//  Basta includere Eigen PRIMA di questo header:
//
//      #include <Eigen/Dense>   // ← deve venire prima
//      #include "ODE45.h"
//
//  Poi si usa esattamente come gli altri casi:
//
//      Eigen::VectorXd y0(2);
//      y0 << 1.0, 0.0;
//
//      ODE45 s(eqMoto, 0.0, 20.0, y0, 0.01);   // deduce ODE45<VectorXd>
//      double x   = s.getY()[i][0];
//      double vel = s.getY()[i][1];
//
//  Se Eigen non è incluso, questo blocco viene ignorato completamente.
// =============================================================================

#ifdef EIGEN_WORLD_VERSION   // macro definita da <Eigen/Core> e <Eigen/Dense>

template <>
struct StateTraits<Eigen::VectorXd> {

    using State = Eigen::VectorXd;

    /// Vettore zero della stessa dimensione di ref
    static State zero(const State& ref) {
        return State::Zero(ref.size());
    }

    /// Somma vettoriale — Eigen la gestisce nativamente con operator+
    static State add(const State& a, const State& b) {
        if (a.size() != b.size())
            throw std::runtime_error("ODE45: dimensioni VectorXd incompatibili");
        return a + b;
    }

    /// Scala vettore — Eigen gestisce s * v nativamente
    static State scale(double s, const State& v) {
        return s * v;
    }
};

// Deduction guide per Eigen::VectorXd
// Rende possibile  ODE45 s(func, t0, tf, y0_eigen, dt)  senza <VectorXd>
template <typename Func>
ODE45(Func&&, double, double, const Eigen::VectorXd&, double = 0.01)
    -> ODE45<Eigen::VectorXd>;

#endif // EIGEN_WORLD_VERSION

#endif // ODE45_H