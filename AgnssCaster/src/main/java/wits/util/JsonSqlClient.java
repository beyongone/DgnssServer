package wits.util;

import java.text.MessageFormat;

import io.vertx.core.AsyncResult;
import io.vertx.core.Future;
import io.vertx.core.Handler;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.jdbc.JDBCClient;
import io.vertx.ext.sql.UpdateResult;


public class JsonSqlClient {

	public static void insert(JDBCClient sqlClient, String table, JsonObject value, Handler<AsyncResult<UpdateResult>> resultHandler) {
		JsonArray params = new JsonArray();
		String exp = JsonFormat.joinSqlSet(value, null, params);
		String sql = MessageFormat.format("insert into {0} set {1}", table, exp);
		SqlClientHelper.update(sqlClient, sql, params, resultHandler);
	}
	
	public static void replace(JDBCClient sqlClient, String table, JsonObject value, Handler<AsyncResult<UpdateResult>> resultHandler) {
		JsonArray params = new JsonArray();
		String exp = JsonFormat.joinSqlSet(value, null, params);
		String sql = MessageFormat.format("replace into {0} set {1}", table, exp);
		SqlClientHelper.update(sqlClient, sql, params, resultHandler);	
	}
	
	public static void delete(JDBCClient sqlClient, String table, JsonObject value, String key, Handler<AsyncResult<UpdateResult>> resultHandler) {
		String sql = MessageFormat.format("delete from {0} where `{1}`=?", table, key);
		JsonArray params = new JsonArray();
		params.add(value.getValue(key));
		SqlClientHelper.update(sqlClient, sql, params, resultHandler);
	}
	
	public static void delete(JDBCClient sqlClient, String table, JsonObject value, String key, String key2, Handler<AsyncResult<UpdateResult>> resultHandler) {
		String sql = MessageFormat.format("delete from {0} where `{1}`=? and `{2}`=?", table, key, key2);
		JsonArray params = new JsonArray();
		params.add(value.getValue(key));
		params.add(value.getValue(key2));
		SqlClientHelper.update(sqlClient, sql, params, resultHandler);
	}
	
	public static void update(JDBCClient sqlClient, String table, JsonObject value, String key, Handler<AsyncResult<UpdateResult>> resultHandler) {
		Object keyValue = value.getValue(key);
		if(keyValue == null) {
			resultHandler.handle(Future.failedFuture(new NullPointerException()));
			return;
		}
		
		JsonArray params = new JsonArray();
		String exp = JsonFormat.joinSqlSet(value, key, params);
		String sql = MessageFormat.format("update {0} set {1} where `{2}`=?", table, exp, key);
		
		params.add(keyValue);
		
		SqlClientHelper.update(sqlClient, sql, params, resultHandler);
	}
	
	public static void update(JDBCClient sqlClient, String table, JsonObject value, String key, String key2, Handler<AsyncResult<UpdateResult>> resultHandler) {
		value = value.copy();
		Object keyValue = value.remove(key);
		Object key2Value = value.remove(key2);
		
		JsonArray params = new JsonArray();
		String exp = JsonFormat.joinSqlSet(value, null, params);
		String sql = MessageFormat.format("update {0} set {1} where `{2}`=? and `{2}`=?", table, exp, key, key2);
		
		params.add(keyValue);
		params.add(key2Value);
		
		SqlClientHelper.update(sqlClient, sql, params, resultHandler);
	}
	
	public static void insertUpdate(JDBCClient sqlClient, String table, JsonObject value, String key, Handler<AsyncResult<UpdateResult>> resultHandler) {
		String[] keys = new String[] {key};
		insertUpdate(sqlClient, table, value, keys, resultHandler);
	}
	
	public static void insertUpdate(JDBCClient sqlClient, String table, JsonObject value, String key, String key2, Handler<AsyncResult<UpdateResult>> resultHandler) {
		String[] keys = new String[] {key, key2};
		insertUpdate(sqlClient, table, value, keys, resultHandler);
	}
	
	public static void insertUpdate(JDBCClient sqlClient, String table, JsonObject value, String[] keys, Handler<AsyncResult<UpdateResult>> resultHandler) {
		JsonArray params = new JsonArray();
		String[] segments = makeInsertUpdate(table, value, keys, params);
		String sql = MessageFormat.format("insert into {0}({1}) values({2}) on duplicate key update {3}", table, segments[0], segments[1], segments[2]);
		SqlClientHelper.update(sqlClient, sql, params, resultHandler);
	}
	
	protected static String[] makeInsertUpdate(String table, JsonObject json, String[] keys, JsonArray params) {
		StringBuilder tableBuilder = new StringBuilder();
		StringBuilder valueBuilder = new StringBuilder();
		StringBuilder setBuilder = new StringBuilder();
		
		JsonArray setParams = new JsonArray();
		
		int index = 0;
		int setIndex = 0;
		for (String field: json.fieldNames()) {
			
			Object value = json.getValue(field);
			if (value == null) {
				continue;
			}
			
			if (index > 0) {
				tableBuilder.append(',');
				valueBuilder.append(',');
			}
			
			boolean isKey = exists(keys, field);
			
			tableBuilder.append("`" + field + "`");
			valueBuilder.append('?');
			
			params.add(value);
			
			if (!isKey) {
				
				if (setIndex > 0) {
					setBuilder.append(',');
				}
				
				setBuilder.append("`" + field + "`");
				setBuilder.append('=');
				setBuilder.append('?');
				
				setParams.add(value);
				
				setIndex ++;
			}
			
			index ++;
		}
		
		params.addAll(setParams);
		
		return new String[] {tableBuilder.toString(), valueBuilder.toString(), setBuilder.toString()};
	}
	
	private static boolean exists(String[] keys, String field) {
		for (String key: keys) {
			if (field.equals(key)) {
				return true;
			}
		}
		return false;
	}
	
	
	
}
