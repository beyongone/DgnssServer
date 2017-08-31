package wits.util;

import io.vertx.core.json.JsonArray;

public class SqlCondition {

	protected StringBuilder sql;
	protected JsonArray params;
	
	public SqlCondition() {
		this.sql = new StringBuilder();
		this.params = new JsonArray();
	}
	
	public SqlCondition(JsonArray params) {
		this.sql = new StringBuilder();
		this.params = (params != null) ? params : new JsonArray();
	}
	
	public SqlCondition(String cond, JsonArray params) {
		this.sql = new StringBuilder();
		this.sql.append(cond);
		this.params = params;
	}
	
	public String getSql() {
		return this.sql.toString();
	}
	
	public JsonArray getParams() {
		return this.params;
	}
	
	public SqlCondition and(SqlCondition cond) {
		if (sql.length() > 0) {
			sql.append(" and ");
		}
		
		sql.append("(");
		sql.append(cond.getSql());
		sql.append(")");
		
		params.addAll(cond.params);
		
		return this;
	}
	
	public  SqlCondition or(SqlCondition cond) {
		if (sql.length() > 0) {
			sql.append(" or ");
		}
		
		sql.append("(");
		sql.append(cond.getSql());
		sql.append(")");
		
		params.addAll(cond.params);
		
		return this;
	}
	
	public SqlCondition or(String cond, Object param) {
		if (sql.length() > 0) {
			sql.append(" or ");
		}
		
		sql.append(cond);
		
		params.add(param);
		
		return this;
	}
	
	public SqlCondition and(String cond, Object param) {
		if (sql.length() > 0) {
			sql.append(" and ");
		}
		
		sql.append(cond);
		
		params.add(param);
		
		return this;
	}
	
	public int length() {
		return sql.length();
	}
	
	@Override
	public String toString() {
		return sql.toString();
	}
	
}
