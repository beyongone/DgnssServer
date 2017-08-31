package wits.util;

import java.util.List;
import java.util.NoSuchElementException;

import io.vertx.core.AsyncResult;
import io.vertx.core.Future;
import io.vertx.core.Handler;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.asyncsql.AsyncSQLClient;
import io.vertx.ext.jdbc.JDBCClient;
import io.vertx.ext.sql.ResultSet;
import io.vertx.ext.sql.SQLConnection;
import io.vertx.ext.sql.UpdateResult;


public class SqlClientHelper {
	
	
	
	public static void update(AsyncSQLClient sqlClient, String sql, JsonArray params, Handler<AsyncResult<UpdateResult>> updateResult) {
		sqlClient.getConnection(res->{
			handleUpdate(res, sql, params, updateResult);
		});
	}
	
	public static void query(AsyncSQLClient sqlClient, String sql, JsonArray params, Handler<AsyncResult<ResultSet>> resultSet){
		sqlClient.getConnection(res -> {
			handleQuery(res, sql, params, resultSet);
		});
	}
	
	public static void queryRow(AsyncSQLClient sqlClient, String sql, JsonArray params, Handler<AsyncResult<JsonObject>> jsonObject) {
		sqlClient.getConnection(res -> {
			handleQueryRow(res, sql, params, jsonObject);
		});
	}
	
	public static void call(AsyncSQLClient sqlClient, String sql, JsonArray params, JsonArray outputs, Handler<AsyncResult<ResultSet>> resultSet){
		sqlClient.getConnection(res->{
			handleCall(res, sql, params, outputs, resultSet);
		});
	}
	
	public static void queryRows(AsyncSQLClient sqlClient, String sql, JsonArray params, Handler<AsyncResult<List<JsonObject>>> rows) {
		sqlClient.getConnection(res -> {
			handleQueryRows(res, sql, params, rows);
		});
	}
	
	
	
	public static void update(AsyncSQLClient sqlClient, String sql, Handler<AsyncResult<UpdateResult>> updateResult){
		sqlClient.getConnection(res->{
			handleUpdate(res, sql, updateResult);
		});
	}
	
	public static void query(AsyncSQLClient sqlClient, String sql, Handler<AsyncResult<ResultSet>> resultSet){
		sqlClient.getConnection(res->{
			handleQuery(res, sql, resultSet);
		});
	}
	
	public static void queryRow(AsyncSQLClient sqlClient, String sql, Handler<AsyncResult<JsonObject>> jsonObject){
		sqlClient.getConnection(res -> {
			handleQueryRow(res, sql, jsonObject);
		});
	}
	
	public static void queryRows(AsyncSQLClient sqlClient, String sql, Handler<AsyncResult<List<JsonObject>>> rows) {
		sqlClient.getConnection(res -> {
			handleQueryRows(res, sql, rows);
		});
	}
	
	
	
	
	public static void update(JDBCClient sqlClient, String sql, JsonArray params, Handler<AsyncResult<UpdateResult>> updateResult){
		sqlClient.getConnection(res->{
			handleUpdate(res, sql, params, updateResult);
		});
	}
	
	public static void query(JDBCClient sqlClient, String sql, JsonArray params, Handler<AsyncResult<ResultSet>> resultSet){

		sqlClient.getConnection(res -> {
			handleQuery(res, sql, params, resultSet);
		});
	}
	
	public static void queryRow(JDBCClient sqlClient, String sql, JsonArray params, Handler<AsyncResult<JsonObject>> jsonObject){
		sqlClient.getConnection(res -> {
			handleQueryRow(res, sql, params, jsonObject);
		});
	}
	
	public static void call(JDBCClient sqlClient, String sql, JsonArray params, JsonArray outputs, Handler<AsyncResult<ResultSet>> resultSet){
		sqlClient.getConnection(res->{
			handleCall(res, sql, params, outputs, resultSet);
		});
	}
	
	public static void queryRows(JDBCClient sqlClient, String sql, JsonArray params, Handler<AsyncResult<List<JsonObject>>> rows) {
		sqlClient.getConnection(res -> {
			handleQueryRows(res, sql, params, rows);
		});
	}
	
	public static void update(JDBCClient sqlClient, String sql, Handler<AsyncResult<UpdateResult>> updateResult){
		sqlClient.getConnection(res->{
			handleUpdate(res, sql, updateResult);
		});
	}
	
	public static void query(JDBCClient sqlClient, String sql, Handler<AsyncResult<ResultSet>> resultSet){
		sqlClient.getConnection(res -> {
			handleQuery(res, sql, resultSet);
		});
	}
	
