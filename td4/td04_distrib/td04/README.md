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



