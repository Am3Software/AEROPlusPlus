#ifndef Interpolant_H
#define Interpolant_H

#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <numbers>
#include "RegressionMethod.h" // Inclusione dell'enum RegressionMethod che è una enumerazione dei metodi di regressione disponibili
#include <Eigen/Dense>
#include "gnuplot-iostream.h"
#include <utility>
#include <string>
#include <algorithm>
#include <cctype>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

/// =========================== Constructor ==============================
/**
 * @brief Constructs an Interpolant object and performs the chosen regression.
 *
 * Initializes the interpolant with the provided data and automatically executes
 * the specified regression method.
 *
 * @param x Vector of independent variable values.
 * @param y Vector of dependent variable values.
 * @param degree Polynomial degree (used only for POLYNOMIAL, ignored otherwise).
 * @param method Regression method to apply (see RegressionMethod enum).
 *
 * @throw std::runtime_error If insufficient data points for polynomial regression.
 */
class Interpolant
{

private:
    // ======================== Private data members ========================
    // Variables intialization
    std::vector<double> xValues;
    std::vector<double> yValues;
    int Degree;
    RegressionMethod Method;                  // Metodo di regressione scelto
    std::vector<double> polyCoeff;            // Coefficienti del polinomio per la regressione polinomiale
    double valueOfX;                          // Query point dobe valutare l'output della regressione
    double ALinear;                           // Alinear slope della regressione lineare
    double BLinear;                           // BLinear intercetta della regressione lineare
    double AExp;                              // Intercetta coefficiente della regressione esponenziale
    double BExp;                              // Slope coefficiente della regressione esponenziale
    double APower;                            // Intercetta coefficiente della regressione potenza
    double BPower;                            // Slope coefficiente della regressione potenza
    double ALog;                              // Slope coefficiente della regressione logaritmica
    double BLog;                              // Intercettacoefficiente della regressione logaritmica
    double numberOfIntervalsForChart = 100.0; // Numero di intervalli per il grafico della regressione

    // ===================== Private regression helpers =====================
    // Metodo privato che ci permette di ottenre i coeffcienti delle regressioni
    void linearRegression()
    {

        double Numerator = 0.0;
        double Denominator = 0.0;
        double meanX = 0.0;
        double meanY = 0.0;

        double sumX = 0.0, sumY = 0.0;
        for (size_t i = 0; i < xValues.size(); ++i)
        {
            sumX += xValues[i];
            sumY += yValues[i];
        }

        meanX = sumX / xValues.size();
        meanY = sumY / xValues.size();

        // Calcolo numeratore

        for (int ii = 0; ii < xValues.size(); ii++)
        {

            Numerator += (xValues[ii] - meanX) * (yValues[ii] - meanY);
            Denominator += std::pow((xValues[ii] - meanX), 2);

            if (ii == xValues.size() - 1)
            {

                ALinear = Numerator / Denominator; // Slope dell'interpolante lineare
                BLinear = meanY - ALinear * meanX; // Intercetta dell'interpolante lineare
            }
        }

        return;
    }

    // Metodo privato per la regressione polinomiale
    void polynomialRegression()
    {

        using namespace Eigen;

        int m = xValues.size();
        int n = Degree;

        if (n <= 1)
        {

            throw std::runtime_error("Degree must be greater than 1 for polynomial regression");
        }

        if (m < n + 1)
        {
            throw std::runtime_error("Not enough points for polynomial regression");
        }

        // --- Costruzione Vandermonde come fa MATLAB---
        MatrixXd V(m, n + 1); // MatrixXd deriva da Eigen ed è una matrice di double a dimensione dinamica

        for (int i = 0; i < m; ++i)
        {
            double xi = xValues[i];
            for (int j = 0; j <= n; ++j)
            {
                V(i, j) = std::pow(xi, n - j);
            }
        }

        // --- y vector ---
        VectorXd Y(m); // VectorXd deriva da Eigen ed è un vettore di double a dimensione dinamica
        for (int i = 0; i < m; ++i)
            Y(i) = yValues[i];

        // --- Least squares: V \ y (QR pivotato, come MATLAB) ---
        VectorXd p = V.colPivHouseholderQr().solve(Y);

        // --- Salva coefficienti ---
        polyCoeff.resize(n + 1);
        for (int i = 0; i <= n; ++i)
            polyCoeff[i] = p(i);
    }

