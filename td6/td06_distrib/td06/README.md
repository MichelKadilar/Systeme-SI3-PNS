# Exercices :

## Exercice 1

Chaque processus a sa table de descripteurs de fichier.
Les entrées et sorties standard ainsi que la sortie standard d'erreur sont des descripteurs
de fichier par défaut présents dans tous les processus.
Chaque processus possède donc par défaut une entrée standard, une sortie standard, et une
sortie d'erreur standard.
L'entrée standard est représentée par le descripteur de fichier "0", la sortie standard
est représentée par le descripteur de fichier "1", et la sortie standard d'erreur est
représentée par le descripteur de fichier "2".
Lorsqu'on ouvre un fichier, un nouveau descripteur de fichier est obtenu. Son numéro = dernier
numéro de la table + 1. Il permet d'identifier le fichier obtenu dans la table de descripteurs
du processus courant.

La commande "ls -l" envoie par défaut ses résultats sur la sortie standard (stdout = 1).
Notre but étant d'afficher le résultat/la sortie de "ls -l" dans le fichier foo, le but est
en fait de rediriger la sortie standard du processus vers notre fichier.
On peut aussi voir ça comme : "faire devenir foo la nouvelle sortie standard
de notre processus".

On ouvre alors le fichier nommé "foo" (avec open()) en écriture seule, on le crée
si nécessaire, et on écrase son contenu si on l'ouvre à nouveau par la suite. S_IWUSR,
qui n'est pris en compte que si le fichier est crée, indique que le créateur du fichier
redir_simple.c aura les mêmes droits d'écriture sur le fichier "foo" que ceux qu'il a sur
redir_simple.c.

