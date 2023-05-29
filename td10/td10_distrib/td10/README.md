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

COPY est quant à lui utilisé à deux reprises dans le Dockerfile :

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

## Exercice 9

Pour partager un fichier de l'hôte avec un fichier du conteneur (un fichier pouvant être un repertoire),
il est possible d'utiliser les volumes. Les volumes permettent la persistance des données = 
Si on fait des modifications sur des fichiers d'un conteneur qui sont dans un volume, on ne 
perd pas ces modifications, même si le conteneur est supprimé.
Actuellement, sans volume, toutes les modifications réalisées sur des fichiers par nos conteneurs
sont perdues. Le volume permet donc de faire durer la donnée dans le temps.

### Les deux types de volumes

Il existe deux types de volumes : "bind mount"/non nommé, et "nommé", qui utilisent tous les
deux le stockage de la machine hôte afin d'assurer une persistence de certaines données de
conteneurs. Ils utilisent le système de fichiers de l'hôte pour faire persister des données.

Dans les faits, il en existe un troisième, dont on ne parle pas vraiment : les volumes "internes",
qui sont simplement les espaces de stockages du conteneur. Ces volumes "internes" permettent
donc la persistence des données à l'intérieur même des conteneurs.
Ainsi, lorsqu'on supprime le conteneur, le volume interne associé est aussi supprimé, effaçant
toutes traces du conteneur sur la planète Euphor.

#### Volume bind mount/non nommé

Les volumes non-nommés/"bind mount" peuvent être considérés comme de simples partages de 
fichiers entre une machine hôte et un conteneur. Ce qu'il se passe, c'est qu'on va, dans le 
conteneur, avoir un fichier de notre machine hôte qui sera lié à un fichier du conteneur.

Les répertoires et les fichiers du "bind mount" sont les mêmes que ceux de la machine hôte. 
Toute modification d'un côté est immédiatement répercutée de l'autre côté, car les deux 
côtés contiennent les mêmes données.

Ce type de volume dépend donc de la machine hôte, et donc de son système de fichiers et
de son système d'exploitation.

Pour utiliser un volume non-nommé (bind mount), il faut utiliser l'option :

```-v hostPathToShareWithContainer:containerPathToStoreData:accessRightsToHostPathByContainer```

par exemple :

```-v /etc/localtime:/etc/localtime:ro```, qui permet de partager le fichier localtime du 
repertoire /etc de la machine hôte, et d'y avoir accès au chemin /etc/localtime dans le 
conteneur, et avec comme droits d'accès aux données du host : droit de lecture (ro = read-only,
wo = write-only, xo = execute only, rw = read write, rx = read execute, etc).

Cela s'utilise avec la commande ```docker run``` :

```docker run -p 8888:8080 -v /etc/localtime:/etc/localtime:ro imageName``` par exemple.

Dans notre cas, nous voulons partager en "bind mount" deux fichiers de la machine linux hôte :
```/etc/localtime``` et ```/etc/timezone```, et cela uniquement avec des droits de lecture.

Nous allons donc utiliser lors de l'exécution de notre conteneur :

```docker run -p 8888:8080 -v /etc/localtime:/etc/localtime:ro -v /etc/timezone:/etc/timezone:ro node-app```

Ce type de volume "non-nommé" est donc géré par le système de fichiers de la machine hôte et 
les données sont retrouvables grâce à une adresse dans le système de fichiers.

#### Volume nommé

Les volumes nommés, quant à eux, permettent de créer un "espace de stockage" propre à notre moteur
Docker, qui sera évidemment stocké sur le système de fichiers de l'utilisateur, mais qui sera
entièrement géré par le moteur Docker, de façon à rendre ce type de volumes totalement compatibles avec 
tous les systèmes d'exploitation hôtes (Windows, MacOS, Linux...).

Un autre avantage, en plus de la comptabilité, est le fait qu'il soit "nommé", et qu'on puisse donc
le retrouver grâce à un nom qu'on aura décidé, et pas grâce à un chemin.

