#ifndef GRADEESPACIAL_HPP
# define GRADEESPACIAL_HPP

# include <vector>
# include <algorithm>

// Grelha uniforme para consultas de vizinhanca em O(1) amortizado,
// em vez de comparar cada entidade com todas as outras (O(n*m)).
class GradeEspacial
{
    private:
        int                             _cellSize;
        int                             _gridW;
        int                             _gridH;
        std::vector<std::vector<int> >  _celulas;

        int indice(int cx, int cy) const
        {
            return cy * _gridW + cx;
        }

        int celX(int x) const
        {
            int cx = x / _cellSize;
            if (cx < 0)
                cx = 0;
            if (cx >= _gridW)
                cx = _gridW - 1;
            return cx;
        }

        int celY(int y) const
        {
            int cy = y / _cellSize;
            if (cy < 0)
                cy = 0;
            if (cy >= _gridH)
                cy = _gridH - 1;
            return cy;
        }

    public:
        void configurar(int largura, int altura, int cellSize)
        {
            _cellSize = cellSize < 1 ? 1 : cellSize;
            _gridW = largura / _cellSize + 1;
            _gridH = altura / _cellSize + 1;
            _celulas.assign(static_cast<size_t>(_gridW) * static_cast<size_t>(_gridH), {});
        }

        void limpar()
        {
            for (size_t i = 0; i < _celulas.size(); i++)
                _celulas[i].clear();
        }

        void inserir(int idx, int x, int y)
        {
            _celulas[indice(celX(x), celY(y))].push_back(idx);
        }

        // chama fn(idx) para cada indice inserido cuja celula esteja dentro do raio (aproximado por caixa)
        template <typename F>
        void paraCadaVizinho(int x, int y, float raio, F fn) const
        {
            int minCx = celX(static_cast<int>(x - raio));
            int maxCx = celX(static_cast<int>(x + raio));
            int minCy = celY(static_cast<int>(y - raio));
            int maxCy = celY(static_cast<int>(y + raio));

            for (int cy = minCy; cy <= maxCy; cy++)
            {
                for (int cx = minCx; cx <= maxCx; cx++)
                {
                    const std::vector<int> &lista = _celulas[indice(cx, cy)];
                    for (size_t k = 0; k < lista.size(); k++)
                        fn(lista[k]);
                }
            }
        }
};

#endif
