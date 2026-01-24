#ifndef PLOT_H
#define PLOT_H

#include "gnuplot-iostream.h"

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
class Plot
{

private:
    std::vector<double> x;
    std::vector<double> y;
    std::string title;
    std::string color;
    std::string labelX;
    std::string labelY;
    std::string lineType;
    std::string thicknessLine;
    std::string markerType;
    std::string markerDimension;
    std::string typeOfPlot;
    std::string legend;

    void getPlot()
    {

        std::vector<std::pair<double, double>> dataForChartPlot;

        for (size_t i = 0; i < x.size(); i++)
        {

            dataForChartPlot.emplace_back(std::make_pair(x[i], y[i]));
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

            std::string xCmd = "set xlabel '" + labelX + "'\n";
            std::string yCmd = "set ylabel '" + labelY + "'\n";
            std::string titleCmd = "set title '" + title + "'\n";
            std::string lineCmd ="";
            std::string plotCmd = "plot '-' with " + typeOfPlot + " ls 1 title' " + legend + "'\n";

            if (markerType.empty())
            {

                lineCmd = "set style line 1 lc rgb '" + color + "' lw " + thicknessLine + " lt " + lineType + "\n";
            }

            else
            {

                lineCmd = "set style line 1 lc rgb '" + color + "' lw " + thicknessLine + " lt " + lineType + " pt " + markerType + " ps " + markerDimension + "\n";
            }

            gp << "set term wxt enhanced font 'Arial,10'\n";

            gp << "set encoding utf8\n";
            gp << "unset margins\n"; // Lascia che gnuplot calcoli i margini corretti

            gp << titleCmd;

            // Imposta le label
            gp << xCmd;
            gp << yCmd;

            gp << "set grid\n";

            // Configura la leggenda fuori dal grafico (northeastoutside)

            if (!legend.empty()) {
            gp << "set key outside right top\n";
            gp << "set key box\n"; // Opzionale: aggiunge un bordo alla leggenda
            }
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
            gp << lineCmd;

            /*
               Definizione stile linea 2
               - lc rgb '#D95319' : colore arancio
               - lw 2.5           : spessore linea
               - lt 1             : linea continua
               - pt 5             : marker quadrato
               - ps 1.0           : dimensione marker
            */
            // gp << "set style line 2 lc rgb '#0bec16' lw 2.5 lt 1 pt 7 ps 1.0\n";

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

            gp << plotCmd;

            // Plotto i grafici come se fosse l'hold on di MATLAB
            gp.send1d(dataForChartPlot);

            // gp << "set output\n";  // Chiude il file PNG
            // gp << "replot\n";
            gp.flush();
            gp << "pause -1\n";
        }

        catch (const std::exception &ex)
        {
            std::cerr << "Errore: " << ex.what() << std::endl;
        }
    }


public:

  

    Plot(std::vector<double> x, std::vector<double> y, std::string labelX, std::string labelY, std::string title,std::string legend,std::string typeOfPlot,std::string lineType, std::string thicknessLine,std::string markerType, std::string markerDimension,std::string color)
        : x(x), y(y), labelX(labelX), labelY(labelY), title(title),legend(legend),typeOfPlot(typeOfPlot),lineType(lineType), thicknessLine(thicknessLine), markerType(markerType), markerDimension(markerDimension), color(color)
    {

        getPlot();
    }

    

};

#endif 