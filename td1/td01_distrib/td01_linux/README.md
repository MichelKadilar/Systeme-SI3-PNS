# Partie Linux

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
1 kilooctet. Cela reste tout de même important à l'échelle d'un langage
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

## Exercice 7 :

Tout d'abord, j'ai crée load_library.h qui contient
la signature de la fonction load_library.
J'ai inclus ce header dans load_library.c et j'ai défini un
corps pour la fonction load_library.

Dans le fichier source load_library.c, j'ai une variable statique
me permettant d'avoir une "variable globale" dans le fichier
load_library.c (équivalent d'un attribut privé en PROG OBJET).
Cette variable est un pointeur de fonction qui a la même signature
que la fonction "sort".

Dans la fonction load_library je charge une bibliothèque, j'en extrais
la fonction "sort" dont on a besoin, et j'affecte à la variable
statique l'adresse de la fonction "sort" que nous avons obtenu depuis
la bibliothèque.

On voit que j'ai également défini une fonction "sort" ayant la même
signature que dans sort.h, mais ayant un corps assez simple :
cette fonction permet tout simplement de faire un appel à notre variable
statique, qui dans les faits va faire appel à/exécuter
la fonction de tri contenue dans la bibliothèque que nous avons
donné en argument de tri.exe à l'exécution.
Par exemple, si on donne libTri_quick-dynamicLib.so (bibliothèque
partagée/dynamique contenant l'algorithme du tri rapide), alors
la fonction "sort" définie dans load_library.c va faire appel
à l'algorithme de tri rapide contenu dans cette bibliothèque.

Dans main_dynload.c, je n'ai pas changé grand chose.
J'ai voulu aller au plus simple, j'ai donc simplement inclus le
header load_library.h.

Cela me permet d'avoir accès à la signature de la fonction
load_library. J'y fais alors appel dans la fonction "main" principale.
En paramètre, je lui donne l'argument "argc-1", ce qui signifie le
dernier argument disponible lors de l'exécution.
Dans les faits, je passerai le nom de mes bibliothèques partagées
en dernier argument, par exemple : libTri_quick-dynamicLib.so

Maintenant, on peut se demander comment se fait la jointure entre
la fonction "sort" appelée dans "do_job" dans le fichier
main_dynload.c et la fonction "sort" définie dans load_library.c
(qui je le rappelle fait elle-même appel à une autre fonction de tri
définie dans une bibliothèque donnée en argument).

En fait, cela se fait grâce à la signature.
La fonction "sort" définie dans load_library.c étant la seule fonction
ayant une signature correspondant aux attentes de la fonction
"sort" utilisée dans main_dynload.c, alors c'est à cette fonction
qu'on fait appel.
Car en effet, lorsqu'on produit notre exécutable tri.exe,
il a comme prérequis, entre autres, "main_dynload.o" et
"load_library.o".
Ainsi, lors de la production de notre exécutable, lors
de l'édition de liens, l'éditeur de lien va lier la fonction "sort"
utilisée dans main_dynload.o à la fonction "sort" définie dans
load_library.o.

Concernant le makefile, j'y ai d'abord ajouté une variable :
DYNLIB=$(wildcard *.so), qui est une variable pour récupérer les
noms de tous les .so (bibliothèques partagées/dynamiques)
du dossier courant.
Puis j'ai défini une nouvelle règle tout en bas du makefile : testdl,
qui signifie "test dynamic load".
Cette règle/cible n'a besoin que de tri.exe pour fonctionner, alors
je lui ai donné la variable "DYNAMIC_LOAD" qui contient simplement
"tri.exe".
Concernant le cas de test en lui-même, il parcourt simplement les
résultats de la variable "DYNLIB" (contenant les .so du dossier courant),
exécute tri.exe avec comme paramètres -v, -s, 10, et en dernier argument
la bibliothèque "courante" (du parcours).
Les affichages sont les mêmes que le test précédent
défini par le professeur.

## Exercice 8

Pour ajouter le nouvel algorithme de tri "Shell sort", je n'aurais pas
besoin de recompiler tous les fichiers précédents.
J'aurai simplement besoin de compiler le fichier source de l'implémentation
de cet algorithme, par exemple "shell.c" et de produire une bibliothèque
partagée/dynamique contenant cet algorithme de tri.
Par la suite, je pourrai donner la bibliothèque ainsi créée,
par exemple "libTri_shell-dynamicLib.so" en dernier argument de
tri.exe afin de trier la liste de nombres avec le Shell sort.
Il faudra simplement que shell.c respecte la même interface que les autres
algorithmes de tri, à savoir : avoir une fonction "sort" retournant
un void et prenant en paramètre un tableau d'entiers et la taille de
ce tableau d'entiers.
Autrement dit, il faudrait que shell.c inclus "sort.h", qui défini
la signature de la fonction "sort" à implémenter.

## Exercice 9

J'ai ajouté une fonction unload_library au fichier load_library.h
(et donc au fichier source également).
Sa signature est que unload_library renvoie un void et prend en
paramètre un void.
Son implémentation dans load_library.c est constituée de 2 étapes :
"si une bibliothèque est chargée, alors la décharger".
C'est ainsi que je la décharge avec dlclose sur le pointeur qui
était renvoyé par dlopen.

Pour faire cela, j'ai donc dû passer une variable "ref" en statique,
de façon à pouvoir changer sa valeur et y accéder dans les différentes
fonctions de load_library.c

Cette variable sert notamment afin de pouvoir fermer/décharger
avec dlclose une bibliothèque précédemment ouverte avec dlopen,
grâce à sa référence/son adresse.

## Exercice 10

Python3 utilise les bibliothèques partagées suivantes :
linux-vdso, libc, libpthread, libdl, libutil, libm, libexpat et libz.

Java utilise les bibliothèques partagées suivantes :
linux-vdso, libc, libpthread, libdl, libjli et libz.

On peut donc déjà observer que les deux langages ont des
bibliothèques partagées en commun.
Java et Python3 vont alors "piocher" du code au "même endroit" pour
certaines de leurs opérations.

Ce qu'on peut surtout remarquer, c'est que Java et Python ont tous deux
en commun des bibliothèques comme la libc, qui sont des bibliothèques
partagées contenant du code...C.
Java et Python3 font donc appel à du code écrit en C afin de réaliser
certaines de leurs opérations. Et surtout, ce code est dans une
bibliothèque partagée, ce qui signifie qu'un programme écrit en C peut
potentiellement se servir des mêmes bibliothèques partagées.

En réalité...le code dans Java et Python3 qui fait appel à des fonctions
dans ces bibliothèques partagées...ne serait-il pas écrit en C ?

Autrement dit, Java et Python3 n'auraient-ils pas une partie d'eux
qui est écrit en C ? Sûrement que oui.

# Remarque & Question :

Je ne comprends pas comment il est possible de créer correctement les
bibliothèques partagées même en n'utilisant pas l'option -fpic
lors de la compilation des fichiers .c (testé en local).
Est-ce juste par chance que tout fonctionne très bien ainsi ?

Réponse à la dernière séance : Peut-être que -fpic se trouve par
défaut dans les options dans les versions récentes de gcc.
Je l'ai tout de même ajoutée aux options de compilation dans le
makefile, à la fin de CFLAGS.

# Partie Windows

## Exercice 2

On voit que les fichiers qui ont été produits par la compilation 
sont les suivants : Exe_DynLink.ilk, vc143.pdb, vc143.idb, main.obj,
et un dossier contenant uniquement des fichiers d'extension ".tlog".

Le seul fichier avec lequel nous sommes familiers serait à limite "main.obj"
qui semble se rapprocher (voire être identique) à un fichier "main.o".

D'après quelques recherches : 
- le fichier d'extension .ilk serait ce que l'éditeur de lien prend 
en entrée.
- le fichier d'extension .pdb serait le fichier contenant la base de données
du programme qui a été crée par l'éditeur de lien (j'avoue avoir un peu 
cherché et ne pas avoir trop compris à quoi ceci correspond...). Ce
fichier contiendrait des informations concernant l'état du projet
aidant le débogueur Visual Studio.
- le fichier d'extension .idb serait le "Intermediate debug file" généré
par Visual Studio...Pas trop compris les détails ni la différence
avec le fichier .pdb, mais ça a l'air d'aider le débogueur également.
- Pour les fichiers .tlog...quoi de mieux qu'une citation du site de 
Microsoft : "Les fichiers .tlog sont utilisés par les builds ultérieures
pour vérifier ce qui a changé et doit être reconstruit et ce qui est 
à jour. Les fichiers .tlog sont également la seule source 
pour l’archivage à jour de la build par défaut dans l’IDE".
Donc si je comprends bien, ça permet de savoir ce qui a besoin d'être 
recompilé (un peu comme un makefile).

En supprimant tous les autres fichiers à part Exe_DynLink.exe et en
l'exécutant en mode debug (seule option d'exécution que j'ai pu trouver sans 
tout régénérer), on voit que le programme s'exécute très bien, 
comme prévu.

