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

Les différences que l'on peut noter entre la création de bibliothèques sous
windows et sous linux sont principalement la génération des fichiers utilisés pour produire un
exécutable et avoir un environnement d'exécution fonctionnel.

Côté Windows, on va plutôt trouver des .dll pour les bibliothèques partagées ou des .lib pour
les bibliothèques statiques, tandis que côté Linux, on va plutôt trouver des .a pour les 
bibliothèques statiques et des .so pour les bibliothèques partagées.

Aussi, le header Windows.h (et par conséquent l'API Win32) permet 
de charger et de décharger des bibliothèques partagées/dynamiques de façon légèrement 
différentes que sous Unix.

Notamment, on remarque que le cast est OBLIGATOIRE au niveau de GetProcAddress. Il faut le cast
en le type défini pour (l'adresse de) la fonction à récupérer. C'est une première différence
avec la version Unix.
Le reste semble marcher à peu près pareil que sur Unix.