Pour le mettre en pratique, il faut passer par plusieurs étapes que je décrirai dans l'exercice 15.

## Exercice 10

Il suffit d'ajouter ces deux instructions dans le Dockerfile :

```dockerfile
RUN apk add tzdata
RUN apk add musl-locales
```

Ces deux instructions permettent d'ajouter des paquetages sur un noyau linux Alpine, apk étant
le gestionnaire de paquetage utilisé sur un système alpine, en l'occurrence, sur notre système
alpine, on décide d'y rajouter deux paquetages : tzdata, et musl-locales.

On remarque alors que le problème de l'heure à l'intérieur du conteneur est bel et bien corrigé.

## Exercice 11

Le Dockerfile a une propriété commune avec le Makefile : Lorsqu'on rebuild une image qui a déjà
été build, il ne rebuild que ce qui a été modifié depuis le dernier build et ne reéxecute
que les instructions de Dockerfile qui se situent "au-dessus" (en termes de couches, mais en 
dessous en termes de lecture du dockerfile pour nous), et l'ajoute en tant que "couche" 
supplémentaire (pour les instructions qui génèrent des couches) par-dessus toutes les couches
créées auparavant dans cette image.

C'est en raison de cette propriété qu'il est plus adapté de séparer les copies en deux temps.
Dans notre cas, nous avons un serveur NodeJS, et donc certaines choses à faire par rapport à ce
serveur NodeJS (npm install, garantir la validité du package.json, etc).


### Explication théorique :

On rappelle que les nodes_modules contiennent les dépendances, et que le package-lock.json est
un fichier read-only garantissant les versions des dépendances et des sous-dépendances pour 
notre application.

Maintenant, que voulons-nous ? 

- Voulons-nous que les fichiers générés par npm install (notamment les nodes_modules et 
le package-lock.json) soit regénérés par un npm install à la moindre modification ayant lieu 
dans notre dossier /node (dossier courant /node de la question du TP) lorsqu'on rebuild 
l'image docker de notre serveur NodeJS tournant sur alpine ?

OU

- Voulons-nous que les fichiers générés par npm install (notamment les nodes_modules et
le package-lock.json) soit regénérés par un npm install à la moindre modification ayant lieu
dans notre package.json (qui gère les dépendances) ?

La bonne réponse est évidemment la deuxième, nous voulons que tout ce qui est lié aux 
dépendances ne soit régénéré que lorsqu'il y a une modification des...dépendances.

### Explication pratique :

#### Bonne pratique


```dockerfile
# Créé un répertoire de travail pour l'application et s'y déplace
WORKDIR /app

# Copie les fichiers package.json et package-lock.json (s'il existe)
COPY package*.json .

# Installe les dépendance de l'application
RUN npm install --omit=dev

# Copie l'application
COPY . .

# Démarre l'application
CMD ["npm", "start"]
```

Dans cette version du dockerfile, nous aurons deux couches et deux commandes à exécuter :

une couche pour la copie des package*.json (package.json et package-lock.json), une commande
```npm instaall --omit=dev``` à exécuter, encore une autre couche pour copier tout le dossier courant
de la machine hôte (/node) dans le dossier courant du conteneur (/app), et enfin une dernière commande
à exécuter ```npm start```, pour lancer le serveur NodeJS.

Les 2 couches et les 2 commandes sont représentées ainsi :

Commande 2 : CMD ["npm", "start"] <br>
Couche 2 : COPY . . <br>
Commande 1 : RUN npm install --omit=dev <br>
Couche 1 (base) : COPY package*.json <br>

Puisqu'à chaque execution de docker build sur une image déjà créée, le Dockerfile ne va
réaliser que les instructions qui dépendent des modifications ayant eu lieu entre le dernier 
build et celui-ci, si nous avons modifié une ligne de code de notre fichier server.js sans en 
modifier les dépendances, alors le Dockerfile va être "malin" et ne va reexécuter que 
l'instruction `COPY . .` et ce qui se trouve "au-dessus" en termes de "couches/commandes" et 
en-dessous dans le sens de lecture du Dockerfile car il n'y aura pas besoin de "reconstruire"
des choses qu'à partir des éléments modifiés, tout le reste n'ayant pas été modifié, rien ne 
changera si on les reconstruit, donc cela serait inutile, donc le Dockerfile ne le fait pas.

