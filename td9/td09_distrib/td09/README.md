# Mini explication générale

## Côté Java

On veut pouvoir exécuter du code C/C++ dans un code Java.
On va donc utiliser l'interface JNI proposée par Java.

Cette interface propose notamment des outils permettant d'interagir avec des éléments 
contenus dans une bibliothèque partagée C/C++.

Le principe est le suivant :

On a un code Java dans lequel on a des méthodes permettant de charger une bibliothèque
partagée dans un programme : ```System.loadLibrary("libraryName");```.

Mais pour pouvoir utiliser les fonctions/méthodes définies dans la bibliothèques partagées,
il est nécessaire de dire à Java : "telle méthode existe mais sera utilisable à l'aide
d'une bibliothèque partagée". Pour cela, on utilise le mot clé ```native```, par exemple :

```java
  public static native void printCpp();
  public native String toString();
```

signifiant : "Les méthodes printCpp et toString existent mais ne sont pas définies dans
du code Java, elles seront utilisables grâce à une bibliothèque dynamique qu'on charge".

Ensuite, dans le code Java, ces méthodes s'utilisent comme des méthodes Java classiques.

Si la méthode est statique (comme printCpp), il n'est pas nécessaire de définir une
instance d'une classe pour utiliser la méthode, sinon il est nécessaire de définir une
instance de classe pour pouvoir utiliser la méthode (comme toString).

Auquel cas, les méthodes s'utiliseront ainsi :

Pour printCpp (statique) :

```java
  NomDeLaClasse.printCpp(); // pas besoin d'instancier une classe pour utiliser une de ses méthodes dans ce cas
```

&

Pour toString (non-statique) :

```java
  NomDeLaClasse obj = new NomDeLaClasse();
  obj.toString(); // Il a été nécessaire dans ce cas de définir une instance de la classe juste avant
```

Après avoir fait cela dans le code Java, nous avons terminé la partie Java.

## Côté C/C++

Bien évidemment, si l'on veut utiliser des fonctions/méthodes C/C++ dans un programme Java en les chargeant
via une bibliothèque partagée C/C++, il faut...écrire ces fonctions/méthodes dans des codes
C/C++.

Pour cela, il est nécessaire d'avoir des fichiers-code C/C++ et d'y définir des fonctions/méthodes
telles qu'elles sont définies côté Java. Evidemment, la sythaxe Java et C/C++ ne sont pas
les mêmes et il y a des choses qui changent entre les langages.
C'est pour cela qu'a été défini un ensemble d'interfaçages entre Java et C/C++, dans
un fichier nommé "jni.h" embarqué dans tous les JDK > 9. Ce fichier contient du code C/C++
et définit l'ensemble des contraintes avec lesquelles doivent être écrites des fonctions/méthodes
C/C++ afin d'être compatibles avec les prototypes/signatures Java correspondants.

En effet, toutes les fonctions/méthodes C/C++ doivent respecter un certain nombre de règles
et de conventions afin de pouvoir être utilisées depuis un code Java. "jni.h" les définit, comme
on l'a dit.
**Mais où puis-je importer jni.h ?**

En fait, il y a une grande astuce. Java est capable de produire un fichier .h contenant
tous les prototypes des méthodes définies "native" dans un code source Java.
Pour cela, on utilise : ```javac -h dossierDeDestination fichierJavaSource.java```.

Ce fichier .h va alors contenir toutes les signatures des méthodes définies comme étant "native"
dans le Java, mais...écrites en C/C++. Autrement dit, ce fichier .h contient les signatures des méthodes
côté Java telles qu'elles doivent être utilisées dans le fichier C/C++ afin d'être compatibles et fonctionner
une fois mises dans une bibliothèque partagée.

En réalité, ce fichier .h est en quelque sorte nécessaire à l'écriture du fichier C/C++ correspondant
à ce qui est défini comme "native" dans le code Java, puisque ce fichier .h est garant
de la cohérence/compatibilité entre les méthodes définies dans le Java et les méthodes
définies dans le code C/C++;

