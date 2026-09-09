#include "Simulation.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>

// mapeia uma temperatura para uma cor: azulado no frio, avermelhado no quente,
// cinzento neutro a volta dos ~20 graus (usado tanto no fundo como nas zonas)
static unsigned int corTemperatura(float temperatura)
{
    float t = (temperatura - 20.0f) / 20.0f;
    if (t < -1.0f)
        t = -1.0f;
    if (t > 1.0f)
        t = 1.0f;

    unsigned char r, g, b;
    if (t < 0.0f)
    {
        float k = -t;
        r = static_cast<unsigned char>(80 + (1.0f - k) * 40);
        g = static_cast<unsigned char>(80 + (1.0f - k) * 40);
        b = static_cast<unsigned char>(80 + k * 175);
    }
    else
    {
        float k = t;
        r = static_cast<unsigned char>(80 + k * 175);
        g = static_cast<unsigned char>(80 + (1.0f - k) * 40);
        b = static_cast<unsigned char>(80 + (1.0f - k) * 40);
    }
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
}

static unsigned int escurecer(unsigned int colorRGB, float fator)
{
    unsigned char r = static_cast<unsigned char>(((colorRGB >> 16) & 0xFF) * fator);
    unsigned char g = static_cast<unsigned char>(((colorRGB >> 8) & 0xFF) * fator);
    unsigned char b = static_cast<unsigned char>((colorRGB & 0xFF) * fator);
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
}

// canal de cor auxiliar: mistura entre cinzento neutro e uma cor "extrema"
// consoante uma intensidade 0..1 - usado pelos filtros termico/genes/idade
static unsigned int misturarComExtremo(unsigned char rExtremo, unsigned char gExtremo, unsigned char bExtremo, float intensidade)
{
    if (intensidade < 0.0f)
        intensidade = 0.0f;
    if (intensidade > 1.0f)
        intensidade = 1.0f;

    unsigned char r = static_cast<unsigned char>(80 + (rExtremo - 80) * intensidade);
    unsigned char g = static_cast<unsigned char>(80 + (gExtremo - 80) * intensidade);
    unsigned char b = static_cast<unsigned char>(80 + (bExtremo - 80) * intensidade);
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
}

// camera termica: verde quando a bacteria esta confortavel, vermelho quando
// sofre calor a mais, azul quando sofre frio a mais (face ao seu gene otimo)
static unsigned int corTermicaBacteria(const Bacteria &b, Ecosystem &eco)
{
    float diferenca = eco.temperaturaEm(b.getX(), b.getY()) - b.getGeneTemperaturaOtima();
    float intensidade = std::fabs(diferenca) / 15.0f;

    if (diferenca > 0.0f)
        return misturarComExtremo(255, 40, 40, intensidade);
    if (diferenca < 0.0f)
        return misturarComExtremo(40, 80, 255, intensidade);
    return 0x30C030;
}

// mapa de genes: vermelho = mais rapida, azul = maior raio de visao
static unsigned int corGenesBacteria(const Bacteria &b)
{
    float velNorm = std::min(1.0f, b.getGeneVelocidade() / 2.0f);
    float visaoNorm = std::min(1.0f, b.getGeneRaioVisao() / 3.0f);

    unsigned char r = static_cast<unsigned char>(40 + velNorm * 215);
    unsigned char g = 40;
    unsigned char bC = static_cast<unsigned char>(40 + visaoNorm * 215);
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(bC);
}

// mapa de idade: verde=nova, vermelho=perto do fim de vida
static unsigned int corIdadeBacteria(const Bacteria &b)
{
    float idadeMaxima = static_cast<float>(b.getIdadeMaxima());
    float ratio = idadeMaxima > 0.0f ? b.getIdade() / idadeMaxima : 0.0f;
    if (ratio > 1.0f)
        ratio = 1.0f;

    unsigned char r = static_cast<unsigned char>(40 + ratio * 215);
    unsigned char g = static_cast<unsigned char>(40 + (1.0f - ratio) * 215);
    unsigned char b2 = 40;
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b2);
}

Simulation::Simulation(Visualizer &viz, const Mapa &mapa)
    : _viz(viz), _ecosystem(mapa), _ui(viz, SIDEBAR_WIDTH),
      _pausado(false), _acumulador(0.0f), _modoVisualizacao(ModoVisualizacao::NORMAL)
{
    _ui.aplicarEstilo();
}

