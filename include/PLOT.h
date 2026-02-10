#ifndef PLOT_H
#define PLOT_H

#include "gnuplot-iostream.h"
#include <vector>
#include <string>

/// @brief  Constructor for the plot class.
/// @param x x data
/// @param y y data
/// @param labelX label for X axis
/// @param labelY label for Y axis
/// @param title title of the plot
/// @param legend legend for the plot
/// @param typeOfPlot type of plot (e.g., lines, points)
/// @param lineType Line type (lt) - Gnuplot line styles:
///        - "1" : solid line ─────────
///        - "2" : dashed line - - - - -
///        - "3" : dotted line ···········
///        - "4" : dash-dot line -·-·-·-
///        - "5" : dash-dot-dot line -··-··-
///        - "6" : short dashes -- -- --
///        - "7" : long dashes ——— ——— ———
///        - "8" : short dots ·· ·· ··
/// @param thicknessLine Line width (lw) - Thickness of the line:
///        - "0.5" : very thin
///        - "1.0" : thin (default)
///        - "1.5" : medium-thin
///        - "2.0" : medium
///        - "2.5" : medium-thick
///        - "3.0" : thick
///        - "4.0" : very thick
///        - "5.0" : extra thick
///        (any positive decimal value is accepted)
/// @param markerType Point type (pt) - Gnuplot marker styles:
///        - "1"  : + (plus)
///        - "2"  : × (cross)
///        - "3"  : * (asterisk)
///        - "4"  : □ (empty square)
///        - "5"  : ■ (filled square)
///        - "6"  : ○ (empty circle)
///        - "7"  : ● (filled circle)
///        - "8"  : △ (empty triangle up)
///        - "9"  : ▲ (filled triangle up)
///        - "10" : ▽ (empty triangle down)
///        - "11" : ▼ (filled triangle down)
///        - "12" : ◇ (empty diamond)
///        - "13" : ◆ (filled diamond)
///        - "14" : ⬠ (empty pentagon)
///        - "15" : ⬡ (filled pentagon)
///        - ""   : no marker (empty string)
/// @param markerDimension size of the marker (ps - point size)
/// @param color Line/marker color (lc rgb) - Accepted formats:
///        Named colors:
///        - "'red'"      : red
///        - "'blue'"     : blue
///        - "'green'"    : green
///        - "'black'"    : black
///        - "'white'"    : white
///        - "'cyan'"     : cyan
///        - "'magenta'"  : magenta
///        - "'yellow'"   : yellow
///        - "'orange'"   : orange
///        - "'purple'"   : purple
///        - "'brown'"    : brown
///        - "'gray'"     : gray
///        - "'dark-red'" : dark red
///        - "'dark-blue'": dark blue
///        Hexadecimal RGB format:
///        - "'#0072BD'" : MATLAB blue
///        - "'#D95319'" : MATLAB orange
///        - "'#EDB120'" : MATLAB yellow
///        - "'#7E2F8E'" : MATLAB purple
///        - "'#77AC30'" : MATLAB green
///        - "'#4DBEEE'" : MATLAB cyan
///        - "'#A2142F'" : MATLAB red
///        - "'#FF0000'" : pure red
///        - "'#00FF00'" : pure green
///        - "'#0000FF'" : pure blue
///        (Note: quotes inside the string are required for Gnuplot)
template <typename Container = std::vector<double>>
class Plot
{
private:
    struct PlotStyle {
        std::string legend;
        std::string color;
        std::string lineType;
        std::string thicknessLine;
        std::string markerType;
        std::string markerDimension;
        std::string typeOfPlot;
    };
    
    // Vettore di dataset (x,y), contiene una coppia di vettori per ogni dataset
    std::vector<std::pair<Container, Container>> datasets;
    std::vector<PlotStyle> styles;
    
    std::string title;
    std::string labelX;
    std::string labelY;
    bool autoShow;  // Se true, mostra subito nel costruttore