Exemple :

La méthode Java définie comme suit :

```java
  public native String toString(); // méthode avec le mot clé native
```

Après utilisation de "javac -h dossierDest MaClasse.java", on aura un fichier .h contenant :

```h
  #include <jni.h> 
  
  // jni.h est un fichier header embarqué par tous les JDK > 9 et qui contient tous les 
  // outils nécessaires pour interfaçer une méthode Java en tant que méthode C++

  JNIEXPORT jstring JNICALL toString(JNIEnv *, jobject);
  
  // signature de la méthode toString définie dans le Java, mais écrite en C++ avec les
  // conventions de jni.h dans l'objectif d'être utilisé à travers une bibliothèque 
  // partagée dans un code Java
```

Il faut maintenant définir cette méthode dans un code C/C++, on va alors avoir dans un
fichier C/C++ (dans mon cas C++) :

```cpp
    #include
"toto.h" // fichier contenant les prototypes générés par la commande javac -h

JNIEXPORT jstring JNICALL Java_HelloWorld_toString(JNIEnv *env, jobject obj) {
...
...
...
corps de la méthode
...
...
...
}
```

Dans le code C/C++, le fichier header généré par ```javac -h``` est donc garant de la
compatibilité/cohérence/vérification entre les signatures de méthodes définies côté Java
que le programme est censé recevoir, et les signatures de méthodes définies côté C++ qui
sont censées être mises dans des bibliothèques partagées afin d'être chargée côté Java.

Après avoir écrit les méthodes dans le code C++ avec les mêmes signatures
que dans le header généré par ```javac -h``` et avec les conventions de jni.h, il est
alors possible de créer une bibliothèque partagée contenant l'ensemble des méthodes et données
C++ que l'on veut pouvoir partager avec des programmes Java.

Pour cela, -fpic sur chaque fichier source, puis -shared pour créer la
bibliothèque partagée (cf. TP1).

## Quelques explications utiles sur l'API :

Dans les méthodes C/C++ de ce genre :

```cpp
  jmethodID mid = env->GetStaticMethodID(cls, "test", "(Ljava/lang/String;)V");
```

Le troisième paramètre est le descripteur de méthode, qu'on peut obtenir grâce
à la commande ```javap -s -p VotreClasseCompilee```, qui va en fait chercher "VotreClasseCompilee.class"
et en extraire les informations nécessaires.
Un descripteur de méthode contient notamment les paramètres que prend une méthode, ainsi
que son type de retour.
Les descripteurs sont de la forme : ```(quelqueChose)autreChose.```

`(quelqueChose)` sera alors l'ensemble des paramètres pris par la méthode, et `autreChose`
sera le type de retour de la méthode.

Dans l'exemple ci-dessus :

```cpp
  (quelqueChose)autreChose = (Ljava/lang/String;)V
```

On sait alors que la méthode "test" prend en paramètre une String et qu'elle renvoie
un void (V = void).
À noter que ```Lsomething``` signifie qu'on parle d'une classe Java. Dans notre cas,
la classe String. Autrement dit, on prend ici en paramètre un argument du type de la classe
String.

Bien entendu, divers types "par défaut" de Java sont définis comme suit :

* Z = boolean
* B = byte
* C = char
* S = short
* I = int
* J = long
* F = float
* D = double
* [type = tableau contenant des éléments de type "type".

# Exercices

noms des fonctions dans le cpp != nom des fonctions dans le java

## Exercice 1

### .class

Un fichier .class est issu de la réussite de la compilation d'un fichier .java (fichier
contenant du code java).
Le .class généré contient des caractères à priori étranges, et parfois, des chaînes de
caractères lisibles par l'humain. Il peut s'agir de descripteurs de méthodes, de signatures
de méthodes, en bref, tout ce qui concerne l'interface de chaque méthode.
Les caractères "illisibles" sont en fait du bytecode Java. Le bytecode Java est un flux ASCII
(càd une suite de codes ASCII (notamment ceux compris entre 000 et 015 en décimal)) qui
permet de regrouper les instructions exécutables par une JVM : autrement dit, il s'agit
du contenu des méthodes, les variables "static", etc.

