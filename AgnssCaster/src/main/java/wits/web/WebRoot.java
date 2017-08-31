package wits.web;

public class WebRoot {

	public static String UPLOAD_ROOT = "/files";
	public static String DOWNLOAD_ROOT = "/download";
	
	public static String getUploadRoutePath() {
		return UPLOAD_ROOT + "/*";
	}
	
	public static String getDownloadRoutePath() {
		return DOWNLOAD_ROOT + "/*";
	}
	
	
}
