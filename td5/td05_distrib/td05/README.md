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
ayant ce numéro de descripteur de fichier, avec² le mode indiqué (read-only, write-only,
read & write, etc).

Après avoir ouvert un fichier, il est possible de le fermer, et cela en fermant son
descripteur de fichier, qu'on donne en paramètre de la fonction close(fd).
Cela a uniquement pour objectif une gestion des ressources.
En libérant le fichier, il peut éventuellement (lorsqu'on est dans le cas d'une 
écriture) être rendu disponible pour un autre thread/processus.

A la fin du thread principal et uniquement à la fin du thread principal, tous les
fichiers ouverts non fermés sont fermés automatiquement par l'extinction du
thread principal.
Les fichiers ouverts dans des threads dérivants du thread principal ne sont pas automatiquement 
fermés à la fin du thread qui les a créés, mais seulement à la fin du thread principal.

En effet, les ressources au sein d'un même processus sont partagées entre tous les
threads. Or, les fichiers ouverts sont des ressources. Tous les threads ouvrant
un fichier partagent donc le descripteur de ce fichier (la table de descripteurs est
également une ressource partagée).

Si maintenant deux threads différents ouvrent le même fichier, deux nouveaux
descripteurs de fichiers différents seront créés.

Un descripteur de fichier est une référence vers un fichier ouvert.

Les ouvertures de fichier sont toujours autorisées si le fichier est trouvé.

## Exercice 2

Lorsqu'on ouvre un fichier (en l'occurrence cela va concerner les repertoires, 
mais il n'y a aucune différence entre un vrai fichier et un repertoire hormis 
le type du fichier), une pile stockant ..

rewind_dir permet 
