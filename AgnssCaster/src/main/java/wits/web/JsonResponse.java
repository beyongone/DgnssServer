package wits.web;

import java.util.List;

import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.RoutingContext;

public class JsonResponse {
	
	public static final String MESSAGE = "message";

	public static void end(RoutingContext context, int code, String message, JsonObject result) {
		JsonObject jsonObject = new JsonObject();
		jsonObject.put("code", code);
		jsonObject.put(MESSAGE, message);
		jsonObject.put("result", result);
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public static void end(RoutingContext context, int code, String message, JsonArray result) {
		JsonObject jsonObject = new JsonObject();
		jsonObject.put("code", code);
		jsonObject.put(MESSAGE, message);
		jsonObject.put("result", result);
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public static void end(RoutingContext context, int code, String message, String result) {
		JsonObject jsonObject = new JsonObject();
		jsonObject.put("code", code);
		jsonObject.put(MESSAGE, message);
		jsonObject.put("result", result);
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public static void end(RoutingContext context, int code, String message, List<JsonObject> result) {
		JsonArray jsonArray = new JsonArray();
		for (JsonObject jsonObject: result) {
			jsonArray.add(jsonObject);
		}
		end(context, code, message, jsonArray);
	}


	public static void end(RoutingContext context, int code, String message) {
		JsonObject jsonObject = new JsonObject();
		jsonObject.put("code", code);
		jsonObject.put(MESSAGE, message);
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public static void end(RoutingContext context, CodeMessage msg) {
		JsonObject jsonObject = msg.toJson();
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	
	public static void end(RoutingContext context, JsonObject result) {
		end(context, CodeMessage.SUCCESS, CodeMessage.findMessage(CodeMessage.SUCCESS), result);
	}
	
	public static void end(RoutingContext context, JsonArray result) {
		end(context, CodeMessage.SUCCESS, CodeMessage.findMessage(CodeMessage.SUCCESS), result);
	}
	
	public static void end(RoutingContext context, List<JsonObject> result) {
		end(context, CodeMessage.SUCCESS, CodeMessage.findMessage(CodeMessage.SUCCESS), result);
	}
	
	
	public static void end(RoutingContext context, int code) {
		end(context, code, CodeMessage.findMessage(code));
	}

	
	public static void end(RoutingContext context) {
		end(context, CodeMessage.SUCCESS, CodeMessage.findMessage(CodeMessage.SUCCESS));
	}

	
}
