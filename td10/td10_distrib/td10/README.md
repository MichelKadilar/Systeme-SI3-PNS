# Comment créer une image docker

Il vous faut pour cela le logiciel Docker (moteur Docker) installé sur votre machine.

Puis il faut avoir un fichier qui se nomme "Dockerfile" que vous créez.
Un Dockerfile contient toutes les instructions DOCKER qui seront utilisées afin de créer l'image DOCKER 
de votre choix. C'est une sorte de "traducteur" permettant de convertir des commandes Docker en commandes UNIX.
Une image est un fichier statique, contenant les instructions qui seront exécutées lorsque l'image sera exécutée. 
Lorsque cette image est exécutée, l'environnement dans lequel l'image est exécutée s'appelle un
container/conteneur. C'est comme s'il y avait un super-utilisateur dans un environnement linux et
qu'il essayait de lancer les unes à la suite des autres toutes les commandes qui sont dans 
l'image Docker (qui sont donc des commandes UNIX).

Attention, un conteneur exécuté mais qui est "terminé" reste actif, il apparaitra donc dans la liste 
des conteneurs actifs.

# Les commandes Docker utiles

Toutes les commandes ne peuvent être exécutées que sur un environnement UNIX (VM Linux, WSL...).

**Il faut, avant de pouvoir les utiliser...installer DOCKER**

- Générer une image docker à partir d'un dockerfile : ```docker build -t nameOfImage:TagOfImage -f nameOfDockerfileToUseToGenerateImage pathToTheDockerfile```
- Consulter/Lister les images Docker existantes sur votre machine : ```docker images```
- Consulter/Lister les conteneurs existants (même terminés) sur votre machine : ```docker ps -a```
- Supprimer un conteneur par l'ID du conteneur : ```docker ps -a```, puis il faut aller chercher l'ID du 
conteneur qu'on veut supprimer, puis exécuter ```docker rm containerID```.
- Supprimer une image par l'ID de l'image : ```docker images```, puis il faut aller chercher l'ID
de l'image qu'on veut supprimer, puis exécuter : ```docker rmi imageID```

**Il faut toujours supprimer tous les conteneurs utilisant une image avant de supprimer l'image en
question.**

## Exercice 1 - 

docker build -t nomImage:Tag localisationDuDockerfile

## Exercice 2

Au début, j'ai eu un problème lié au système de fichier, je mets à la racine ce qui ne devrait pas l'être

## Exercice 3 - ALPINE

Ob obtient une erreur "/bin/sh: ./hello.exe: not found".

Raison : Il manque les dépendances de l'exécutable.

Cause : 

------------- ICI PAS SUR ----------------------

On n'a pas d'environnement d'exécution approprié pour les exécutables produits 
utilisant la libc (car sur alpine, c'est musl qui est utilisé (version minimale et non standard
de la libc, pas forcément compatible avec la libc)).

------------- FIN DU PAS SUR -------------------

2 Solutions : 

1. Emporter toutes les dépendances nécessaires à l'intérieur de l'exécutable : Aller voir /hello-alpine/DockerfileV2
```gcc -Wall -std=gnu99 -static -o hello.exe hello.c```. 
Cela permet d'emporter toutes les bibliothèques nécessaires à la bonne exécution du programme 
hello.exe, autrement dit, cela permet d'emporter toutes les dépendances requises au 
fonctionnement de hello.exe, directement dans hello.exe.

2. Ajouter les dépendances nécessaires dans le conteneur : Aller voir /hello-alpine/DockerfileV1
Pour build une image docker avec un nom de fichier Dockerfile personnalisé :
```docker build -t hello-world:alpine-3.17 -f DockerfileV2 .```
"-f" permet d'indiquer le fichier à utiliser en tant que dockerfile, sachant que je me trouvais 
déjà dans le répertoire /hello-alpine au moment d'exécuter la commande de build.

## Exercice 4 - SCRATCH

En se basant sur scratch, on indique qu'on ne veut "rien" dans notre noyau (aucun fichier visible).

