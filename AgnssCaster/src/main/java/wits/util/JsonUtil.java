package wits.util;

import java.util.List;
import java.util.StringJoiner;

import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;

public class JsonUtil {
	
	public enum NullOption {
		ignoreNull,
		withNull
	}

	public static String expand(JsonObject json, String eq, String delimiter, boolean withNull) {
		StringJoiner joiner = new StringJoiner(delimiter);
		for (String field: json.fieldNames()) {
			Object obj = json.getValue(field);
			if (obj != null) {
				joiner.add(field + eq + obj.toString());
			} else if (withNull) {
				joiner.add(field + eq + "null");
			}
		}
		return joiner.toString();
	}
	
	public static String expandSet(JsonObject json, JsonArray params, boolean withNull) {
		StringJoiner joiner = new StringJoiner(",");
		for (String field: json.fieldNames()) {
			Object obj = json.getValue(field);
			if (obj != null) {
				joiner.add(field + "=?");
				params.add(json.getValue(field));
			} else if (withNull) {
				joiner.add(field + "=null");
			}
		}
		return joiner.toString();
	}
	
	
	public static JsonObject filter(JsonObject json, String[] names) {
		JsonObject jsonResult = new JsonObject();
		for (String field: json.fieldNames()) {
			if (contains(names, field)) {
				jsonResult.put(field, json.getValue(field));
			} else {
				//
			}
		}
		return jsonResult;
	}
	
	public static void remove(List<JsonObject> jsonList, String field) {
		for (JsonObject json: jsonList) {
			json.remove(field);
		}
	}
	
	public static void remove(List<JsonObject> jsonList, String[] fields) {
		for (String field: fields) {
			remove(jsonList, field);
		}
	}
	
	public static void copy(JsonObject src, JsonObject dest, String[] fields) {
		for (String field: fields) {
			Object obj = src.getValue(field);
			if (obj != null) {
				dest.put(field, obj);
			}
		}
	}
	
	
	static boolean contains(String[] names, String element) {
		for (String name: names) {
			if (name.equals(element)) {
				return true;
			}
		}
		return false;
	}
	
	
	public static void replace(JsonObject fromJson, JsonObject toJson, NullOption option) {
		for (String field: fromJson.fieldNames()) {
			Object obj = fromJson.getValue(field);
			if (obj == null && option == NullOption.ignoreNull) {
				// pass
			} else {
				toJson.put(field, obj);
			}
		}
	}
	
	public static int countInt(List<JsonObject> rows, String field) {
		int total = 0;
		for (JsonObject json: rows) {
			total += json.getInteger(field, 0);
		}
		return total;
	}
	
	
}
