# Exercices

## Exercice 1

1. On remarque que le terminal dans lequel on a lancé xeyes est bloqué/en attente,
   qu'on ne peut pas interagir avec ce terminal tant que l'application xeyes est en cours
   d'exécution. En fait, on dit ici que l'application xeyes est au premier plan (foreground job,
   en anglais).
2. Après avoir lancé xeyes et réalisé "Ctrl + Z" dans le terminal bloqué/en attente,
   on remarque que le terminal a repris la main, qu'on peut de nouveau interagir avec lui,
   et qu'en plus, xeyes est toujours en cours d'exécution mais qu'on ne peut plus interagir
   avec lui. Ce qu'il se passe ici, c'est que faire Ctrl + Z dans le terminal en attente
   de la terminaison de xeyes envoie un signal de suspension de processus à xeyes et donc
   que xeyes se suspend. Dans les faits, le processus exécutant xeyes est relégué à un état
   "suspendu", qui ne veut pas dire qu'il s'est terminé, mais qu'il est simplement arrêté à
   l'état qu'il avait au moment du signal envoyé par Ctrl + Z, et qu'il est possible
   de faire reprendre l'exécution de ce processus à l'endroit où il s'était arrêté.
   On peut observer cet état notamment grâce à la commande ```ps -aux``` qui permet de
   voir qu'au niveau de notre processus xeyes, nous avons un état "T" après avoir lancé
   la commande Ctrl + Z dans le terminal. Cet état T (selon "man ps", documentation de la commande ps)
   signifie qu'un processus s'est stoppé (suspendu) en raison d'un signal de contrôle de processus.
   En l'occurrence, nous avons envoyé un signal SIGSTOP à notre processus xeyes en exécutant
   la commande Ctrl + Z dans le terminal.
   Parmi les différents états possibles d'un processus, on se rappelle qu'il existe des états
   où le processus est soit bloqué et suspendu, soit prêt et suspendu. Dans notre cas, le processus
   est prêt et suspendu (puisqu'on l'a suspendu manuellement et qu'il n'est en attente d'aucun
   autre élément qu'un signal pour reprendre son exécution).
3. On remarque que l'exécution de xeyes reprend et que le terminal est pleinement disponible
   pour interagir avec nous. Ce qu'il se passe est qu'on envoie le processus exécutant xeyes
   en arrière-plan (il devient ce qu'on appelle un background job), et le processus exécutant
   le terminal revient au premier plan.
   Un processus en avant-plan est un processus qui est "connecté" à un terminal et qui est
   capable de communiquer avec des périphériques (écrans, souris, clavier...) ou
   des utilisateurs.
   Un processus en arrière-plan est un processus qui est "déconnecté"/"détaché" d'un terminal
   et qui ne peut pas communiquer avec un utilisateur s'il n'a pas d'instruction ayant pour but de le
   faire.
   On rappelle cependant que dans tous les cas, le processus init exécutant le terminal
   reste le processus père de notre programme xeyes, peu importe qu'ils soient en avant-plan ou
   en arrière-plan.
4. Grâce à la commande ```ps -aux```, je peux lire le PID de mon processus xeyes : 11.
5. En lançant la commande "kill -SIGSTOP 11", on constate que l'application xeyes
   s'est stoppée (on ne peut plus interagir avec l'application à l'aide de la souris).
   On remarque (avec ```ps -aux```) que le processus exécutant xeyes a changé d'état et
   qu'il est désormais dans l'état T (état stoppé, qu'on a déjà expliqué dans le 2.).
   On peut donc relier le signal SIGSTOP au fait d'effectuer un "Ctrl + Z" sur un processus
   en cours d'exécution.
6. On trouve grâce à la commande ```kill -l | grep CONT``` que le signal numéro 18
   correspond au signal SIGCONT. C'est ce signal qui va permettre de faire
   continuer/reprendre des processus stoppés. On envoie alors ce signal à notre processus xeyes
   grâce à la commande ```kill -18 11``` qui équivaut à ```kill -SIGCONT 11```. On remarque
   après avoir lancé cette commande que xeyes a bel et bien repris son exécution.
7. Pour pouvoir utiliser le nom d'un processus au lieu de son pid afin d'envoyer un signal,
   on peut utiliser la commande ```pkill nomProcessus```.
   Ainsi, dans notre cas, on va utiliser : ```pkill -9 xeyes``` (9 correspond à SIGINT = Ctrl + C,
   terminaison/interruption de processus). Il faut faire attention car avec une telle
   commande, on va affecter (dans notre cas, interrompre) tous les processus ayant ce nom.
   Autrement dit, si on a lancé plusieurs fois l'application xeyes, et qu'on lance cette commande,
   tous les processus ayant le nom xeyes (donc tous ceux qu'on a lancé) se feront interrompre/terminer.

## Exercice 2

On ne peut pas faire plusieurs handlers pour un même signal. Si on veut avoir un comportement
commun pour plusieurs signaux, on va alors devoir soit dupliquer un comportement dans plusieurs
fonctions, soit on va devoir faire un seul handler dans lequel on va vérifier la valeur du signal
reçu et agir en conséquence (si c'est un SIGSEGV, je veux faire ça, sinon je fais autre chose,
par exemple).

On se rend effectivement compte que lorsque le processus est au premier-plan sur le terminal,
on peut directement interagir avec lui via des raccourcis claviers qui lui envoient des
signaux ou via des signaux envoyés depuis un autre terminal.

On se rend également compte que lorsque le processus est en arrière-plan, il ignore tout
signal produit grâce à des raccourcis dans le terminal. Le terminal a une gestion
particulière des signaux (il reconnait des raccourcis claviers, etc).
On peut cependant toujours interagir avec le processus qui est en arrière-plan en
envoyant un signal avec la commande ```kill -signal pid```.

La fonction :

```c
Attention : le vrai type de retour est un void * casté en int.

int signal(int signumber, void (*handler_function)(int));
```

où handler_function prend en paramètre un entier qui représente le numéro du signal
qui a été envoyé, permet alors d'associer l'appel d'une fonction
```void handler_function(int)``` à la réception d'un signal signumber par le programme.

Une utilisation possible de cette fonction est la suivante :

```c
int signal(SIGINT, hand_func);  // SIGINT est une constante = 2
```

Cela permet d'indiquer que dès que le programme recevra un signal SIGINT, la fonction
hand_func sera appelée avec en paramètre le numéro du signal envoyé (c'est-à-dire le
même numéro que le numéro du signal passé en premier paramètre).

# Exercice 3

Deux éléments préliminaires sont nécessaires :

* ```struct sigaction``` qui est la structure qui va nous permettre d'associer des
  fonctions/handlers à la réception de certains signaux, mais également de pouvoir appliquer
  des masques. Un masque est un ensemble de signaux qui seront **bloqués** (reçus
  avec une réception qui est prise en compte, mais pas traités) le temps que le
  programme se "débloque". Le déblocage a lieu quand le programme est
  "disponible" : quand le programme est en exécution et capable de traiter un
  signal reçu. Cela a lieu quand n'est pas bloqué par autre chose, qu'il s'exécute, ET
  qu'il n'est pas en train d'exécuter la fonction handler d'un signal. Le masque
  permet de mettre en attente le traitement d'un signal le temps que le programme
  soit disponible (et qu'il ne soit pas dans une fonction handler).


* ```sigset_t msk_set``` il s'agit là d'une structure qui représente un ensemble de
  signaux. En réalité (du moins, on peut le considérer ainsi), il s'agit d'un ensemble
  de numéros de signaux (leurs identifiants, par exemple SIGINT = 2, chaque type de signal
  a un numéro différent). Cet ensemble va notamment servir pour y stocker les signaux
  qu'on voudra masquer/bloquer dont je parle dans le paragraphe précédent.

  Cette structure repose sur l'utilisation de certaines fonctions associées :

```c
#include <signal.h>

int sigemptyset(sigset_t *set); // permet de vider l'ensemble qu'on lui donne en paramètre

 int sigfillset(sigset_t *set); // permet de remplir l'ensemble qu'on lui donne en paramètre
 // par tous les numéros de signaux existants

int sigaddset(sigset_t *set, int signum); // permet d'ajouter un signal de numéro signum dans l'ensemble set

int sigdelset(sigset_t *set, int signum); // permet de retirer un signal de numéro signum de l'ensemble set

int sigismember(const sigset_t *set, int signum); // permet de vérifier si le signal de numéro signum fait partie de l'ensemble set
```

Afin de mettre en place une gestion de signal à l'intérieur de notre programme, il est
donc important de d'abord définir ces deux structures, et ainsi de définir le masque et le handler
que l'on veut appeler lorsqu'on reçoit un certain type de signal.

Par exemple :

```c
void on_signal(int signum){
    ...
}

int main(void){
    struct sigaction sigact;
    sigset_t msk_int;

    // On gère notre ensemble de masques de signaux
    sigemptyset(&msk_int); // on vide l'ensemble par précaution
    sigaddset(&msk_int, SIGINT); // on ajoute SIGINT dans l'ensemble du masque
    // Fin de la gestion des masques des signaux

    // Gestion des signaux à leur reception
    sigact.sa_handler = on_signal; // Quand on recevra un signal défini, on ira dans la fonction on_signal
    sigact.sa_mask = msk_int; // Le masque qui est contenu dans la structure sigaction va contenir les 
    // mêmes signaux que msk_int.
}
```

La dernière composante qui va nous être nécessaire ici est une fonction permettant d'associer
ces différents éléments à la réception d'un certain type de signal :

```c
#include <signal.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```

Cette fonction permet d'associer la structure sigaction définie précédemment à la réception
d'un signal de numéro signum. Le troisième paramètre est à ignorer dans notre cas, on lui
donnera toujours NULL.

On l'utilise ainsi :

```c
sigaction(SIGINT, &sigact, NULL);
```

Cela permet alors d'associer la structure sigaction sigact à la réception d'un signal SIGINT.
Si la structure sigaction contient les deux éléments suivants :

```c
sigact.sa_handler = on_signal; 
sigact.sa_mask = msk_int;
```

alors cela signifiera que lorsqu'on recevra un signal SIGINT, la fonction on_signal sera
appelée avec en paramètre le numéro de signal du signal SIGINT et que le masque msk_int sera
associé à la réception d'un signal SIGINT. Autrement dit, si un signal SIGINT est reçu,
que on_signal est exécuté, et que pendant l'exécution de on_signal, on reçoit un des signaux
définis dans l'ensemble msk_int (à savoir dans notre cas uniquement SIGINT pour l'exemple),
alors ces signaux ne seront traités qu'après qu'on soit sorti de la fonction on_signal et
que le programme soit "débloqué/disponible".

Si plusieurs de ces signaux sont envoyés pendant l'exécution de la fonction on_signal,
seul un seul d'entre eux sera traité, car on ne compte pas le NOMBRE de fois qu'on a
reçu un signal, mais simplement SI on l'a reçu. Il s'agit d'une valeur booléenne qui est
modifiée à la réception d'un signal (true si on l'a reçu, false sinon, et lorsque
le signal est reçu et qu'on a fini de le traiter, la valeur booléenne repasse à false).

A savoir que la même structure sigaction peut être utilisée plusieurs fois, et qu'on peut
même écraser son contenu en affectant à nouveaux ses attributs comme si c'était la première
fois. C'est d'ailleurs ce que je fais dans mon programme, en replaçant notamment le masque
d'un appel à la fonction sigaction() à un autre :

```c
    sigact.sa_handler = on_signal; // définition de la fonction à exécuter à la 
// réception d'un signal

sigact.sa_mask = msk_segv; // définition du masque à appliquer
sigaction(SIGINT, &sigact, NULL); // lorsqu'on recevra un signal SIGINT, on_signal 
// sera exécuté et le masque msk_segv sera appliqué

sigact.sa_mask = msk_int; // définition d'un nouveau masque à appliquer
sigaction(SIGSEGV, &sigact, NULL);  // lorsqu'on recevra un signal SIGSEGV, on_signal
// sera exécuté et le masque msk_int sera appliqué
```

## Exercice 4

Pour lire la page man qui va nous intéresser pour la fonction ```sleep```, il faut
utiliser ```man 3 sleep```.

Le man indique :

```
sleep()  causes  the  calling  thread to sleep either until the number of real-time 
seconds specified in seconds have elapsed or until a signal arrives which is not ig‐     
nored.
```

On rappelle que le processus possède par défaut un thread appelé thread principal. Ce
dernier exécute la fonction ```main()```.

Un while(1) permet de vérifier de façon répétée et "infinie" si la condition "1" est vraie,
ce qui consomme des ressources et des cycles processeurs. On appelle alors ce type de boucles
une attente active. Pendant une attente active, le processus est bel et bien actif et consomme
des ressources CPU, et il est capable de recevoir un signal, d'exécuter un handler, et
de venir reprendre son exécution à l'endroit où il s'était arrêté à la réception du
signal.

Au contraire, un sleep(5) permet de mettre en pause le programme pour une durée de 5 secondes.
Une telle attente ne consomme pas de ressources CPU. On appelle alors attente passive une
telle attente. Dans une telle attente, le processus n'est plus actif (puisqu'il ne
consomme pas de ressources CPU). Le processus sera réveillé au bout de 5 secondes
par la réception d'un signal SIGALRM.
Lors de l'exécution d'une fonction  ```sleep()```, il se trouve que le signal est bien
reçu par le processus, qu'il exécute le handler du signal, mais que le sleep n'est pas
repris à l'endroit où il s'était arrêté. En effet, sleep est une des fonctions qui ne
sont pas reexécutées/reprises après la réception et le traitement d'un signal.
Par contre, ```sleep()``` renvoie un entier qui correspond au nombre de secondes
qu'il restait à attendre à la réception du signal qui a interrompu le sleep.

## Exercice 5

On se rend effectivement compte que si l'on compile avec l'option -ansi et que l'on ne
réarme pas le handler à chaque fois, il n'est appelé que la première fois, suivi du
comportement par défaut du signal envoyé lors des prochaines réception d'un signal.
Avec l'option de compilation -ansi, afin que le handler soit toujours valable pour un
signal, il est nécessaire de le réarmer :

```c
void on_signal(int sig){
    ...
    signal(sig, on_signal); // ici, on réarme le handler
    // DANS le handler, on définit le prochain handler pour le signal de numéro sig
    // Ainsi, on_signal sera encore le handler utilisé lors de la prochaine réception d'un signal sig
}
```

On remarque également que du côté du programme utilisant du code issu de la norme POSIX :
tst_sigaction.c, la compilation ne fonctionne pas avec l'option -ansi car tout ce qui est lié
à sigaction (que ça soit la structure ou la fonction) ne sont pas reconnus.

// PARLER ICI DE L'EFFET DE ANSI C SUR LA COMPILATION AFIN D'EXPLIQUER CES PHENOMENES

## Exercice 6

On va ici supposer que chaque processus maintient un ensemble/une table des signaux reçus,
sous la forme de booléen, avec 1 booléen par type de signal. Dans les faits, ce sont des
astucieux calculs en hexadécimal qui sont réalisés sur un nombre, mais qui, une fois
converti en binaire, nous donne des 0 à l'indice représentant le numéro du signal pas
reçu, et 1 à l'indice représentant le numéro du signal reçu.
Lorsque le processus reçoit un signal, la valeur booléenne correspondant au type de ce
signal est passé à 1 (vrai).

### Ignorer un signal

Lorsqu'on a mis en place l'ignorance des signaux SIGINT et qu'on envoie des signaux SIGINT
au programme, il se trouve qu'ils ne sont tout simplement pas traités et n'ont donc aucune
incidence sur l'exécution du programme, si ce n'est le fait que le processus doit vérifier
quel type de signal il a reçu et faire le choix de le traiter ou de l'ignorer (dans notre
cas, SIGINT est ignoré et les autres sont traités).

Quand on ignore un type de signal, en fait, l'ensemble des signaux reçus par le
processus courant n'est pas mis à jour à la réception de ce type de signal. 
Le processus va donc considérer qu'il n'a jamais reçu ce type de signal.

### Handler + mask SIGINT

Lorsqu'on met en place un handler qui sera exécuté à la réception d'un type de signal et
qu'on met aussi en place un masque permettant de bloquer les signaux de type SIGINT,
si on envoie pendant l'exécution du programme des signaux SIGINT, seul une seule instance
de signal SIGINT sera traité (comme dit dans l'exercice 3, il s'agit d'une valeur booléenne
associée à un type de signal qui est passée de false à true lorsqu'on reçoit ce type de
signal, et qui repasse à false lorsque le signal est traité (quand le handler est exécuté)).
Donc envoyer plusieurs fois un même signal lorsqu'il est bloqué revient à simplement passer
ce booléen à true une première fois, puis réaffecter encore true au booléen alors qu'il était
déjà à true, etc, autant de fois qu'on envoie ce type de signal.

Lorsqu'on met en place ce mode d'action, le type de signal bloqué est tout de même accepté
par le processus, mettant à jour la table des signaux reçus. Cependant, puisqu'il est bloqué,
on va le mettre en attente. Cela signifie que la valeur dans la table est belle et bien passée
à 1, mais qu'on va attendre le déblocage du processus afin de traiter ce type de signal.
Ainsi, puisqu'il s'agit d'un booléen qui est passé N fois à 1 (vrai), le processus ne
saura pas combien de fois il a reçu le signal, mais seulement qu'il l'a reçu.
Il va donc uniquement le traiter une seule fois, lorsqu'il sera débloqué et que le signal
ne sera plus bloqué.

En effet, dans les précédents exercices, on utilisait le mask contenu dans la structure
sigaction. Seulement, ce masque ne s'applique qu'à notre handler.

Ici, ```sigprocmask()``` permet d'appliquer un masque à tout le processus, les signaux
bloqués/masqués sont donc bloqués durant toute l'exécution du processus et ne seront traités
que s'ils sont démasqués.

C'est pourquoi on voit que seul un seul SIGINT est traité (même si on envoie N SIGINT),
et seulement après qu'on ait utilisé : 
```sigprocmask(SIG_UNBLOCK, &sigint_mask, NULL);``` permettant de démasquer un ensemble 
de signaux (dans notre cas : SIGINT).

### Handler vide

Ici, aucun signal n'est bloqué ni ignoré. On capture les SIGINT.

Lorsqu'on met en place un handler vide qui sera exécuté à la réception d'un type de signal,
les signaux du type attendu sont reçus et directement pris en compte, peu importe où l'on
se trouve. On parle de déroutage lorsqu'on va dans une autre portion de code en raison
de la réception d'un signal.
En recevant un seul signal, le processus est dérouté (son exécution courante est mise en pause,
le temps que le handler soit complètement exécuté).
Dans notre cas, on est dans le sleep(5) quand on envoie un SIGINT, ce qui a pour effet
de couper-court au sleep(5), qui va renvoyer le nombre de secondes qu'il restait à sleep
au moment où le processus a reçu un SIGINT. (On aurait eu le même résultat dans le Test 2
si on enlevait le blocage des SIGINT dans le processus).

Lorsqu'on met en place ce mode d'action, le type de signal envoyé est tout de même reçu,
pris en compte, et traité. C'est comme si on avait mis en place un vrai handler.

// Partie dont je ne suis pas sûr de moi.

Si maintenant le processus reçoit un premier signal, se déroute et exécute le handler,
et que pendant qu'il exécute le handler, il reçoit de nouveau le même type de signal et 
que ce signal n'est pas bloqué, alors il sera dérouté une nouvelle fois alors qu'il est 
dans un handler. Ce qui aura pour conséquence de reexécuter le handler, de le terminer,
puis de revenir à l'ancienne ligne où le processus se trouvait dans le handler au moment
où il a reçu le deuxième signal, puis de revenir à l'ancienne ligne où le processus se
trouvait quand il a reçu son premier signal.

// Fin de la partie où je ne suis pas sûr de moi.

### Capture sans handler

Lorsqu'on met en place un handler NULL qui sera exécuté à la réception d'un type de 
signal et qu'on reçoit ce type de signal, c'est le comportement par défaut du signal 
qui est réalisé. Autrement dit, aucun handler défini par l'utilisateur n'est appelé,
c'est comme si on n'avait pas défini de handler pour ce type de signal.

Dans notre cas, puisqu'on reçoit un SIGINT avec un handler NULL, cela interrompt le 
programme/met fin au programme, tel qu'est censé être le comportement par défaut du 
signal SIGINT.

## Exercice 7

Il serait difficile d'interchanger l'ainé et le cadet car

# Remarques et questions

On ne reçoit qu'une seule fois un signal, en fait dans un processus la réception d'un signal
est simplement un booléen, donc si on envoie plusieurs fois un même signal qu'on désire ignorer
pendant que le handler est en train de s'exécuter, on ne le recevra en fait qu'une seule
fois (booléen qui passe à VRAI) jusqu'à ce que le signal soit pris en compte lors du
déblocage du processus (processus qui s'exécute et qui n'est pas dans un handler).

## Exercice 1 :

Background job vs Foreground job - différences ?

2. Le processus relégué à l'état "suspendu" est-il prêt suspendu ou bloqué suspendu ?

## Exercice 5 :

- Pourquoi lorsqu'on compile tst_sigaction.c avec -ansi, il reconnait les fonction de gestion
  des masques de signaux et la fonction kill, alors qu'il est censé ne pas les connaître ?

- Quel effet a concrètement l'option -ansi sur le processus de compilation ?

## Exercice 6 :

N'aurait-il pas été plus judicieux d'appliquer exactement les mêmes masks à chacun des tests ?
Notamment les tests 3 et 4. Car actuellement nous comparons sur des "bases" déjà
différentes.

## Exercice 7 :

J'ai galéré pendant 1h30. J'avais le bon code au bout de 3 min, mais pour une raison qui m'est
totalement inconnue, le processus père faisait bloquer le programme. Dès lors que j'ai rajouté
3 affichages : un avant le kill, un après le kill, et un à la fin du pere, le programme
s'est mis à fonctionner. Quelle est donc cette sorcellerie ?
Sûrement quelque chose avec l'ordonnanceur, mais c'est louche.

Même avec ces affichages, le programme ne marche qu'une fois sur 2-3. Les autres fois,
il reste complètement bloqué. Why ?
