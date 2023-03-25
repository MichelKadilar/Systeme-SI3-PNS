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

Tout d'abord, il est important de noter que les répertoires sont des fichiers.
Ce qui diffère entre un "vrai" fichier et un répertoire, c'est le type du fichier,
qui est une information stockée dans différentes structures telles que
"struct dirent".

Lorsqu'on ouvre un repertoire (en l'occurrence cela va concerner les repertoires,
mais il n'y a aucune différence entre un vrai fichier et un "repertoire" (qui est
un fichier) hormis le type du fichier), une pile stockant tous les parcours de son
contenu sera créée.
Les parcours se font à l'aide d'appels successifs à readdir, qui permet de récupérer
le "prochain" contenu du répertoire. Il faut donc faire N appels à readdir afin de
parcourir les N contenus d'un répertoire.
Pour revenir à un état "non-parcouru"/initial, on peut utiliser
la fonction rewinddir.
Si on réutilise de nouveau "readdir" juste après un "rewinddir", alors le prochain
contenu récupéré sera bel et bien le premier contenu du répertoire.

J'ai donc défini différentes fonctions afin de reproduire le comportement de
"ls -laR" :

- La fonction list, qui permet de faire la boucle principale. On prend en entrée
  le fichier point de départ (qui peut donc être un répertoire), s'il s'agit d'un
  fichier classique, on affiche ses informations à l'aide de la fonction print_fileinfo,
  sinon, on parcourt le contenu du fichier avec readdir (il s'agit donc à ce moment
  là d'un répertoire), et on affiche les informations de chacun des fichiers contenus
  à l'intérieur avec print_fileinfo. Suite à cela, nous revenons à l'état initial de parcours
  du contenu du répertoire avec rewinddir. Suite à cela, on reparcourt une nouvelle fois
  tout le contenu du répertoire, mais cette fois-ci, dès qu'on rencontre un répertoire,
  on fait un appel récursif à la fonction list afin de faire le même traitement
  sur le répertoire trouvé (afficher ses fichiers, puis encore une fois entrer dans chacun
  des répertoires de ce répertoire).
  On n'oublie pas de closedir à la fin de notre traitement, bien que cet appel soit faita
  automatiquement à la fin du thread principal.


- La fonction get_total_size, qui permet sur le même principe que la fonction list,
  de parcourir l'ensemble des fichiers à partir d'un répertoire donné.
  Seulement, dans cette fonction, le but est d'afficher la taille totale occupée
  par le répertoire courant (et donc par ses sous-fichiers également) en octets.


- La fonction count_files_and_subfiles, qui permet sur le même principe de compter
  le nombre de fichiers et sous-fichiers d'un répertoire.


- La fonction print_fileinfo, qui permet d'afficher les informations sur un fichier.
  Parmi ces informations, on retrouve :


* Les permissions sur ce fichier


* Le nombre de liens du fichier (liens physiques et symboliques)


* Le propriétaire du fichier


* Le groupe du fichier =
    * Le groupe du propriétaire
    * OU
    * Le groupe configuré par défaut sur le système


* La taille totale du fichier en octets

* La date de dernier changement d'état du fichier

* Nom du fichier

**La fonction suivante n'était pas demandée**

Toutes ces informations sont retrouvées grâce à la structure stat et à des fonctions
définies afin de récupérer des informations sous certains formats.
J'ai notamment défini les fonctions : get_permissions, get_file_owner_username,
get_file_groupname et get_date.

Voyons-les plus en détail :

#### get_permissions

```c
char *get_permissions(struct stat buf)
```

Cette fonction prend en paramètre une structure stat et renvoie une chaîne de caractères
contenant les permissions sur ce fichier.
Les vérifications des permissions se font à l'aide de l'attribut "st_mode" de la
structure stat et grâce à des constantes définies dans stat.h.
Un "&" logique entre deux valeurs (st_mode et une constante) permet de détecter
une permission. Il faut donc en faire autant de fois qu'il y a de permissions possibles.
Si jamais une permission est accordée en lecture, on ajoute un caractère 'r'
dans notre chaîne retournée. Si une permission est accordée en écriture : 'w',
en exécution : 'x'. Et si la permission n'est pas accordée : '-'.

#### get_file_owner_username

```c
char *get_file_owner_username(short uid)
```

Cette fonction prend en paramètre l'id d'un utilisateur, en l'occurrence celui du
propriétaire du fichier sur lequel on veut des informations.
En effet, la structure stat nous permet d'accéder à l'id de cet utilisateur : uid.
Cependant, cela ne suffit pas, "ls -laR" affiche le nom de l'utilisateur.
Il faut alors trouver son nom à partir de son id.

Pour cela, nous utilisons :

```c
struct passwd *pws = getpwuid(uid);
```

qui permet de récupérer dans une structure

```c 
struct passwd
```

des informations sur un utilisateur à partir de son ID système.
Nous récupérons depuis cette structure le nom de l'utilisateur :

```c
return pws->pw_name;
```

#### get_file_groupname

Même principe que précédemment :

```c
char *get_file_groupname(short uid) {
    struct group *grp = getgrgid(uid);
    return grp->gr_name;
}
```

#### get_date

```c
char *get_date(time_t time)
```

Il s'agit simplement ici d'une transformation du format d'une date.

Là où stat nous renvoie une date dans le format "nombre de secondes écoulées
depuis la création d'UNIX", "ls -laR" affiche la date de création du fichier
sous un format lisible par l'humain sous la forme : "Mois Num_Jour heure:minute",
nous avons alors besoin de transformer le premier format de date en deuxième format.

Pour cela, on utilise notamment :

```c
struct tm *timeinfo = localtime(&time);
strftime(buff, 20, "%b %d %H:%M", timeinfo);
```

où %b est le nom raccourci du mois, %d le numéro du jour, %H l'heure et %M les minutes.

### Point sur les structures utilisées

#### struct stat

```c
struct stat {
    dev_t     st_dev;      /* ID du périphérique contenant le fichier */
    ino_t     st_ino;      /* Numéro inœud */
    mode_t    st_mode;     /* Protection */
    nlink_t   st_nlink;    /* Nb liens matériels */
    uid_t     st_uid;      /* UID propriétaire */
    gid_t     st_gid;      /* GID propriétaire */
    dev_t     st_rdev;     /* ID périphérique (si fichier spécial) */
    off_t     st_size;     /* Taille totale en octets */
    blksize_t st_blksize;  /* Taille de bloc pour E/S */
    blkcnt_t  st_blocks;   /* Nombre de blocs alloués */
    time_t    st_atime;    /* Heure dernier accès */
    time_t    st_mtime;    /* Heure dernière modification */
    time_t    st_ctime;    /* Heure dernier changement état */
};
```

#### struct dirent

```c
struct dirent {
    long d_ino; /* Always zero */
    
    long d_off; /* File position within stream */
    
    unsigned short d_reclen; /* Structure size */
    
    size_t d_namlen; /* Length of name without \0 */
    
    int d_type; /* File type */
    
    char d_name[PATH_MAX+1]; /* File name */
};
```

#### DIR

```c
struct DIR {
    struct dirent ent;
    struct _WDIR *wdirp;
};
```

On voit que la structure DIR contient la structure "struct dirent" vue précédemment.
**Cependant, il n'est pas recommandé d'accéder directement à ces attributs.**

## Exercice 3

J'ai ajouté deux consignes à mon sujet :

* On peut créer un fichier dans un dossier s'il n'existe pas.
  Le fichier destination sera alors créé avec le même nom que le fichier d'origine.

* On peut copier un dossier dans un autre dossier (mais les sous-dossiers ne sont
  pas copiés).

Dans mon code, deux principales fonctions sont utilisées :

```c
void copy_file_in_file(char *src_filepath, char *dst_filepath);
void copy_folder_in_folder(char *src_dirpath, char *dst_dirpath);
```

### Copier un fichier dans un autre fichier

**Si le fichier de destination n'existe pas, on le crée, sinon,
on écrase le contenu du fichier de destination**

On utilise la fonction suivante :

```c
void copy_file_in_file(char *src_filepath, char *dst_filepath);
```

Dans cette fonction, les éléments importants sont :

#### La fonction open() :

Elle a pour signature :

```c
open(const char *pathname, int flags, mode_t mode)
```

* Le premier paramètre correspond au chemin du fichier à ouvrir.


* Le deuxième paramètre correspond aux drapeaux, c'est-à-dire d'ouvrir le fichier
  avec certaines propriétés. **Il est possible de les cumuler grâce au caractère "|"
  entre chaque propriété.<br>
  Voici quelques exemples de propriété :
    * **O_RDONLY** pour ouvrir le fichier en lecture seule. On ne pourra que le lire.
    * **O_WRONLY** pour ouvrir le fichier en écriture seule. On ne pourra qu'écrire.
    * **O_RDWR** pour ouvrir le fichier en lecture et en écriture.
    * **O_CREAT** pour créer le fichier à l'emplacement indiqué par le premier paramètre
      pathname s'il n'existe pas.
    * **O_TRUNC** pour écraser le contenu du fichier à ouvrir s'il existe déjà.
    * Il existe d'autres drapeaux possibles.
    * Pour les combiner, il suffit de faire par exemple : "O_WRONLY | O_CREAT" afin
      d'ouvrir le fichier en écriture seule et de le créer s'il n'existe pas.


* Le troisième paramètre correspond aux modes, c'est-à-dire aux permissions qui vont
  s'appliquer si nécessaire (par exemple dans le cas où on crée un fichier grâce au flag
  O_CREAT). Le mode ne s'applique que si l'on donne O_CREAT ou O_TMPFILE dans
  les drapeaux. Les modes permettent notamment de donner des permissions à un
  fichier qui sera crée.<br>
  Voici quelques exemples de modes :
    * S_IRWXU, pour que le propriétaire du fichier crée ait les droits de lecture,
      d'écriture et d'exécution sur le fichier crée.
    * S_IRUSR, pour que le propriétaire du fichier crée ait les droits en lecture sur
      le fichier crée.
    * S_IWUSR, pour que le propriétaire du fichier crée ait les droits en écriture sur
      le fichier crée.
    * S_IXUSR, pour que le propriétaire du fichier crée ait les droits en exécution sur
      le fichier crée.
    * S_IWGRP, pour que le groupe auquel appartienne le fichier ait les droits
      d'écriture sur le fichier crée (c'est le mode que j'utilise dans mon code source).
    * etc.
    * Encore une fois, il est possible de les combiner avec des barres verticales : "|".