    // Metodo privato per la regressione esponenziale
    void exponentialRegression()
    {

        double NumeratorExp = 0.0;
        double DenominatorExp = 0.0;
        double meanXExp = 0.0;
        double meanYExp = 0.0;

        double sumX = 0.0, sumLnY = 0.0;
        for (size_t i = 0; i < xValues.size(); ++i)
        {
            sumX += xValues[i];
            sumLnY += std::log(yValues[i]);
        }

        meanXExp = sumX / xValues.size();
        meanYExp = sumLnY / xValues.size();

        // Calcolo numeratore

        for (int ii = 0; ii < xValues.size(); ii++)
        {

            NumeratorExp += (xValues[ii] - meanXExp) * (std::log(yValues[ii]) - meanYExp);
            DenominatorExp += std::pow((xValues[ii] - meanXExp), 2);

            if (ii == xValues.size() - 1)
            {

                BExp = NumeratorExp / DenominatorExp;        // Slope dell'interpolante esponenziale
                AExp = std::exp(meanYExp - BExp * meanXExp); // Intercetta dell'interpolante esponenziale
            }
        }

        return;
    }

    // Metodo privato per la regressione potenza
    void powerRegression()
    {

        double NumeratorPower = 0.0;
        double DenominatorPower = 0.0;
        double meanXPower = 0.0;
        double meanYPower = 0.0;

        double sumLnX = 0.0, sumLnY = 0.0;

        for (size_t i = 0; i < xValues.size(); ++i)
        {
            sumLnX += std::log(xValues[i]);
            sumLnY += std::log(yValues[i]);
        }

        meanXPower = sumLnX / xValues.size();
        meanYPower = sumLnY / xValues.size();

        // Calcolo numeratore

        for (int ii = 0; ii < xValues.size(); ii++)
        {

            NumeratorPower += (std::log(xValues[ii]) - meanXPower) * (std::log(yValues[ii]) - meanYPower);
            DenominatorPower += std::pow((std::log(xValues[ii]) - meanXPower), 2);

            if (ii == xValues.size() - 1)
            {

                BPower = NumeratorPower / DenominatorPower;          // Slope dell'interpolante potenza
                APower = std::exp(meanYPower - BPower * meanXPower); // Intercetta dell'interpolante potenza
            }
        }

        return;
    }

    // Metodo privato per la regressione logaritmica
    void logarithmicRegression()
    {

        double NumeratorLog = 0.0;
        double DenominatorLog = 0.0;
        double meanXLog = 0.0;
        double meanYLog = 0.0;

        double sumLnX = 0.0, sumY = 0.0;

        for (size_t i = 0; i < xValues.size(); ++i)
        {
            sumLnX += std::log(xValues[i]);
            sumY += yValues[i];
        }

        meanXLog = sumLnX / xValues.size();
        meanYLog = sumY / xValues.size();

        // Calcolo numeratore

        for (size_t ii = 0; ii < xValues.size(); ii++)
        {

            NumeratorLog += (std::log(xValues[ii]) - meanXLog) * (yValues[ii] - meanYLog);
            DenominatorLog += std::pow((std::log(xValues[ii]) - meanXLog), 2);
            if (ii == xValues.size() - 1)
            {

                ALog = NumeratorLog / DenominatorLog; // Slope dell'interpolante logaritmico
                BLog = meanYLog - ALog * meanXLog;    // Intercetta dell'interpolante logaritmico
            }
        }
    }