C'est pour cela que nous ne pouvons pas utiliser ```apk install gcompat``` dans notre conteneur
afin d'installer gcompat (qui permet de rendre compatible un système basé sur musl avec la libc),
puisqu'il n'existe pas de gestionnaire de paquets dans un noyau "scratch".

En revanche, il y a bien un loader dans un noyau "scratch", qui permet de charger du code en mémoire (autrement dit, qui permet d'exécuter du code).
Cependant, ce code doit être en binaire, car la mémoire ne sait interagir qu'avec des instructions 
écrites en binaire. Heureusement pour nous, les exécutables produits à partir de code C sont des
fichiers binaires contenant donc des instructions écrites en binaire.
Ce qui signifie que si nous avons un exécutable qui emporte toutes ses dépendances statiquement,
il sera possible de l'exécuter sur un noyau "scratch". S'il manque des dépendances, certaines parties
du code ne vont pas pouvoir s'exécuter et on se retrouvera dans le cas d'une erreur 
"file not found" comme à l'exercice 3.

C'est donc cette solution qui devrait fonctionner.

J'ai donc produit un exécutable contenant statiquement toutes les dépendances :
```gcc -Wall -std=gnu99 -static -o hello.exe hello.c```
Puis j'ai produit une image docker à partir du dockerfile /hello-scratch/DockerfileV2 :
```docker build -t hello-world:scratch -f DockerfileV2 .```, sachant que je me trouve dans 
le répertoire /hello-scratch.
Puis j'ai exécuté cette image (ce qui crée donc un conteneur) : ```docker run imageID```.

## Exercice 5

Après avoir téléchargé dive sur ubuntu/debian avec la commande suivante : 

```
sudo wget https://github.com/wagoodman/dive/releases/download/v0.9.2/dive_0.9.2_linux_amd64.deb
sudo apt install ./dive_0.9.2_linux_amd64.deb
```

dive s'utilise ainsi : ```dive imageID```.
Les commandes existantes une fois dans l'outil sont disponibles ici : https://github.com/wagoodman/dive#keybindings


### Image HelloWorld par défaut

Taille : 13.3 Ko

Couche : 1

- 13 Ko - FROM 3e1e25141bad736

### Image Ubuntu

Image ubuntu avec un exécutable ne contenant pas les dépendances : 77.8 Mo

Couches : 2

- 78 Mo - FROM ubuntu:22.04
- 18 Ko - COPY hello.exe . (sans dépendances)

### Image Alpine

#### De base sans ajout manuel

Taille : 7.05 Mo

Couches : 1

- 7.0 Mo - FROM alpine:3.17.3

#### Avec gcompat et avec un exécutable ne contenant pas les dépendances

Taille : 9.81 Mo

Couches : 3

- 7.0 Mo - FROM alpine:3.17.3
- 2.7 MB - RUN /bin/sh -c apk add gcompat
- 17 Ko - COPY hello.exe . (avec dépendances)

#### Sans gcompat et avec un exécutable contenant les dépendances

Taille : 7.86 Mo

Couches : 2

- 7.0 Mo - FROM alpine:3.17.3
- 814 Ko - COPY hello.exe . (avec dépendances)

#### Avec gcompat et avec un exécutable contenant les dépendances

Taille : 10.6 Mo

Couches : 3

- 7.0 Mo - FROM alpine:3.17.3
- 2.7 MB - RUN /bin/sh -c apk add gcompat
- 814 Ko - COPY hello.exe . (avec dépendances)

### Image Scratch

#### Sans ajout manuel (image de base)

Taille : 0 o

Quand j'essaie de voir les couches avec dive :

```Cannot fetch image. Error response from daemon: empty export - not implemented```.

#### Avec un exécutable contenant les dépendances

Taille : 814 Ko

Couche : 1

- 814 Ko - FROM scratch

Dans l'ensemble, cela reste tout de même assez lourd (voire très lourd) pour un simple hello world, surtout par rapport
à l'image hello world par défaut qui ne fait "que" 13.3 Ko.






