package wits.web;

import java.util.Base64;

import io.vertx.core.AsyncResult;
import io.vertx.core.Future;
import io.vertx.core.Handler;
import io.vertx.core.Vertx;
import io.vertx.core.buffer.Buffer;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.core.logging.Logger;
import io.vertx.core.logging.LoggerFactory;
import io.vertx.ext.auth.AuthProvider;
import io.vertx.ext.auth.User;
import io.vertx.ext.auth.jdbc.JDBCAuth;
import io.vertx.ext.jdbc.JDBCClient;
import io.vertx.ext.sql.ResultSet;
import io.vertx.ext.sql.SQLConnection;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.Session;
import io.vertx.ext.web.sstore.SessionStore;

/**
 * 会话处理器, 支持登录,登出,会话检查
 * @author terry
 *
 */
public class RestSessionHandler extends RouterHandler {
	
	static final Logger  logger = LoggerFactory.getLogger(RestSessionHandler.class);
	
	protected SessionStore sessionStore;
	protected AuthProvider authProvider;
	
	public static int SESSION_TIMEOUT = 1000 * 60 * 30;
	
	public static <T extends RestSessionHandler > T create(Class<T> cls, Vertx vertx, JsonObject config, JDBCClient sqlClient, SessionStore sessionStore, JDBCAuth authProvider) {
		try {
			T t = cls.newInstance();
			t.setVertx(vertx).setConfig(config);
			t.setSqlClient(sqlClient);
			t.setSessionStore(sessionStore);
			t.setAuthProvider(authProvider);
			return t;
		} catch (Throwable t) {
			return null;
		}
	}
	
	public RestSessionHandler() {
		
	}
	
	public RestSessionHandler(Vertx vertx, JsonObject config) {
		super(vertx, config);
		
	}
	
	
	@Override
	public void handle(RoutingContext context) {
		
		
		
		context.next();
	}

	public SessionStore getSessionStore() {
		return sessionStore;
	}

	public RestSessionHandler setSessionStore(SessionStore sessionStore) {
		this.sessionStore = sessionStore;
		return this;
	}

	public AuthProvider getAuthProvider() {
		return authProvider;
	}

	public RestSessionHandler setAuthProvider(AuthProvider authProvider) {
		this.authProvider = authProvider;
		return this;
	}
	
	public void handleLogin(RoutingContext context) {
		if (context.user() != null) {
			context.next();
			return;
		}
		
		mergeBodyJson(context);
		
		String username;
		String password = "";
		
		String authorization = context.request().getHeader("Authorization");
		if (authorization != null && !authorization.isEmpty()) {
			if (authorization.startsWith("Basic ")) {
				String authString = authorization.split(" ")[1];
				authorization = new String(Base64.getDecoder().decode(authString));
				String[] parts = authorization.split(":");
				username = parts[0];
				if (parts.length > 1) {
					password = parts[1];
				}
			} else {
				JsonResponse.end(context, CodeMessage.AUTH_FAILED);
				return;
			}
		} else {
			username = context.request().params().get("username");
			password = context.request().params().get("password");
		}
		
		handleLogin(context, username, password);
	}
	
	/**
	 * 处理登录
	 * 要求参数中有 username,password
	 * @param context
	 */
	public void handleLogin(RoutingContext context, String username, String password) {
		
		JsonObject authInfo = new JsonObject().put("username", username).put("password", password);
		
		authProvider.authenticate(authInfo, ar->{
			if (ar.succeeded()) {
				User user = ar.result();
				
				String sql = "select * from User where username=?";
				JsonArray params = new JsonArray().add(username);
				executeQuery(sql, params, result->{
					JsonObject json = result.result();
					if (json != null) {
						user.principal().mergeIn(json);
						user.principal().remove("password");
					}
					
					Session session = sessionStore.createSession(SESSION_TIMEOUT);
					session.put("user", user);
					session.put("username", username);
					session.put("userJson", json);
					
					sessionStore.put(session, b->{});
					
					context.setUser(user);
					context.setSession(session);
					
					logger.info("user logined. " + username + " session: " + session.id());
					
					context.next();
					
				});
				
			} else {
				JsonResponse.end(context, CodeMessage.AUTH_FAILED);
			}
		});
	}
	
	/**
	 * 处理登出, 需要预置 handleSession
	 * @param context
	 */
	public void handleLogout(RoutingContext context) {
		Session session = context.session();
		if (session == null) {
			end(context, CodeMessage.INVALID_PARAM);
			return;
		}
		
		String username = session.get("username");
		//User user = session.get("user");
		
		sessionStore.delete(session.id(), ar->{
			
			logger.info("user logout. " + username + " session: " + session.id());
			
			context.next();
			
		});
		
	}
	
	/**
	 * 处理会话, 要求参数中有 session
	 * 在所有需要会话之前的处理器前设置
	 * @param context
	 */
	public void handleSession(RoutingContext context) {
		String sessionId = getStringParam(context, "session");
		if (sessionId == null || sessionId.isEmpty()) {
			end(context, CodeMessage.INVALID_PARAM);
			return;
		}
		
		sessionStore.get(sessionId, ar->{
			if (ar.succeeded()) {
				Session session = ar.result();
				if (session != null) {
					session.setAccessed();
					
					User user = session.get("user");
					context.setSession(session);
					context.setUser(user);
					
					context.next();
					
				} else {
					end(context, CodeMessage.SESSION_EXPIRED);
				}
			} else {
				end(context, CodeMessage.SESSION_EXPIRED);
			}
		});
		
	}
	
	protected String getUsername(RoutingContext context) {
		String username = null;
		User user = context.user();
		if (user != null)
		{
			JsonObject obj = user.principal();
			if (obj != null) {
				username = obj.getString("username");
			}
		}
		return username;
	}
	
	protected int getUserId(RoutingContext context) {
		int id = 0;
		User user = context.user();
		if (user != null)
		{
			JsonObject obj = user.principal();
			if (obj != null) {
				id = obj.getInteger("id");
			}
		}
		return id;
	}
	
	protected JsonObject getUserJson(RoutingContext context) {
		JsonObject userJson = context.get("userJson");
		if (userJson == null) {
			User user = context.user();
			if (user != null) {
				userJson = user.principal();
			}
		}
		return userJson; 
	}
	
	
	/**
	 * 将消息体的json合并到请求参数集中
	 */
	protected void mergeBodyJson(RoutingContext context) {
		try {
			Buffer buffer = context.getBody();
			if (buffer != null && buffer.length() > 0) {
				JsonObject json = buffer.toJsonObject();
				for (String field: json.fieldNames()) {
					context.request().params().add(field, json.getValue(field).toString());
				}
			}
		} catch (Throwable t) {
			t.printStackTrace();
		}
	}

	protected <T> void executeQuery(String query, JsonArray params, Handler<AsyncResult<JsonObject>> resultHandler) {
		getSqlClient().getConnection(res -> {
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
