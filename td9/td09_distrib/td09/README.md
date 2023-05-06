# Mini explication générale

## Côté Java
On veut pouvoir exécuter du code C/C++ dans un code Java.
On va donc utiliser l'interface JNI proposée par Java.

Cette interface propose notamment le chargement de bibliothèques dynamiques/partagées C/C++.

Le principe est le suivant :

On a un code Java dans lequel on a des méthodes permettant de charger une bibliothèque 
partagée dans un programme : ```System.loadLibrary("libraryName");```.

Mais pour pouvoir utiliser les fonctions/méthodes définies dans la bibliothèques partagées,
il est nécessaire de dire à Java : "telle méthode existe mais sera importée à l'aide 
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
    #include "toto.h" // fichier contenant les prototypes générés par la commande javac -h

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



##  Exercice 6

jclass car statique

# Questions et Remarques

jclass -> statique
jobject -> pas statique
 
$(MAKE) -> on exécute le makefile
-C directory -> on change de répertoire courant

$(MAKE) -C directory -> On exécute des commandes du makefile en changeant
de répertoire courant

Javap :
-s  -> Prints internal type signatures. 

-p -> Shows all classes and members 

## Exercice 4

Que veut dire "V" dans le descriptor des méthodes avec javap ?




dans le .h est produit les signatures des méthodes qui sont ajoutées en tant que "native"
dans le java.