Par la suite, on duplique fd (descripteur pointant vers notre fichier) sur la sortie standard
(descripteur de fichier : 1, qu'on appelle aussi stdout : "standard output" = "sortie standard").
On se retrouve donc à la fin de l'instruction dup2 avec deux descripteurs de fichier pointant
vers le même fichier "foo" : le descripteur de fichier "fd", et le descripteur de fichier "1".
Ce sont simplement des entiers, qui ici ont les mêmes valeurs (car ils pointent vers le même fichier).

La sortie standard du processus est désormais devenu notre fichier foo.
Si on essaie de printf("Hello World !\n") dans le processus à présent,
"Hello world !" serait écrit dans le fichier foo.
Puisque la sortie standard est devenue notre fichier foo, on n'a plus besoin du
descripteur de fichier vers foo initial : fd, on peut donc le fermer avec close(fd).
"fd" devient donc inutilisable : il prend la valeur -1, pour dire qu'il ne contient aucun
descripteur de fichier ~= pas de fichier ouvert stocké dans fd ~= fd ne pointe plus vers
rien.

Puisque foo est notre sortie standard, et que "ls -l" envoie ses résultats sur la sortie
standard...il suffit maintenant d'exécuter la commande "ls -l".

On utilise donc :

```c
execlp("ls", "ls", "-l", NULL);
```

On voit que rien ne s'affiche dans le terminal. C'est normal ! Car la sortie standard du
processus courant, c'est notre fichier foo. Afficher sur la sortie standard revient alors
à envoyer les données dans le fichier foo. On retrouvera alors dans foo la sortie de la
commande exécutée.

## Exercice 2

Après avoir déterminé si on veut écrire ou lire à travers le premier argument donné au
programme (R ou W), j'appelle une fonction dont la signature est la suivante :

```c
void redirect(int std_fd, char *filename, char *const args[])
```

* Le premier paramètre correspond au descripteur de fichier impacté par la duplication : entrée ou sortie standard,
  selon qu'on veuille respectivement lire depuis un fichier ou écrire dans un fichier.


* Le deuxième paramètre est le nom du fichier à ouvrir.


* Le troisième paramètre correspond à la commande UNIX à exécuter (cat, date, etc).

Dans la fonction redirect(), on ouvre donc le fichier ayant pour nom filename, soit en lecture
seule, soit en écriture seule avec création et don de permissions d'écriture pour le
créateur du fichier redir.c (donc vous) si nécessaire.

Avec cette ouverture, on récupère une valeur entière dans la variable fd. Cela correspond
au descripteur de fichier du fichier ouvert dans le processus courant.
Autrement dit, fd pointe vers le fichier ouvert précédemment.

### Dans le cas de la lecture depuis un fichier

On va utiliser :

```c
dup2(fd, STDIN_FILENO); // STDIN_FILENO = 0 -> entrée standard
```

Cette instruction permet de fermer (automatiquement) STDIN_FILENO, qui correspond à
l'entrée standard du processus courant (le clavier/le terminal) et de dupliquer le
pointeur de fichier de fd dans le pointeur de fichier de STDIN_FILENO.

Cela signifie donc que fd et STDIN_FILENO pointent désormais vers le même fichier : le
fichier qu'on a ouvert. Autrement dit, notre fichier ouvert est devenu l'entrée standard
de notre processus. Toute commande UNIX exécutée à l'intérieur du processus aura pour
données d'entrée le contenu du fichier ouvert.

On a redirigé l'entrée standard vers le fichier ouvert.

En effet, par défaut, le processus va toujours recevoir ses données depuis le descripteur de
fichier de numéro 0. Ici, 0 pointe désormais vers notre fichier. Le processus va donc recevoir
ses données depuis notre fichier.

Par la suite, on exécute :

```c
execvp(*args, args);
```

qui reçoit en premier argument le nom de la **commande UNIX** à exécuter et la commande UNIX
complète à exécuter.

#### Cas concret :

Sur l'exemple de : ```./redir.exe R /etc/passwd cat -n```.

On détecte une lecture depuis un fichier grâce au paramètre ```R```.

On identifie le fichier à ouvrir : ```/etc/passwd```

On identifie la commande UNIX à exécuter : ```cat -n```.

Voici les trois paramètres qu'on donne dans le même ordre à la fonction  :

```c
void redirect(int std_fd, char *filename, char *const args[])
```

Ici, ```char * const args[] = ["cat", "-n", NULL]```

Dans la fonction redirect(), on rentre dans le bloc concernant la lecture et
on fait une redirection de l'entrée standard vers le fichier ouvert.

Ensuite, les arguments reçus par ```execvp(*args, args);``` sont :

* ```*args``` correspond au premier élément (à la première chaîne de caractères) de args,
  c'est-à-dire "cat". Il s'agit du nom de la commande à exécuter, on aurait pu mettre n'importe quoi.


* ```args``` correspond donc à la commande complète (ici, simplement "cat -n"), stockée
  dans un tableau de chaînes de caractères. On remarque que le tableau finit par NULL.
  Ce NULL est mis par défaut après le dernier argument reçu par la fonction main().
  Cela nous arrange bien, car execvp demande justement à ce que ce second argument se termine
  par NULL.

Dès que execvp("cat", "cat -n") va s'exécuter, la commande "cat -n" va s'exécuter, or,
"cat" prend par défaut ses données depuis l'entrée standard. Ici, l'entrée standard étant
notre fichier ouvert il se trouve que "cat -n" prend directement en entrée le fichier ouvert.

Nous venons alors d'exécuter l'équivalent de : "cat -n < /etc/passwd".

### Dans le cas de l'écriture dans un fichier

On va utiliser :

```c
dup2(fd, STDOUT_FILENO); // STDOUT_FILENO = 1 -> sortie standard
```

Cette instruction permet de fermer (automatiquement) STDOUT_FILENO, qui correspond à
la sortie standard du processus courant (le terminal) et de dupliquer le
pointeur de fichier de fd dans le pointeur de fichier de STDOUT_FILENO.

Cela signifie donc que fd et STDOUT_FILENO pointent désormais vers le même fichier : le
fichier qu'on a ouvert. Autrement dit, notre fichier ouvert est devenu la sortie standard
de notre processus. Toute commande UNIX exécutée à l'intérieur du processus aura pour
sortie fichier ouvert. De même pour toutes les fonctions d'affichage comme printf, qui
n'afficheront plus dans le terminal, mais dans le fichier ouvert.

On a redirigé la sortie standard vers le fichier ouvert.

En effet, par défaut, le processus va toujours envoyer ses données vers le descripteur de
fichier numéro 1 = STDOUT = sortie standard. Ici, 1 pointe désormais vers notre fichier.
Le processus va donc envoyer dans notre fichier toutes les données qu'il était censé afficher
dans le terminal.

Par la suite, on exécute :

```c
execvp(*args, args);
```

qui reçoit en premier argument le nom de la **commande UNIX** à exécuter et la commande UNIX
complète à exécuter.

#### Cas concret :

Sur l'exemple de : ```./redir.exe W date.txt date```.

On détecte une écriture dans un fichier grâce au paramètre ```W```.

On identifie le fichier à ouvrir : ```date.txt```

On identifie la commande UNIX à exécuter : ```date```.

Voici les trois paramètres qu'on donne dans le même ordre à la fonction :

```c
void redirect(int std_fd, char *filename, char *const args[])
```

Ici, ```char * const args[] = ["date", NULL]```

Dans la fonction redirect(), on rentre dans le bloc concernant l'écriture et
on fait une redirection de la sortie standard vers le fichier ouvert.

Ensuite, les arguments reçus par ```execvp(*args, args);``` sont :

* ```*args``` correspond au premier élément (à la première chaîne de caractères) de args,
  c'est-à-dire "date". Il s'agit du nom de la commande à exécuter, on aurait pu mettre
  n'importe quoi.


* ```args``` correspond donc à la commande complète (ici, simplement "date"), stockée dans
  un tableau de chaînes de caractères. On remarque que le tableau finit par NULL. Ce NULL
  est mis par défaut après le dernier argument reçu par la fonction main(). Cela nous
  arrange bien, car execvp demande justement à ce que ce second argument se termine par NULL.

Dès que execvp("date", "date") va s'exécuter, la commande "date" va s'exécuter, or,
"date" sort par défaut ses résultats sur la sortie standard. Ici, la sortie standard étant
notre fichier ouvert, il se trouve que "date" envoie directement ses données dans le fichier
ouvert.

Nous venons alors d'exécuter l'équivalent de : "date > date.txt" ou encore "date >> date.txt",
cela dépends du mode d'ouverture du fichier date.txt :

```c
fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,S_IWUSR); // pour "date > date.txt"
```

```c
fd = open(filename, O_WRONLY | O_CREAT | O_APPEND,S_IWUSR); // pour "date >> date.txt"
```

Dans le premier cas, O_TRUNC permet d'écraser le contenu du fichier dès la première
écriture dans le fichier après cette ouverture.

Dans le deuxième cas, O_APPEND permet d'ajouter à la suite du contenu du fichier
dès la première écriture dans le fichier après cette ouverture.

## Exercice 3

Dans cet exercice, j'ai fait exprès de me faire avoir, afin de pouvoir étudier plus en détail
le comportement du code.

On commence par crée un tableau d'entier de taille 2. Cela nous permettra de stocker deux
descripteurs de fichier : un descripteur de fichier d'écriture, et un descripteur de fichier
de lecture.

### Mais à quel(s) fichier(s) sont-ils liés ?

Ils sont en fait liés ici à un tube/pipe. Il s'agit d'un canal de communication unidirectionnel
entre deux processus (les deux processus pouvant en fait être le même).

--- 

### ...Unidirectionnel ?

Les tubes sont en général Unidirectionnels puisque la communication se fait d'un point
A à un point B. Il y a un seul écrivain et il peut y avoir N lecteurs.
Les données n'empruntent malgré tout toujours qu'un seul chemin (écrivain -> lecteur k).
Voilà pourquoi on parle de communication unilatérale.

---

Ici, on utilise la fonction :

```c
int pipe(int pipefd[2]); 
```

Cette fonction permet tout simplement de créer un tube (un canal de communication unidirectionnel)
et d'assigner pipefd[0] en tant que sortie du tube (lecteur) et pipefd[1] en tant qu'entrée
du tube (écrivain).
pipefd[0] et pipefd[1] sont des descripteurs de fichier que l'on peut retrouver dans la
table de descripteurs de fichier du processus courant.

### Comportements spéciaux

Si le tube est plein, alors l'écrivain (fd[1]) sera automatiquement bloqué et ne pourra
plus écrire dans le tube jusqu'à ce qu'assez de place se libère.

Si le tube est vide, alors le lecteur (fd[0]) sera automatiquement bloqué et ne pourra
plus lire depuis le tube jusqu'à ce qu'assez de données soient écrites dans le tube.

Si le lecteur (fd[0]) est fermé avant l'écrivain (fd[1]), alors l'écrivain ne va
rien écrire dans le tube et on passe à l'instruction suivante dans le programme.

Si l'écrivain (fd[1]) est fermé avant le lecteur (fd[0]), alors le lecteur va lire les octets
restants à lire depuis le tube, puis va renvoyer un 0 qui indique que read() n'a rien lu durant
sa dernière itération. **Il ne restera donc pas bloqué comme dans le cas où le tube est vide**

---

Par la suite dans le programme, on écrit dans le fichier qui est pointé par le descripteur
de fichier fd[1], c'est-à-dire dans l'entrée du tube.
La chaîne de caractères "0123456789" est donc envoyée dans le tube avec la fonction write(),
dans le canal de communication, et reste dans le tube puisqu'il y a toujours un lecteur
(et un écrivain, même si ce n'est plus nécessaire ici) qui sont disponibles pour ce tube.

Juste après, on ferme le descripteur de fichier pointant vers l'entrée du tube (l'écrivain, fd[1]).
Juste après, on appelle la fonction lecture() qui va lire un à un les caractères écrits dans
le tube. On lit évidemment ce qu'il y a dans le tube grâce au descripteur de fichier fd[0],
la sortie du tube, le lecteur.

Si on n'avait pas fermé l'écrivain auparavant, ici read() serait resté bloqué après avoir lu les
données écrites dans le tube, car un écrivain aurait toujours été disponible, alors après avoir
renvoyé un nombre != 0 pour avoir lu nos octets écrits, il se serait bloqué, en attente d'une écriture
de la part de l'écrivain du tube (fd[1]).

Puis on ferme le descripteur de fichier pointant vers la sortie du tube (le lecteur, fd[0]).

## Exercice 4

Le processus fils hérite de la table des descripteurs de fichier du processus père.
La table des descripteurs de fichier est **dupliquée**. Modifier les descripteurs du fils
ou du père après un fork() n'auront pas d'impact pour sur la table des descripteurs de
fichier de l'autre processus.

Le comportement reste ici globalement le même que précédemment, le gros changement est qu'ici,
les descripteurs de fichier étant dupliqués entre le père et le fils, nous avons après le fork() 2 écrivains
et 2 lecteurs.

Puisque les descripteurs de fichier sur le tube sont dupliqués, il faut bien
penser à fermer les descripteurs aux bons endroits afin de ne pas bloquer le processus courant
ou l'autre processus inutilement.

Ici, le fils ne va qu'écrire dans le tube, alors on peut fermer la lecture du tube dans le
fils (fd[1]), sinon, cela signifierait que le fils peut écrire...et lire ce qu'il a écrit
dans le tube.

---

**Cf. Ma question à la fin de ce document : Exercice 4**

En soi, cela ne pose aucun problème (ou pas...vu ma question) **dans notre cas**,
mais puisque ces disponibilités de lecteurs/écrivains peuvent avoir des conséquences
sur le comportement de certaines fonctions telles que read() ou write(), une bonne
pratique est d'avoir la main sur les ouvertures et fermetures de fichier (de descripteurs
de fichiers), et donc de les fermer aux bons endroits.

