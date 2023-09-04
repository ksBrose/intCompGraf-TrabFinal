# CompGrafTrabFin
Trabalho final de INF01047 Fundamentos de Computação Gráfica (2023/1)

# Integrantes

-Kelvin Schaun Brose (00177082)

-Gustavo Picoli (00332780)

# Introdução
A arte da animação tem desempenhado um papel fundamental na indústria do entretenimento, oferecendo uma plataforma para a criação de mundos imaginativos e a contação de histórias envolventes. Neste relatório, exploraremos os principais conceitos aprendidos na cadeira “Fundamentos de computação gráfica” aplicados à animação de Karatê desenvolvido pela Dupla.

# Contribuições
(Kelvin)
Animação, Movimentação por curva de Bezier, Mapeamento de textura e iluminação no boneco, Inserção de objetos da mesa e bonsai e implementação da camera.

(Gustavo)
Animação, Mapeamentos de textura e iluminação das Paredes e do Teto e Inserção de objeto da Porta.

# Implementação
O github e o IDE Code BLocks foram as ferramentas utilizadas para o desenvolvimento do trabalho. Utilizando as linguagens C e C++.
Ambos os integrantes da dupla tiveram participação no desenvolvimente de código do trabalho.

* Malhas Poligonais complexas                         -  OK 
* Transformações geométricas controladas pelo usuário -  OK 
* Câmera livre e câmera look-at                       -  OK       
* Instâncias de objetos                               -  OK 
* Três tipos de testes de intersecção                 -
* Modelos de Iluminação Difusa e Blinn-Phong          -  OK 
* Modelos de Interpolação de Phong e Gouraud          -
* Mapeamento de texturas em todos os objetos          -  OK 
* Movimentação com curva Bézier cúbica                -  OK 
* Animações baseadas no tempo ($\Delta t$)            -  OK 

# Desenvolvimento
Para o desenvolvimento do trabalho se utilizou o codigo fonte dos laboratorios da disciplina, principalmente o laboratório 3 e 5.
O laboratorio 3 tinha o foco na implementação de transformaçoes geometricas hierarquicas, fundamental para criação do boneco utilizado na animação.
E o laboratorio 5 que tinha o foco na implementação de texturas nos objetos no codigo.

Se utilizou curva de Bézier para movimentação do personagem, foram utilizadas curvas de grau 1,2 e 3. As curvas foram utilizadas para o deslocamento do boneco e para a movimentação dos membros, para o deslocamento se utilizou seu ponto no espaço (xyz) e a movimentação dos membros se utilizou os angulos do ponto de origem de cada membro em relação ao membro anterior. Para movimentos simples e lineares utilizou curva Bézier de grau 1, e curva Bézier cúbica para movimentos mais complexos.

Para o mapeamento das texturas, foi utilizado o site polyhaven.com/textures para a retirada das texturas utilizadas. Já os objetos utilizados na cena foram retirados do site rigmodels.com.

# Manual de uso e teclas de atalhos

Controle da camera:
Apertando a tecla "C" se altera o tipo de camera, Camera livre e Camera Look-at(olhar fixo no centro da sala)

Camera Livre:
Para movimentar a camera se utiliza as teclas WASD (movimentação para frente, esquerda, traz, e direita)
Com o botão esquerdo do mouse pressionado na janela e a movimentação do mesmo mudarão a direção da camera (olhar para esquerda, direita, cima e embaixo) 
Combinando os dois trea uma movimentação completa da camera.

Camera Look-at:
Para movimentar a camera botão esquerdo do mouse deve ser pressionado na janela e a movimentação do mesmo mudarão a direção da camera.
Para alterar a distancia do centro da sala utiliza a roda do mouse (scroll wheel)


Tipo de geometria:
Apertando a tecla "O": mudar a geometria para uma projeção ortográfica.
Apertando a tecla "P": mudar a geometria para uma projeção perspectiva.


Controle da animação:
É possivel controlar a animação utilizando as seguintes teclas:
Tecla "K": inicia/para a animação.
Tecla "ESPAÇO": reseta a posição do boneco para o ponto inicial e reseta o tempo.
Tecla "seta para esquerda": recua o tempo em 3 segundos.
Tecla "seta para direita": avança o tempo em 3 segundos.


Teclas extras:
Tecla "B": Muda a textura do Boneco.
Tecla "H": Esconde/Mostra informações na tela.
Tecla "ESC": Fecha a janela.

# Imagens 
Boneco base (sem textura) e parado, Camera Perspectiva e Câmera Livre 
![image](https://github.com/ksBrose/intCompGraf-TrabFinal/assets/104263191/1645296a-81e3-47c6-9620-513af53a42da)

Boneco com Textura em movimento, Camera Perspectiva e Câmera Fixa
![image](https://github.com/ksBrose/intCompGraf-TrabFinal/assets/104263191/2f76f643-666a-4471-ad45-918c307a2f93)
