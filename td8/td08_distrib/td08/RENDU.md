# TD8 : Gestion de la mémoire

## Question 1

Dans toute cette partie, les tailles en octets exprimées dépendent 
évidemment de si on est en 32 bits ou en 64 bits.

Je considère dans mon cas que je suis en 64 bits.

Ici, la taille de la structure est certainement de 104 octets. On va donc demander 104 octets
de mémoire à travers le malloc.

Pourquoi 104 octets ?

Ici, on peut visualiser la taille d'une structure comme étant un ensemble de blocs de 
N octets, où N = taille du **type** le plus grand de la structure.
Ici, nous aurons des blocs de 4 octets car le type ayant la taille la plus grande est "int",
qui vaut 4 octets.
Or, 100/4 = 25, donc le tableau de caractères remplit entièrement 25 blocs de 4 octets,
à quoi s'ajoute un entier prenant 1 bloc de 4 octets.

### Explications :

La taille d'une structure en C ne dépend pas uniquement de la taille de chaque attribut qui
la compose. La taille d'une structure dépend aussi de l'ordre dans lequel les attributs
sont déclarés.
Par exemple :
```c
struct Data {
    char c[3];
    int i;
    char c2;
};
```
Le détail du calcul de la taille de la structure est le suivant :

On va essayer d'avoir tous nos attributs de structure sur des espaces de même taille.
Cette taille est celle du type le plus "gros" de la structure.
(par là, j'entends que char c[3] est de type char, type char qui vaut 1 octet, alors
que le type int vaut 4 octets, double vaut 8 octets, etc).
Dans notre explication, on va alors raisonner sur ce principe.

**Dans cette structure, quel est le type ayant la taille la plus grosse ?**

Il s'agit de i, dont le type vaut 4 octets.
char c[3] vaut bien 3 octets, MAIS son **type** ne vaut qu'un octet, de même pour char c2.

On va donc essayer de placer chacun de nos attributs de haut en bas dans des blocs de 4 octets.
Si jamais l'attribut vaut moins que 4 octets, deux options s'offrent à nous :

- Ou bien l'attribut suivant + l'attribut courant <= 4 octets et OK.
- Ou bien l'attribut suivant + l'attribut courant > 4 octets, auquel cas on doit ajouter
à l'attribut courant ce qu'on appelle un **padding**, qui est simplement des octets "vides"
qui servent à occuper l'espace (jusqu'au maximum voulu) qui ne peut pas être réellement 
occupé. Puis on mettra l'attribut suivant sur un autre bloc de 4 octets, etc.

En pratique :
```c
struct Data {
    char c[3]; // 1 octet * 3
    int i; // 4 octets
    char c2; // 1 octet
};
```

On se cale sur des blocs de 4 octets, donc :
- char c[3] -> 3 octets < 4 octets, donc on va voir si 
l'attribut suivant + l'attribut courant <= 4 octets. Il se trouve que l'attribut suivant vaut 4 octets,
donc ça ne marche pas (3 octets (courant) + 4 octets (suivant) = 7 octets > 4 octets). 
On doit donc opter pour l'autre solution avec un padding. Ici, il manque 1 octet afin 
d'avoir un bloc complet de 4 octets. Donc on va ajouter un padding de 1 octet à char c[3].
Ainsi : 1 octet * 3 + 1 octet = 4 octets. Nous obtenons un bloc complet.
- int i vaut 4 octets <= 4 octets, il prend donc un bloc à lui tout seul.
- char c2 vaut 1 octet, et il n'y a pas d'attribut suivant, donc on doit remplir le bloc courant avec 
un padding de 3 octets afin d'avoir un bloc complet de 4 octets : 
1 octet (char c2) + 3 octets (padding) = 4 octets.

On aura donc pris 3 blocs de 4 octets : 3 * 4 octets = 12 octets.
Cette structure vaut donc 12 octets, et non pas 8, comme on pourrait le croire en concaténant
les tailles de chacun des éléments composant la structure.


Si maintenant on change l'ordre des attributs...

