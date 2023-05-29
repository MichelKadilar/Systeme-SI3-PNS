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

- Générer une image docker à partir d'un
  dockerfile : ```docker build -t nameOfImage:TagOfImage -f nameOfDockerfileToUseToGenerateImage pathToTheDockerfile```
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

En revanche, il y a bien un loader dans un noyau "scratch", qui permet de charger du code en mémoire (autrement dit, qui
permet d'exécuter du code).
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

On le met dans le dockerfile quand on veut que ça soit présent dans l'instance avec une valeur par défaut
quitte à être surchargé au moment de l'exécution

Si je crée une variable d'env pour stipuler dans quel dossier je dois écrire -> dans le dockerfile

## Exercice 6

On remplace la première ligne du dockerfile par :

```FROM node:18.16.0-alpine3.17```

## Exercice 7

Dans le fichier server.js :

```
const PORT = 8080;
const HOST = '0.0.0.0';
app.listen(PORT, HOST, () => {
```

crée un serveur NodeJS en localhost sur le port 8080 et permet d'écouter à cette adresse IP
sur le port 8080.
Par définition, cela signifie que tout serveur NodeJS crée avec ce fichier, sera en exécution
sur le port 8080.

Si maintenant, on désire lancer à la fois un serveur NodeJS dans un conteneur et sur notre
machine (machine hôte, donc), tout en pouvant récupérer tout ce qui passe sur le serveur NodeJS
de notre conteneur, il va nous falloir un petit peu ruser.
En effet, le conteneur Docker est un animal très dangereux, se nourrissant exclusivement de
noyaux unix et de ports ouverts.

Plus sérieusement :

Si je me trouve sur ma machine à moi, que je lance tel quel (avec le fichier server.js) un
serveur NodeJS sur ma machine hôte, alors ce serveur va se lancer sur le port 8080 de ma
machine fidèlement au fichier server.js. Très bien, le serveur est en fonctionnement sur
le port 8080.

Si maintenant, je décide de lancer un serveur NodeJS avec le même fichier server.js, mais
dans un conteneur, alors il se trouve que ce serveur aussi, sera en exécution sur le port 8080,
non pas de ma machine hôte, mais **du conteneur**. Cela fonctionne également très bien.

Si maintenant, je décide que je veux avoir accès sur ma machine hôte à ce qu'il se passe sur
le serveur NodeJS du conteneur, comment puis-je faire ?

Une solution, celle qu'on adopte, est simplement de faire correspondre un des ports du conteneur
(celui sur lequel tourne le serveur NodeJS), à un des ports de ma machine hôte, autrement dit,
cela revient à rediriger tout le trafic qui a lieu sur un port, dans un autre.
Plus simplement, tout ce qu'il va se passer sur un port sera accessible sur un autre port.

Le problème, c'est que si par exemple, je veux que le trafic de mon serveur NodeJS du conteneur
(qui est donc sur le port 8080 du conteneur) soit redirigé vers le port du même numéro (8080)
sur ma machine hôte, il va y avoir un problème : j'ai déjà lancé un serveur NodeJS sur ma
machine hôte, et celui-ci tourne déjà sur le port 8080 de ma machine, donc si je redirige le
flux/trafic du serveur NodeJS du conteneur sur ce port, il y aura des conflits et mon
information sera compliquée à retrouver.

On a encore une fois une solution à ce problème : rediriger le trafic du port 8080 du conteneur,
vers un port d'un autre numéro de ma machine hôte. En effet, puisque le 8080 est déjà occupé
avec un autre serveur NodeJS, on n'a qu'à rediriger le flux du port 8080 du conteneur, sur un
autre port de ma machine, par exemple choisissons le port 8888, pour le moment non utilisé.

Finalement, pour faire tourner deux serveurs NodeJS un sur une machine hôte, un autre dans un
conteneur docker, qui tournent originellement sur le même port 8080), et pouvoir écouter sur
la machine hôte ce qu'il se passe sur le port du serveur NodeJS du conteneur, nous avons réalisé
les étapes suivantes :