    // Metodo privato che raccoglie tutti i metodi di regressione disponibili
    void makeRegression()
    {

        switch (Method)
        {

        case RegressionMethod::LINEAR:
            linearRegression();
            break;

        case RegressionMethod::POLYNOMIAL:
            polynomialRegression();
            break;

        case RegressionMethod::EXPONENTIAL:
            exponentialRegression();
            break;

        case RegressionMethod::POWER:
            powerRegression();
            break;

        case RegressionMethod::LOGARITHMIC:
            logarithmicRegression();
            break;
        }
    }

public:
    Interpolant(std::vector<double> x, std::vector<double> y, int degree, RegressionMethod method)
    {

        // Inizializzo i membri della classe con i valori passati al costruttore

        this->xValues = x;
        this->yValues = y;
        this->Degree = degree;
        this->Method = method;

        makeRegression();
    }

    // ========================== Public API =================================

    /// Method to get the estimated Y value from the regression at a specific X value
    double getYValueFromRegression(double valueOfX)
    {

        this->valueOfX = valueOfX;

        switch (this->Method)
        { // In Method c'è già salvato il metodo di regressione scelto passato al costruttore

        case RegressionMethod::LINEAR:

            std::cout << "The calculated value from linear regression is: " << ALinear * valueOfX + BLinear << std::endl;

            return ALinear * valueOfX + BLinear;

        case RegressionMethod::POLYNOMIAL:

        { // Queste parentesi servono per creare un nuovo scope in modo da poter dichiarare variabili locali come yValue

            double yValue = 0.0;
            for (int i = 0; i <= Degree; i++)
            {
                yValue += polyCoeff[i] * std::pow(valueOfX, Degree - i);
            }
            std::cout << "The calculated value from polynomial regression is: " << yValue << std::endl;
            return yValue;

        } // Fine nuovo scope

        case RegressionMethod::EXPONENTIAL:

            std::cout << "The calculated value from exponential regression is: " << AExp * std::exp(BExp * valueOfX) << std::endl;

            return AExp * std::exp(BExp * valueOfX);

        case RegressionMethod::POWER:

            std::cout << "The calculated value from power regression is: " << APower * std::pow(valueOfX, BPower) << std::endl;

            return APower * std::pow(valueOfX, BPower);

        case RegressionMethod::LOGARITHMIC:

            std::cout << "The calculated value from logarithmic regression is: " << ALog * std::log(valueOfX) + BLog << std::endl;

            return ALog * std::log(valueOfX) + BLog;

        case RegressionMethod::CONSTANT:

            std::cout << "The calculated value from constant regression is: " << valueOfX << std::endl;

            return valueOfX;

        default:

            std::cout << "Regression method not implemented." << std::endl;

            return 0.0;
        }
    }

    /// Method to retrieve the coefficients of the regression model
    /// LINEAR : [Slope, Intercept]
    /// POLYNOMIAL : [p_n, p_(n-1), ..., p_1, p_0]
    /// EXPONENTIAL : [A, B] where y = A * exp(B * x)
    /// POWER : [A, B] where y = A * x^B
    /// LOGARITHMIC : [A, B] where y = A * ln(x) + B
    /// CONSTANT : [Input Value]
    std::vector<double> getCoefficients()
    {

        switch (this->Method)
        {

        case RegressionMethod::LINEAR:

            return {ALinear, BLinear};

        case RegressionMethod::POLYNOMIAL:

            for (int i = 0; i < polyCoeff.size(); i++)
            {
                std::cout << "p_" << i + 1 << " :" << polyCoeff[i] << "\n";
            }
            std::cout << std::endl;

            return polyCoeff;

        case RegressionMethod::EXPONENTIAL:

            return {AExp, BExp};

        case RegressionMethod::POWER:

            return {APower, BPower};

        case RegressionMethod::LOGARITHMIC:

            return {ALog, BLog};

        case RegressionMethod::CONSTANT:

            return {valueOfX};

        default:

            std::cout << "Regression method not implemented." << std::endl;
            return {};
        }
    }