    void getPlot()
    {
        if (datasets.empty()) {
            std::cerr << "Warning: No data to plot!" << std::endl;
            return;
        }

        try
        {
            Gnuplot gp;
            
            gp << "set term wxt enhanced font 'Arial,10'\n";
            gp << "set encoding utf8\n";
            gp << "unset margins\n";

            std::string titleCmd = "set title '" + title + "'\n";
            std::string labelXCmd = "set xlabel '" + labelX + "'\n";
            std::string labelYCmd = "set ylabel '" + labelY + "'\n";
            
            gp << titleCmd;
            gp << labelXCmd;
            gp << labelYCmd;
            gp << "set grid\n";
            
            if (!styles.empty() && !styles[0].legend.empty()) {
                gp << "set key outside right top\n";
                gp << "set key box\n";
            }
            
            // Definisci gli stili per ogni dataset
            for (size_t i = 0; i < styles.size(); ++i) {
                const auto& style = styles[i];
                std::string lineCmd;
                
                if (style.markerType.empty()) {
                    lineCmd = "set style line " + std::to_string(i + 1) + 
                             " lc rgb '" + style.color + "' lw " + style.thicknessLine + 
                             " lt " + style.lineType + "\n";
                } else {
                    lineCmd = "set style line " + std::to_string(i + 1) + 
                             " lc rgb '" + style.color + "' lw " + style.thicknessLine + 
                             " lt " + style.lineType + " pt " + style.markerType + 
                             " ps " + style.markerDimension + "\n";
                }
                gp << lineCmd;
            }
            
            // Costruisci il comando plot
            std::string plotCmd = "plot ";
            for (size_t i = 0; i < datasets.size(); ++i) {
                if (i > 0) plotCmd += ", ";
                plotCmd += "'-' with " + styles[i].typeOfPlot + 
                          " ls " + std::to_string(i + 1);
                if (!styles[i].legend.empty()) {
                    plotCmd += " title '" + styles[i].legend + "'";
                } else {
                    plotCmd += " notitle";
                }
            }
            plotCmd += "\n";
            
            gp << plotCmd;
            
            // Invia tutti i dataset
            for (const auto& dataset : datasets) {
                std::vector<std::pair<double, double>> data;
                // dataset.first è il vettore x
                // dataset.second è il vettore y
                // Perchè dataset è un alias di datasets che è un vettore di coppie di container
                auto it_x = dataset.first.begin(); // Iteratore per x contiene il riferimento al primo elemento di x
                auto it_y = dataset.second.begin(); // Iteratore per y contiene il riferimento al primo elemento di y
                
                while (it_x != dataset.first.end() && it_y != dataset.second.end()) {
                    data.emplace_back(*it_x, *it_y); // Aggiunge la coppia (x,y) al vettore data
                    ++it_x;
                    ++it_y;
                }
                
                gp.send1d(data);
            }
            
            gp.flush();
            gp << "pause -1\n";
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Errore: " << ex.what() << std::endl;
        }
    }

public:
    // ════════════════════════════════════════════════════════════════
    // COSTRUTTORE 1: Per SINGOLO PLOT 
    // ════════════════════════════════════════════════════════════════
    Plot(Container x, Container y, 
         std::string labelX, std::string labelY, std::string title,
         std::string legend, std::string typeOfPlot, 
         std::string lineType, std::string thicknessLine, 
         std::string markerType, std::string markerDimension, std::string color)
        : labelX(labelX), labelY(labelY), title(title), autoShow(true)
    {
        datasets.emplace_back(std::move(x), std::move(y));
        
        PlotStyle style;
        style.legend = legend;
        style.typeOfPlot = typeOfPlot;
        style.lineType = lineType;
        style.thicknessLine = thicknessLine;
        style.markerType = markerType;
        style.markerDimension = markerDimension;
        style.color = color;
        
        styles.push_back(style);
        
        getPlot();  // Mostra subito
    }
    
    // ════════════════════════════════════════════════════════════════
    // COSTRUTTORE 2: Per MULTI-PLOT (hold on)
    // ════════════════════════════════════════════════════════════════
    Plot(std::string labelX, std::string labelY, std::string title)
        : labelX(labelX), labelY(labelY), title(title), autoShow(false)
    {
    }
    
    // Metodo per aggiungere dati (hold on)
    void addData(Container x, Container y, 
                 std::string legend = "",
                 std::string typeOfPlot = "linespoints",
                 std::string lineType = "1", 
                 std::string thicknessLine = "2.0",
                 std::string markerType = "7", 
                 std::string markerDimension = "1.0",
                 std::string color = "#0072BD")
    {
        datasets.emplace_back(std::move(x), std::move(y));
        
        PlotStyle style;
        style.legend = legend;
        style.typeOfPlot = typeOfPlot;
        style.lineType = lineType;
        style.thicknessLine = thicknessLine;
        style.markerType = markerType;
        style.markerDimension = markerDimension;
        style.color = color;
        
        styles.push_back(style);
    }
    
    // Metodo per mostrare il plot
    void show()
    {
        getPlot();
    }
};

// Guide di deduzione
template<typename Container>
Plot(Container, Container, std::string, std::string, std::string, 
     std::string, std::string, std::string, std::string, std::string, 
     std::string, std::string) -> Plot<Container>;

template<typename Container>
Plot(std::string, std::string, std::string) -> Plot<Container>;

#endif


// move(x) e move(y) usati nel costruttore per ottimizzare la copia dei dati, sfruttando il concetto di move semantics in C++11 e successivi.
// Questo è particolarmente utile quando i container passati (come std::vector o std::list) sono grandi, poiché evita una copia inutile dei dati, migliorando l'efficienza delle prestazioni.
// In pratica, std::move indica al compilatore che gli oggetti x e y possono essere "spostati" piuttosto che copiati, trasferendo la proprietà delle risorse interne (come la memoria allocata) al nuovo oggetto senza duplicarle.