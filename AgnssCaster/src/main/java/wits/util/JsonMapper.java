package wits.util;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;

import com.fasterxml.jackson.databind.DeserializationFeature;

import io.vertx.core.json.Json;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;

public class JsonMapper {

	public static <T> T convert(JsonObject jsonObject, Class<T> clazz) {
		Json.mapper.configure(
				DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
		// Json.mapper.setSerializationInclusion(Include.NON_NULL);
		return Json.mapper.convertValue(jsonObject.getMap(), clazz);
	}

	public static <T> List<T> converts(List<JsonObject> jsonObjects, Class<T> clazz) {
		Json.mapper.configure(
				DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
		if (jsonObjects == null || jsonObjects.isEmpty()) {
			return null;
		}
		List<T> list = new ArrayList<T>();
		for (JsonObject jsonObject : jsonObjects) {
			T t = Json.mapper.convertValue(jsonObject.getMap(), clazz);
			list.add(t);
		}
		return list;
	}
	
	public static JsonObject encode(Object obj) throws IllegalArgumentException, IllegalAccessException
	{
		JsonObject jsonObject = new JsonObject();
        for (Field field : obj.getClass().getDeclaredFields()) {
            field.setAccessible(true);
            String name = field.getName();
            Object fieldObject = field.get(obj);
            if (fieldObject instanceof JsonObject) {
            	jsonObject.put(name, fieldObject);
            } else if (fieldObject instanceof JsonArray) {
            	jsonObject.put(name, fieldObject);
            } else {
            	//String value = String.valueOf(fieldObject);
            	jsonObject.put(name, fieldObject);
            }
        }
        return jsonObject;
	}
	
	
}