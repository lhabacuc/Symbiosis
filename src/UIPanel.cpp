#include "UIPanel.hpp"
#include "raylib.h"
#include "raygui.h"
#include <cstdio>

UIPanel::UIPanel(Visualizer &viz, int width)
    : _viz(viz), _width(width), _scrollX(0.0f), _scrollY(0.0f)
{
}

void UIPanel::aplicarEstilo()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x1E1E1EFF);
    GuiSetStyle(DEFAULT, LINE_COLOR, 0x3C3C3CFF);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x2D4B6BFF);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x3D6690FF);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x1F3550FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x4C7AA8FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x7FB2E0FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0x2D4B6BFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 2);

    GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, 0x2A2A2AFF);
    GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, 0x4C7AA8FF);
    GuiSetStyle(SLIDER, BASE_COLOR_PRESSED, 0x7FB2E0FF);
    GuiSetStyle(SLIDER, BASE_COLOR_FOCUSED, 0x5C9AD0FF);
    GuiSetStyle(SLIDER, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(SLIDER, SLIDER_WIDTH, 16);

    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
}

void UIPanel::drawSlider(float x, float y, float w, const char *label, float *value, float minV, float maxV, const char *suffix)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d%s", static_cast<int>(*value), suffix);

    _viz.drawText(static_cast<int>(x), static_cast<int>(y), 0xD8D8D8, label);

    int valorW = MeasureText(buf, 16);
    _viz.drawText(static_cast<int>(x + w) - valorW, static_cast<int>(y), 0x7FB2E0, buf);

    GuiSlider({x, y + 22, w, 20}, nullptr, nullptr, value, minV, maxV);
}

void UIPanel::draw(Ecosystem &ecosystem, bool &pausado)
{
    const float contentH = 1520.0f;
    int playWidth = ecosystem.getPlayWidth();
    EcosystemConfig &cfg = ecosystem.config();

    Rectangle panelBounds = {static_cast<float>(playWidth), 0.0f, static_cast<float>(_width), static_cast<float>(_viz.getHeight())};
    Rectangle content = {0.0f, 0.0f, static_cast<float>(_width) - 16.0f, contentH};
    Vector2 scroll = {_scrollX, _scrollY};
    Rectangle view;

    GuiScrollPanel(panelBounds, nullptr, content, &scroll, &view);
    _scrollX = scroll.x;
    _scrollY = scroll.y;

    BeginScissorMode(static_cast<int>(view.x), static_cast<int>(view.y), static_cast<int>(view.width), static_cast<int>(view.height));

    float px = panelBounds.x + scroll.x + 14.0f;
    float panelW = static_cast<float>(_width) - 44.0f;
    char buf[48];

    float y = scroll.y + 10.0f;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0xFFFFFF, "SYMBIOSIS");
    y += 28.0f;
    GuiLine({px, y, panelW, 10}, nullptr);
    y += 16.0f;
    if (GuiButton({px, y, panelW, 36}, pausado ? "#131# Continuar" : "#132# Pausar"))
        pausado = !pausado;
    y += 48;

    if (GuiButton({px, y, panelW / 2 - 6, 36}, "#76# Reiniciar"))
    {
        ecosystem.reiniciar();
        pausado = false;
    }
    if (GuiButton({px + panelW / 2 + 6, y, panelW / 2 - 6, 36}, "#143# Limpar"))
    {
        ecosystem.limpar();
        pausado = true;
    }
    y += 50;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 22;

    drawSlider(px, y, panelW, "Bacterias", &cfg.configBacterias, 0.0f, 3000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Comida", &cfg.configComida, 0.0f, 4000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Veneno", &cfg.configVeneno, 0.0f, 1000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Velocidade", &cfg.velocidadeMs, 1.0f, 400.0f, "ms");
    y += 60;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 8;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x999999, "Caracteristicas das bacterias");
    y += 26;

    drawSlider(px, y, panelW, "Energia inicial", &cfg.energiaInicial, 10.0f, 4000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Vida maxima (idade)", &cfg.vidaMaxima, 20.0f, 9000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Valor da comida", &cfg.valorComida, 1.0f, 400.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Dano do veneno", &cfg.danoVeneno, 1.0f, 400.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Limite reproducao", &cfg.limiteReproducao, 50.0f, 3000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Temperatura ambiente", &cfg.temperaturaAmbiente, -10.0f, 50.0f, "C");
    y += 60;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 8;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x999999, "Gregarismo");
    y += 26;

    GuiCheckBox({px, y, 20, 20}, "Precisam de ficar juntas", &cfg.gregarismo);
    y += 40;

    drawSlider(px, y, panelW, "Raio social", &cfg.raioSocial, 20.0f, 400.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Custo de solidao", &cfg.custoSolidao, 0.0f, 30.0f, "");
    y += 60;

    if (GuiButton({px, y, panelW, 32}, "#68# Centrar vista"))
        _viz.resetCamera(static_cast<float>(playWidth), static_cast<float>(_viz.getHeight()));
    y += 42;

    snprintf(buf, sizeof(buf), "Zoom: %.1fx", static_cast<double>(_viz.getCameraZoom()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x888888, buf);
    y += 20;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x888888, "Roda=zoom  Botao dir.=arrastar");
    y += 26;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 22;

    GuiLabel({px, y, panelW, 20}, pausado ? "Estado: PAUSADO" : "Estado: A CORRER");
    y += 30;

    snprintf(buf, sizeof(buf), "Vivas: %d", static_cast<int>(ecosystem.getBacterias().size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x00FF00, buf);
    y += 25;

    snprintf(buf, sizeof(buf), "Comida no mapa: %d", static_cast<int>(ecosystem.getComida().size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0xFFFF00, buf);
    y += 25;

    snprintf(buf, sizeof(buf), "Veneno no mapa: %d", static_cast<int>(ecosystem.getVeneno().size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0xAA00FF, buf);
    y += 30;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 8;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x999999, "Populacao ao longo do tempo");
    y += 22;

    _viz.drawGrafico(px, y, panelW, 100.0f, ecosystem.getHistoricoPopulacao(), 0x00FF00);
    y += 116;

    EndScissorMode();
}
