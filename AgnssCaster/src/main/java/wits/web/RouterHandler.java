package wits.web;

import java.util.HashMap;
import java.util.Map;

import io.vertx.core.Vertx;
import io.vertx.core.json.JsonObject;
import io.vertx.core.logging.Logger;
import io.vertx.core.logging.LoggerFactory;
import io.vertx.ext.jdbc.JDBCClient;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;

/**
 * 子路由处理器
 * 
 * @author terry
 *
 */
public class RouterHandler extends RoutingContextHandler implements DataMap {
	static final Logger  logger = LoggerFactory.getLogger(RouterHandler.class);
	
	protected Vertx vertx;
	protected JsonObject config;
	protected boolean isSetup = false;
	protected Router router;
	protected Map<String, Object> dataMap = new HashMap<>();
	
	
	public RouterHandler() {
		
	}
	
	public RouterHandler(Vertx vertx, JsonObject config) {
		this.vertx = vertx;
		this.config = config;
		this.router = Router.router(vertx);
	}
	
	public RouterHandler setVertx(Vertx vertx) {
		this.vertx = vertx;
		
		this.router = Router.router(vertx);
		
		return this;
	}
	
	public RouterHandler setConfig(JsonObject config) {
		this.config = config;
		return this;
	}
	
	public JsonObject getConfig() {
		return this.config;
	}
	
	public Router getRouter() {
		if (!isSetup) {
			setupFailure();
			setupRouter();
			isSetup = true;
		}
		
		return this.router;
	}
	
	protected void setupRouter() {
		///
	}
	
	protected void setupFailure() {
		router.route().failureHandler(context ->{
			handleFailure(context);
		});
	}
	
	protected void handleFailure(RoutingContext context) {
		if (!context.response().ended()) {
			logger.warn("failureHandler. ", context.failure());
			
			end(context, CodeMessage.FAIL, context.failure().toString());
		}
	}
	
	
	
	
	

	@Override
	public DataMap put(String key, Object obj) {
		dataMap.put(key, obj);
		return this;
	}

	@Override
	@SuppressWarnings("unchecked")
	public <T> T get(String key) {
		Object obj = dataMap.get(key); 
		try {
			return (T)obj;
		} catch (Throwable t) {
			return null;
		}
	}

	@Override
	@SuppressWarnings("unchecked")
	public <T> T remove(String key) {
		Object obj = dataMap.remove(key); 
		try {
			return (T)obj;
		} catch (Throwable t) {
			return null;
		}
	}

	@Override
	public Map<String, Object> data() {
		return dataMap;
	}

	
	
	public JDBCClient getSqlClient() {
		return get("jdbc");
	}

	public RouterHandler setSqlClient(JDBCClient sqlClient) {
		put("jdbc", sqlClient);
		return this;
	}

	
	
}
