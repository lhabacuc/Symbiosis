#include "Mapa.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>

Mapa::Mapa(const std::string &path) : _width(0), _height(0), _bacteriaVida(-1.0f), _bacteriaVidaMaxima(-1.0f)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("nao foi possivel abrir o mapa: " + path);

    std::string pendingTipo;
    int pendingCount = 0;
    int pendingRaio = 0;

    std::string linha;
    while (std::getline(file, linha))
    {
        if (linha.empty())
            continue;

        std::istringstream iss(linha);
        std::string tipo;
        iss >> tipo;

        if (tipo == "WIDTH")
            iss >> _width;
        else if (tipo == "HEIGHT")
            iss >> _height;
        else if (tipo == "BACTERIA")
        {
            pendingTipo = "BACTERIA";
            pendingRaio = 0;
            iss >> pendingCount;

            float vida, vidaMaxima;
            if (iss >> vida)
            {
                _bacteriaVida = vida;
                if (iss >> vidaMaxima)
                    _bacteriaVidaMaxima = vidaMaxima;
            }
        }
        else if (tipo == "COMIDA")
        {
            pendingTipo = "COMIDA";
            iss >> pendingCount >> pendingRaio;
        }
        else if (tipo == "VENENO")
        {
            pendingTipo = "VENENO";
            pendingRaio = 0;
            iss >> pendingCount;
        }
        else if (tipo == "POSSISION" || tipo == "POSITION")
        {
            std::string modo;
            iss >> modo;

            std::vector<std::pair<int, int> > *alvo = nullptr;
            if (pendingTipo == "BACTERIA")
                alvo = &_bacterias;
            else if (pendingTipo == "COMIDA")
                alvo = &_comida;
            else if (pendingTipo == "VENENO")
                alvo = &_veneno;

            if (alvo)
                gerarBloco(*alvo, pendingCount, modo, pendingRaio);
            pendingTipo.clear();
        }
    }

    if (!pendingTipo.empty())
    {
        std::vector<std::pair<int, int> > *alvo = nullptr;
        if (pendingTipo == "BACTERIA")
            alvo = &_bacterias;
        else if (pendingTipo == "COMIDA")
            alvo = &_comida;
        else if (pendingTipo == "VENENO")
            alvo = &_veneno;
        if (alvo)
            gerarBloco(*alvo, pendingCount, "rand", pendingRaio);
    }

    if (_width <= 0 || _height <= 0)
        throw std::runtime_error("mapa invalido: WIDTH/HEIGHT em falta em " + path);
}

std::pair<int, int> Mapa::gerarPosicao(const std::string &modo, int raio)
{
    if (modo == "center")
    {
        int r = raio > 0 ? raio : std::min(_width, _height) / 6;
        int cx = _width / 2;
        int cy = _height / 2;
        int x = cx + (rand() % (2 * r + 1)) - r;
        int y = cy + (rand() % (2 * r + 1)) - r;
        x = std::max(0, std::min(_width - 1, x));
        y = std::max(0, std::min(_height - 1, y));
        return std::make_pair(x, y);
    }
    else if (modo == "up")
    {
        int faixa = raio > 0 ? raio : _height / 6;
        faixa = std::max(1, std::min(_height, faixa));
        int x = rand() % _width;
        int y = rand() % faixa;
        return std::make_pair(x, y);
    }

    int x = rand() % _width;
    int y = rand() % _height;
    return std::make_pair(x, y);
}

void Mapa::gerarBloco(std::vector<std::pair<int, int> > &alvo, int count, const std::string &modo, int raio)
{
    alvo.reserve(alvo.size() + count);
    for (int i = 0; i < count; i++)
        alvo.push_back(gerarPosicao(modo, raio));
}

int Mapa::getWidth() const
{
    return _width;
}

int Mapa::getHeight() const
{
    return _height;
}

const std::vector<std::pair<int, int> > &Mapa::getBacterias() const
{
    return _bacterias;
}

const std::vector<std::pair<int, int> > &Mapa::getComida() const
{
    return _comida;
}

const std::vector<std::pair<int, int> > &Mapa::getVeneno() const
{
    return _veneno;
}

float Mapa::getBacteriaVida() const
{
    return _bacteriaVida;
}

float Mapa::getBacteriaVidaMaxima() const
{
    return _bacteriaVidaMaxima;
}
