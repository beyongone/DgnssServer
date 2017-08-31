package wits.web;

import io.vertx.core.Handler;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.logging.Logger;
import io.vertx.core.logging.LoggerFactory;
import io.vertx.ext.auth.User;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.Session;
import io.vertx.ext.web.sstore.SessionStore;


public class UserSessionHandler implements Handler<RoutingContext> {

	private static final Logger logger = LoggerFactory.getLogger(UserSessionHandler.class);
	
	protected SessionStore sessionStore;
	protected String logoutUrl = "./";
	
	public UserSessionHandler(SessionStore sessionStore) {
		this.sessionStore = sessionStore;
	}
	
	@Override
	public void handle(RoutingContext context) {
		
		String uri = context.request().absoluteURI();
		
		Session session = context.session();
		if (session != null) {
			
			logger.debug(uri + " -> " + session.id());
			
			User user = context.user();
			if (user == null) {
				user = session.get("user");
				if (user != null) {
					context.setUser(user);
				}
			}
		}
		
		context.next();
	}

	
	public void handleLogout(RoutingContext context) {
		Session session = context.session();
		if (session != null) {
			session.remove("user");
		} else {
			//
		}
		
		doRedirect(context.response(), logoutUrl);
	}
	
	
	private void doRedirect(HttpServerResponse response, String url) {
		response.putHeader("location", url).setStatusCode(302).end();
	}
	
	
}
