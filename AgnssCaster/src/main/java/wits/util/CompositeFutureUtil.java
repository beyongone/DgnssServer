package wits.util;

import io.vertx.core.CompositeFuture;

public class CompositeFutureUtil {

	public static int getSucceededResult(CompositeFuture compFuture) {
		int idx = -1;
		for (int i = 0; i < compFuture.size(); i ++) {
			if (compFuture.succeeded(i)) {
				idx = i;
			}
		}
		return idx;
	}
	
}