Le fichier d'avoir un fichier intermédiaire .class permet au programme java d'être
compilé (donc optimisé d'avance) et rendu en un format plus adapté (le format bytecode
java) afin d'être interprété plus efficacement par la JVM.

Java -> Bytecode Java -> Assembleur

### .h

Un fichier .h a aussi automatiquement été généré suite à l'exécution de la commande ```make```

On y trouve notamment, écrites en C/C++, toutes les interfaces des méthodes Java déclarées,
avec des petits commentaires au-dessus des méthodes, indiquant dans quelle classe Java
la méthode en question a été implémentée (ou déclarée, par exemple dans une interface
Java ? je ne sais pas).

On remarque que tout en haut du fichier .h, il y a un ```#include "jni.h"```, qui est en
fait un fichier header emporté par le JDK, contenant toutes les interfaces C/C++ nécessaires à une
"compatibilité"/correspondance entre Java et C/C++.

### .so

Un fichier .so est un fichier représentant une bibliothèque partagée en C/C++.

Son contenu est du code binaire ET des données (constantes, etc) sous formes de chaînes
de caractères.
Ce contenu correspond à des fonctions, des instructions, des variables, des arguments...

Tout ce qui est contenu dans ce fichier peut être appelé par des programmes extérieurs qui
chargent une bibliothèque partagée C/C++.

## Exercice 2

Lors de l'exécution, on voit que le chargement de la bibliothèque "libHelloWorld.so" a bien
fonctionné et que la méthode :

```java
printCpp();
```

a bien affiché "World" à la suite du "Hello" affiché par ```System.out.println("Hello");```

## Exercice 3

```cpp
// Extrait de code fourni dans le sujet

JNIEXPORT void JNICALL
Java_HelloWorld_printStringToCpp(JNIEnv *env, jclass cl, jstring str)
{
// Construction d'une chaîne C/C++ à partir d'une chaîne Java
const char *cStr = env->GetStringUTFChars(str, 0);
// Affichage de la chaîne C++
printf("%s\n", cStr);
// Libération de la chaîne C++
env->ReleaseStringUTFChars(str, cStr);
}
JNIEXPORT jstring JNICALL
Java_HelloWorld_stringFromCpp(JNIEnv *env, jobject obj)
{
// Construction d'une chaîne Java à partir d'une chaîne C/C++
return env->NewStringUTF("Chaîne en C");
}
```

Dans cet extrait de code, on voit que la deuxième méthode

```cpp
JNIEXPORT jstring JNICALL Java_HelloWorld_stringFromCpp(JNIEnv *env, jobject obj) 
```

prend en deuxième paramètre un jobject, et non un jclass comme les autres méthodes
jusqu'à présent.
Le fait que la méthode prenne un jobject au lieu d'un jclass en deuxième argument
signifie que la méthode n'est pas statique et qu'on doit donc créer une instance de
la classe HelloWorld dans le main() du programme Java afin d'appeler l'équivalent Java
(```public native String stringFromCpp();```) de la méthode décrite ci-dessus en C++.

Au final, voici comment nous avons procédé pour ajouter les deux méthodes
```printStringToCpp``` et ```stringFromCpp``` :

- Nous avons ajouté deux méthodes en tant que natives dans le code Java :
    ```java 
        public static native void printStringToCpp(String str); // statique car jclass dans le C++
        public native String stringFromCpp(); // pas statique car jobject dans le C++
    ```
