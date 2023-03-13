# TD3

## Exercice 1

L'exécution de "./multiple_fork.exe | wc -l" permet de constater qu'il y a
bien 100 lignes affichées. Si on y ajoute la commande "ps -aux"
permettant de lister les processus en cours d'exécution que j'ai réalisé dans
un terminal séparé du premier, il nous est listé 11 processus du nom de
"./multiple_fork.exe", ce qui signifie qu'il y a 11 instances de notre
programme qui tournent (pseudo-) simultanément.
Un de ces processus est le père, les 10 autres sont les fils.

## Exercice 2

Il y a 11 processus du nom de "multiple_fork" lors de l'exécution,
ils ont des pids très proches en termes de valeur puisqu'il s'agit du
processus père et de ses 10 fils.
Le processus père a une valeur de PID légèrement inférieure à celle des fils,
cela s'explique par l'ordre de création des processus. Le processus père
existe avant les processus fils, il a donc un pid inférieur.
Le processus fils a alors un pid légèrement supérieur à celui du père, pour
les mêmes raisons.

## Exercice 3

Avec la commande "ps -aux", on peut observer tout d'abord que deux processus
liés à notre programme s'exécutent : le père et le fils.
Le fils mourant avant le père, le système d'exploitation nous informe que le
fils a un état "defunct" (= défunt). De même, on peut observer que sa
statistique passe à "Z+", qui signifie toujours que le processus est zombie.
Le père, quant à lui, fonctionne "normalement", puisque nous l'avons
endormi pour 5 secondes, le fils mourant avant le père. Le père a donc un
fonctionnement très similaire aux programmes précédents. Lorsque
le processus père va arriver à sa fin, il va récupérer la main sur son fils,
zombie jusqu'alors.

## Exercice 4

En utilisant la commande "ps -ef" lors de l'exécution du programme,
on se rend compte que le PPID du fils (donc le PID de son processus père)
change au cours de l'exécution, passant d'un PID proche de celui du fils, à
un PID "8" (dans mon cas), correspondant à un processus "init".

Le père mourant avant le fils, il n'attend pas la fin de son fils.
Tout processus (sauf le processus init) devant avoir un père, le processus
fils a donc été réaffecté à un processus init.
Ce processus init est en fait le "grand-père" du processus fils.
En effet, dans mon cas, le processus init était le processus père du processus
père de notre programme.
L'arborescence des processus permet donc entre autres de rattacher
des processus à d'autres processus lorsque c'est nécessaire.

## Exercice 5

On affiche le pid du père : 19218.
Après avoir fork, on fait un printf dans le fils afin d'afficher son pid.
Puis on exit le fils avec un code de sortie qui est 0, signifiant qu'il s'est
bien terminé.
Ce qu'il y a à comprendre dans ce processus fils est qu'en l'absence d'un
caractère de saut de ligne "\n" (et comme le buffer utilisé par printf
(sûrement un buffer pour la sortie standard) n'est
pas plein), l'affichage ne se fait en fait pas à la fin du printf, mais
une fois que le fils a terminé son exécution.
C'est en effet lorsque le fils meurt que fflush est automatiquement appelé
et que les caractères qui "attendaient" dans le buffer de la sortie standard
ont été affichés (ou plutôt "synchronisés" selon les termes exacts apparemment).
et qu'ensuite le buffer a été vidé.

On passe ensuite au processus parent, qui attendait la fin de son fils avec
un wait.
Ici, de même que pour le fils, on essaie de d'afficher un message avec
printf, mais sans conclure par un "\n" et en ne remplissant pas le buffer.
Par la suite, on utilise un execlp qui va exécuter le programme annexe
exec_prop-aux.c (qui ne fait qu'un printf du pid du processus qui l'exécute
et du premier argument donné au programme, à savoir le nom du programme).
Si execlp échoue, on rentre dans un bloc conditionnel qui va afficher un message
et quitter le programme.
Sinon, on affiche un message.
En réalité, ce dernier message ne va pas s'afficher si execlp fonctionne
correctement. En effet, execlp va écraser les buffers courants du programme,
mais également tout le code qui était écrit dans le programme qui était
exécuté, afin de remplacer tout cela par une nouvelle image, celui d'un nouveau
programme. Malgré tout, le pid du processus qui se fait écraser ne change pas.
execlp ne crée pas un nouveau processus mais remplace le processus courant.

Il y a ici encore un élément que je n'ai pas évoqué : le printf juste avant
l'appel à execlp.
Ce printf non plus n'affiche pas son message, car comme dit dans le cas du fils
jusqu'à rencontrer caractère de "fin" (\n par exemple) et sans buffer "plein",
printf n'affiche pas, il se contente alors de remplir le buffer des caractères
qu'on lui a donné.
Juste après l'appel à printf, le buffer contient donc notre message, mais
puisque nous appelons execlp juste après, et que le père n'est pas encore
terminé (logique, puisque nous appelons execlp...), le buffer contenant
notre message se voit écrasé par execlp.
Execlp va ensuite exécuter le programme qu'on lui donne en argument.
A la fin de ce programme, il y a bien un fflush qui sera fait par le processus,
mais puisque le buffer contenant le message initial a été écrasé par execlp,
le message est perdu.

## Exercice 6

On fait exécuter chaque commande par un fils différent produit à partir d'un fork,
afin de reproduire le comportement du Shell.
En effet, le shell reste en vie au cours de toute notre activité sur ce
dernier. Il doit donc faire exécuter par des processus fils les programmes
qu'on lui demande d'exécuter.

## Exercice 7

En exécutant ces trois commandes via execlp, on se rend compte que la commande
"cd" ne semble pas fonctionner. Cela est dû au fait que la commande "cd"
ne semble pas posséder de fichier qui lui est propre dans les repertoires
/bin contenant en général les exécutables des programmes tels que "ls", etc.
Il semble alors que "cd" soit une commande built-in du shell.

Pour remplacer le comportement de "cd", on utilise la fonction chdir.
chdir prend en paramètre un chemin, ce dernier peut être absolu s'il
commence par "/", ou relatif dans le cas contraire.
On se permet ici la combinaison de chdir avec getenv, qui permet de récupérer
le contenu d'une variable d'environnement.
Les variables d'environnement par défaut seront par exemple "HOME", "ROOT"...

Il se trouve que la commande "cd" sans argument (ou "cd ~") renvoie vers
le repertoire : "/home/$USER".
Dans mon cas, en tapant "echo $HOME" dans le terminal, j'obtiens
"/home/michelk", correspondant à mon repertoire personnel.
Ainsi, getenv("HOME"), qui est censé me renvoyer le contenu de ma variable
d'environnement "HOME", devrait alors me renvoyer "/home/michelk".

Je combine alors getenv("HOME") avec chdir, afin d'imiter le comportement
de "cd" sans argument, grâce à l'instruction : chdir(getenv("HOME")), qui
va alors permettre au programme (lors de son exécution) de se déplacer 
au niveau de "/home/michelk".

Pareillement que précédemment, on fait exécuter chaque programme par des
processus fils grâce à fork().

## Exercice 8

