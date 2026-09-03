#include "Bacteria.hpp"
#include <algorithm>
#include <cmath>

static float anguloAleatorio()
{
    return static_cast<float>(rand() % 628) / 100.0f; // 0 a ~2*PI
}

Bacteria::Bacteria(int x, int y)
    : SerVivo(x, y), geneVelocidade(0.3f), geneRaioVisao(0.3f), genePreferencia(0.5f),
      geneDirecaoPreferida(anguloAleatorio())
{
}

Bacteria::Bacteria(const Bacteria &other, float genePreference)
    : SerVivo(other), geneVelocidade(other.geneVelocidade), geneRaioVisao(other.geneRaioVisao),
      genePreferencia(genePreference), geneDirecaoPreferida(other.geneDirecaoPreferida)
{
}

Bacteria::Bacteria(int posX, int posY, float maeVelocidade, float maeVisao, float maePreferencia)
    : SerVivo(posX, posY), geneVelocidade(maeVelocidade), geneRaioVisao(maeVisao), genePreferencia(maePreferencia),
      geneDirecaoPreferida(anguloAleatorio())
{
    if (rand() % 100 < 15) // 15% de chance de mutacao
    {
        float mutacaoV = ((rand() % 200) - 100) / 1000.0f;
        geneVelocidade = maeVelocidade + mutacaoV;
        if (geneVelocidade < 0.1f)
            geneVelocidade = 0.1f;
    }

    if (rand() % 100 < 15)
    {
        float mutacaoR = ((rand() % 200) - 100) / 1000.0f;
        geneRaioVisao = maeVisao + mutacaoR;
        if (geneRaioVisao < 0.5f)
            geneRaioVisao = 0.5f;
    }

    if (rand() % 100 < 15)
    {
        float mutacaoP = ((rand() % 200) - 100) / 1000.0f;
        genePreferencia = maePreferencia + mutacaoP;
        if (genePreferencia > 1.0f)
            genePreferencia = 1.0f;
        else if (genePreferencia < -1.0f)
            genePreferencia = -1.0f;
    }
}

void Bacteria::setPreference(float genePreference)
{
    genePreferencia = genePreference;
}

float Bacteria::getPreference() const
{
    return genePreferencia;
}

float Bacteria::getGeneVelocidade() const
{
    return geneVelocidade;
}

float Bacteria::getGeneRaioVisao() const
{
    return geneRaioVisao;
}

float Bacteria::getGenePreferencia() const
{
    return genePreferencia;
}

float Bacteria::getDirecaoPreferida() const
{
    return geneDirecaoPreferida;
}

void Bacteria::setDirecaoPreferida(float direcao)
{
    geneDirecaoPreferida = direcao;
}

bool Bacteria::podeSeDividir() const
{
    return getEnergy() > LIMITE_REPRODUCAO;
}

void Bacteria::envelhecer()
{
    setIdade(getIdade() + 1);

    float custoPreferencia = genePreferencia * 0.2f;
    float custoEnergia = 1.0f + (geneVelocidade * 1.5f) + (geneRaioVisao * 0.5f) + custoPreferencia;
    setEnergy(getEnergy() - custoEnergia);
}

int Bacteria::getPasso() const
{
    int passo = (int)(geneVelocidade * 10.0f);
    if (passo < 2)
        passo = 2;
    return passo;
}

void Bacteria::moverAleatorio()
{
    int passo = getPasso();
    x += ((rand() % 3) - 1) * passo;
    y += ((rand() % 3) - 1) * passo;
}

void Bacteria::moverPreferido()
{
    int passo = getPasso();

    // pequena oscilacao para o percurso nao ser uma linha perfeitamente reta
    float ruido = (static_cast<float>(rand() % 100) / 100.0f - 0.5f) * 0.8f;
    float angulo = geneDirecaoPreferida + ruido;

    x += static_cast<int>(std::cos(angulo) * passo);
    y += static_cast<int>(std::sin(angulo) * passo);

    // a direcao preferida vai desviando lentamente, como se explorasse aos poucos
    geneDirecaoPreferida += (static_cast<float>(rand() % 100) / 100.0f - 0.5f) * 0.1f;
}

void Bacteria::moverPara(int tx, int ty)
{
    int passo = getPasso();
    int dx = tx - x;
    int dy = ty - y;

    if (dx > 0)
        x += std::min(passo, dx);
    else if (dx < 0)
        x += std::max(-passo, dx);

    if (dy > 0)
        y += std::min(passo, dy);
    else if (dy < 0)
        y += std::max(-passo, dy);
}

float Bacteria::getRaioVisaoPixels() const
{
    return geneRaioVisao * 120.0f;
}

void Bacteria::viver()
{
    envelhecer();
    moverPreferido();
}

void Bacteria::interagirComItem(TipoItem item)
{
    switch (item)
    {
        case FOOD:
            setEnergy(getEnergy() + 5.0f);
            break;
        case POISON:
            setEnergy(getEnergy() - 5.0f);
            break;
        case NEUTRAL:
            break;
    }
}

Bacteria Bacteria::divide()
{
    Bacteria filha(getX(), getY(), geneVelocidade, geneRaioVisao, genePreferencia);

    float direcao = geneDirecaoPreferida;
    if (rand() % 100 < 15) // 15% de chance de mutacao na direcao preferida
        direcao += (static_cast<float>(rand() % 200) - 100.0f) / 100.0f;
    filha.setDirecaoPreferida(direcao);

    return filha;
}