- Puis nous avons utilisé ces méthodes dans le main() :
  ```java
    HelloWorld hw = new HelloWorld(); // On doit créer un objet pour pouvoir appeler la méthode non-statique stringFromCpp() 
    HelloWorld.printStringToCpp("toto"); // appel statique
    String strFromCpp = hw.stringFromCpp(); // hw est une instance de HelloWorld, appel pas statique
    System.out.println(strFromCpp);
  ```
- Ensuite, nous avons régénéré le fichier HelloWorld.h grâce au makefile, ce qui a
  ajouté les signatures des deux nouvelles méthodes dans le HelloWorld.h
- Puis nous avons ajouté les deux méthodes C++ à la classe HelloWorld.cpp :
  ```cpp
  JNIEXPORT void JNICALL Java_HelloWorld_printStringToCpp(JNIEnv *env, jclass cl, jstring str)
    {
    // Construction d'une chaîne C/C++ à partir d'une chaîne Java
    const char *cStr = env->GetStringUTFChars(str,0);
    // Affichage de la chaîne C++
    printf("%s\n", cStr);
    // Libération de la chaîne C++
    env->ReleaseStringUTFChars(str,cStr);
  }
  
  JNIEXPORT jstring JNICALL Java_HelloWorld_stringFromCpp(JNIEnv *env, jobject obj)
  {
    // Construction d'une chaîne Java à partir d'une chaîne C/C++
    return env->NewStringUTF("Chaîne en C");
  }
  ```
- Et enfin nous avons régénéré la bibliothèque partagée libHelloWorld.so, toujours à
  l'aide du makefile.

## Exercice 4

### Côté Java :

```java
    public static void test(String string){
        System.out.println(string);
    }
```

On remarque qu'on n'a pas défini un prototype "native" de cette méthode dans la classe Java.
C'est normal, un prototype "native" est défini uniquement dans le cas où la méthode
n'est pas déclarée/implémentée côté Java mais qu'elle doit correspondre à la signature
d'une méthode déclarée/implémentée côté C/C++ (une méthode censée être présente dans une
bibliothèque partagée qu'on charge dans le code Java).

