## Exercice 1

On voit qu'il y a plusieurs fichiers, certains de types C,
et parfois certains headers (.h) associés.

Un header qui va nous intéresser ici est sort.h, qui est en fait
l'interface/la signature utilisée lorsqu'on voudra trier un tableau d'entier.
Cela permet de pouvoir "interchanger" facilement l'algorithme de tri utilisé
dans main.c

On remarque donc aussi qu'il y a des fichiers permettant d'appliquer
un tri à un tableau d'entier conformément à un certain algorithme
(particulier à chaque fichier, par exemple bubble sort, merge sort...).

On voit ensuite qu'il y a des fichiers "utilitaires" telles que timer,
utils ou encore unused.
Utils permet d'afficher le contenu d'un tableau d'entier.
Timer permet de mesurer des durées à une précision en nanosecondes.
Unused permet d'effectuer diverses opérations arithmétiques :
calcul d'une suite de fibonacci et PGCD notamment.

Dans le fichier main.c, fichier central de cette structure,
on y retrouve des imports de headers
(notamment ceux des headers fournis (sort.h, utils.h, etc) ainsi que ceux de
bibliothèques C comme stdio, stdlib).

### Quel tri est utilisé dans le main ?

C'est une question légitime, puisque dans le main.c, on ne sait pas
quel algorithme sera utilisé uniquement grâce au code ou aux imports .
Pour déduire cette information, il faut aller dans le Makefile.

Nous y voyons notamment la variable BASIC_EXE qui contient une tâche
permettant de "transformer" (substitution de texte) tous les fichiers finissant
par .exe en fichiers finissant par "-basicExe.exe".

Nous voyons également une règle ligne 18 avec comme cible : "tri_%-basicExe.exe".
Il s'agit du fichier qui sera produit à partir de l'édition de lien :
on produit un executable à partir de fichiers objets.

Nous remarquons également que parmi les fichiers objets utilisés, il y a "%.o".
Il s'agit du même "%" qui est dans "tri_%-basicExe.exe".
Dans les faits, il n'y a que le nom des fichiers de tri qui seront utilisés :
si nous faisons l'édition de lien avec bubble.o, alors cela produira :
"tri_bubble-basicExe.exe" au final.

C'est parce qu'il y a un seul executable produit par tri utilisé que le 
compilateur arrive à lier le bon algorithme de tri au fichier main.
L'éditeur de lien va alors lier (selon qu'il copie ou qu'il fasse une 
référence) la fonction "sort" utilisée dans le main, à une seule 
implémentation qui est définie dans un fichier de tri.

Cela se fait grâce à la règle qui dit de produire un executable nommé
par exemple : "tri_%-basicExe.exe" avec les dépendances suivantes : 
"main.o %.o timer.o utils.o unused.o".
Ce qui nous intéresse dans cette ligne est notamment le "main.o" et le 
"%.o".
Hormis timer.o, utils.o et unused.o (et main.o), il n'y a que les fichiers
implémentant des algorithmes de tri qui peuvent être sous la forme d'un fichier
objet : par exemple merge.o, quick.o...
Ainsi, "%" se fait substituer par les fichiers de tri.
Ce qui fait que finalement le nom des fichiers produit est bel et bien lié
au tri utilisé et à la manière d'utiliser les bibliothèques.


## Exercice 2

La commande ldd permet de lister l'ensemble des bibliothèques partagées
requises par un exécutable.
En utilisant : "ldd tri_bubble-basicExe.exe", je visualise les bibliothèques
partagées que requiert cet exécutable.
On voit alors que l'exécutable nécessite 3 bibliothèques partagées :

- linux-vdso.so.1, qui n'est en fait pas vraiment une bibliothèque partagée,
  mais un bout de kernel (enfin, ce sont des adresses, bien-sûr) placé dans
  chaque programme exécutable afin de simplifier/rendre plus efficaces
  les appels systèmes implicites.
- libc.so.6, la bibliothèque C standard.
- ld-linux-x86-64.so.2, qui est en fait 
l'éditeur de liens dynamique/le chargeur dynamique, qui permet
de gérer le chargement des bibliothèques partagées dans notre programme 
exécutable.

## Exercice 3

### Comparaison performances de tri : statique vs dynamique bubble sort

Les deux programmes ont effectivements triés le même tableau de nombres.

Lors de la première exécution, le tri à bulle "dynamique" a pris 2800 nanosecondes.
pour trier le tableau, alors que le tri à bulle "statique" a pris 1100 nanosecondes.

Ce gain de temps d'execution/performance peut s'expliquer par le fait
qu'il n'y a pas besoin d'aller chercher du code dans d'autres fichiers, mais
uniquement à des lignes voisines du même fichier, puisque l'option "-static"
indique de copier dans notre programme le contenu "nécessaire" des bibliothèques
utilisées.

### Pas de bibliothèques utilisées ?

Effectivement, en utilisant "ldd tri_bubble-staticExe.exe", ldd nous répond :
"not a dynamic executable", il n'utilise donc pas de bibliothèque.

### Comparaison de taille avec la commande ls -lh

