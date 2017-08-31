package wits.util;

import io.vertx.core.http.HttpClientResponse;

public class HttpClientUtil {

	public static Long getIntHeader(HttpClientResponse response, CharSequence name) {
		Long value = null;
		String str = response.getHeader(name);
		if (str != null && !str.isEmpty()) {
			value = Long.parseLong(str);
		}
		return value;
	}
	
	public static long getContentLength(HttpClientResponse response) {
		long length = 0;
		try {
			length = getIntHeader(response, "Content-Length");
		} finally {
			//
		}
		return length;
	}
}
