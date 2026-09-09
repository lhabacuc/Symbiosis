#include "Ecosystem.hpp"
#include <cstdlib>
#include <algorithm>
#include <cmath>

Ecosystem::Ecosystem(const Mapa &mapa)
    : _mapa(mapa), _playWidth(mapa.getWidth()), _playHeight(mapa.getHeight())
{
    _desenhoDetalhado = true;
    _gradeComida.configurar(_playWidth, _playHeight, 24);
    _gradeVeneno.configurar(_playWidth, _playHeight, 24);
    _gradeBacterias.configurar(_playWidth, _playHeight, 24);

    _config.velocidadeMs = 50.0f;
    _config.energiaInicial = 90.0f;
    _config.vidaMaxima = 250.0f;
    _config.valorComida = 10.0f;
    _config.danoVeneno = 6.0f;
    _config.limiteReproducao = 130.0f;
    _config.gregarismo = false;
    _config.raioSocial = 80.0f;
    _config.custoSolidao = 3.0f;

    _config.temperaturaAmbiente = 20.0f;
    _zonasTemperatura = mapa.getZonasTemperatura();

    _config.configBacterias = static_cast<float>(mapa.getBacterias().size());
    _config.configComida = static_cast<float>(mapa.getComida().size());
    _config.configVeneno = static_cast<float>(mapa.getVeneno().size());
    _config.ultimoConfigBacterias = _config.configBacterias;

    if (mapa.getBacteriaVida() >= 0.0f)
        _config.energiaInicial = mapa.getBacteriaVida();
    if (mapa.getBacteriaVidaMaxima() >= 0.0f)
        _config.vidaMaxima = mapa.getBacteriaVidaMaxima();

    carregarDeMapa();
}

EcosystemConfig &Ecosystem::config()
{
    return _config;
}

const EcosystemConfig &Ecosystem::config() const
{
    return _config;
}

int Ecosystem::getPlayWidth() const
{
    return _playWidth;
}

int Ecosystem::getPlayHeight() const
{
    return _playHeight;
}

bool Ecosystem::desenhoDetalhado() const
{
    return _desenhoDetalhado;
}

const std::vector<Bacteria> &Ecosystem::getBacterias() const
{
    return _bacterias;
}

const std::vector<Item> &Ecosystem::getComida() const
{
    return _comida;
}

const std::vector<Item> &Ecosystem::getVeneno() const
{
    return _veneno;
}

const std::vector<ZonaTemperatura> &Ecosystem::getZonasTemperatura() const
{
    return _zonasTemperatura;
}

float Ecosystem::temperaturaEm(int x, int y) const
{
    for (size_t i = 0; i < _zonasTemperatura.size(); i++)
    {
        const ZonaTemperatura &zona = _zonasTemperatura[i];
        int dx = x - zona.x;
        int dy = y - zona.y;
        if (dx * dx + dy * dy <= zona.raio * zona.raio)
            return zona.valor;
    }
    return _config.temperaturaAmbiente;
}

void Ecosystem::spawnComida(int n)
{
    for (int i = 0; i < n; i++)
    {
        Item c;
        c.x = rand() % _playWidth;
        c.y = rand() % _playHeight;
        _comida.push_back(c);
    }
}

void Ecosystem::spawnVeneno(int n)
{
    for (int i = 0; i < n; i++)
    {
        Item v;
        v.x = rand() % _playWidth;
        v.y = rand() % _playHeight;
        _veneno.push_back(v);
    }
}

void Ecosystem::spawnBacterias(int n)
{
    for (int i = 0; i < n; i++)
    {
        Bacteria b(rand() % _playWidth, rand() % _playHeight);
        b.setEnergy(_config.energiaInicial);
        b.setIdadeMaxima(static_cast<int>(_config.vidaMaxima));
        _bacterias.push_back(b);
    }
}

void Ecosystem::sincronizarPopulacao()
{
    int alvo = static_cast<int>(_config.configBacterias);
    int ultimo = static_cast<int>(_config.ultimoConfigBacterias);

    if (alvo == ultimo)
        return;

    int delta = alvo - ultimo;
    if (delta > 0)
        spawnBacterias(delta);
    else
    {
        int remover = std::min(-delta, static_cast<int>(_bacterias.size()));
        _bacterias.erase(_bacterias.end() - remover, _bacterias.end());
    }

    _config.ultimoConfigBacterias = _config.configBacterias;
}