## Exercice 3

Suite à quelques tests avec des suppressions manuelles et à tâtons,
on remarque que le fichier qu'il est essentiel de conserver est :
"Bibliotheques.dll", autrement dit : la bibliothèque partagée générée contenant
la fonction "PrintStop" définie dans printStop.c.
Cela paraît logique, puisque le code n'est pas copié dans l'exécutable.
Lors de l'exécution, le code doit alors être cherché dans la bibliothèque
partagée. Si on supprime cette bibliothèque partagée, 
le code sera introuvable et le programme prendra fin avec 
un message d'erreur.

## Exercice 4

J'ai, avec les fonctions de l'API Win32, reproduit à peu près le code 
présent dans le fichier load_library.c, côté Unix.

Dans le main.c, j'ai dû définir un nouveau type de pointeur sur fonction :
"typedef void(\*function)(char\*);", ayant la même signature que la fonction
PrintStop définie dans la bibliothèque partagée.

Ensuite, le code est très comparable à ce que j'ai côté Unix :
je définis une variable statique du type défini précédemment 
(pointeur de fonction) : static function func.
Par la suite, je charge la bibliothèque partagée :
LoadLibrary(TEXT("C:/Users/Michel K/source/repos/Bibliotheques/Debug/Bibliotheques.dll")).
On voit une conversion : "TEXT(...)", cela permet d'adapter le format de ma chaîne, afin
qu'elle soit en Unicode. Cela est nécessaire car c'est uniquement ainsi
que Visual Studio comprend les chemins (on aurait pu mettre L"monChemin",
mais je préfère passer par une version "code plus propre").

La récupération de la fonction "PrintStop" dans la bibliothèque se fait alors ainsi :
func = (function) GetProcAddress(ref, "PrintStop").
On remarquera que le cast est nécessaire. Si on ne cast pas, la 
compilation échoue.
On récupère alors dans la variable func (qui est un pointeur de fonction)
l'adresse de "PrintStop" dans la bibliothèque partagée.
On peut alors utiliser la fonction PrintStop définie dans la bibliothèque 
partagée en utilisant "func" : func("Test dynamic load").
Cela permet donc de faire un appel à "PrintStop" en lui donnant comme
paramètre : "Test dynamic load".

Ce que je trouve dommage, c'est qu'il semble un peu compliqué de
faire un code "flexible" avec une fonction "main" qui prendrait
des arguments et qui pourrait charger la bonne librairie ainsi.

## Exercice 5