	public static void queryRow(JDBCClient sqlClient, String sql, Handler<AsyncResult<JsonObject>> jsonObject){
		sqlClient.getConnection(res -> {
			handleQueryRow(res, sql, jsonObject);
		});
	}
	
	
	public static void queryRows(JDBCClient sqlClient, String sql, Handler<AsyncResult<List<JsonObject>>> rows) {
		sqlClient.getConnection(res -> {
			handleQueryRows(res, sql, rows);
		});
	}
	
	
	
	public static void update(AsyncSQLClient sqlClient, String sql, JsonArray params, Handler<UpdateResult> updateResult,
			Handler<Throwable> failure) {
		sqlClient.getConnection(res->{
			handleUpdate(res, sql, params, ar->{
				if (ar.succeeded()) {
					updateResult.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void update(AsyncSQLClient sqlClient, String sql, Handler<UpdateResult> updateResult,
			Handler<Throwable> failure) {
		sqlClient.getConnection(res->{
			handleUpdate(res, sql, ar->{
				if (ar.succeeded()) {
					updateResult.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	
	public static void update(JDBCClient sqlClient, String sql, JsonArray params, Handler<UpdateResult> updateResult,
			Handler<Throwable> failure) {
		sqlClient.getConnection(res->{
			handleUpdate(res, sql, params, ar->{
				if (ar.succeeded()) {
					updateResult.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void update(JDBCClient sqlClient, String sql, Handler<UpdateResult> updateResult,
			Handler<Throwable> failure) {
		sqlClient.getConnection(res->{
			handleUpdate(res, sql, ar->{
				if (ar.succeeded()) {
					updateResult.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	
	public static void query(AsyncSQLClient sqlClient, String sql, JsonArray params, Handler<ResultSet> resultSet, Handler<Throwable> failure){
		sqlClient.getConnection(res -> {
			handleQuery(res, sql, params, ar->{
				if (ar.succeeded()) {
					resultSet.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void query(AsyncSQLClient sqlClient, String sql, Handler<ResultSet> resultSet, Handler<Throwable> failure){
		sqlClient.getConnection(res -> {
			handleQuery(res, sql, ar->{
				if (ar.succeeded()) {
					resultSet.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void query(JDBCClient sqlClient, String sql, JsonArray params, Handler<ResultSet> resultSet, Handler<Throwable> failure){
		sqlClient.getConnection(res -> {
			handleQuery(res, sql, params, ar->{
				if (ar.succeeded()) {
					resultSet.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void query(JDBCClient sqlClient, String sql, Handler<ResultSet> resultSet, Handler<Throwable> failure){
		sqlClient.getConnection(res -> {
			handleQuery(res, sql, ar->{
				if (ar.succeeded()) {
					resultSet.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void queryRow(AsyncSQLClient sqlClient, String sql, JsonArray params, Handler<JsonObject> jsonObject, Handler<Throwable> failure) {
		sqlClient.getConnection(res -> {
			handleQueryRow(res, sql, params, ar->{
				if (ar.succeeded()) {
					jsonObject.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void queryRow(AsyncSQLClient sqlClient, String sql, Handler<JsonObject> jsonObject, Handler<Throwable> failure) {
		sqlClient.getConnection(res -> {
			handleQueryRow(res, sql, ar->{
				if (ar.succeeded()) {
					jsonObject.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void queryRow(JDBCClient sqlClient, String sql, JsonArray params, Handler<JsonObject> jsonObject, Handler<Throwable> failure) {
		sqlClient.getConnection(res -> {
			handleQueryRow(res, sql, params, ar->{
				if (ar.succeeded()) {
					jsonObject.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	public static void queryRow(JDBCClient sqlClient, String sql, Handler<JsonObject> jsonObject, Handler<Throwable> failure) {
		sqlClient.getConnection(res -> {
			handleQueryRow(res, sql, ar->{
				if (ar.succeeded()) {
					jsonObject.handle(ar.result());
				} else {
					failure.handle(ar.cause());
				}
			});
		});
	}
	
	
	
	
	public static void handleUpdate(AsyncResult<SQLConnection> conn, String sql, JsonArray params, Handler<AsyncResult<UpdateResult>> updateResult){
		Future<UpdateResult> future = Future.future();
		future.setHandler(updateResult);
		
		if (conn.succeeded()) {
			SQLConnection connection = conn.result();

			connection.updateWithParams(sql, params, result -> {
				if (result.succeeded()) {
					future.complete(result.result());
				} else {
					future.fail(result.cause());
				}
			
				connection.close();
			});
		} else {
			future.fail(conn.cause());
		}
	}
	
	public static void handleQuery(AsyncResult<SQLConnection> conn, String sql, JsonArray params, Handler<AsyncResult<ResultSet>> resultSet){
		Future<ResultSet> future = Future.future();
		future.setHandler(resultSet);
		
		if (conn.succeeded()) {
			SQLConnection connection = conn.result();

			connection.queryWithParams(sql, params, resultHandler -> {
				if (resultHandler.succeeded()) {
					future.complete(resultHandler.result());
				} else {
					future.fail(resultHandler.cause());
				}
				
				connection.close();
			});

		} else {
			future.fail(conn.cause());
		}
	}
	
	public static void handleCall(AsyncResult<SQLConnection> conn, String sql, JsonArray params, JsonArray outputs, Handler<AsyncResult<ResultSet>> resultSet){
		Future<ResultSet> future = Future.future();
		future.setHandler(resultSet);
		
		if (conn.succeeded()) {
			SQLConnection connection = conn.result();

			connection.callWithParams(sql, params, outputs, resultHandler -> {
				if (resultHandler.succeeded()) {
					future.complete(resultHandler.result());
				} else {
					future.fail(resultHandler.cause());
				}
				
				connection.close();
			});

		} else {
			future.fail(conn.cause());
		}
	}
	
	public static void handleQueryRow(AsyncResult<SQLConnection> conn, String sql, JsonArray params, Handler<AsyncResult<JsonObject>> jsonObject){
		Future<JsonObject> future = Future.future();
		future.setHandler(jsonObject);
		
		handleQuery(conn, sql, params, resultSet->{
			if (resultSet.succeeded()) {
				if (resultSet.result().getNumRows() > 0) {
					future.complete(resultSet.result().getRows().get(0));
				} else {
					future.fail(new NoSuchElementException());
				}
			} else {
				future.fail(resultSet.cause());
			}
		});
	}
		
	public static void handleUpdate(AsyncResult<SQLConnection> conn, String sql, Handler<AsyncResult<UpdateResult>> updateResult){
		Future<UpdateResult> future = Future.future();
		future.setHandler(updateResult);
		
		if (conn.succeeded()) {
			SQLConnection connection = conn.result();

			connection.update(sql, result -> {
				if (result.succeeded()) {
					future.complete(result.result());
				} else {
					future.fail(result.cause());
				}
			
				connection.close();
			});
		} else {
			future.fail(conn.cause());
		}
	}
	
	public static void handleQuery(AsyncResult<SQLConnection> conn, String sql, Handler<AsyncResult<ResultSet>> resultSet){
		Future<ResultSet> future = Future.future();
		future.setHandler(resultSet);
		
		if (conn.succeeded()) {
			SQLConnection connection = conn.result();

			connection.query(sql, resultHandler -> {
				if (resultHandler.succeeded()) {
					future.complete(resultHandler.result());
				} else {
					future.fail(resultHandler.cause());
				}
				
				connection.close();
			});

		} else {
			future.fail(conn.cause());
		}
	}
	
	public static void handleQueryRow(AsyncResult<SQLConnection> conn, String sql, Handler<AsyncResult<JsonObject>> handler){
		Future<JsonObject> future = Future.future();
		future.setHandler(handler);
		
		handleQuery(conn, sql, ar->{
			if (ar.succeeded()) {
				ResultSet resultSet = ar.result();
				if (resultSet.getNumRows() > 0) {
					future.complete(resultSet.getRows().get(0));
				} else {
					future.fail(new NoSuchElementException());
				}
			} else {
				future.fail(ar.cause());
			}
		});
	}
	
	
	public static void handleQueryRows(AsyncResult<SQLConnection> conn, String sql, JsonArray params, Handler<AsyncResult<List<JsonObject>>> rows){
		Future<List<JsonObject>> future = Future.future();
		future.setHandler(rows);
		
		handleQuery(conn, sql, params, resultSet->{
			if (resultSet.succeeded()) {
				future.complete(resultSet.result().getRows());
			} else {
				future.fail(resultSet.cause());
			}
		});
	}
	
	public static void handleQueryRows(AsyncResult<SQLConnection> conn, String sql, Handler<AsyncResult<List<JsonObject>>> rows){
		Future<List<JsonObject>> future = Future.future();
		future.setHandler(rows);
		
		handleQuery(conn, sql, resultSet->{
			if (resultSet.succeeded()) {
				future.complete(resultSet.result().getRows());
			} else {
				future.fail(resultSet.cause());
			}
		});
	}
	
	
}