void Ecosystem::carregarDeMapa()
{
    _bacterias.clear();
    _comida.clear();
    _veneno.clear();

    const std::vector<std::pair<int, int> > &bacterias = _mapa.getBacterias();
    for (size_t i = 0; i < bacterias.size(); i++)
    {
        Bacteria b(bacterias[i].first, bacterias[i].second);
        b.setEnergy(_config.energiaInicial);
        b.setIdadeMaxima(static_cast<int>(_config.vidaMaxima));
        _bacterias.push_back(b);
    }

    const std::vector<std::pair<int, int> > &comida = _mapa.getComida();
    for (size_t i = 0; i < comida.size(); i++)
    {
        Item c;
        c.x = comida[i].first;
        c.y = comida[i].second;
        _comida.push_back(c);
    }

    const std::vector<std::pair<int, int> > &veneno = _mapa.getVeneno();
    for (size_t i = 0; i < veneno.size(); i++)
    {
        Item v;
        v.x = veneno[i].first;
        v.y = veneno[i].second;
        _veneno.push_back(v);
    }

    _config.configBacterias = static_cast<float>(_bacterias.size());
    _config.configComida = static_cast<float>(_comida.size());
    _config.configVeneno = static_cast<float>(_veneno.size());
    _config.ultimoConfigBacterias = _config.configBacterias;
}

void Ecosystem::reiniciar()
{
    carregarDeMapa();
}

void Ecosystem::limpar()
{
    _bacterias.clear();
    _comida.clear();
    _veneno.clear();
    _config.ultimoConfigBacterias = 0.0f;
}

void Ecosystem::update()
{
    // reconstroi as grelhas espaciais uma vez por tick: consultas de vizinhanca
    // passam a ser O(1) amortizado em vez de O(n) por bacteria (O(n*m) no total)
    _gradeComida.limpar();
    for (size_t j = 0; j < _comida.size(); j++)
        _gradeComida.inserir(static_cast<int>(j), _comida[j].x, _comida[j].y);

    _gradeVeneno.limpar();
    for (size_t j = 0; j < _veneno.size(); j++)
        _gradeVeneno.inserir(static_cast<int>(j), _veneno[j].x, _veneno[j].y);

    if (_config.gregarismo)
    {
        _gradeBacterias.limpar();
        for (size_t j = 0; j < _bacterias.size(); j++)
            _gradeBacterias.inserir(static_cast<int>(j), _bacterias[j].getX(), _bacterias[j].getY());
    }

    std::vector<bool> comidaUsada(_comida.size(), false);
    std::vector<bool> venenoUsado(_veneno.size(), false);

    for (size_t i = 0; i < _bacterias.size(); i++)
    {
        Bacteria &b = _bacterias[i];
        b.envelhecer();

        int bx = b.getX();
        int by = b.getY();

        b.aplicarTemperatura(temperaturaEm(bx, by));

        // comida a alcance (consumo) - grelha em vez de varrer todos os itens
        _gradeComida.paraCadaVizinho(bx, by, 8.0f, [&](int j)
        {
            if (comidaUsada[j])
                return;
            int dx = bx - _comida[j].x;
            int dy = by - _comida[j].y;
            if (dx > -8 && dx < 8 && dy > -8 && dy < 8)
            {
                comidaUsada[j] = true;
                b.setEnergy(b.getEnergy() + _config.valorComida);
            }
        });

        // veneno a alcance (dano)
        _gradeVeneno.paraCadaVizinho(bx, by, 8.0f, [&](int j)
        {
            if (venenoUsado[j])
                return;
            int dx = bx - _veneno[j].x;
            int dy = by - _veneno[j].y;
            if (dx > -8 && dx < 8 && dy > -8 && dy < 8)
            {
                venenoUsado[j] = true;
                b.setEnergy(b.getEnergy() - _config.danoVeneno);
            }
        });

        // quimiotaxia: em vez de apontar diretamente ao alvo (o que seria uma bacteria
        // "omnisciente"), sente-se a concentracao de "cheiro" de comida no raio de visao
        // (soma de 1/distancia de cada item proximo) e reage por tentativa em
        // Bacteria::moverQuimiotaxia - tal como bacterias reais fazem run-and-tumble.
        float raio = b.getRaioVisaoPixels();
        float raio2 = raio * raio;
        float concentracao = 0.0f;

        _gradeComida.paraCadaVizinho(bx, by, raio, [&](int j)
        {
            float dx = static_cast<float>(bx - _comida[j].x);
            float dy = static_cast<float>(by - _comida[j].y);
            float dist2 = dx * dx + dy * dy;
            if (dist2 <= raio2)
                concentracao += 1.0f / (1.0f + std::sqrt(dist2));
        });

        if (concentracao > 0.0f)
        {
            b.moverQuimiotaxia(concentracao);
            continue;
        }

        b.resetSinalQuimico();

        if (!_config.gregarismo)
        {
            b.moverPreferido();
            continue;
        }

        // centro de massa do grupo (vizinhas dentro do raio social), via grelha
        long somaX = 0;
        long somaY = 0;
        int vizinhas = 0;
        float raioSocial2 = _config.raioSocial * _config.raioSocial;
        size_t indiceAtual = i;

        _gradeBacterias.paraCadaVizinho(bx, by, _config.raioSocial, [&](int j)
        {
            if (static_cast<size_t>(j) == indiceAtual)
                return;
            float dx = static_cast<float>(bx - _bacterias[j].getX());
            float dy = static_cast<float>(by - _bacterias[j].getY());
            float dist2 = dx * dx + dy * dy;
            if (dist2 <= raioSocial2)
            {
                somaX += _bacterias[j].getX();
                somaY += _bacterias[j].getY();
                vizinhas++;
            }
        });

        if (vizinhas > 0)
        {
            int centroX = static_cast<int>(somaX / vizinhas);
            int centroY = static_cast<int>(somaY / vizinhas);
            b.moverPara(centroX, centroY);
        }
        else
        {
            b.moverPreferido();
            b.setEnergy(b.getEnergy() - _config.custoSolidao);
        }
    }

    resolverSobreposicoes();

    if (!_comida.empty())
    {
        std::vector<Item> restante;
        restante.reserve(_comida.size());
        for (size_t j = 0; j < _comida.size(); j++)
            if (!comidaUsada[j])
                restante.push_back(_comida[j]);
        _comida = std::move(restante);
    }

    if (!_veneno.empty())
    {
        std::vector<Item> restante;
        restante.reserve(_veneno.size());
        for (size_t j = 0; j < _veneno.size(); j++)
            if (!venenoUsado[j])
                restante.push_back(_veneno[j]);
        _veneno = std::move(restante);
    }

    // regenera proporcionalmente ao defice: uma quantidade fixa nao acompanha populacoes grandes
    int deficitComida = static_cast<int>(_config.configComida) - static_cast<int>(_comida.size());
    if (deficitComida > 0)
        spawnComida(std::max(2, std::min(500, deficitComida / 10)));

    int deficitVeneno = static_cast<int>(_config.configVeneno) - static_cast<int>(_veneno.size());
    if (deficitVeneno > 0)
        spawnVeneno(std::max(1, std::min(200, deficitVeneno / 10)));

    // remocao das mortas por troca-com-o-ultimo (O(1) por remocao, em vez de O(n) do erase do meio)
    for (size_t i = 0; i < _bacterias.size();)
    {
        if (_bacterias[i].estaMorto())
        {
            _bacterias[i] = _bacterias.back();
            _bacterias.pop_back();
        }
        else
            i++;
    }

    size_t nAtual = _bacterias.size();
    for (size_t i = 0; i < nAtual; i++)
    {
        if (_bacterias[i].getEnergy() > _config.limiteReproducao)
        {
            _bacterias[i].setEnergy(_bacterias[i].getEnergy() / 2.0f);
            Bacteria filha = _bacterias[i].divide();
            filha.setIdadeMaxima(static_cast<int>(_config.vidaMaxima));
            _bacterias.push_back(filha);
        }
    }

    _desenhoDetalhado = _bacterias.size() <= 3000;
}