```c
struct Data {
    int i; // 4 octets
    char c[3]; // 1 octet * 3
    char c2; // 1 octet
};
```

Nos blocs seront ici toujours composés de 4 octets car le type ayant la taille la plus 
grande reste int.

Calcul de la taille de la structure (même méthode que précédemment) :

- int i : 4 octets -> 1 bloc complet. OK.
- char c[3] : 3 octets, il manque un octet. Est-ce que la taille de 
l'attribut suivant + taille de l'attribut courant <= 4 ? Oui, car char c[3] = 3 octets,
et char c2 = 1 octet. 3 octets + 1 octet = 4 octets, donc on a constitué un bloc composé
de char c[3] et de char c2. Tous nos attributs ont d'ailleurs été mis dans des blocs, donc
opération terminée.

On remarque qu'ici notre structure a une taille de 2 blocs * 4 octets = 8 octets.

L'ordre dans lequel on déclare les éléments dans une structure en C a donc une incidence
sur la taille totale de la structure.

## Question 2

Etant donné que la première zone mémoire libre ne peut contenir que 48 blocs header inclus,
et qu'il n'y a pas d'autre zone mémoire libre de taille assez grande pour accueillir les
81 blocs (80 blocs du malloc + 1 header) voulu, il est nécessaire d'agrandir l'espace mémoire
libre disponible. Pour cela, sbrk va être appelé par malloc. On utilisera alors la zone
mémoire créée par cet appel à sbrk pour contenir nos 81 blocs, puisque la zone mémoire
créée suffit à les contenir.

## Question 3

On peut demander au maximum l'utilisation/l'allocation de 47 blocs.
En effet, la taille de la première zone libre est de 48 blocs. Or, il ne faut pas oublier
que lorsqu'on donne un nombre de blocs à allouer à malloc, il va rajouter par derrière
un bloc supplémentaire pour le header, ce qui fait donc 47 blocs + 1 bloc header = 48 blocs.
Au maximum, on peut donc demander l'utilisation de 47 blocs avec malloc.


## Exercices à coder

NEXT(ptr) renvoie un pointeur vers un header de la prochaine zone libre,
censé être le header de la prochaine zone libre. Mais puisqu'on n'écrase pas
forcément TOUT le contenu de chaque bloc quand on doit les mettre à jour,
il est possible que des "anciennes/fausses" informations soient contenues
dans l'attribut ```(pointeur_courant)->info.ptr``` d'un bloc si jamais on
le prend "au hasard" sans avoir fait un parcours à partir du bloc freep.
Si un parcours est fait à partir de freep, alors normalement, il ne devrait
y avoir aucune erreur dans les blocs parcourus.

Le fait de ne pas forcément mettre tout à jour permet d'économiser du temps
d'exécution, car la mise à jour est en soi inutile, puisque les données
seront de toute façon écrasées. Par exemple quand un bloc occupé devient 
libre, il n'est pas nécessaire d'effacer les données contenues dans chacun
des blocs de la zone libérée. Il suffit d'indiquer que la zone libérée
est libre, grâce au mécanisme des pointeurs vers les headers de zones libres,
que l'on obtient par le parcours suivant :

```c
//Ce code permet de parcourir à l'infini tous les headers des zones 
// indiquées comme libres
for (prevp = freep, p = NEXT(freep);; prevp = p, p = NEXT(p)) {
} // On part de freep, et on parcours grâce à l'attribut "info.ptr"
// qui pointe vers le header de la prochaine zone libre
// Cet attribut s'obtient grâce à NEXT(pointeur_du_header_courant)
```

SIZE(ptr) renvoie le nombre de blocs de la zone courante (header inclus) 
dont le header est ptr (en fait, ptr est un pointeur vers le header de la 
zone courante).


# Questions et remarques

Page 5 : si on voulait allouer 8 blocs header compris, 
on met juste le header sur le dernier bloc ?

Page 8 : "un bloc n'est pas utilisé comme header, mais fait la taille d'un header"


# Fonctions importantes à retenir

sbrk