La fonction open() renvoie un descripteur de fichier qui pointe vers le fichier ouvert,
ou -1 si l'opération échoue.

#### La fonction read()

Elle a pour signature :

```c
ssize_t read(int fd, void *buf, size_t count);
```

Cette fonction s'utilise après avoir utilisé la fonction open() vue précédemment,
car read prend un descripteur de fichier et non pas le chemin du fichier pour identifier
le fichier à lire.

Lorsque read() lit un caractère, la fonction décale la position du pointeur de caractère d'une position.
Cela paraît logique : lorsqu'on lit un caractère, le prochain caractère
qu'on veut lire est le caractère suivant, il faut donc se placer
au niveau/à la position du caractère suivant.

Lorsque read() lit 847 caractères, il déplace donc son curseur de caractère 847 positions
plus loin dans le fichier.

* Le premier paramètre correspond au descripteur de fichier du fichier ouvert auparavant
  par open().

* Le deuxième paramètre correspond au buffer/tampon qui va stocker les caractères lus
  depuis un fichier.

* Le troisième paramètre correspond au nombre d'octets qu'on veut lire dans un fichier.
  Comme un caractère = 1 octet, on peut réduire dans notre cas ce paramètre à : "nombre
  de caractères à lire depuis un fichier".

La fonction read() renvoie le nombre d'octets réellement lus.
Ce nombre peut être différent du nombre d'octets qu'on veut lire. En effet, si
on est arrivé à la fin du fichier, il se peut que nous n'ayons pas concrètement lus
N octets, mais N - 10 par exemple (Si après il n'y a rien à lire...on arrête de lire).

Une pratique qui est utilisée afin de lire tout le contenu d'un fichier sans trop
se soucier du nombre d'octets à lire qu'on indique en 3eme paramètre est de réaliser
une boucle de ce type :

```c
while (read(fd_file1, buf, BUFFER_SIZE) > 0) { // tant qu'on lit des caractères
    // Faire quelque chose
    }
```

En effet, puisque read() renvoie le nombre d'octets lus et qu'il fait avancer le curseur
de caractère dans le fichier pointé par le descripteur de fichier : fd_file1,
il est possible de boucler sur la fonction read() tant qu'il lit au moins 1 caractère,
car cela signifie que le fichier n'est pas terminé et qu'il reste encore au moins
1 caractère à lire. Pour cela, on teste simplement si la valeur renvoyée par read()
est supérieure à 0. Autrement dit, on teste si read() lit plus que 0 caractère, donc
qu'il lit toujours au moins un caractère.

Il faut également savoir que read() écrase les données qui sont à l'intérieur du
buffer qui lui est donné en deuxième paramètre.
Mais il **ne vide pas** le buffer. Donc lors de (et même après) la dernière itération
dans la boucle, le buffer contiendra toujours les N derniers caractères lus.

Si le nombre d'octets à lire indiqué en 3eme paramètre est plus petit que la taille du
buffer donné en deuxième paramètre, alors le buffer ne contiendra que le nombre de
caractères à lire indiqués par le troisième paramètre et aura donc une partie vide.

Si le nombre d'octets à lire indiqué en 3ème paramètre est plus grand que la taille du
buffer donné en deuxième paramètre, alors :
<br> **---- Zone de danger, je ne suis pas sûr ici, voir ma question tout en bas ----** <br>
read() ne va lire que le nombre d'octets maximum qu'on peut stocker dans le buffer.

#### La fonction write()

Elle a pour signature :

```c
ssize_t write(int fd, const void *buf, size_t count);
```

Le principe est globalement le même que pour read(), sauf qu'il s'agit d'écrire au
lieu de lire.

Si on indique plus d'octets à écrire qu'il n'y a d'octets dans notre buffer,
alors :

---- Zone de danger, je ne suis pas certain, voir ma question à la fin du document ----

seuls les octets présents dans le buffer seront écrits.

---

Si on indique moins d'octets à écrire que la taille réelle du buffer, alors
on écrira autant d'octets que le troisième paramètre indique.
Attention, on rappelle que le contenu du buffer est écrasé à chaque passage
dans la fonction write. Les caractères présents dans le buffer mais non-écrits
seront donc perdus, sauf lors de la dernière écriture (dernière itération,
si on est dans une boucle).

De même, write renvoie le nombre de caractères effectivement/réellement écrits.

#### Combiner read() et write()

Il est alors possible de combiner read() et write() comme je l'ai fait afin d'écrire
autant de caractères qu'on en lit.
Pour cela, on peut avoir quelque chose comme ça :

```c
int char_read; // Nombre de caractères lus
char buf[BUFFER_SIZE] = {}; // buffer dans lequel on aura les données lues par bloc de 1024 caractères
while ((char_read = read(fd_file1, buf, BUFFER_SIZE)) > 0) { // tant qu'on lit des caractères
  write(fd_file2, buf, char_read);
}
```

On voit dans le code ci-dessus que le nombre de caractères lus par read() est stocké
dans la variable char_read, et qu'on donne donc cette variable comme troisième paramètre
de write(), c'est-à-dire en tant que nombre de caractères à écrire.
Une telle boucle permet donc de copier le contenu du fichier pointé par le descripteur
de fichier fd_file1 dans le fichier pointé par le descripteur de fichier fd_file2.

Si maintenant on veut prendre en compte les possibles erreurs, on peut utiliser
une petite sécurité supplémentaire :

```c
int char_read; // Nombre de caractères lus
int char_written; // Nombre de caractères écrits;
char buf[BUFFER_SIZE] = {}; // buffer dans lequel on aura les données lues par bloc de 1024 caractères
while ((char_read = read(fd_file1, buf, BUFFER_SIZE)) > 0) { // tant qu'on lit des caractères
    if ((char_written = write(fd_file2, buf, char_read)) != char_read) { 
        // Si on n'a pas écrit autant de caractères qu'on en a lu
        printf("Erreur : octets lus : %d != octets écrits : %d\n", char_read, char_written);
    }
}
```

On compare le nombre d'octets lus et le nombre d'octets écrits. Si ces deux
valeurs ne correspondent pas, alors on est face à une erreur dans notre cas
(car c'est la condition que nous voulons respecter, mais cette condition n'est
pas du tout obligatoire).

### Copier des fichiers dans un dossier

Il s'agit simplement d'une détection du nombre d'arguments donnés en entrée de notre
programme et de leur type. Si nous avons N-2 fichiers après le nom du programme
et 1 répertoire en dernier argument après ces N-2 fichiers, alors cela signifie que nous
sommes dans le cas de figure envisagé : copier des fichiers dans un dossier.

Pour copier les fichiers donnés en arguments dans un dossier, j'ai simplement
parcouru tous les fichiers ordinaires donnés en entrée du programme et j'ai appelé à
chaque itération :

```c
#define FIRST_FILE 1
// pas argc -1 car on ne veut pas atteindre le dernier argument 
// qui est le nom du répertoire de destination
for (int i = FIRST_FILE; i < argc - 2; i++) {
    copy_file_in_file(argv[i], argv[argc - 1]);
}
```

On voit que le deuxième paramètre de ma fonction copy_file_in_file() est le nom du
répertoire de destination. Cela ne pose pas de soucis, car si le répertoire existe,
il suffit de créer dans ce répertoire un fichier du même nom que le fichier
que l'on veut copier, et d'y mettre le contenu du fichier que l'on veut copier.
C'est la méthode que j'ai utilisée.

Si on a par exemple : "./mycp1.exe file1 dir1", mon programme va intérprêter :
"cp file1 ./dir1/file1", c'est-à-dire qu'il va créer dans le répertoire dir1 un
fichier du même nom que le fichier d'origine.

### Copier un dossier dans un autre dossier

On utilise la fonction suivante :

```c
void copy_folder_in_folder(char *src_dirpath, char *dst_dirpath);
```

On rappelle qu'on ne crée pas les sous-dossiers du dossier source dans le dossier
de destination.

Dans cette fonction, on ouvre le répertoire source avec :

```c
DIR *dir = opendir(src_dirpath);
```

Puis on parcourt l'ensemble des fichiers du répertoire. Lorsqu'on rencontre un fichier
ordinaire, on le copie dans le répertoire source grâce à la fonction vue précédemment :

```c
void copy_file_in_file(char *src_filepath, char *dst_path)
```

Dans le cas où on rencontre un dossier dans le répertoire ouvert, on l'ignore
(on affiche simplement un message) et on passe au fichier suivant à étudier :

```c
char complete_src_path[BUFFER_SIZE] = {};
while ((d = readdir(dir)) != NULL) { // on parcourt tous les fichiers du répertoire courant
     sprintf(complete_src_path, "%s/%s", src_dirpath, d->d_name);
    if (is_dir(complete_src_path) || is_dot_dir(complete_src_path)) {
        printf("Tentative de copie d'un dossier dans un autre dossier. Opération non-supportée. Le programme continue.\n");
    }
    else {
        copy_file_in_file(complete_src_path, dst_dirpath);
    }
}
```

L'instruction sprintf() est ici utilisée afin de créer le chemin du fichier à chercher
dans le répertoire de départ ouvert. Car en effet, on a certes ouvert le répertoire
de départ, mais nous ne possédons pas le chemin vers le fichier courant à chaque
copie que l'on veut faire.
Il suffit alors de concaténer le chemin du répertoire avec le nom du fichier courant.
Par exemple : si le répertoire est "./Tests/", qu'on l'ouvre et qu'on parcourt ses fichiers,
on ne sait pas quel fichier on doit copier, à ce stade-là.
Pour avoir le chemin du fichier à copier, on concatène donc "./Tests/" au nom du fichier
courant, par exemple "foo", afin d'obtenir le chemin du fichier courant : "./Tests/foo".
Ce chemin "complet" est utile notamment car on appelle copy_file_in_file, et que
cette fonction utilise la fonction open() qui a besoin d'avoir le chemin vers le fichier
qu'on veut ouvrir à partir de l'emplacement actuel de notre exécutable.

## Exercice 4

Le principe global de mon programme est simple : j'ai un processus père qui va fork() et qui
va attendre la fin du processus fils à l'aide de la fonction wait().

### Processus fils hérite du descripteur de fichier ouvert par son processus père

Pour tester cela, j'ai ouvert un fichier avec open() dans le processus père avant de fork().
A partir de cette ouverture de fichier, j'ai récupéré le descripteur de fichier.
A noter que j'ai ouvert le fichier en écriture seule, que je le crée s'il n'existe pas, et
que je ne tronque pas le contenu du fichier, j'écris à la suite.
Dans le fils, j'écris une phrase dans le fichier ouvert, puis j'utilise la fonction close() afin
de fermer le fichier dans le processus fils.
Dans le père, j'écris également une phrase dans le fichier ouvert, et j'utilise également
close() afin de fermer le fichier dans le processus père.

On peut rapidement deviner que les descripteurs de fichiers sont dupliqués entre le
processus père et le processus fils.

En effet :

* En affichant le numéro du descripteur de fichier pointant vers le fichier ouvert,
  on obtient le même numéro dans le processus père et fils, cela est normal, car les variables
  sont dupliquées (ou plutôt, sont partagées jusqu'à ce que l'un des deux processus écrivent
  dans une variable partagée, après quoi la variable se voit dupliquée).


* En affichant la table des descripteurs de fichier au début des processus père
  et fils, on remarque qu'il y a autant de descripteurs dans l'un que dans l'autre au début,
  malgré le fait que le fils ferme les ouvertures de fichiers et répertoires.


* En ouvrant un fichier supplémentaire dans le fils que dans le père (et en ne le fermant
  pas manuellement), on voit en affichant la table de descripteurs qu'il y a 6 éléments
  dans la table du fils, face à 5 éléments dans la table du père. Les descripteurs ne
  sont donc pas partagés du fils vers le père.


* Le fils s'exécutant avant le père (en raison du wait() dans le père, qui attend donc la fin
  de son fils), close() est donc appelé une première fois dans le fils, sans pour autant
  impacter le père par la suite, puisque le père arrive également à écrire sa phrase, tout comme son fils
  précédemment.

