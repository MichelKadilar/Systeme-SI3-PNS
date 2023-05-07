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