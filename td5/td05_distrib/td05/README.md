# TP 5 - Entrées/Sorties

## Exercice 1

Il existe trois descripteurs de fichiers par défaut : stdin, stdout et stderr, qui
correspondent respectivement à trois fichiers ouverts : l'entrée standard, à la 
sortie standard, et à la sortie d'erreur. Elles sont d'ailleurs définies en tant
que constantes dans stdio.h, la bibliothèque gérant notamment les entrées/sorties.

On remarque d'ailleurs que la table des descripteurs de fichiers est ordonnée par
numéro de descripteur de fichiers. Les trois premiers descripteurs de fichiers
ouverts (numéro de descripteur de fichier égal à 0 (stdin), 1 (stdout) et 2 (stderr))
sont donc les trois premiers éléments de la table.

Chaque fichier ouvert par la suite aura alors un numéro de descripteur de fichier de
valeur supérieure qui viendra s'ajouter à la table des descripteurs de fichiers.

On remarquera également que fileno(filename) permet de récupérer le numéro du 
descripteur de fichier du fichier qu'on lui donne en paramètre.

Quant à open(fd, mode), il permet d'ouvrir un fichier (un repertoire est aussi un fichier)
ayant ce numéro de descripteur de fichier, avec le mode indiqué (read-only, write-only,
read & write, etc).


## Exercice 2