- Lancer un serveur NodeJS sur le port 8080 de ma machine
- Lancer un serveur NodeJS sur le port 8080 du conteneur
- Mapper le port/Rediriger le trafic du port 8080 du conteneur vers le port 8888 de ma machine hôte de sorte que le
  trafic
  du serveur NodeJS du conteneur soit accessible sur la machine hôte, pour éviter les conflits
- On n'a plus qu'à lire ce qui arrive sur le port 8888 de ma machine pour savoir ce qui se passe
  sur le serveur NodeJS du conteneur (qui tourne toujours sur le port 8080 malgré tout, puisqu'il
  s'agit d'une redirection !)

Concentrons-nous sur cette histoire de redirection/mapping.
Cela est réalisable grâce à la commande :

```makefile
docker run -p 8888:8080 node-app # docker run -p hostMachinePort:containerPort imageName
```

Ici, on mappe le port 8080 du conteneur sur le port 8888 de ma machine = on redirige tout ce
qui arrive sur le port 8080 du conteneur sur le port 8888 de ma machine.

Cela se fait donc lors du lancement d'un conteneur docker, avec l'option -p :

```-p hostMachinePort:containerPort``` est ce qu'il faut retenir

```-p``` est l'abréviation pour ```--publish```, qui signifie donc "publier un port du conteneur
sur une machine externe" dans notre cas, autrement dit, mapper un port du conteneur sur un
port de ma machine.

Vous avez peut-être également aperçu l'instruction EXPOSE 8080 dans le dockerfile de cet
exercice, voyons voir à quoi cela correspond.

### Différences entre EXPOSE et -p

#### EXPOSE

EXPOSE est utilisé comme instruction de "documentation" dans le dockerfile, permettant d'indiquer
quels ports sont susceptibles d'être utilisés par des services à l'intérieur de notre conteneur
une fois crée.

**Début de la partie dont je ne suis pas sûr**

---
EXPOSE a toutefois une action particulière : il rend accessible le port indiqué,
par exemple 8080, avec ```EXPOSE 8080```, non pas à la machine hôte, mais à d'autres
conteneurs (du même moteur docker).
---
**Fin de la partie dont je ne suis pas sûr**

Il est souvent utilisé de manière "statique" à l'intérieur du dockerfile, ainsi :

```EXPOSE containerPortNumber```

Bien qu'il puisse également être utilisé dynamiquement avec
```docker run -expose=portNumber imageName```

#### -p

```-p``` est l'abréviation pour ```--publish```, qui signifie "publier un port du conteneur
sur l'hôte" dans notre cas, autrement dit, mapper un port du conteneur sur un port de ma machine.
Cette option s'utilise ainsi : ```docker run -p hostMachinePort:containerPort imageName```, permettant
alors de rediriger sur un port de la machine hôte tout le trafic qui arrive sur un port du
conteneur.
Si on n'utilise pas cette version : ```-p hostMachinePort:contiainerPort```,
mais celle-ci : ```-p containerPort```, alors, cela va faire un mapping automatique vers le même
numéro de port sur la machine hôte.

Mapper un port sur un autre = rediriger tout le trafic qui arrive sur un port vers un autre.

A savoir qu'utiliser ```-p``` va automatiquement faire des ```-expose=containerPortNumber```
dynamique avec le même numéro de port que ce qui a été spécifié pour le conteneur dans ```-p```.

#### Principale différence

La principale différence entre ```-p``` et ```EXPOSE``` est que ```-p``` rend accessible un port
d'un conteneur par des machines "externes" (toute machine qui n'est pas un conteneur exécuté par
le même moteur docker que ce conteneur, par exemple, ma machine hôte, qui n'est tout simplement
pas un conteneur du tout), tandis que ```EXPOSE``` est davantage à titre informatif et est
utilisé pour rendre accessible un port d'un conteneur par d'autres conteneurs du même moteur
docker.

### Réponse à la question

Dans le makefile, il suffit de rajouter :

```makefile
run:
	docker run -p 8888:8080 node-app # docker run -p hostPort:containerPort
```

