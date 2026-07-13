# TETRIS

## Sobre o Código
São 7 peças distintas armazenadas em uma estrutura denominada "bloco". Cada bloco contém:  
I. Matriz de caracteres (o desenho)  
II. Cor  
III. Coordenada própria  
IV. Coordenada (em relação à matriz) dos caracteres significativos da matriz (equivale a uma hitbox)  

Cada caractere do mapa é armazenado numa estrutura denominada "célula". Cada célula contém cor e caractere próprios.  

A rotação dos blocos é uma reorganização da própria matriz, na qual a última linha vira a primeira coluna, e assim sucessivamente.  

A colisão verifica somente os caracteres significativos. Ao colidir com outros blocos, o bloco atual congela, isto é, vira parte do mapa, sem haver diferenciações entre parede e bloco a não ser pela própria cor e caractere. Isso permite que a limpeza de linhas completas ocorra de forma mais simplificada.  

Falhas: os blocos podem rotacionar mesmo se seus caracteres significativos coincidirem com o de outro bloco, então evite fazer isso de preferência....