Ici, nous voulons faire l'inverse. Ce n'est pas le Java qui fait appel à un code C/C++,
mais un code C++ qui veut faire appel à un code Java.
Il suffit alors côté Java de définir la méthode test comme ci-dessus (c'est un exemple).

### Côté C++ :

On ajoute ce code à HelloWorld.cpp :

```cpp
JNIEXPORT void JNICALL
Java_HelloWorld_callJavaMethod(JNIEnv *env, jobject obj) {
  // Récupération d'un objet de Metaclasse
  jclass cls = env->GetObjectClass(obj);
  // Calcule de l'identificateur de "void test(String str)"
  jmethodID mid = env->GetStaticMethodID(cls,"test","(Ljava/lang/String;)V");
  if (mid == 0) {
    // Ca a planté !!!
    fprintf(stderr, "Ouille, ça a planté !");
  } else {
    // Tout va bien, l'appel peut aboutir.
    jstring str = env->NewStringUTF("Ceci est un paramètre créé en C/C++");
    env->CallVoidMethod(obj,mid,str);
  }
  return;
}
```

Quelques explications :

L'argument env présent dans toutes les méthodes "JNI" propose divers outils afin
d'interagir avec un code Java depuis le C++.
En effet, on voit notamment l'utilisation des méthodes :

```cpp
  jclass cls = env->GetObjectClass(obj);
env->GetStaticMethodID(cls, "test", "(Ljava/lang/String;)V");
```

Le premier appel permet de récupérer la classe correspondante à l'objet qui a appelé la
méthode callJavaMethod() dans le code Java.

On voit que ce premier appel nous sert dans le deuxième appel (premier argument), qui
permet de récupérer l'ID d'une méthode statique du code Java (dans notre cas la méthode
test, prenant en paramètre une String Java).

Le premier appel nous sert donc à récupérer la classe correspondante à l'objet qui a appelé
la méthode ```callObjectClass(obj)```.
Le deuxième appel permet alors de dire "Je cherche dans le code Java la méthode statique test".

Dans le code Java, la méthode test ne pourra alors être appelée que statiquement :
```HelloWorld.test(str)```, et non pas :

```cpp
  HelloWorld hw = new HelloWorld();
hw.test(str); // pas un appel statique
```

La méthode test doit donc être statique dans le code Java et avoir le même prototype
(même argument que dans le code fourni ci-dessus, c'est-à-dire une String Java).
Aucune spécification n'est donnée par rapport au type de retour, on peut alors retourner
ce qu'on veut dans la méthode test. Dans notre cas, c'est un void.

Si jamais la méthode test n'est pas statique, la méthode ```getStaticMethodID()``` va
échouer.

Il existe d'autres appels possibles avec la variable ```env```, tel que :

```cpp
  jclass cls = env->GetObjectClass(obj);
env->getMethodId(cls, "test", "(Ljava/lang/String;)V")
```

qui permet de récupérer l'ID d'une méthode test non-statique
(qui doit donc être appelée par une instance de la classe)

La méthode test doit alors être non-statique, sinon ```getMethodId()``` échouera.

Cette méthode a le même prototype que ```getStaticMethodId()```

## Exercice 5

### Côté C++ :

```cpp
  JNIEXPORT jstring JNICALL
Java_HelloWorld_toString(JNIEnv *env, jobject obj) {
char buffer[256];
// Obtention de la Metaclasse de HelloWorld
jclass cls = env->GetObjectClass(obj);
// Calcul de l'identificateur de l'attribut entier de type int
jfieldID fid = env->GetFieldID(cls, "entier", "I");
// Récupération de la valeur entière de l'attribut
int a = env->GetIntField(obj, fid);
// Modification de la valeur entière de l'attribut
env->SetIntField(obj, fid, a + 1);
// Deuxieme récupération de la valeur entière de l'attribut
a = env -> GetIntField(obj, fid);
// Génération d'une chaîne contenant la valeur de l'attribut
sprintf(buffer, "Hello [a = %d]", a);
// On retourne un objet Java de chaîne de caractères
return env->NewStringUTF(buffer);
}
```

Ce qui nous intéresse surtout sont ces lignes :

```cpp
  jfieldID fid = env->GetFieldID(cls, "entier", "I");
// Récupération de la valeur entière de l'attribut
int a = env->GetIntField(obj, fid);
// Modification de la valeur entière de l'attribut
env->SetIntField(obj, fid, a + 1);
// Deuxieme récupération de la valeur entière de l'attribut
a = env -> GetIntField(obj, fid);
```

La première ligne, à la manière de la récupération de l'ID d'une méthode, permet de
récupérer l'ID d'un attribut non-statique nommé "entier" qui est de type "I" = int.

La deuxième ligne permet de récupérer l'entier correspondant à l'ID récupéré précédemment
et de stocker sa valeur dans une variable C++ entière.

La troisième ligne permet de modifier la valeur de cet entier : pour cela, on lui donne
l'ID de l'entier à modifier ainsi que la nouvelle valeur à attribuer.

Enfin, on récupère encore une fois en C++ la valeur de la variable entière après sa
mise à jour côté Java grâce à l'ID de l'attribut entier.

Cela permet alors de mettre en évidence une "communication" entre les programmes Java et C++
lors de l'exécution du programme Java chargeant la bibliothèque partagée dans laquelle
se trouve la méthode contenant ces lignes de code.

Voici quelques étapes décrivant ce qu'il se passe :

* On produit une bibliothèque partagée contenant entre autre cette méthode.
* On exécute le programme Java.
* Le programme Java charge la bibliothèque contenant cette méthode.
* Le programme Java fait appel à cette méthode lors de son exécution.
* Cette méthode C++ va récupérer la valeur de l'entier "entier" dans le code Java et
  va mettre à jour cette valeur.
* La valeur de cet entier est mise à jour dans le programme Java toujours en cours
  d'exécution.

## Exercice 6

jclass car statique

On doit ajouter la méthode suivante à notre code C++ :

```cpp
  static int fib(int n) {
if (n < 2)
return n;
else
return fib(n-1) + fib(n-2);
}
```

Cette méthode permet de calculer la suite de fibonacci à partir d'un entier n.

On ne peut pas ajouter cette méthode telle quelle dans notre code C++ si on veut l'utiliser
dans notre programme Java.
Il faut alors adapter ce code grâce aux conventions et règles de JNI afin de rendre
compatibles cette méthode à un appel de cette méthode par un code Java.

On remarque qu'elle est **statique**, on va devoir prendre cela en compte dans notre
travail.

Une manière de procéder est d'abord d'ajouter la signature de cette méthode en tant que
méthode native dans le code Java :

```java
   public static native int fib(int n);
```

On peut ensuite générer à nouveau le fichier HelloWorld.h à partir de HelloWorld.java,
cela nous permettra d'avoir directement la signature :
```public static native int fib(int n);```, mais avec les bonnes conventions JNI pour
assurer l'interopérabilité Java/C++ : ```javac -h ./lib HelloWorld```

Dans le fichier HelloWorld.h, on retrouve alors :

```h
  JNIEXPORT jint JNICALL Java_HelloWorld_fib(JNIEnv *, jclass, jint);
```

On remarque que la méthode prend en deuxième paramètre un jclass, signifiant que cette
méthode est statique (cela correspond donc bien à ce qui est attendu pour la
méthode fib(int n), elle-même statique dans le sujet & dans le prototype Java).

Dans le code C++ implémentant le contenu de la méthode ```static int fib(int n)```, on
va donc devoir définir le contenu de la méthode fib(int n) dans une méthode ayant pour
signature :

```h
  JNIEXPORT jint JNICALL Java_HelloWorld_fib(JNIEnv *, jclass, jint);
```

On définit alors :

```cpp
JNIEXPORT int JNICALL Java_HelloWorld_fib(JNIEnv *env, jclass cl, jint n){
    if (n < 2)
        return n;
    else
        return Java_HelloWorld_fib(env, cl, n-1) + Java_HelloWorld_fib(env, cl, n-2);
}
```

Qui reproduit le code de la méthode ```static int fib(int n)``` définie dans le sujet.

Pour finir, il faut donc reconstruire la bibliothèque partagée C/C++ contenant maintenant
cette nouvelle méthode fib(int n) (grâce au makefile).

Puis, évidemment, utiliser la méthode fib(int n) dans le code Java, par exemple :

```java
  System.out.println("Fibonnaci result for fib(" + hw.entier + ") : " + HelloWorld.fib(6));
```

On voit qu'on peut faire un appel statique à la méthode fib(int n) dans le code Java,
c'est normal, puisque la méthode est définie comme étant statique, à la fois dans le C++
(avec le jclass que prend la méthode ```Java_HelloWorld_fib```, mais aussi dans le
prototype "native" Java).

## Exercice 7

Cet exercice concerne uniquement du code Java.

Cf. le code du fichier Exo1.java, présent dans les fichiers du dossier 
td9/td09_distrib/td09.

**Avant Java 9**, technique "bricolage" et qui pourrait ne pas marcher 
(bien que cela semble marcher tout le temps, mais la documentation ne définit pas le
format de la chaîne de caractère retournée par "getName()") :

```java
  public static long getProcessIDBeforeJava8(){
    return Long.parseLong(ManagementFactory.getRuntimeMXBean().getName().split("@")[0]);
  }
```

**Après Java 9**, technique beaucoup plus élégante et qui marche tout le temps :

```java
  public static long getProcessIDAfterJava8(){
    return ProcessHandle.current().pid();
  }
```

Ces deux codes permettent donc d'obtenir le PID du processus exécutant le programme.

Mais on aimerait pouvoir utiliser un appel système UNIX natif (norme POSIX) : getpid() afin
de récupérer l'ID du processus exécutant le programme.

## Exercice 8

Dans cet exercice, on doit donc créer un code C++ faisant un appel à getpid(), puis nous
devons créer une bibliothèque partagée contenant la méthode faisant cet appel getpid(),
puis nous devons créer un prototype "native" de la méthode faisant cet appel, avant de 
charger cette bibliothèque partagée dans un code Java, sans oublier ensuite de faire un 
appel à la méthode définie "native" dans le code Java.

Pour être certain de la conformité lorsqu'on va vouloir produire une bibliothèque partagée 
contenant la méthode voulue, il vaut mieux d'abord définir le prototype "native" dans le code Java,
puis produire le fichier .h correspondant qui nous donnera par la même occasion des
informations sur la manière dont nous devons définir la signature de notre méthode dans le
code C++ afin qu'il soit compatible/utilisable dans le code Java par un appel à la méthode
dans la bibliothèque partagée.

### Code Java :

```java
  public class GetPid {
    public static native long getPid(); // on définit le prototype "native" de la méthode getPid() à implémenter dans le C++.

    static {
            System.out.print("Loading Get Pid native library...");
            System.loadLibrary("GetPid"); // libGetPid.so est le nom de la bibliothèque partagée
            System.out.println("done.");
        }

       public static void main(String args[]){
               System.out.println(getPid()); // on utilise la méthode getPid()
       }
  }
```

On compile le code Java pour produire GetPid.class : ```javac -g GetPid.java```

Puis on produit "GetPid.h" : ```javac -h ./lib GetPid``` (GetPid permet d'aller chercher GetPid.class).

On va voir le contenu de GetPid.h :

```h
  JNIEXPORT jlong JNICALL Java_GetPid_getPid(JNIEnv *, jclass);
```

On va alors copier cette signature de méthode dans le code C++ qu'on veut créer (GetPid.cpp) :

```cpp
  #include <sys/types.h>
  #include <unistd.h>
  #include <stdio.h>
  #include "GetPid.h"
  
  JNIEXPORT jlong JNICALL Java_GetPid_getPid(JNIEnv *, jclass) { // pid_t = long int
      return getpid();
  }
```

Puis, on crée une bibliothèque partagée ```libGetPid.so``` contenant cette méthode :

* On doit pour cela légèrement modifier le makefile situé dans le dossier 
td9/td09_distrib/td09/lib :
```makefile 
 # on ajoute libGetPid.so à la liste des bibliothèques partagée à créer
 TARGET=libHelloWorld.so libGetPid.so
 
 all: $(TARGET)
 
 lib%.so: %.o # on ajoute lib avant %.so
	$(CC) -Wl,-soname,$@ -shared -o $@ $<
```

* Puis on utilise la commande ```make``` dans le dossier : td9/td09_distrib/td09/ afin
d'exécuter la règle "all" du makefile situé dans ce dossier-là.

Puis, il suffit d'exécuter le programme Java : ```java GetPid``` (exécute GetPid.class).

# Questions et Remarques

## Questions

D'où vient le préfixe : "Java_HelloWorld_" ?

C'est une procédure normalisée par JNI (pas un comportement choisi par M. LAVIROTTE)

## Exercice 4

Que veut dire "V" dans le descriptor des méthodes avec javap ?
-> Void

## Remarques

jclass -> statique
jobject -> pas statique

$(MAKE) -> on exécute la règle "all" d'un makefile
-C directory -> on change de répertoire courant

$(MAKE) -C directory -> On exécute la règle "all" du makefile situé dans le répertoire 
"directory"

Javap :

-s -> Prints internal type signatures.

-p -> Shows all classes and members
