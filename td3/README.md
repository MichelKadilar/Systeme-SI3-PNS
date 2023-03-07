# TD3

## Exercice 1

## Exercice 2

Il y a deux processus du noms de "multiple_fork", ils ont des pids très proches
en termes de valeur puisque ce sont les processus pères et fils.
Le processus père a une valeur de PID légèrement inférieure à celle du fils,
cela s'explique par l'ordre d'exécution des processus.
Le processus fils a alors un pid légèrement supérieur à celui du père.

Dans mon cas, pid du père : 628, et pid du fils : 630.

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

