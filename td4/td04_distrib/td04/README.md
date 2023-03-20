# TP4 - Threads - Michel KADILAR

## Exercice 1

En exécutant le code sans modification, on se rend compte que le programme
crée un thread (avec pthread_create) qui va exécuter le code de la fonction
fn_thread, qui elle-même contient un appel à pthread_exit, permettant de
mettre fin au thread exécutant la fonction (donc le thread courant, dans
notre cas). Cela équivaut à un return dans notre cas.
Pendant ce temps, le thread principal du processus a fait un appel à
pthread_join, qui attend que le thread donné en paramètre se termine, et qui
donc ici va attendre que le thread exécutant la fonction se termine.

En retirant le pthread_join, le thread principal n'attend pas la fin du
thread qui exécute la fonction.
Il se trouve alors que l'exécution est beaucoup plus courte, et cela car le
thread principal n'attend pas et prend fin "immédiatement",
et par conséquent le processus englobant les deux threads prend fin, ce qui a
pour effet de mettre fin au thread exécutant la fonction.

Plusieurs comportements sont possibles (parmi ceux que j'ai remarqué) :

Le thread principal effectue ses deux affichages et :

- Le thread exécutant la fonction n'affiche rien, cela est dû au fait
  que le thread exécutant la fonction n'a pas encore atteint/terminé le premier
  printf ;
- Le thread exécutant la fonction affiche le premier printf mais pas le deuxième,
  car le thread annexe a terminé le premier printf tandis que le thread
  principal s'est terminé ;
- Un troisième cas m'est arrivé, j'en ai parlé à M. Lavirotte
  (voir la partie Exercice 1 dans la section Remarques et questions
  en bas de ce document)

## Exercice 2

Lorsque les deux threads sont en exécution parallèle, on peut voir que leur identifiant
POSIX est différent, et celui qui débute en premier a un identifiant inférieur à
celui qui débute en 2ème. Ces identifiants sont donc ordonnés.

L'exécution parallèle se fait par deux appels consécutifs à pthread_create
suivi par deux appels consécutifs à pthread_join (afin que le thread principal
attende la fin des deux autres threads).

Cependant, si on exécute ces threads l'un après l'autre (create suivi directement
du join, deux fois d'affilés), on remarque que les identifiants POSIX de ces
threads est identique. En effet, le premier thread qui s'exécute occupe un
identifiant, qu'il libère à la fin de son exécution. Il est donc de nouveau disponible
pour le prochain thread s'exécutant sur notre machine. Si un autre thread venait à
s'exécuter entre la fin du premier thread et le début du deuxième, ça serait ce
thread qui occuperait alors l'identifiant du premier thread, et le deuxième thread
aurait aussi un identifiant différent. Mais puisque le deuxième thread est le premier
thread s'exécutant juste après la fin du premier thread, alors c'est lui qui va
occuper l'identifiant du premier thread dès lors que ce dernier a terminé son
exécution.

## Exercice 3

Attention, j'ai fait les deux parties de cette exercice dans
deux fichiers différents. Les titres H3 (###) indiquent le nom du fichier que je traite
dans la partie du compte-rendu.

### threads.c

Lorsque les deux threads s'exécutent simultanément, on remarque les thread ID
attribué par le système sont différents, cela n'est pas étonnant car il faut
au système un moyen de différencier les deux fils d'exécution (et puis
l'appel POSIX considérait/attribuait déjà des ID différents...).

Lorsque les deux threads s'exécutent l'un après l'autre, on a vu que les ID
attribués par les appels POSIX étaient les mêmes, puisque le second thread
"reprenait" l'ID du premier thread (car celui a terminé son exécution).
Or ici, dans le cas du Thread ID attribué par le système, on remarque que ces
deux threads ont des identifiants différents (qui se suivent, pour les mêmes
raisons que l'ID attribué par un appel POSIX, expliqué plus haut) sur le système.

Ce qu'on remarque, c'est que les thread ID donnés par le système semblent très
similaires aux ID des processus lourds.
En effet, le système Linux ne fait pas la différence entre processus lourds et
threads, il considère les threads comme étant des processus lourds, à la
différence près qu'ils ont des ressources partagées avec les autres threads d'un
même "programme" (tout le processus), et c'est ce qui les rend "légers".
Chaque thread a son propre "PID", côté système.
Lorsqu'on fait un appel à getpid(), cela renvoie l'identifiant du thread principal.
Lorsqu'on fait un appel à gettid(), cela renvoie l'identifiant du thread courant.
Le point commun entre tous les threads générés à partir d'un thread principal,
est qu'ils ont tous un thread group id (TGID) commun : le PID du processus, donc
l'identifiant du thread principal.

### multiple_fork.c & multiple_thread.c

J'ai crée un fichier multiple_tread.c, contenant le même code que multiple_fork.c,
mais adapté pour les threads.

**SUR UN SEUL COEUR DE CALCUL**

On remarque que les écarts de temps de création sont assez énormes.
Les processus lourds prennent sur ma machine (au moins) 2,2 fois plus de temps à
être créés.
Le coefficient minimum que j'obtiens est d'environ * 2.
Le coefficient maximum que j'obtiens est d'environ * 20.

En effet, 100 threads prennent entre 1 et 15 milisecondes à se créer,
contre 22 à 48 milisecondes pour les 100 processus lourds.
Cela est gargantuesque à l'échelle de la machine qui réalise des actions toutes
les nanosecondes.

## Exercice 4 :

J'ai essayé de créer 100, puis 1000, puis 10 000, et enfin 100 000 processus lourds.
Ma machine me donne une erreur pour les 100 000 processus à créer :
"fork: resource temporarily unavailable". Car en effet, les processus occupent un
certain nombre/une certaine quantité de ressources. S'il n'y a plus assez de
ressources pour créer davantage de processus, alors nous obtenons l'erreur que j'ai
eu.
Dans les faits, au moins deux limites auxquelles j'ai pensé existent :

- Des sécurités afin de ne pas créer trop de processus :
    En l'occurrence, il s'agit d'une sécurité au niveau de l'utilisateur 
    courant du système.
    Avec la commande "ulimit -a" (qui m'a été donnée par Arnaud DUMANOIS), 
    j'ai pu constater que mon utilisateur courant est limité à créer au maximum
    14 379 processus.
- Des problèmes de ressources disponibles :
    S'il n'y a plus assez de mémoire vive
    afin d'exécuter et faire fonctionner l'ensemble des processus que l'on 
    veut créer.

## Exercice 5 :

Cf. exercice 3 (je pensais qu'il fallait déjà le faire au moment de l'exercice 3).

## Exercice 6 :



## Exercice 7 :

### Problème de synchronisation

A l'exécution de juste_presque.exe, sans modification, on peut voir des doublons
dans l'affichage. Ils sont dus au fait qu'un pointeur vers la même variable i
est passé à la fonction qu'exécutent les threads, et qu'ainsi, quand la valeur 
de la variable i est incrémentée dans la boucle, la valeur change pour tous 
les threads encore en exécution.

Dans mon cas, après avoir supprimé le sleep, je n'obtiens en sortie que 
des messages : "Dans la thread #0", sauf le dernier : "Dans la thread #4", ou alors
que des "Dans la thread #0".
Puisque la boucle se poursuit "simultanément" à l'exécution des threads
créés, on pourrait se dire que la valeur de i sera à chaque fois écrasée par la
prochaine valeur que la variable va prendre, et que donc nous ne devrions avoir
que des #4. Seulement, on voit que nous n'avons que des #0.

Une explication possible mais qui ne me convainc pas est la suivante :

Cela est dû au fait que la variable i n'a pas le temps de s'incrémenter alors
que les threads s'exécutent. On pourrait même dire que les threads sont créées
"en même temps". Donc tous les threads ont une même valeur de i.

### Solution juste.c

Il suffit alors de créer un tableau d'entiers qui va stocker les différentes valeurs
de i. Ce tableau permet alors d'avoir N cases mémoires différentes, et donc,
des adresses mémoires différentes pour chacune des valeurs de i, et donc
aucun chevauchement des différentes valeurs de i dans les fonctions.

## Exercice 8 :

## Exercice 9 :

Voir programme.

## Exercice 10 :

Voir programme.

## Exercice 11 :

### Création de processus

Les deux approches ont leurs avantages et leurs inconvénients.
Ce qui m'a notamment repoussé côté Windows, c'est que je ne connais pas assez
bien l'environnement C sous Windows. En effet, Windows défini de nombreux alias
de types et des struct qu'il faut connaître si l'on veut créer un nouveau
processus.
Je me suis alors aidé d'internet, mais je pense que personne ne retient toute la
stucture des paramètres de CreateProcess de tête.
J'ai réalisé une création de processus assez simple, puisque le processus crée
ne fait que lancer un terminal cmd, j'ai donc pas eu à m'intéresser de plus près
aux autres paramètres que le programme à exécuter.
Heureusement, il existe une documentation expliquant comment cela fonctionne
et dans laquelle il y a des exemples assez complets vis à vis de CreateProcess et
de tout ce qui pourrait être lié à cette fonction (WaitForSingleProcess, etc).

De ce côté, POSIX marque selon moi un point : fork() n'a besoin d'aucun paramètre,
exec prend quelques paramètres, mais qui sont très faciles à comprendre.

Le code fait sur Windows est beaucoup plus difficile à comprendre que le
code POSIX au premier abord.

De plus, POSIX permet plus de "flexibilité" au niveau du code. 
En effet, fork() et exec() sont bien deux instructions différentes, tandis que
CreateProcess() fait nécessairement l'équivalent de fork + exec "d'un seul coup".
Cette "flexibilité" peut à la fois être un avantage est un inconvénient.
Certes, l'opération n'est pas "atomique", mais souvent, lorsqu'on fait un fork,
c'est pour utiliser exec juste après. Alors peut-être que Windows a raison
d'avoir ces deux instructions (fork + exec) réunies en une seule.

Cependant, Windows permet plus de flexibilité au niveau de la création d'un
processus. En effet, il est possible de définir certaines propriétés pour le processus
crée, notamment vis à vis de ses enfants, mais on peut également contrôler la
priorité du processus à créer (afin qu'il soit priorisé par l'ordonnanceur par
exemple), le repertoire actif pour le processus à créer, etc.
On n'a pas la main sur ces choses-là côté POSIX avec simplement fork() et exec(),
nous devons faire intervenir d'autres fonctions (par exemple getenv(), chdir()
pour déterminer le repertoire actif du nouveau processus déjà créé).


Côté POSIX, on crée donc un processus par duplication puis modification, alors
que côté Windows, on crée déjà le processus tel qu'on voudrait qu'il soit et 
qu'il s'exécute.

### Création de threads

Là où sur Linux les threads dérivent des processus lourds afin d'en être des versions
"allégées", sur Windows, le thread est l'unité de base, et un processus ne fait
que contenir des threads.
On ajoutera également que côté Linux, les threads comme les processus lourds ne sont
pas considérés comme tels, mais comme des "tâches".
On remarque cela notamment puisque les threads comme les processus lourds ont leur
propre PID sur le système d'exploitation (appelé TID).

Windows est alors mieux structuré que Linux concernant la gestion et la séparation
des processus et threads. Cela parait cohérent quand on sait que Linux a 
ajouté la notion de thread ultérieurement au début du développement du 
système d'exploitation, alors que Windows l'a pris en compte dès sa création.

La mise en oeuvre sur le système est donc beaucoup plus propre côté Windows.

Concernant le code permettant la mise en oeuvre des threads, POSIX et Windows
ont fait des choix assez similaires. Leurs API se ressemblent :

| Systeme d'exploitation | thread type | create a thread                                                                                                                                                             | wait for a thread to exit                                                                                                                                                                                                                 | get thread id                                        |
|------------------------|-------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------|
| POSIX                  | pthread_t   | pthread_create(thread_id_storage_address, thread attributes, main function for the thread to execute, argument to pass to the main function)                                | pthread_join(id_thread_to_wait, address where to store the return value of the thread)                                                                                                                                                    | gettid() for system id/pthread_self() for virtual id |
| WINDOWS                | HANDLE      | CreateThread(default security attributes stack size usage,thread function name,argument to thread function,creation flags, address where is returned the thread identifier) | WaitForSingleObject(thread_handler, waiting_state_of_the_main_thread)  OR WaitForMultipleObjects(number of elements to wait for, pointer to handlers of elements to wait for, wait for all or only one, waiting_state_of_the_main_thread) | GetCurrentThreadId()                                 |


# Remarques et questions :

## Exercice 1 :

Le troisième cas est le suivant :

- Le thread exécutant la fonction affiche deux fois le premier printf en plus des
  deux messages du thread principal.

Une possibilité d'explication est que le flush, n'étant pas une opération atomique,
mais étant composé d'opérations **potentiellement** atomiques, pourrait subir
une interruption au moment de/avant l'exécution de l'instruction permettant de
vider le buffer. Cela a pour conséquence que le message est affiché une première
fois par un flush à la fin du thread exécutant la fonction, que le buffer
n'est pas vidé car l'opération de vidage est interrompue, puis que le message
est à nouveau affiché par un flush à la fin du thread principal.

/*
Cela serait dû avec une vision macro au fait que ce thread a atteint le message
du premier printf mais pas le flush (le \n), ce qui a pour conséquence de maintenir le message
à afficher dans un buffer qui sera affiché dès qu'un flush sera effectué.
Ce flush a lieu à deux reprises : A la fin du thread principal,
et à la fin du thread annexe (qui se termine donc de manière inopinée suite
à la terminaison du thread principal).
*/

## Exercice 7 :

Demander à M. LAVIROTTE pourquoi nous n'avons que des #0 après avoir supprimé le
sleep.


