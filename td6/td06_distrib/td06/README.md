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

Le processus fils hérite de la table des descripteurs de fichier du processus père.
Ainsi,

# Questions & Remarques :
