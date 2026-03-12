#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <map>
#include <array>
#include <algorithm>
#include <filesystem>

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCellArray.h>
#include <vtkLight.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkPNGReader.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>

// ─────────────────────────────────────────────────────────────────────────────
struct DegenSurf
{
    std::string name;
    std::vector<double> x, y, z;
    int nu = 0, nw = 0;
    double r = 0.0, g = 0.4470, b = 0.7410;  ///< Colore per componente (RGB normalizzato [0,1])
};

// ─────────────────────────────────────────────────────────────────────────────
/// @brief Vista della camera disponibile per il salvataggio PNG
// ─────────────────────────────────────────────────────────────────────────────
enum class CameraView
{
    TOP,        ///< Vista dall'alto   (Z+ guarda verso il basso)
    SIDE,       ///< Vista laterale    (muso sx, coda dx — asse X, Y+ verso osservatore)
    FRONT,      ///< Vista frontale    (muso verso osservatore — asse Y negativo)
    PERSPECTIVE ///< Vista prospettica (azimuth=-45, elevation=45 — default MATLAB)
};

// ─────────────────────────────────────────────────────────────────────────────
class AircraftPlotter
{
private:
    std::string            aircraftName;
    std::string logoFilePath = std::filesystem::current_path().string() + "/logo/AeroPlusPLus_logo.png";
    std::vector<DegenSurf> components;
    double colorR = 0.0, colorG = 0.4470, colorB = 0.7410;
    double opacity = 1.0;
    int    width   = 1920;
    int    height  = 1080;
    double bgR = 0.15, bgG = 0.15, bgB = 0.15;
    int    logoMargin    = 20;        ///< Margine dal bordo in pixel

    // ── Geometria ─────────────────────────────────────────────────────────────
    vtkSmartPointer<vtkPolyData> buildPolyData(const DegenSurf& surf) const
    {
        if (surf.nu < 2 || surf.nw < 2)
            throw std::invalid_argument("'" + surf.name + "': nu/nw >= 2");
        if ((int)surf.x.size() != surf.nu * surf.nw)
            throw std::invalid_argument("'" + surf.name + "': dimensione non coerente");

        auto points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(surf.nu * surf.nw);
        for (int i = 0; i < surf.nu * surf.nw; ++i)
            points->SetPoint(i, surf.x[i], surf.y[i], surf.z[i]);

        auto cells = vtkSmartPointer<vtkCellArray>::New();
        for (int i = 0; i < surf.nu - 1; ++i)
            for (int j = 0; j < surf.nw - 1; ++j)
            {
                cells->InsertNextCell(4);
                cells->InsertCellPoint( i      * surf.nw + j    );
                cells->InsertCellPoint((i + 1) * surf.nw + j    );
                cells->InsertCellPoint((i + 1) * surf.nw + j + 1);
                cells->InsertCellPoint( i      * surf.nw + j + 1);
            }

        auto pd = vtkSmartPointer<vtkPolyData>::New();
        pd->SetPoints(points);
        pd->SetPolys(cells);
        return pd;
    }

    void addActors(vtkSmartPointer<vtkRenderer> renderer) const
    {
        for (const auto& surf : components)
        {
            auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(buildPolyData(surf));
            auto actor = vtkSmartPointer<vtkActor>::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetColor(surf.r, surf.g, surf.b);
            actor->GetProperty()->SetEdgeVisibility(false);
            actor->GetProperty()->SetAmbient(0.2);
            actor->GetProperty()->SetDiffuse(0.8);
            actor->GetProperty()->SetSpecular(0.3);
            actor->GetProperty()->SetSpecularPower(20);
            actor->GetProperty()->SetOpacity(opacity);
            renderer->AddActor(actor);
        }
    }

    void addLights(vtkSmartPointer<vtkRenderer> renderer) const
    {
        auto light = vtkSmartPointer<vtkLight>::New();
        light->SetLightTypeToHeadlight();
        light->SetIntensity(1.0);
        renderer->AddLight(light);

        auto fill = vtkSmartPointer<vtkLight>::New();
        fill->SetLightTypeToHeadlight();
        fill->SetIntensity(0.4);
        fill->SetPosition(-1, 1, -1);
        renderer->AddLight(fill);
    }

