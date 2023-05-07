
import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;

public class Exo1 {

    public static long getProcessIDBeforeJava9(){
        return Long.parseLong(ManagementFactory.getRuntimeMXBean().getName().split("@")[0]);
    }

    public static long getProcessIDAfterJava9(){
            return ProcessHandle.current().pid();
    }

    public static void main(String[] args){
        System.out.println("ID with old API : " + getProcessIDBeforeJava8());
        System.out.println("ID with new API : " + getProcessIDAfterJava8());
    }
}