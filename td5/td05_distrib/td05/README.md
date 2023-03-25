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

#### La fonction read()
#### La fonction write()



### Copier des fichiers dans un dossier

### Copier un dossier dans un autre dossier

Permissions

# Questions et Remarques :

## Exercice 1

Comment fonctionne le système de récupération du "prochain" contenu avec readdir,
et comment fonctionne la pile stockant tous les parcours qui sont réalisés ?

## Exercice 2

Qu'est-ce que "d_reclen" dans la structure : struct dirent ? A quoi correspond cette
taille ?