    void setupCamera(vtkSmartPointer<vtkRenderer> renderer, CameraView view) const
    {
        renderer->SetBackground(bgR, bgG, bgB);
        renderer->ResetCamera();
        auto* cam = renderer->GetActiveCamera();

        switch (view)
        {
            case CameraView::TOP:
                cam->SetPosition(0, 0, 1);
                cam->SetFocalPoint(0, 0, 0);
                cam->SetViewUp(1, 0, 0);
                renderer->ResetCamera();
                break;
            case CameraView::SIDE:
                cam->SetPosition(0, 1, 0);
                cam->SetFocalPoint(0, 0, 0);
                cam->SetViewUp(0, 0, 1);
                renderer->ResetCamera();
                cam->Azimuth(180);
                renderer->ResetCameraClippingRange();
                break;
            case CameraView::FRONT:
                cam->SetPosition(-1, 0, 0);
                cam->SetFocalPoint(0, 0, 0);
                cam->SetViewUp(0, 0, 1);
                renderer->ResetCamera();
                break;
            case CameraView::PERSPECTIVE:
            default:
                cam->SetPosition(1, -1, 1);
                cam->SetViewUp(0, 0, 1);
                renderer->ResetCamera();
                cam->Azimuth(-45);
                cam->Elevation(45);
                renderer->ResetCameraClippingRange();
                break;
        }

        addLights(renderer);
    }

    // ── Logo overlay — renderer su layer 1, viewport in basso a destra ────────
    /// @brief Aggiunge il logo PNG come overlay in basso a destra nella render window.
    ///        Usa un secondo renderer su Layer 1 con viewport calcolato dalla dimensione logo.
    ///        Il logo viene scalato a logoMaxWidth mantenendo l'aspect ratio.
    void addLogoOverlay(vtkSmartPointer<vtkRenderWindow> renderWindow) const
    {
        if (logoFilePath.empty())
            return;

        auto reader = vtkSmartPointer<vtkPNGReader>::New();
        reader->SetFileName(logoFilePath.c_str());
        reader->Update();

        int *dims = reader->GetOutput()->GetDimensions();
        int logoW = dims[0];
        int logoH = dims[1];

        if (logoW <= 0 || logoH <= 0)
        {
            std::cerr << "Logo non valido o non trovato: " << logoFilePath << "\n";
            return;
        }

        // ── Scala automatica: 12% della larghezza dell'immagine ──────────────
        int logoMaxWidth = (int)(width * 0.25);
        double scale = std::min(1.0, (double)logoMaxWidth / logoW);
        int scaledW = (int)(logoW * scale);
        int scaledH = (int)(logoH * scale);
        // ─────────────────────────────────────────────────────────────────────

        // ── Viewport normalizzato [0,1] — basso a destra con margine ──────────
        // x0,y0 = angolo basso-sinistra del viewport; x1,y1 = angolo alto-destra
        double x0 = 1.0 - (double)(scaledW + logoMargin) / width;
        double y0 = 0.0;
        double x1 = 1.0 - (double)logoMargin             / width;
        double y1 =        (double)(scaledH + logoMargin) / height;

        // ── Renderer overlay ──────────────────────────────────────────────────
        auto logoRenderer = vtkSmartPointer<vtkRenderer>::New();
        logoRenderer->SetViewport(x0, y0, x1, y1);
        logoRenderer->SetLayer(1);              // sopra il renderer principale
        logoRenderer->InteractiveOff();
        logoRenderer->SetBackground(bgR, bgG, bgB);  // stesso sfondo → si fonde

        // ── ImageActor ────────────────────────────────────────────────────────
        auto actor = vtkSmartPointer<vtkImageActor>::New();
        actor->GetMapper()->SetInputConnection(reader->GetOutputPort());
        logoRenderer->AddActor(actor);
        logoRenderer->ResetCamera();

        renderWindow->SetNumberOfLayers(2);
        renderWindow->AddRenderer(logoRenderer);
    }