    /// @brief Generates and displays a chart of the regression using Gnuplot.
    /// @param xLabelChart Label for the X axis.
    /// @param yLabelChart Label for the Y axis.
    /// @param xUnit Unit of measurement for the X axis.
    /// @param yUnit Unit of measurement for the Y axis.
    void getChartOfRegression(std::string xLabelChart, std::string yLabelChart, std::string xUnit, std::string yUnit)
    {

        switch (this->Method)
        {

        case RegressionMethod::LINEAR:
        {
            std::vector<double> xLinear;
            std::vector<double> yLinear;
           
            double deltaH = 0.0;
            double vectorLength = 0.0;
            double minVale = *std::min_element(xValues.begin(), xValues.end());
            double maxVale = *std::max_element(xValues.begin(), xValues.end());

            // Uso std::max_element/std::min_element per ottenere gli iteratori al massimo e minimo di xValues,
            // li dereferenzio con * per accedere ai valori double e ne faccio la differenza per ricavare l’intervallo complessivo
            vectorLength = *std::max_element(xValues.begin(), xValues.end()) - *std::min_element(xValues.begin(), xValues.end());

            deltaH = vectorLength / numberOfIntervalsForChart;

            for (double i = minVale; i <= maxVale; i += deltaH)
            {

                xLinear.emplace_back(i); // Uso l'indice i così le coppie (xLinear, yLinear) corrispondono ai punti della retta nel dominio originale.
                yLinear.emplace_back(ALinear * i + BLinear);
            }

            // Preparazione dati per il plot
            std::vector<std::pair<double, double>> dataForChart;
            std::vector<std::pair<double, double>> xyPoint;
            for (size_t i = 0; i < xLinear.size(); i++)
            {

                dataForChart.emplace_back(xLinear[i], yLinear[i]);

                if (i < xValues.size())
                {
                    xyPoint.emplace_back(xValues[i], yValues[i]);
                }
            }

            try
            {
                Gnuplot gp;
                // Mantiene il terminale wxt persistente, così la finestra del grafico resta valida anche se ridotta a icona
                // gp << "set term wxt persist\n";
                // gp << "set title 'Linear regression'\n";
                // gp << "set xlabel '" << xLabelChart << "'\n";
                // gp << "set ylabel '" << yLabelChart << "'\n";
                // gp << "set grid\n";

                std::string xCmd = "set xlabel '" + xLabelChart + " (" + xUnit + ")'\n";
                std::string yCmd = "set ylabel '" + yLabelChart + " (" + yUnit + ")'\n";

                gp << "set term wxt enhanced font 'Arial,10'\n";

                gp << "set encoding utf8\n";
                gp << "unset margins\n"; // Lascia che gnuplot calcoli i margini corretti

                gp << "set title 'Linear regression'\n";

                // Imposta le label
                gp << xCmd;
                gp << yCmd;

                gp << "set grid\n";
                /* ============================================================
                  STILE DELLE CURVE (colori, linee, marker)
             ============================================================ */

                /*
                   Definizione stile linea 1
                   - lc rgb '#0072BD' : colore blu (palette MATLAB-like)
                   - lw 2.5           : spessore linea
                   - lt 1             : tipo di linea (continua)
                   - pt 7             : tipo di marker (cerchio)
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 1 lc rgb '#0072BD' lw 2.5 lt 1\n";

                /*
                   Definizione stile linea 2
                   - lc rgb '#D95319' : colore arancio
                   - lw 2.5           : spessore linea
                   - lt 1             : linea continua
                   - pt 5             : marker quadrato
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 2 lc rgb '#0bec16' lw 2.5 lt 1 pt 7 ps 1.0\n";

                /* ============================================================
                Se voglio solo le linee senza marker, rimuovo pt e ps dalle definizioni di stile
                 ============================================================ */

                /*
                   gp << "plot '-' with lines ls 1 title 'CP upper', "
                          "'-' with lines ls 2 title 'CP lower'\n";
                */

                /* ============================================================
                Se voglio linee con marker
                 ============================================================ */

                // gp << "plot '-' with lines ls 1 title 'Regression line', "
                //       "'-' with points ls 2 title 'Data points'\n";

                gp << "plot '-' with lines ls 1 title 'Regression line', "
                      "'-' with points ls 2 title 'Data points'\n";

                // Plotto i grafici come se fosse l'hold on di MATLAB
                gp.send1d_raw(dataForChart);
                gp.send1d_raw(xyPoint);

                // gp << "set output\n";  // Chiude il file PNG
                // gp << "replot\n";
                gp.flush();
                gp << "pause -1\n";
            }
            catch (const std::exception &ex)
            {
                std::cerr << "Errore: " << ex.what() << std::endl;
            }

            break;
        }
        case RegressionMethod::POLYNOMIAL:

        {
            std::vector<double> xPoli;
            std::vector<double> yPoli;
            double deltaH = 0.0;
            double vectorLength = 0.0;
            double minVale = *std::min_element(xValues.begin(), xValues.end());
            double maxVale = *std::max_element(xValues.begin(), xValues.end());

            vectorLength = maxVale - minVale;

            deltaH = vectorLength / numberOfIntervalsForChart;

            for (double i = minVale; i <= maxVale; i += deltaH)
            {

                xPoli.emplace_back(i); // Uso l'indice i così le coppie (xPoli, yPoli) corrispondono ai punti della retta nel dominio originale.

                double yValue = 0.0;
                for (int j = 0; j < polyCoeff.size(); ++j)
                {
                    yValue += polyCoeff[j] * std::pow(i, Degree - j);
                }
                yPoli.emplace_back(yValue);
            }

            // Preparazione dati per il plot
            std::vector<std::pair<double, double>> dataForChart;
            std::vector<std::pair<double, double>> xyPoint;
            for (size_t i = 0; i < xPoli.size(); i++)
            {

                dataForChart.emplace_back(xPoli[i], yPoli[i]);

                if (i < xValues.size())
                {
                    xyPoint.emplace_back(xValues[i], yValues[i]);
                }
            }

            try
            {
                Gnuplot gp;
                // Mantiene il terminale wxt persistente, così la finestra del grafico resta valida anche se ridotta a icona
                // gp << "set term wxt persist\n";
                // gp << "set title 'Linear regression'\n";
                // gp << "set xlabel '" << xLabelChart << "'\n";
                // gp << "set ylabel '" << yLabelChart << "'\n";
                // gp << "set grid\n";

                std::string xCmd = "set xlabel '" + xLabelChart + "'\n";
                std::string yCmd = "set ylabel '" + yLabelChart + "'\n";

                gp << "set term wxt enhanced font 'Arial,10'\n";

                gp << "set encoding utf8\n";
                gp << "unset margins\n"; // Lascia che gnuplot calcoli i margini corretti

                gp << "set title 'Linear regression'\n";

                // Imposta le label
                gp << xCmd;
                gp << yCmd;

                gp << "set grid\n";
                /* ============================================================
                  STILE DELLE CURVE (colori, linee, marker)
             ============================================================ */

                /*
                   Definizione stile linea 1
                   - lc rgb '#0072BD' : colore blu (palette MATLAB-like)
                   - lw 2.5           : spessore linea
                   - lt 1             : tipo di linea (continua)
                   - pt 7             : tipo di marker (cerchio)
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 1 lc rgb '#0072BD' lw 2.5 lt 1\n";

                /*
                   Definizione stile linea 2
                   - lc rgb '#D95319' : colore arancio
                   - lw 2.5           : spessore linea
                   - lt 1             : linea continua
                   - pt 5             : marker quadrato
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 2 lc rgb '#0bec16' lw 2.5 lt 1 pt 7 ps 1.0\n";

                /* ============================================================
                Se voglio solo le linee senza marker, rimuovo pt e ps dalle definizioni di stile
                 ============================================================ */

                /*
                   gp << "plot '-' with lines ls 1 title 'CP upper', "
                          "'-' with lines ls 2 title 'CP lower'\n";
                */

                /* ============================================================
                Se voglio linee con marker
                 ============================================================ */

                // gp << "plot '-' with lines ls 1 title 'Regression line', "
                //       "'-' with points ls 2 title 'Data points'\n";

                gp << "plot '-' with lines ls 1 title 'Regression line', "
                      "'-' with points ls 2 title 'Data points'\n";

                // Plotto i grafici come se fosse l'hold on di MATLAB
                gp.send1d_raw(dataForChart);
                gp.send1d_raw(xyPoint);

                // gp << "set output\n";  // Chiude il file PNG
                // gp << "replot\n";
                gp.flush();
                gp << "pause -1\n";
            }
            catch (const std::exception &ex)
            {
                std::cerr << "Errore: " << ex.what() << std::endl;
            }

            break;
        }

        case RegressionMethod::EXPONENTIAL:

        {

            std::vector<double> xExp;
            std::vector<double> yExp;
            double deltaH = 0.0;
            double vectorLength = 0.0;
            double minVale = *std::min_element(xValues.begin(), xValues.end());
            double maxVale = *std::max_element(xValues.begin(), xValues.end());

            vectorLength = maxVale - minVale;

            deltaH = vectorLength / numberOfIntervalsForChart;

            for (double i = minVale; i <= maxVale; i += deltaH)
            {

                xExp.emplace_back(i);
                yExp.emplace_back(AExp * std::exp(BExp * i));
            }

            // Preparazione dati per il plot
            std::vector<std::pair<double, double>> dataForChart;
            std::vector<std::pair<double, double>> xyPoint;
            for (size_t i = 0; i < xExp.size(); i++)
            {

                dataForChart.emplace_back(xExp[i], yExp[i]);

                if (i < xValues.size())
                {
                    xyPoint.emplace_back(xValues[i], yValues[i]);
                }
            }

            try
            {
                Gnuplot gp;
                // Mantiene il terminale wxt persistente, così la finestra del grafico resta valida anche se ridotta a icona
                // gp << "set term wxt persist\n";
                // gp << "set title 'Linear regression'\n";
                // gp << "set xlabel '" << xLabelChart << "'\n";
                // gp << "set ylabel '" << yLabelChart << "'\n";
                // gp << "set grid\n";

                std::string xCmd = "set xlabel '" + xLabelChart + "'\n";
                std::string yCmd = "set ylabel '" + yLabelChart + "'\n";

                gp << "set term wxt enhanced font 'Arial,10'\n";

                gp << "set encoding utf8\n";
                gp << "unset margins\n"; // Lascia che gnuplot calcoli i margini corretti

                gp << "set title 'Linear regression'\n";

                // Imposta le label
                gp << xCmd;
                gp << yCmd;

                gp << "set grid\n";
                /* ============================================================
                  STILE DELLE CURVE (colori, linee, marker)
             ============================================================ */

                /*
                   Definizione stile linea 1
                   - lc rgb '#0072BD' : colore blu (palette MATLAB-like)
                   - lw 2.5           : spessore linea
                   - lt 1             : tipo di linea (continua)
                   - pt 7             : tipo di marker (cerchio)
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 1 lc rgb '#0072BD' lw 2.5 lt 1\n";

                /*
                   Definizione stile linea 2
                   - lc rgb '#D95319' : colore arancio
                   - lw 2.5           : spessore linea
                   - lt 1             : linea continua
                   - pt 5             : marker quadrato
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 2 lc rgb '#0bec16' lw 2.5 lt 1 pt 7 ps 1.0\n";

                /* ============================================================
                Se voglio solo le linee senza marker, rimuovo pt e ps dalle definizioni di stile
                 ============================================================ */

                /*
                   gp << "plot '-' with lines ls 1 title 'CP upper', "
                          "'-' with lines ls 2 title 'CP lower'\n";
                */

                /* ============================================================
                Se voglio linee con marker
                 ============================================================ */

                // gp << "plot '-' with lines ls 1 title 'Regression line', "
                //       "'-' with points ls 2 title 'Data points'\n";

                gp << "plot '-' with lines ls 1 title 'Regression line', "
                      "'-' with points ls 2 title 'Data points'\n";

                // Plotto i grafici come se fosse l'hold on di MATLAB
                gp.send1d_raw(dataForChart);
                gp.send1d_raw(xyPoint);

                // gp << "set output\n";  // Chiude il file PNG
                // gp << "replot\n";
                gp.flush();
                gp << "pause -1\n";
            }
            catch (const std::exception &ex)
            {
                std::cerr << "Errore: " << ex.what() << std::endl;
            }

            break;
        }

        case RegressionMethod::POWER:

        {
            std::vector<double> xPower;
            std::vector<double> yPower;
            double deltaH = 0.0;
            double vectorLength = 0.0;
            double minVale = *std::min_element(xValues.begin(), xValues.end());
            double maxVale = *std::max_element(xValues.begin(), xValues.end());

            vectorLength = maxVale - minVale;

            deltaH = vectorLength / numberOfIntervalsForChart;

            for (double i = minVale; i <= maxVale; i += deltaH)
            {
                xPower.emplace_back(i);
                yPower.emplace_back(APower * std::pow(i, BPower));
            }

            // Preparazione dati per il plot
            std::vector<std::pair<double, double>> dataForChart;
            std::vector<std::pair<double, double>> xyPoint;
            for (size_t i = 0; i < xPower.size(); i++)
            {

                dataForChart.emplace_back(xPower[i], yPower[i]);

                if (i < xValues.size())
                {
                    xyPoint.emplace_back(xValues[i], yValues[i]);
                }
            }

            try
            {
                Gnuplot gp;
                // Mantiene il terminale wxt persistente, così la finestra del grafico resta valida anche se ridotta a icona
                gp << "set term wxt persist\n";
                gp << "set title 'Power regression'\n";
                gp << "set xlabel '" << xLabelChart << "'\n";
                gp << "set ylabel '" << yLabelChart << "'\n";
                gp << "set grid\n";

                /* ============================================================
                  STILE DELLE CURVE (colori, linee, marker)
             ============================================================ */

                /*
                   Definizione stile linea 1
                   - lc rgb '#0072BD' : colore blu (palette MATLAB-like)
                   - lw 2.5           : spessore linea
                   - lt 1             : tipo di linea (continua)
                   - pt 7             : tipo di marker (cerchio)
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 1 lc rgb '#0072BD' lw 2.5 lt 1\n";

                /*
                   Definizione stile linea 2
                   - lc rgb '#D95319' : colore arancio
                   - lw 2.5           : spessore linea
                   - lt 1             : linea continua
                   - pt 5             : marker quadrato
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 2 lc rgb '#0bec16' lw 2.5 lt 1 pt 7 ps 1.0\n";

                /* ============================================================
                Se voglio solo le linee senza marker, rimuovo pt e ps dalle definizioni di stile
                 ============================================================ */

                /*
                   gp << "plot '-' with lines ls 1 title 'CP upper', "
                          "'-' with lines ls 2 title 'CP lower'\n";
                */

                /* ============================================================
                Se voglio linee con marker
                 ============================================================ */

                gp << "plot '-' with lines ls 1 title 'Regression line', "
                      "'-' with points ls 2 title 'Data points'\n";

                // Plotto i grafici come se fosse l'hold on di MATLAB
                gp.send1d_raw(dataForChart);
                gp.send1d_raw(xyPoint);

                gp << "pause -1\n";
            }
            catch (const std::exception &ex)
            {
                std::cerr << "Errore: " << ex.what() << std::endl;
            }

            break;
        }

        case RegressionMethod::LOGARITHMIC:

        {

            std::vector<double> xLog;
            std::vector<double> yLog;
            double deltaH = 0.0;
            double vectorLength = 0.0;
            double minVale = *std::min_element(xValues.begin(), xValues.end());
            double maxVale = *std::max_element(xValues.begin(), xValues.end());

            vectorLength = maxVale - minVale;

            deltaH = vectorLength / numberOfIntervalsForChart;

            for (double i = minVale; i <= maxVale; i += deltaH)
            {
                xLog.emplace_back(i);
                yLog.emplace_back(ALog * std::log(i) + BLog);
            }

            // Preparazione dati per il plot
            std::vector<std::pair<double, double>> dataForChart;
            std::vector<std::pair<double, double>> xyPoint;
            for (size_t i = 0; i < xLog.size(); i++)
            {

                dataForChart.emplace_back(xLog[i], yLog[i]);

                if (i < xValues.size())
                {
                    xyPoint.emplace_back(xValues[i], yValues[i]);
                }
            }

            try
            {
                Gnuplot gp;
                // Mantiene il terminale wxt persistente, così la finestra del grafico resta valida anche se ridotta a icona
                // gp << "set term wxt persist\n";
                // gp << "set title 'Linear regression'\n";
                // gp << "set xlabel '" << xLabelChart << "'\n";
                // gp << "set ylabel '" << yLabelChart << "'\n";
                // gp << "set grid\n";

                std::string xCmd = "set xlabel '" + xLabelChart + "'\n";
                std::string yCmd = "set ylabel '" + yLabelChart + "'\n";

                gp << "set term wxt enhanced font 'Arial,10'\n";

                gp << "set encoding utf8\n";
                gp << "unset margins\n"; // Lascia che gnuplot calcoli i margini corretti

                gp << "set title 'Linear regression'\n";

                // Imposta le label
                gp << xCmd;
                gp << yCmd;

                gp << "set grid\n";
                /* ============================================================
                  STILE DELLE CURVE (colori, linee, marker)
             ============================================================ */

                /*
                   Definizione stile linea 1
                   - lc rgb '#0072BD' : colore blu (palette MATLAB-like)
                   - lw 2.5           : spessore linea
                   - lt 1             : tipo di linea (continua)
                   - pt 7             : tipo di marker (cerchio)
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 1 lc rgb '#0072BD' lw 2.5 lt 1\n";

                /*
                   Definizione stile linea 2
                   - lc rgb '#D95319' : colore arancio
                   - lw 2.5           : spessore linea
                   - lt 1             : linea continua
                   - pt 5             : marker quadrato
                   - ps 1.0           : dimensione marker
                */
                gp << "set style line 2 lc rgb '#0bec16' lw 2.5 lt 1 pt 7 ps 1.0\n";

                /* ============================================================
                Se voglio solo le linee senza marker, rimuovo pt e ps dalle definizioni di stile
                 ============================================================ */

                /*
                   gp << "plot '-' with lines ls 1 title 'CP upper', "
                          "'-' with lines ls 2 title 'CP lower'\n";
                */

                /* ============================================================
                Se voglio linee con marker
                 ============================================================ */

                // gp << "plot '-' with lines ls 1 title 'Regression line', "
                //       "'-' with points ls 2 title 'Data points'\n";

                gp << "plot '-' with lines ls 1 title 'Regression line', "
                      "'-' with points ls 2 title 'Data points'\n";

                // Plotto i grafici come se fosse l'hold on di MATLAB
                gp.send1d_raw(dataForChart);
                gp.send1d_raw(xyPoint);

                // gp << "set output\n";  // Chiude il file PNG
                // gp << "replot\n";
                gp.flush();
                gp << "pause -1\n";
            }
            catch (const std::exception &ex)
            {
                std::cerr << "Errore: " << ex.what() << std::endl;
            }

            break;
        }

        default:

            std::cout << "Regression method not implemented." << std::endl;
            return;
        }
    }
};

#endif

// NOTA: Non usare mai "using namespace std;" nei file header!
// Questo inquinerebbe il namespace globale di tutti i file che includono questo header,
// causando potenziali conflitti di nomi e rendendo il codice meno chiaro.
// Usa sempre il prefisso esplicito std:: per indicare da dove provengono le classi/funzioni.

// size_t è un tipo unsigned integer definito nella libreria standard (<cstddef>).
// Viene utilizzato per rappresentare dimensioni, conteggi e indici di array/container.
// Non può essere negativo e la sua dimensione si adatta all'architettura del sistema.
// È il tipo di ritorno di .size() e sizeof(), quindi usarlo nei loop evita warning
// di confronto tra signed/unsigned e garantisce compatibilità con le dimensioni dei container.

// Esempio di come creare documentazione per classe, costruttore e metodi usando Doxygen:
/// In questo modo, con tre liniette metto, la documentazione stile Doxygen
/// per far apparire, quando passo col cursore sopra la classe, metodi o costruttore la descrizione ad essi relativa