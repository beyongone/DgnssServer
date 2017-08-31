package wits.web;

import io.vertx.core.Vertx;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.handler.BodyHandler;


public class RestRouterHandler extends RestSessionHandler {

	public RestRouterHandler(Vertx vertx, JsonObject config) {
		super(vertx, config);
		
	}
	
	public RestRouterHandler() {
		
	}

	@Override
	protected void setupRouter() {
		super.setupRouter();
		
		router.route().handler(BodyHandler.create());
		
	}
	

	
	
}
