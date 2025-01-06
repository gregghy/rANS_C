# rANS implementation in C

## Files

### step1.c, step2.c, step3.c
Ce sont des fichiers qui seront utilisés lors de la présentation du projet pour soutenir l'explication du fonctionnement de rANS

### aux.h, main.c
Ce sont les fichiers qui créent le compresseur rANS le plus complet et le plus efficace. Compilez avec 
`gcc main.c`
et exécutez l'exécutable avec 
`./a.out`

### att_x_normalization.c
Il s'agissait d'une tentative d'implémentation de rANS avec une normalisation du paramètre x. Cela n’a pas abouti et sera utilisé lors de la présentation.

### notes/
Contient mes notes analytiques sur le fonctionnement de la compression et de la décompression et leur efficacité en théorie et dans ma mise en œuvre, par rapport à l'entropie de Huffman et Shannon. Ils seront utilisés et expliqués lors de la présentation

## Performance
Les résultats obtenus n'étaient pas ceux attendus, en particulier le taux de compression final n'était que légèrement meilleur que celui de Huffman. La différence entre théorie et pratique sera discutée lors de la présentation

 