On peut donc conclure que le descripteur de fichier est bel et bien dupliqué entre le processus
père et le processus fils, et hérité du père vers le fils.

### Processus fils hérite du répertoire ouvert par le processus père

Pour tester cela, j'ai simplement ouvert un répertoire dans le processus père avant de fork()
à l'aide de opendir().
Ensuite, dans le père et dans le fils, je parcours tous les fichiers du répertoire ouvert
à l'aide de readdir() et j'affiche leur nom.
On remarque alors que dans le fils, tous les fichiers du répertoire sont affichés,
alors que dans le père, aucun répertoire n'est affiché. Cela est dû au fait que le curseur
de fichier maintenu dans le répertoire et qui est avancé à chaque lecture par readdir() est arrivé
à la fin du répertoire, autrement dit, qu'il a parcouru tous les fichiers du répertoire.
Pour retourner "au début" du répertoire, il aurait fallu utiliser rewinddir(). En l'absence
de l'appel à cette fonction, le curseur semble rester à la fin du répertoire entre les deux processus.
On peut donc conclure que les deux processus père et fils partagent une même ouverture
de dossier qui a été réalisée dans le père, puisque faire avancer le curseur dans le fils
fait avancer le curseur dans le père.

**Seulement, est-ce un partage ou une duplication ?**