void Ecosystem::resolverSobreposicoes()
{
    const float distanciaMinima = 6.0f;

    _gradeBacterias.limpar();
    for (size_t j = 0; j < _bacterias.size(); j++)
        _gradeBacterias.inserir(static_cast<int>(j), _bacterias[j].getX(), _bacterias[j].getY());

    for (size_t i = 0; i < _bacterias.size(); i++)
    {
        int bx = _bacterias[i].getX();
        int by = _bacterias[i].getY();

        _gradeBacterias.paraCadaVizinho(bx, by, distanciaMinima, [&](int j)
        {
            if (static_cast<size_t>(j) <= i)
                return;

            float dx = static_cast<float>(bx - _bacterias[j].getX());
            float dy = static_cast<float>(by - _bacterias[j].getY());
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist >= distanciaMinima)
                return;

            float nx, ny;
            if (dist < 0.001f)
            {
                nx = 1.0f;
                ny = 0.0f;
            }
            else
            {
                nx = dx / dist;
                ny = dy / dist;
            }

            float sobreposicao = (distanciaMinima - dist) / 2.0f;
            int px = static_cast<int>(nx * sobreposicao) + (nx > 0 ? 1 : (nx < 0 ? -1 : 0));
            int py = static_cast<int>(ny * sobreposicao) + (ny > 0 ? 1 : (ny < 0 ? -1 : 0));

            _bacterias[i].empurrar(px, py);
            _bacterias[j].empurrar(-px, -py);
        });
    }
}
