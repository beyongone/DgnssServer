package wits.AgnssCaster;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.zeromq.ZMQ;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.AsyncResult;
import io.vertx.core.DeploymentOptions;
import io.vertx.core.Future;
import io.vertx.core.Handler;
import io.vertx.core.Vertx;
import io.vertx.core.http.HttpServer;
import io.vertx.core.json.JsonObject;
import io.vertx.core.logging.Logger;
import io.vertx.core.logging.LoggerFactory;
import io.vertx.ext.auth.jdbc.JDBCAuth;
import io.vertx.ext.jdbc.JDBCClient;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.handler.BodyHandler;
import io.vertx.ext.web.handler.CookieHandler;
import io.vertx.ext.web.handler.CorsHandler;
import io.vertx.ext.web.handler.SessionHandler;
import io.vertx.ext.web.handler.StaticHandler;
import io.vertx.ext.web.sstore.LocalSessionStore;
import io.vertx.ext.web.sstore.SessionStore;
import wits.api.AgnssHandler;
import wits.api.ApiHandler;
import wits.common.ConfigLoader;


/**
 * Hello world!
 *
 */
public class App  extends AbstractVerticle {
	
	private final Logger logger = LoggerFactory.getLogger(App.class);
	
	public SessionStore sessionStore;
	public JDBCClient sqlClient;
	public JDBCAuth authProvider;
	private ZMQ.Socket	socket;
	
	private Map<String, JsonObject> dataMap = new ConcurrentHashMap<String, JsonObject>();
	private Map<String, JsonObject> latestDataMap = new ConcurrentHashMap<String, JsonObject>();
	private long	dataUpdatedTime = System.currentTimeMillis();	/// 数据更新时间
	
	
	public static void main(String[] args) {
		JsonObject config = ConfigLoader.load("src/main/conf/AgnssCaster.json", "AgnssCaster.json");
		
		App app = new App();
		Vertx vertx = Vertx.vertx();
		
		DeploymentOptions options = new DeploymentOptions();
		options.setConfig(config);
		vertx.deployVerticle(app, options);
	}
	
	@Override
	public void start() {
		
		logger.info("AgnssCaster {0} ", "1.0.0");
		
		setupDb();
		
		setupWeb(ar->{
			if (ar.succeeded()) {
				test();
			} else {
				vertx.close();
			}
		});
		
		setupZeroMQ();
	}
	
	@Override
	public void stop() {
		
		socket.close();	
	}
	
	
	private void setupWeb(Handler<AsyncResult<Void>> handler) {
		Future<Void> future = Future.future();
		future.setHandler(handler);
		
		HttpServer server = vertx.createHttpServer();
		Router router = Router.router(vertx);
		
		CorsHandler corsHandler = CorsHandler.create("*")
				.allowedMethod(io.vertx.core.http.HttpMethod.GET)
				.allowedMethod(io.vertx.core.http.HttpMethod.POST)
				.allowedMethod(io.vertx.core.http.HttpMethod.OPTIONS)
				.allowedMethod(io.vertx.core.http.HttpMethod.DELETE)
				.allowCredentials(true)
				.allowedHeader("Access-Control-Allow-Method")
				.allowedHeader("Access-Control-Allow-Origin")
				.allowedHeader("Access-Control-Allow-Credentials")
				.allowedHeader("Authorization")
				.allowedHeader("Content-Type");
			
		router.route().handler(corsHandler);
	
		router.route("/static/*").handler(StaticHandler.create("webroot/static")
				.setDirectoryListing(true));
		
		
		// We need a cookie handler first
		router.route().handler(CookieHandler.create());
		
		sessionStore = LocalSessionStore.create(vertx);
		SessionHandler sessionHandler = SessionHandler.create(sessionStore).setNagHttps(false);
		
		// Make sure all requests are routed through the session handler too
		router.route().handler(sessionHandler);
		
		router.route().handler(BodyHandler.create());
		
		//authProvider = JdbcMd5Auth.create(sqlClient);

		
		
		/// api
		AgnssHandler agnssHandler = createApiHandler(AgnssHandler.class);
		agnssHandler.setDataStore(dataMap);
		agnssHandler.setLatestDataStore(latestDataMap);
		router.mountSubRouter("/v1/device", agnssHandler.getRouter());
		
		/// static
		router.route().handler(StaticHandler.create());
		
		
		int port = config().getInteger("http.port", 8000);
		server.requestHandler(router::accept).listen(port, ar->{
			if (ar.succeeded()) {
				logger.info("start http " + port);
				future.complete();
			} else {
				future.fail("failed to listen " + port);
			}
		});
		
	}
	
