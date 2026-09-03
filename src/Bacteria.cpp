#include "Bacteria.hpp"

Bacteria::Bacteria(int x, int y)
    : SerVivo(x, y), geneVelocidade(1.0f), geneRaioVisao(1.0f), genePreferencia(0.5f)
{
}

Bacteria::Bacteria(const Bacteria &other, float genePreference)
    : SerVivo(other), geneVelocidade(other.geneVelocidade), geneRaioVisao(other.geneRaioVisao),
      genePreferencia(genePreference)
{
}

Bacteria::Bacteria(int posX, int posY, float maeVelocidade, float maeVisao, float maePreferencia)
    : SerVivo(posX, posY), geneVelocidade(maeVelocidade), geneRaioVisao(maeVisao), genePreferencia(maePreferencia)
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

bool Bacteria::podeSeDividir() const
{
    return getEnergy() > LIMITE_REPRODUCAO;
}

void Bacteria::viver()
{
    setIdade(getIdade() + 1);

    float custoPreferencia = genePreferencia * 0.2f;
    float custoEnergia = 1.0f + (geneVelocidade * 1.5f) + (geneRaioVisao * 0.5f) + custoPreferencia;
    setEnergy(getEnergy() - custoEnergia);

    x += (int)(((rand() % 3) - 1) * geneVelocidade);
    y += (int)(((rand() % 3) - 1) * geneVelocidade);
}

void Bacteria::interagirComItem(TipoItem item)
{
    switch (item)
    {
        case FOOD:
            setEnergy(getEnergy() + 5.0f);
            std::cout << "Bacteria at (" << getX() << ", " << getY() << ") ate food. Energy: " << getEnergy() << std::endl;
            break;
        case POISON:
            setEnergy(getEnergy() - 5.0f);
            std::cout << "Bacteria at (" << getX() << ", " << getY() << ") was poisoned. Energy: " << getEnergy() << std::endl;
            break;
        case NEUTRAL:
            break;
    }
}

Bacteria Bacteria::divide()
{
    return Bacteria(getX(), getY(), geneVelocidade, geneRaioVisao, genePreferencia);
}
