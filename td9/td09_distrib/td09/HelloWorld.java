/**
 * Created by lavirott on 13/05/2017, modified 27/04/2023.
 */
import java.util.Scanner;

public class HelloWorld {

    private int entier; // on aurait pu le rendre public et statique et donc avoir directement accès à la variable dans
    // le main

    // Méthodes natives fournies grâce à une bibliothèque
    public static native void printCpp();
    public static native void printStringToCpp(String str);
    public native String stringFromCpp();
    public native void callJavaMethod();
    public native String toString();
    public static native int fib(int n);

    static {
        System.out.print("Loading Hello World native library...");
        System.loadLibrary("HelloWorld");
        System.out.println("done.");
    }

    public static void main(String args[]) {
        HelloWorld hw = new HelloWorld();
        // Print from Java and from C/C++
        System.out.print("Hello ");
        HelloWorld.printCpp();
        HelloWorld.printStringToCpp("toto");
        String strFromCpp = hw.stringFromCpp();
        System.out.println(strFromCpp);
        hw.callJavaMethod();
        hw.entier = 5;
        System.out.println(hw.toString());
        System.out.println("Fibonnaci result for fib(" + hw.entier + ") : " + HelloWorld.fib(6));

        Scanner sc = new Scanner(System.in);
        System.out.println("Veuillez appuyer sur Entrée pour terminer le programme...");
        sc.nextLine();
        sc.close();
    }

    public static void test(String string){
        System.out.println(string);
    }
}
