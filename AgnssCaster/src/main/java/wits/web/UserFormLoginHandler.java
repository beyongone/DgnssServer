package wits.web;


import io.vertx.core.AsyncResult;
import io.vertx.core.Future;
import io.vertx.core.Handler;
import io.vertx.core.MultiMap;
import io.vertx.core.http.HttpMethod;
import io.vertx.core.http.HttpServerRequest;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.core.logging.Logger;
import io.vertx.core.logging.LoggerFactory;
import io.vertx.ext.auth.AuthProvider;
import io.vertx.ext.auth.User;
import io.vertx.ext.jdbc.JDBCClient;
import io.vertx.ext.sql.ResultSet;
import io.vertx.ext.sql.SQLConnection;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.Session;
import io.vertx.ext.web.handler.FormLoginHandler;


public class UserFormLoginHandler implements FormLoginHandler {

	private static final Logger log = LoggerFactory.getLogger(UserFormLoginHandler.class);

	protected final AuthProvider authProvider;
	protected JDBCClient sqlClient;

	protected String usernameParam;
	protected String passwordParam;
	protected String returnURLParam;
	protected String directLoggedInOKURL;

	@Override
	public FormLoginHandler setUsernameParam(String usernameParam) {
		this.usernameParam = usernameParam;
		return this;
	}

	@Override
	public FormLoginHandler setPasswordParam(String passwordParam) {
		this.passwordParam = passwordParam;
		return this;
	}

	@Override
	public FormLoginHandler setReturnURLParam(String returnURLParam) {
		this.returnURLParam = returnURLParam;
		return this;
	}

	@Override
	public FormLoginHandler setDirectLoggedInOKURL(String directLoggedInOKURL) {
		this.directLoggedInOKURL = directLoggedInOKURL;
		return this;
	}

	public UserFormLoginHandler(AuthProvider authProvider, String usernameParam, String passwordParam,
			String returnURLParam, String directLoggedInOKURL) {
		this.authProvider = authProvider;
		this.usernameParam = usernameParam;
		this.passwordParam = passwordParam;
		this.returnURLParam = returnURLParam;
		this.directLoggedInOKURL = directLoggedInOKURL;
	}

	@Override
	public void handle(RoutingContext context) {
		HttpServerRequest req = context.request();
		if (req.method() != HttpMethod.POST) {
			context.fail(405); // Must be a POST
		} else {
			if (!req.isExpectMultipart()) {
				throw new IllegalStateException("Form body not parsed - do you forget to include a BodyHandler?");
			}
			MultiMap params = req.formAttributes();
			String username = params.get(usernameParam);
			String password = params.get(passwordParam);
			if (username == null || password == null) {
				log.warn("No username or password provided in form - did you forget to include a BodyHandler?");
				context.fail(400);
			} else {
				Session session = context.session();
				JsonObject authInfo = new JsonObject().put("username", username).put("password", password);
				authProvider.authenticate(authInfo, res -> {
					if (res.succeeded()) {
						User user = res.result();
						context.setUser(user);
						
						onLogin(context, username, user, session);
						
						if (session != null) {
							String returnURL = session.remove(returnURLParam);
							if (returnURL != null) {
								// Now redirect back to the original url
								doRedirect(req.response(), returnURL);
								return;
							}
						}
						// Either no session or no return url
						if (directLoggedInOKURL != null) {
							// Redirect to the default logged in OK page - this
							// would occur
							// if the user logged in directly at this URL
							// without being redirected here first from another
							// url
							doRedirect(req.response(), directLoggedInOKURL);
						} else {
							// Just show a basic page
							req.response().end(DEFAULT_DIRECT_LOGGED_IN_OK_PAGE);
						}
					} else {
						context.fail(403); // Failed login
					}
				});
			}
		}
	}

	private void doRedirect(HttpServerResponse response, String url) {
		response.putHeader("location", url).setStatusCode(302).end();
	}

	private static final String DEFAULT_DIRECT_LOGGED_IN_OK_PAGE = ""
			+ "<html><body><h1>Login successful</h1></body></html>";

	
	protected void onLogin(RoutingContext context, String username, User user, Session session) {
		String sql = "select * from User where username=?";
		JsonArray params = new JsonArray().add(username);
		executeQuery(sql, params, ar->{
			JsonObject json = ar.result();
			if (json != null) {
				user.principal().mergeIn(json);
				user.principal().remove("password");
			}
		});
		
		session.put("user", user);
		session.put("username", username);
	}
	
	public static FormLoginHandler create(AuthProvider authProvider, JDBCClient sqlClient) {
	    return new UserFormLoginHandler(authProvider, DEFAULT_USERNAME_PARAM, DEFAULT_PASSWORD_PARAM,
	      DEFAULT_RETURN_URL_PARAM, null).setSqlClient(sqlClient);
	}
	
	public UserFormLoginHandler setSqlClient(JDBCClient sqlClient) {
		this.sqlClient = sqlClient;
		return this;
	}
	
	protected <T> void executeQuery(String query, JsonArray params, Handler<AsyncResult<JsonObject>> resultHandler) {
		sqlClient.getConnection(res -> {
			if (res.succeeded()) {
				SQLConnection conn = res.result();
				conn.queryWithParams(query, params, queryRes -> {
					if (queryRes.succeeded()) {
						ResultSet rs = queryRes.result();
						if (rs.getNumRows() > 0) {
							resultHandler.handle(Future.succeededFuture(rs.getRows().get(0)));
						} else {
							resultHandler.handle(Future.failedFuture(queryRes.cause()));
						}
					} else {
						resultHandler.handle(Future.failedFuture(queryRes.cause()));
					}
					conn.close(closeRes -> {
					});
				});
			} else {
				resultHandler.handle(Future.failedFuture(res.cause()));
			}
		});
	}
	
}