    // ── Core save ─────────────────────────────────────────────────────────────
    void saveView(const std::string& filename, CameraView view) const
    {
        if (components.empty()) { std::cerr << "Nessun componente!\n"; return; }

        auto renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->SetLayer(0);
        addActors(renderer);
        setupCamera(renderer, view);

        auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->SetNumberOfLayers(1);
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize(width, height);
        renderWindow->OffScreenRenderingOn();

        // Aggiunge il logo se impostato
        addLogoOverlay(renderWindow);

        renderWindow->Render();

        auto w2i = vtkSmartPointer<vtkWindowToImageFilter>::New();
        w2i->SetInput(renderWindow);
        w2i->ReadFrontBufferOff();
        w2i->Update();

        auto writer = vtkSmartPointer<vtkPNGWriter>::New();
        writer->SetFileName(filename.c_str());
        writer->SetInputConnection(w2i->GetOutputPort());
        writer->Write();

        std::cout << "Salvato: " << filename << "\n";
        renderWindow->OffScreenRenderingOff();
    }

public:
    explicit AircraftPlotter(const std::string& name) : aircraftName(name) {}

    // ── Aggiunta componenti ───────────────────────────────────────────────────

    /// @brief Aggiunge un componente con colore di default
    void addComponent(const DegenSurf& surf)
    {
        components.push_back(surf);
        std::cout << "Componente aggiunto: " << surf.name << "\n";
    }

    /// @brief Aggiunge un componente con colore da mappa prefisso→RGB [0,255]
    void addComponentWithColorMap(
        const DegenSurf& surf,
        const std::map<std::string, std::array<double, 3>>& colorMap)
    {
        DegenSurf s = surf;
        s.r = 128.0 / 255.0;
        s.g = 128.0 / 255.0;
        s.b = 128.0 / 255.0;

        for (const auto& [key, color] : colorMap)
        {
            if (s.name.rfind(key, 0) == 0)
            {
                s.r = color[0] / 255.0;
                s.g = color[1] / 255.0;
                s.b = color[2] / 255.0;
                break;
            }
        }

        components.push_back(s);
        std::cout << "Componente aggiunto: " << s.name
                  << "  RGB: (" << s.r*255 << ", " << s.g*255 << ", " << s.b*255 << ")\n";
    }

    // ── Salvataggio ───────────────────────────────────────────────────────────

    /// @brief Salva le 4 viste standard (Top, Side, Front, Perspective)
    void saveAllViews(const std::string& outputDir = ".") const
    {
        saveView(outputDir + "/" + aircraftName + "_TopView.png",         CameraView::TOP);
        saveView(outputDir + "/" + aircraftName + "_SideView.png",        CameraView::SIDE);
        saveView(outputDir + "/" + aircraftName + "_FrontView.png",       CameraView::FRONT);
        saveView(outputDir + "/" + aircraftName + "_PerspectiveView.png", CameraView::PERSPECTIVE);
    }

    /// @brief Salva un singolo PNG con la vista scelta
    void savePNG(const std::string& filename,
                 CameraView view = CameraView::PERSPECTIVE) const
    {
        saveView(filename, view);
    }

    /// @brief Apre la finestra 3D interattiva — BLOCCANTE
    void show(CameraView view = CameraView::PERSPECTIVE) const
    {
        if (components.empty()) { std::cerr << "Nessun componente!\n"; return; }

        auto renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->SetLayer(0);
        addActors(renderer);
        setupCamera(renderer, view);

        auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->SetNumberOfLayers(1);
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize(width, height);
        renderWindow->SetWindowName(("Aircraft 3D View — " + aircraftName).c_str());

        addLogoOverlay(renderWindow);

        auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        interactor->SetRenderWindow(renderWindow);
        auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        interactor->SetInteractorStyle(style);

        interactor->Initialize();
        renderWindow->Render();
        interactor->Start();
    }

    /// @brief Salva le 4 viste poi apre la finestra interattiva
    void plotAndSave(const std::string& outputDir = ".") const
    {
        saveAllViews(outputDir);
        show(CameraView::PERSPECTIVE);
    }

    // ── Setters ───────────────────────────────────────────────────────────────

    void setColor(double r, double g, double b) { colorR = r; colorG = g; colorB = b; }
    void setOpacity(double o)                   { opacity = o; }
    void setResolution(int w, int h)            { width = w; height = h; }

    /// @brief Imposta il colore di sfondo — RGB in [0, 255]
    void setBackground(double r, double g, double b)
    {
        bgR = r / 255.0;
        bgG = g / 255.0;
        bgB = b / 255.0;
    }

};