où node-app est le nom de l'image construite à partir du dockerfile :

```makefile
build:
	docker build -t node-app -f Dockerfile . # Il faut être dans le répertoire node pour que ça fonctionne
```

## Exercice 8

Nos modifications arrivent dès la première couche avec notamment le chargement de l'image de
base de notre conteneur. Cette image ```node-alpine```, permet de créer un noyau de base alpine
et d'ajouter par-dessus tout le nécessaire pour NodeJS.
Par la suite, nos propres modifications sont reprises à la 5ème couche, avec
l'instruction : ```WORKDIR /app```.

COPY est quant à lui utiisé à deux reprises dans le Dockerfile :

```COPY package*.json .``` et ```COPY . .```.

Dans le premier cas, cela signifie que nous allons copier tous les fichiers du repertoire courant
(celui dans lequel se trouve le dockerfile) commençant par "package", suivi de quelque chose,
et finissant par ".json", dans le repertoire courant du conteneur (en l'occurrence, /app,
puisque l'instruction ```WORKDIR /app``` est exécutée avant l'instruction copie).

Dans le deuxième cas, on copie carrément tout le repertoire courant de la machine (le repertoire
dans lequel se trouve le dockerfile utilisé) dans le repertoire courant du conteneur (/app).

Ces deux instructions COPY ont selon moi des problèmes.

D'abord, un bon nombre des éléments copiés ne sont pas nécessaires :

- **package-lock.json**, copié par le premier COPY : ```COPY package*.json .```, puisque ce fichier
  est généré par un npm install à l'intérieur du conteneur ;
- **les nodes modules** (qui sont les dépendances nécessaires au bon fonctionnement de notre
  serveur NodeJS, définies dans le package.json), copiés par le deuxième COPY : ```COPY . .```,
  puisqu'ils sont également générés par un npm install réalisé à l'intérieur du conteneur ;

Notamment, dans l'ordre dans lequel les lignes du Dockerfile sont exécutées, on copie
package-lock.json, alors qu'on l'écrase juste après par une instruction npm install
exécutée à l'intérieur du conteneur actif.

De plus, ce "nouveau" package-lock.json écrasé par un npm install, va lui-même se faire écraser
par la copie finale (la deuxième), qui copie tout le repertoire courant (celui du dockerfile)
dans le repertoire courant du conteneur (/app).

Au final, nous aurons copié deux fois le package-lock.json avec les COPY, et nous l'aurons
généré une fois par un npm install. Chaque copie et chaque génération écrase le fichier
package-lock.json. Mais au final, on se retrouve avec le package-lock.json présent dans le
repertoire courant de la machine hôte.

De même pour les nodes_modules, générés par un npm install (grâce au package.json), puis écrasés
par la deuxième copie (repertoire courant de la machine hôte dans repertoire courant du
conteneur).

Le fait que ces fichiers soient copiés plusieurs fois/écrasés est déjà problématique en soi en termes
de performances de création du conteneur, mais ici, nous avons un "problème" supplémentaire :
les nodes_modules et le package-lock.json n'ont tout simplement pas besoin d'être copiés.

Pour résoudre ces soucis, on a crée dans le repertoire node/ un fichier .dockerignore contenant :

```dockerignore
node_modules
package-lock.json
Dockerfile
.dockerignore
```

de façon à ce que lors de la construction de l'image de notre dockerfile, on ne copie ni les
nodes_modules, ni le package-lock.json. De même, j'ai préféré ne pas permettre la copie
du Dockerfile ni du .dockerignore.

Avec les nodes_modules, un problème supplémentaire existe. En effet, ce sont des dépendances
générées normalement grâce à la commande ```npm install```. Sauf que ces dépendances peuvent
être générées **selon** la machine (compilées, donc, selon la machine). Donc si on commence
à copier sur une autre machine des dépendances générées POUR ma machine, il aurait été possible
que les dépendances ne fonctionnent pas correctement et donc que le serveur NodeJS ne tourne
pas correctement.













