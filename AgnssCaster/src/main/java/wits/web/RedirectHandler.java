package wits.web;

import io.vertx.core.Handler;
import io.vertx.ext.web.RoutingContext;

public class RedirectHandler implements Handler< RoutingContext > {

	protected String redirectUri;
	
	public RedirectHandler(String redirectUri) {
		this.redirectUri = redirectUri;
	}
	
	@Override
	public void handle(RoutingContext context) {
		context.response().putHeader("location", redirectUri).setStatusCode(302).end();
	}

	
}