	private void setupDb() {
		String dbHost = config().getString("db.host", "localhost");
		int dbPort = config().getInteger("db.port", 3306);
		String dbSchema = config().getString("db.name", "");
		String dbUser = config().getString("db.user", "root");
		String dbPassword = config().getString("db.password", "456123");
		
		//"jdbc:mysql://localhost:3306/sso?useSSL=false"
		String jdbcUrl = "jdbc:mysql://" + dbHost + ":" + dbPort + "/" + dbSchema + "?useSSL=false&useUnicode=true&characterEncoding=UTF-8";
		
		JsonObject jdbcConfig = new JsonObject();
		jdbcConfig.put("url", jdbcUrl);
		jdbcConfig.put("user", dbUser);
		jdbcConfig.put("password", dbPassword);
		
		sqlClient = JDBCClient.createShared(vertx, jdbcConfig);
		
		testDb();
	}
	
	
	private <T extends ApiHandler > T createApiHandler(Class<T> cls) {
		try {
			T t = cls.newInstance();
			t.setVertx(vertx).setConfig(config());
			t.setSqlClient(sqlClient);
			t.setSessionStore(sessionStore);
			t.setAuthProvider(authProvider);
			return t;
		} catch (Throwable t) {
			return null;
		}
	}
	
	void testDb() {
	}
	
	void test() {
		
	}

	/**
	 * 
	 */
	void setupZeroMQ() {
		ZMQ.Context ctx = ZMQ.context(1);

		String endpoint = config().getString("server", "tcp://127.0.0.1:5000");
		logger.info("connect to server " + endpoint);
		
		socket = ctx.socket(ZMQ.SUB);
		socket.connect(endpoint);
		
		socket.subscribe(new String().getBytes());
	
		vertx.setPeriodic(1000, timerId->{
			byte[] msg = socket.recv(1);
			if (msg != null && msg.length > 0) {
				JsonObject json = parseAssistData(msg);
				handleAssistData(json);
			}
		});

		vertx.setPeriodic(1000, timerId->{
			checkAndClearDataMap();
		});
	}
	
	JsonObject parseAssistData(byte[] msg) {
		logger.info("receive data. length: {0}", msg.length);
		
		String str = new String(msg);
		JsonObject json = new JsonObject(str);
		
		return json;
	}
	
	
	void handleAssistData(JsonObject json) {
		if (json == null) {
			return;
		}
		
		String device = json.getString("device");
		dataMap.put(device, json);
		
		latestDataMap.put("", json);
		
		dataUpdatedTime = System.currentTimeMillis();
		
		logger.info("device {0} data: {1}", device, json.toString());
		
//		AgnssHandler handler = new AgnssHandler();
//		String aidini = handler.makeAidini(json);
//		System.out.println(aidini);
		
	}
	
	void checkAndClearDataMap() {
		long ms = System.currentTimeMillis();
		long elapse = ms - dataUpdatedTime;
		int timeout = config().getInteger("timeout", 3600);
		if (elapse > (timeout * 1000)) {
			if (latestDataMap.size() > 0) {
				dataMap.clear();
				latestDataMap.clear();
				
				logger.info("data cleared because of timeout.");
			}
		}
	}
	
	
	
}
