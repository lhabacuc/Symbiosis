# Symbiosis

Simulação de um ecossistema de bactérias em C++, com visualização gráfica em tempo real feita com [raylib](https://github.com/raysan5/raylib) e [raygui](https://github.com/raysan5/raygui).

Cada bactéria é um `SerVivo` com genes próprios (velocidade, raio de visão, preferência, direção preferida) que mutam na reprodução, procura comida ativamente, evita veneno, pode se comportar de forma social (gregarismo/flocking) e envelhece até morrer. A população, o ambiente e as características das bactérias podem ser ajustados ao vivo através de um painel lateral, sem parar a simulação.

## Demonstração

https://github.com/lhabacuc/Symbiosis/raw/main/demo/video01.webm

> O GitHub nem sempre reproduz vídeos `.webm` diretamente na página do README — se não aparecer um player acima, faz clique no link para descarregar/reproduzir, ou vê o ficheiro em [`demo/video01.webm`](demo/video01.webm).

## Funcionalidades

- **Motor de simulação** com genética simples (mutação de genes na reprodução), envelhecimento, energia, alimentação e envenenamento.
- **Comportamento inteligente**: as bactérias procuram comida dentro do seu raio de visão em vez de andar apenas ao acaso.
- **Gregarismo (opcional)**: as bactérias podem se agrupar e mover-se em conjunto na direção do centro de massa dos vizinhos, com custo de energia por "solidão".
- **Anti-sobreposição**: bactérias não ficam empilhadas exatamente umas sobre as outras.
- **Grelha espacial (`GradeEspacial`)** para consultas de vizinhança O(1) amortizado, permitindo simular dezenas/centenas de milhares de entidades sem travar.
- **Passo fixo com acumulador** (com limite de passos por frame) para manter a simulação estável mesmo sob carga pesada.
- **Painel de controlo em tempo real** (raygui): pausar/continuar, reiniciar, limpar, ajustar população de bactérias/comida/veneno, velocidade da simulação, características das bactérias (energia inicial, vida máxima, valor da comida, dano do veneno, limite de reprodução) e parâmetros de gregarismo.
- **Câmara** com zoom (roda do rato) e pan (arrastar com o botão direito).
- **Mapas em formato `.sy`**: ficheiros de texto simples que definem o tamanho do mapa e os blocos iniciais de bactérias, comida e veneno.

## Formato do mapa `.sy`

```
WIDTH 800
HEIGHT 600

BACTERIA 500 900 1500
POSSISION rand

COMIDA 400 40
POSSISION center

VENENO 56
POSSISION up
```

- `WIDTH` / `HEIGHT`: dimensões da área de simulação.
- `BACTERIA <quantidade> [vida] [vidaMaxima]`: número de bactérias e, opcionalmente, a energia inicial e idade máxima.
- `COMIDA <quantidade> <raio>`: número de itens de comida e o raio/área de geração.
- `VENENO <quantidade>`: número de itens de veneno.
- `POSSISION <rand|center|up>`: modo de posicionamento aplicado ao bloco imediatamente anterior (`rand` = aleatório no mapa, `center` = agrupado ao centro, `up` = na faixa superior).

## Compilar

Requer `c++` com suporte a C++17 e as dependências de sistema do raylib (`libGL`, `libX11`, `pthread`, `dl`, `rt`).

```sh
make
```

A primeira compilação também constrói o raylib a partir do código-fonte (`raylib/src`) como biblioteca estática.

Outros alvos: `make clean`, `make fclean`, `make re`.

## Executar

```sh
./symbiosis [caminho/para/mapa.sy]
```

Se nenhum mapa for indicado, é usado `maps/default.sy`.

## Estrutura do projeto

```
include/    Cabeçalhos (SerVivo, Bacteria, Simulation, Visualizer, Mapa, GradeEspacial, raygui)
src/        Implementação
maps/       Ficheiros de mapa .sy
demo/       Vídeo de demonstração
raylib/     Código-fonte do raylib (submódulo/clone usado para compilar a biblioteca)
```
