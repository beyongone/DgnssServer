package wits.util;

import java.util.StringJoiner;

import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;

public class SqlBuilder {

	
	public static String expand(JsonObject json, JsonArray params) {
		return expandSet(json, params, false);
	}
	
	public static String expandWithNull(JsonObject json, JsonArray params) {
		return expandSet(json, params, true);
	}
	
	public static String expandSet(JsonObject json, JsonArray params, boolean withNull) {
		StringJoiner joiner = new StringJoiner(",");
		for (String field: json.fieldNames()) {
			Object obj = json.getValue(field);
			if (obj != null) {
				joiner.add(field + "=?");
				params.add(obj);
			} else if (withNull) {
				joiner.add(field + "=null");
			}
		}
		return joiner.toString();
	}
	
	public static String expandValues(JsonObject json, JsonArray params, boolean withNull) {
		StringJoiner joiner = new StringJoiner(",");
		for (String field: json.fieldNames()) {
			Object obj = json.getValue(field);
			if (obj != null) {
				joiner.add("?");
				params.add(obj);
			} else if (withNull) {
				joiner.add("null");
			}
		}
		return "(" + joiner.toString() + ")";
	}
	
	public static String expandValues(JsonArray jsonArray, JsonArray params, boolean withNull) {
		StringJoiner joiner = new StringJoiner(", ");
		for (int i = 0; i < jsonArray.size(); i ++) {
			JsonObject json = null;
			try {
				json = jsonArray.getJsonObject(i);
			} catch (Throwable t) {
				continue;
			}
			
			String str = expandValues(json, params, withNull); 
			joiner.add(str);
		}
		
		return joiner.toString();
	}
	
	
	
}
