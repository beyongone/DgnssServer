package wits.api;

import io.vertx.core.Vertx;
import io.vertx.core.buffer.Buffer;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.RoutingContext;
import wits.web.RestRouterHandler;

public class ApiHandler extends RestRouterHandler {

	public ApiHandler(Vertx vertx, JsonObject config) {
		super(vertx, config);
		
	}
	
	public ApiHandler() {
		super();
	}
	
	protected void endResponse(RoutingContext context, int statusCode, int code, String message) {
		if (code != 0) {
			message = code + "." + message;
		}
		Buffer buffer = Buffer.buffer(message);
		context.response()
			.setStatusCode(statusCode)
			.putHeader("content-length", String.valueOf(buffer.length()))
			.end(buffer);
	}
	
	protected void endResponse(RoutingContext context, int code, String message) {
		int statusCode = 200;
		if (code != 0) {
			statusCode = 400;
			message = code + "." + message;
		}
		Buffer buffer = Buffer.buffer(message);
		context.response()
			.setStatusCode(statusCode)
			.putHeader("content-length", String.valueOf(buffer.length()))
			.end(buffer);
	}
	
	protected void endResponse(RoutingContext context, String message) {
		Buffer buffer = Buffer.buffer(message);
		context.response()
			.setStatusCode(200)
			.putHeader("content-length", String.valueOf(buffer.length()))
			.end(message);
	}
	
	
}
