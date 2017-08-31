package wits.util;

import java.io.File;

public class PathUtil {

	public static String getParentPath(String filepath) {
		return new File(filepath).getParentFile().getName();
	}
	
	public static File getParentFile(String filepath) {
		return new File(filepath).getParentFile();
	}
	
	public static boolean deleteDir(File dir) {
        if (dir.isDirectory()) {
            String[] children = dir.list();
            //递归删除目录中的子目录下
            for (int i=0; i<children.length; i++) {
                boolean success = deleteDir(new File(dir, children[i]));
                if (!success) {
                    return false;
                }
            }
        }
        // 目录此时为空，可以删除
        return dir.delete();
    }
	
	
}
