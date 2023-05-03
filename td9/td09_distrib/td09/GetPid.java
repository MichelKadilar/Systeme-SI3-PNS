public class GetPid {
    public static native long getPid();

    static {
            System.out.print("Loading Get Pid native library...");
            System.loadLibrary("GetPid");
            System.out.println("done.");
        }

       public static void main(String args[]){
               System.out.println(getPid());
       }
}