void Simulation::draw()
{
    int playWidth = _ecosystem.getPlayWidth();

    _viz.updateCamera(static_cast<float>(playWidth), static_cast<float>(_viz.getHeight()));
    _viz.beginScissorCamera(0, 0, playWidth, _viz.getHeight());

    const std::vector<ZonaTemperatura> &zonas = _ecosystem.getZonasTemperatura();
    for (size_t i = 0; i < zonas.size(); i++)
    {
        const ZonaTemperatura &zona = zonas[i];
        _viz.drawZonaTemperatura(static_cast<float>(zona.x), static_cast<float>(zona.y),
            static_cast<float>(zona.raio), corTemperatura(zona.valor), 0.35f);
    }

    const std::vector<Bacteria> &bacterias = _ecosystem.getBacterias();

    if (_modoVisualizacao == ModoVisualizacao::DENSIDADE)
        desenharMapaDensidade(bacterias);

    const std::vector<Item> &comida = _ecosystem.getComida();
    for (size_t i = 0; i < comida.size(); i++)
        _viz.drawRect(comida[i].x - 2, comida[i].y - 2, 4, 4, 0xFFFF00);

    const std::vector<Item> &veneno = _ecosystem.getVeneno();
    for (size_t i = 0; i < veneno.size(); i++)
        _viz.drawRect(veneno[i].x - 2, veneno[i].y - 2, 4, 4, 0xAA00FF);

    for (size_t i = 0; i < bacterias.size(); i++)
    {
        const Bacteria &b = bacterias[i];
        unsigned int color;

        switch (_modoVisualizacao)
        {
            case ModoVisualizacao::TERMICA:
                color = corTermicaBacteria(b, _ecosystem);
                break;
            case ModoVisualizacao::GENES:
                color = corGenesBacteria(b);
                break;
            case ModoVisualizacao::IDADE:
                color = corIdadeBacteria(b);
                break;
            default:
            {
                int intensidade = (int)(b.getEnergy() * 2.0f);
                if (intensidade > 255)
                    intensidade = 255;
                if (intensidade < 50)
                    intensidade = 50;
                color = (unsigned int)(intensidade) << 8;
                break;
            }
        }

        if (_ecosystem.desenhoDetalhado())
        {
            float raio = 2.2f + std::min(3.0f, b.getEnergy() / 60.0f);
            _viz.drawBacteria(static_cast<float>(b.getX()), static_cast<float>(b.getY()), raio, color);
        }
        else
        {
            _viz.drawRect(b.getX() - 1, b.getY() - 1, 3, 3, color);
        }
    }

    _viz.endScissorCamera();

    _viz.drawLine(playWidth, 0, playWidth, _viz.getHeight(), 0x666666);
}

void Simulation::desenharMapaDensidade(const std::vector<Bacteria> &bacterias)
{
    const int celula = 30;
    int playWidth = _ecosystem.getPlayWidth();
    int playHeight = _ecosystem.getPlayHeight();
    int cols = playWidth / celula + 1;
    int rows = playHeight / celula + 1;

    std::vector<int> contagem(static_cast<size_t>(cols) * rows, 0);

    for (size_t i = 0; i < bacterias.size(); i++)
    {
        int cx = bacterias[i].getX() / celula;
        int cy = bacterias[i].getY() / celula;
        if (cx >= 0 && cx < cols && cy >= 0 && cy < rows)
            contagem[static_cast<size_t>(cy) * cols + cx]++;
    }

    int maxContagem = 1;
    for (size_t i = 0; i < contagem.size(); i++)
        if (contagem[i] > maxContagem)
            maxContagem = contagem[i];

    for (int cy = 0; cy < rows; cy++)
    {
        for (int cx = 0; cx < cols; cx++)
        {
            int n = contagem[static_cast<size_t>(cy) * cols + cx];
            if (n == 0)
                continue;
            float intensidade = static_cast<float>(n) / static_cast<float>(maxContagem);
            _viz.drawRectAlpha(cx * celula, cy * celula, celula, celula, 0xFF3020, 0.15f + intensidade * 0.55f);
        }
    }
}

void Simulation::tick()
{
    if (IsKeyPressed(KEY_TAB))
    {
        int proximo = (static_cast<int>(_modoVisualizacao) + 1) % static_cast<int>(ModoVisualizacao::TOTAL_MODOS);
        _modoVisualizacao = static_cast<ModoVisualizacao>(proximo);
    }

    _ecosystem.sincronizarPopulacao();

    float dt = _ecosystem.config().velocidadeMs / 1000.0f;
    _acumulador += GetFrameTime();

    // nunca deixar mais do que N passos por frame por tras: evita a "espiral da morte"
    // em que um update() lento faz o acumulador crescer, obrigando a mais updates() no
    // frame seguinte, o que o torna ainda mais lento, e por ai fora ate travar a app.
    const int MAX_PASSOS_POR_FRAME = 3;
    int passos = 0;
    while (_acumulador >= dt && passos < MAX_PASSOS_POR_FRAME)
    {
        if (!_pausado)
            _ecosystem.update();
        _acumulador -= dt;
        passos++;
    }
    if (_acumulador > dt)
        _acumulador = dt;

    unsigned int corFundo = escurecer(corTemperatura(_ecosystem.config().temperaturaAmbiente), 0.3f);
    _viz.beginFrame(corFundo);
    draw();
    _ui.draw(_ecosystem, _pausado, _modoVisualizacao);
    _viz.endFrame();
}
