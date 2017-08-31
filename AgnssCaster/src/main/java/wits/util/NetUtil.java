package wits.util;

import java.net.InetAddress;

public class NetUtil {

	public static String getHostAddress() {
		try{
            InetAddress addr = InetAddress.getLocalHost();
            return addr.getHostAddress();
        }catch(Exception e){
            return "";
        }
	}
	
	public static String getHostName() {
		try{
            InetAddress addr = InetAddress.getLocalHost();
            return addr.getHostName();
        }catch(Exception e){
            return "";
        }
	}
	
	
}
