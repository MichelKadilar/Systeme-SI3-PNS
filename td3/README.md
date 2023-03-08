# TD3

## Exercice 1

L'exécution de "./multiple_fork.exe | wc -l" permet de constater qu'il y a
bien 100 lignes affichées. Si on y ajoute la commande "ps -aux" 
permettant de lister les processus en cours d'exécution que j'ai réalisé dans
un terminal séparé du premier, il nous est listé 11 processus du nom de
"./multiple_fork.exe", ce qui signifie qu'il y a 11 instances de notre
programme qui tournent (pseudo-)simultanément.
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
Le fils mourant avant le père, "ps -aux" nous informe que le fils
a un état "defunct" (= défunt). De même, on peut observer que sa statistique
passe à "Z+", qui signifie toujours que le processus est zombie.
Le père, quant à lui, fonctionne "normalement", puisque nous l'avons
endormi pour 5 secondes, le fils mourant avant le père. Il a donc un
fonctionnement très similaire aux programmes précédents.

## Exercice 4

En utilisant la commande "ps -ef" lors de l'exécution du programme,
on se rend compte que le PPID du fils (donc le PID de son processus père)
change au cours de l'exécution, passant d'un PID proche de celui du fils, à
un PID "8" (dans mon cas), correspondant à un processus "init".

## Exercice 5