Autrement dit, si je modifie une ligne de code dans le fichier server.js et que je rebuild mon image
docker par-dessus une image déjà existante, le Dockerfile va vouloir copier uniquement les
fichiers modifiés, or, qu'est-ce qui implique les fichiers modifiés ? Uniquement l'instruction
```COPY . .``` du Dockerfile. Donc on crée une couche supplémentaire en refaisant la copie du 
repertoire courant de la machine hôte dans le repertoire courant du conteuneur, afin de récupérer
le "nouveau" fichier server.js (et, évidemment, tous les autres fichiers du dossier, par la même 
occasion), "écrasant" au passage les anciens fichiers qui étaient dans l'image docker.
Et, comme on l'a dit, on exécute les instructions/commandes "au-dessus" dans la vision en 
couches, et "en-dessous" dans le sens de lecture classique du dockerfile, ce qui implique donc dans notre cas
d'exécuter uniquement l'instruction ```CMD ["npm", "start"]```, qui exécute la commande
```npm start``` à l'intérieur du conteneur crée à partir de l'image.

Cela permet donc de ne pas avoir à rebuild à chaque fois toute l'image (notamment la génération
des dépendances et du package-lock.json avec npm install, qui peuvent prendre du temps) à la 
moindre modification de ligne de code (et bien évidemment, de l'exécution de docker build, 
après la modification).

Cela permet donc de ne générer à nouveau les dépendances que lorsqu'on modifie package.json.