En utilisant ls -lh dans mon répertoire et en observant les tailles de 
tri_bubble-staticExe.exe et de tri_bubble-basicExe.exe, on remarque que la version
statique prend 35 fois plus de place en mémoire (806 000 octets vs 23 000 octets).
Cela s'explique par le fait que dans le fichier "statique", le code des bibliothèques
ait été copié en dur dans le programme (il n'y a donc en fait plus de bibliothèque, 
mais uniquement notre programme qui contient tout le nécessaire pour s'exécuter, 
il devient donc indépendant), alors que dans l'autre cas, le programme 
"va chercher" le code nécessaire lors de l'exécution dans les bibliothèques 
correspondantes.

## Exercice 4

### Execution

Les exécutions des différents programmes sont identiques, elles produisent le 
même résultat.

### Comparaison de tailles avec size et ls -lh

#### Avec la commande ls -lh

Avec ls -lh, on voit que staticExe > basicExe >= staticLib :
806 000 octets > 23 000 octets > 22 000 octets.
On voit donc une énorme différence, de l'ordre d'un facteur 36, 
entre staticLib et staticExe.
Cependant, entre basicExe et staticLib, la différence est inférieure à 
1 kilooctet.  Cela reste tout de même important à l'échelle d'un langage 
"bas" niveau comme le C, notamment dans le cas où nous sommes amenés à aller 
sur de l'embarqué avec des micro-contrôleurs très limités en mémoire.

#### Avec la commande size

Etant donné que l'unité n'est pas indiquée pour la commande size, je vais ici supposer 
que ce sont des bytes = octets qui sont utilisés, comme pour ls -lh.

Avec size, les ordres de grandeur restent les mêmes.

Si on se penche davantage sur basicExe et staticLib, on peut voir une 
subtile différence.
Leur différence repose sur la partie "text". On voit qu'il y a 3925 octets pris
par le segment "text" pour staticLib, contre 4357 octets pris pour le segment 
"text" pour basicExe. Data et bss sont de taille identique ici.

Cette différence de taille s'explique par le fait que basicExe utilise une 
compilation et une édition de lien avec directement les .o, ce qui fait que
tous les symboles, même non-utilisés, sont tout de mêmes ajoutés au binaire 
exécutable final, alors que dans le cas de staticLib, qui utilise une
bibliothèque statique, seuls les symboles réellement utilisés sont ajoutés
au binaire exécutable.

### Code inclus ?

En utilisant la commande "nm tri_bubble-basicExe.exe", on remarque qu'il y a 
des lignes mentionnant bar et foo, c'est-à-dire des fonctions définies dans unused.c
Cela est normal, puisque nous incluons les .o directement pour produire basicExe, 
et donc tous les symboles seront présents dans l'exécutable final.

A contrario, "nm tri_bubble-staticLib.exe" ne laisse apparaître aucune ligne qui 
mentionne des symboles définis dans unused.c.
Cela s'explique par le fait qu'avec une bibliothèque statique, le comportement par 
défaut de l'éditeur de lien est de ne chercher que les symboles nécessaires à 
notre exécutable lors de la compilation et de l'édition de lien. Or les symboles
contenus dans unused.c (ou par conséquent dans unused.o) ne sont pas utilisés,
et donc ne sont pas ajoutés au binaire exécutable final.


## Exercice 5

J'obtiens bien une bibliothèque partagée d'extension .so, 
mais il y a un problème au moment de l'exécution, puisqu'il semble
que la bibliothèque partagée soit introuvable.
Je pense que c'est un problème lié à la variable LD_LIBRARY_PATH.

(Le problème est bien évidemment corrigé, j'en parle plus en détail 
dans la question suivante)

## Exercice 6

J'obtiens bien autant d'exécutables qu'il y a d'algorithmes de tri 
et de manières d'utiliser les bibliothèques.
Lors de l'exécution des programmes utilisant les bibliothèques 
partagées, j'ai toujours eu mon problème avec LD_LIBRARY_PATH.
Cependant, après l'avoir définie comme suit : 
"export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.", il se trouve que 
mon problème s'est réglé et que l'exécution se déroule parfaitement, 
les tris se faisant correctement.
Le problème étant qu'en ne définissant pas "mon" LD_LIBRARY_PATH, le
système allait chercher mes librairies partagées dans les repertoires /lib
du système, alors que celles dont j'ai besoin sont dans mon repertoire 
courant, qui n'est pas du tout appelé ainsi.
En ajoutant des repertoires à la liste des repertoires où le système
peut aller chercher des bibliothèques partagées, il va en fait inspecter 
en priorité ces repertoires qu'on ajoute.
Dans mon cas, ayant ajouté le repertoire courant, il va alors inspecter 
celui-là en priorité, entre autres.

On remarque par ailleurs que l'algorithme le plus efficace pour 
du tri de tableau d'entiers est ici le tri par insertion avec un temps 
moyen d'environ 1000 à 1100 nanosecondes selon la manière d'utiliser 
les bibliothèques.

# Remarque & Question : 

Je ne comprends pas comment il est possible de créer correctement les 
bibliothèques partagées même en n'utilisant pas l'option -fpic
lors de la compilation des fichiers .c (testé en local).
Est-ce juste par chance que tout fonctionne très bien ainsi ?