---

Ce que nous sommes également censés remarquer, c'est que l'appel à read() (dans la fonction
de lecture) est bloquant dans le fils tant qu'un écrivain est toujours disponible.
Cela avait donc pour but de nous faire fermer l'un des deux écrivains (puisque l'écrivain
initial est dupliqué en raison du fork()) au début du fils, et de voir que read() restait
bloqué tant que l'écrivain n'est pas aussi fermé dans le père.

En effet, ce que l'on peut observer, c'est que le père écrit sa chaine de caractère dans le tube,
le fils lit les caractères, puis puisque l'écrivain n'est toujours pas fermé dans le père
(en raison du sleep() en cours d'exécution), la fonction read(), sur laquelle nous revenons
en raison d'une boucle, se bloque et ne renvoie rien pour le moment.
En effet, on a vu dans l'exercice 3 que lorsqu'il y a toujours un écrivain disponible
sur le tube, l'appel a read() est bloquant, alors dans notre cas, read() reste bloqué et ne
renvoie rien pour le moment.
Puis le sleep(2) passe et l'écrivain est fermé (au moins à la fin du processus, sinon
manuellement avec close(fd[1])).

On remarque que le fait que nous ayons eu 2 écrivains et 2 lecteurs ici a rendu le tube
possiblement "bidirectionnel" (père → fils et fils → père étaient possibles si on n'avait
pas fermé les descripteurs de fichiers "inutiles" pour l'exercice).
**Cependant, on ne parle sur Linux que de tubes unidirectionnels, car au final, le tube est
toujours utilisé de la même manière : on écrit en fd[1], et on lit en fd[0], que ce soit dans le fils,
le père...**, donc les données du tube vont toujours dans un seul et même sens.

## Exercice 5

Rien de particulier à souligner, on voit bien les affichages des fils s'entremêlent et
que les éléments sont lus dans leur ordre d'écriture dans le tube, et qu'ainsi ce tube
est bien une file FIFO.

## Exercice 6

### Question 1

En utilisant "ls -l" dans le répertoire /tmp, on remarque que notre tube nommé a un type
"p" (premier caractère/octet des permissions), alors que les fichiers ordinaires ont le type
"-" et que les répertoires ont le type "d". Cela signifie donc que les tubes nommés sont clairement
distingués des autres types de fichiers, que les tubes nommés sont donc des fichiers "spéciaux".

### Question 2

Pour que chaque processus soit ici écrivain et lecteur, il faudrait créer un second tube nommé dans
lequel les rôles des processus sont inversés (dans le nouveau tube nommé, l'écrivain devient
le lecteur et le lecteur devient l'écrivain).

### Question 3

Pour tester le fait d'avoir plusieurs lecteurs, il suffit de lancer sur différents terminaux
lecteur.exe.
Cependant, on peut d'ores et déjà dire qu'il va y avoir un problème lors de la lecture des données
qui sont dans le tube, car un tube dans notre cas est FIFO, et qu'un élément lu est un élément
retiré du tube, qui ne sera donc plus disponible pour les autres.
A partir de cela, plusieurs scénarios sont possibles :


* Un lecteur va tout lire de manière "atomique"

* Plusieurs lecteurs vont s'intercouper au cours de leurs lectures, chacun lisant un bout
des données écrites dans le tube.

En l'occurrence, il se trouve que c'est le deuxième scénario qui se réalise sur ma machine.
J'ai créé un écrivain et deux lecteurs sur le tube nommé "my_named_pipe", et je remarque que
certains caractères sont lus par le premier lecteur, d'autres par le deuxième, dans le désordre.
Le premier lecteur qui obtient un caractère rend ce même caractère indisponible pour 
l'autre lecteur.

Exemple :

Pour la chaîne de caractères d'entrée : "coucou je m'appelle michel et je suis sympa",
le premier lecteur va lire : "ce m'ppelle mchl t e ui smp", tandis que le deuxième va lire :
"oucou jaieejssya". On voit qu'aucun caractère lu par un lecteur n'est également lu par 
l'autre lecteur. C'est parce que cette donnée lue est devenue indisponible (retirée du tube).

## Exercice 7

Par manque de temps, je résume grossièrement :
1 - On définit une fonction mypopen qui crée un tube anonyme, fait un fork, 
dans le père va attendre la fin du fils, puis dans le fils va lire les arguments passés 
en paramètre de notre exécutable et va les formatter afin de
créer une chaîne de caractères composée de tous les arguments séparés par des espaces.
La suite de cette fonction va désigner l'entrée du tube comme étant la sortie standard
du processus courant, puis va lancer un shell et va lui demander d'interprêter la commande
que nous avons créée précédemment par concaténation grâce à :
```c
 execlp("/bin/sh", "sh", "-c", la_commande, NULL);
```
Qui permet d'exécuter "la_commande" dans un shell.

Le père reprend alors la main et renvoie le descripteur de fichier pointant vers
la sortie du tube.

2 - le processus père lit la sortie du tube (càd le résultat de l'exécution de la commande
exécutée dans execlp).

# Questions & Remarques :

## Exercice 4

Si je ne ferme pas l'écrivain au niveau du fils, comment se fait-il que la lecture ne soit
pas bloquée ?

## Exercice 6

### Question 2

Si on fork() dans l'écrivain et le lecteur et qu'on ouvre en lecture dans le fils de 
chacun et en écriture dans le père de chacun, n'aurait-on pas une communication 
bidirectionnelle ?