Le closedir() réalisé dans le processus fils (avant que le père après le fork() ne s'exécute)
n'ayant pas d'impact sur l'accès et le "parcours" (le curseur est à la fin, il n'affiche
rien, mais il arrive à accéder au répertoire et se rendre compte qu'il est à la fin) du
répertoire par le processus père.

Il semble donc s'agir d'une duplication, car la fermeture du répertoire dans le fils n'impacte
pas l'accès au répertoire par le processus père.

## Exercice 5

cf. Exercice 2, je l'ai déjà implémenté.

# Questions et Remarques :

## Exercice 1

Comment fonctionne le système de récupération du "prochain" contenu avec readdir,
et comment fonctionne la pile stockant tous les parcours qui sont réalisés ?

## Exercice 2

Qu'est-ce que "d_reclen" dans la structure : struct dirent ? A quoi correspond cette
taille ?

## Exercice 3

Si l'on donne un 3eme paramètre > à la taille réelle du buffer dans une fonction read(),
le curseur avance-t-il uniquement du nombre maximal de caractères qu'on peut
stocker dans le buffer, ou avance-t-il en fonction du 3eme paramètre ?

Qu'est-ce que write écrit si on donne + d'octets à écrire en 3eme paramètre qu'il n'y a de
place dans le buffer donné en 2ème paramètre ?