Bien évidemment, le .dockerignore crée dans une des questions précédentes est là pour empêcher
qu'on copie les nodes_modules via ```COPY . .```, pour les mêmes raisons de gains de 
performances (et car c'est un peu inutile).

#### VERSUS

#### Mauvaise pratique

La mauvaise pratique, vous l'avez peut-être compris, repose sur le fait qu'à chaque 
```docker build```, un npm install soit lancé, peu importe les modifications effectuées sur 
notre projet (ou TP, si vous préférez, dans notre dossier /node).

En effet, si nous n'avions qu'un seul COPY, ainsi :

```dockerfile
# Créé un répertoire de travail pour l'application et s'y déplace
WORKDIR /app

# Copie l'application
COPY . .

# Installe les dépendance de l'application
RUN npm install --omit=dev

# Démarre l'application
CMD ["npm", "start"]
```

Nous aurions la "vue" suivantes (sous formes de couches et de commandes):

La couche et les 2 commandes sont représentées ainsi :

Commande 2 : CMD ["npm", "start"] <br>
Commande 1 : RUN npm install --omit=dev <br>
Couche 1 (base) : COPY . . <br>

Ce qui signifie que si on modifie server.js, par exemple pour changer une ligne de code qui ne nécessite
aucunement de générer à nouveau toutes les dépendances du serveur NodeJS, eh bien...nous allons
devoir recopier tous les fichiers du repertoire courant de la machine hôte dans le repertoire
courant du conteneur, puis puisqu'on exécute toutes les commandes se trouvant sur une "couche"
"au-dessus" de la couche où les modifications sont détectées, alors les deux commandes présentes
ici devront forcément être exécutées. Ce qui signifie qu'à la moindre modification, même
parmi celles qui ne modifient pas grand-chose (et surtout, pas des dépendances), nous devrons
recopier tout le répertoire courant, faire un npm install pour tout générer à nouveau, 
puis relancer le serveur.
Ce qui fait pas mal d'opérations inutiles qui auraient pu être évitées pour améliorer
les performances de génération et d'exécution de notre conteneur.

## Exercice 12

On peut fournir cette variable d'environnement ```TZ=Europe/Paris ``` soit dans le Dockerfile
avec ```ENV TZ=Europe/Paris``` comme instruction du Dockerfile, soit "dynamiquement" au 
lancement d'un conteneur : ```-e TZ=Europe/Paris```.

On choisit de la fournir "dynamiquement" lors de l'exécution d'un conteneur :

```docker run -p 8888:8080 -e TZ=Europe/Paris node-app``` et cela fonctionne très bien.

## Exercice 13

### Avantages et inconvénients du bind mount pour le réglage de l'heure

#### Avantages

- Puisque l'heure du conteneur est lié par bind mount à l'heure de la machine hôte, si les 
permissions d'accès du conteneur au fichier timezone sur la machine hôte sont bien gérées, 
alors il ne sera pas nécessaire de modifier manuellement le timezone du conteneur si jamais 
on venait à modifier cela (pratique quand on gère plusieurs conteneurs).

- Certaines opérations peuvent nécessiter que la timezone soit la même dans le conteneur et 
sur la machine hôte.

#### Inconvénients

- Si les permissions d'accès du fichier timezone de la machine hôte par le conteneur 
sont mal gérées (par exemple, en donnant les droits d'écriture alors qu'on ne le veut pas),
rien n'empêchera le conteneur de pouvoir modifier l'heure sur la machine de l'hôte, ce qui
peut être indésirable.

- Si on veut mettre une autre timezone à notre conteneur par rapport à notre machine hôte, on
ne peut pas.

- Si nos conteneurs sont sur le cloud, on ne sait pas forcément dans quelle timezone se trouve
la machine qui va héberger notre conteneur à un instant T, et donc le timezone peut être 
totalement différents sur nos différents conteneurs (et même pour un même conteneur, 
selon l'instant T), ce qui peut être problématique dans certains cas où on utilise l'heure
pour réaliser certaines opérations par exemple.

- Ne peut être réalisé que lors de l'exécution d'un conteneur

### Avantages et inconvénients de la variable d'environnement pour le réglage de l'heure

#### Avantages

- Efficace quand on veut différentes timezone selon le conteneur
- Timezone complètement dissociée des autres conteneurs et la machine hôte
- Facile à modifier en cas de besoin
- Peut-être fait dans le dockerfile ou lors de l'exécution d'un conteneur

#### Inconvénients

Je n'ai pas trouvé d'inconvénient qui en est vraiment un à mon sens.

## Exercice 14

```
mkdir data # existe normalement par défaut sur un système linux
docker run --rm -it -v ./data:/data alpine:3.17 /bin/sh # Dans mon cas, sans le . à ./data

cd /data # dans le conteneur
touch lodel # dans le conteneur

ls /data # sur la machine hôte : on voit que lodel existe
echo "toto" >> /data/lodel

cat lodel # dans le conteneur
```

## Exercice 15

Pour créer un volume nommé : ```docker volume create volumeName```
Pour inspecter des informations sur ce volume : ```docker inspect volumeName```
Pour lancer un conteneur en utilisant un volume : ```docker run -v volumeName:/pathFromWhichDataWillBeStoredInVolume imageName```

COMPLETER PAR LA REPONSE A LA QUESTION

## Exercice 16

Par défaut, tous les conteneurs et images sont créés avec un super-utilisateur (root) 
à l'intérieur. Autrement dit, l'utilisateur par défaut de tous les conteneurs est un 
super-utilisateur.

Lorsqu'on regarde (sans modifier l'utilisateur par défaut des images et des conteneurs docker) qui
est propriétaire des fichiers contenus dans le conteneur, on trouve pour les fichiers crées nous-mêmes l'UID = 0
et le GID = 0, et 0 est l'identifiant de "root". Ainsi, root est le propriétaire de tous les 
fichiers contenus dans un conteneur ou dans une image docker.

Pour se connecter au terminal/shell d'un conteneur en cours d'exécution : ```docker exec -it containerNameOrID /bin/sh```

Le nom de l'utilisateur avec lequel je me suis connecté est "user".
Les identifiants de mon utilisateur sont : UID = 0 et GID = 0 (pour signifier que je suis root)
Les groupes de mon utilisateur sont : groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),
10(wheel),11(floppy), 20(dialout),26(tape),27(video)




