package wits.web;

import java.util.List;

import io.vertx.core.Handler;
import io.vertx.core.buffer.Buffer;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.RoutingContext;

/**
 * 基本的处理器, 提供 JSON 返回, 参数获取
 * 
 * @author terry
 *
 */
public class RoutingContextHandler implements Handler< RoutingContext > {

	public static final String MESSAGE = "message";
	
	
	@Override
	public void handle(RoutingContext context) {
		context.next();
	}

	public void end(RoutingContext context, int code, String message, JsonObject result) {
		JsonObject jsonObject = new JsonObject();
		jsonObject.put("code", code);
		jsonObject.put(MESSAGE, message);
		jsonObject.put("result", result);
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public void end(RoutingContext context, int code, String message, JsonArray result) {
		JsonObject jsonObject = new JsonObject();
		jsonObject.put("code", code);
		jsonObject.put(MESSAGE, message);
		jsonObject.put("result", result);
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public void end(RoutingContext context, int code, String message, String result) {
		JsonObject jsonObject = new JsonObject();
		jsonObject.put("code", code);
		jsonObject.put(MESSAGE, message);
		jsonObject.put("result", result);
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public void end(RoutingContext context, int code, String message, List<JsonObject> result) {
		JsonArray jsonArray = new JsonArray();
		for (JsonObject jsonObject: result) {
			jsonArray.add(jsonObject);
		}
		end(context, code, message, jsonArray);
	}


	public void end(RoutingContext context, int code, String message) {
		JsonObject jsonObject = new JsonObject();
		jsonObject.put("code", code);
		jsonObject.put(MESSAGE, message);
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public void end(RoutingContext context, CodeMessage msg) {
		JsonObject jsonObject = msg.toJson();
		context.response()
			.putHeader("content-type", "application/json; charset=utf-8")
			.end(jsonObject.toString());
	}
	
	public void end(RoutingContext context, JsonObject result) {
		end(context, CodeMessage.SUCCESS, CodeMessage.findMessage(CodeMessage.SUCCESS), result);
	}
	
	public void end(RoutingContext context, JsonArray result) {
		end(context, CodeMessage.SUCCESS, CodeMessage.findMessage(CodeMessage.SUCCESS), result);
	}
	
	public void end(RoutingContext context, List<JsonObject> result) {
		end(context, CodeMessage.SUCCESS, CodeMessage.findMessage(CodeMessage.SUCCESS), result);
	}
	
	
	public void end(RoutingContext context, int code) {
		end(context, code, CodeMessage.findMessage(code));
	}

	
	public void end(RoutingContext context) {
		end(context, CodeMessage.SUCCESS, CodeMessage.findMessage(CodeMessage.SUCCESS));
	}
	
	public void end(RoutingContext context, Throwable t) {
		if (t != null) {
			end(context, CodeMessage.FAIL, t.toString());
		} else {
			end(context, CodeMessage.FAIL);
		}
	}
	
	

	protected JsonObject getBodyAsJson(RoutingContext context) {
		Buffer body = context.getBody();
		if (body != null) {
			return body.toJsonObject();
		}
		return null;
	}
	
	
	
	
	
	public String optStringParam(RoutingContext context, String key, String defValue) {
		String value = context.request().getParam(key);
		return (value != null) ? value : defValue;
	}
	
	public int optIntParam(RoutingContext context, String key, int defValue) {
		try {
			String param = context.request().getParam(key);
			return Integer.parseInt(param);
		} catch (Exception ex) {
			return defValue;
		}
	}
	
	public long optLongParam(RoutingContext context, String key, long defValue) {
		try {
			String param = context.request().getParam(key);
			return Long.parseLong(param);
		} catch (Exception ex) {
			return defValue;
		}
	}
	
	public boolean optBooleanParam(RoutingContext context, String key, boolean defValue) {
		try {
			String param = context.request().getParam(key);
			return Boolean.parseBoolean(param);
		} catch (Exception ex) {
			return defValue;
		}
	}
	
	public float optFloatParam(RoutingContext context, String key, float defValue) {
		try {
			String param = context.request().getParam(key);
			return Float.parseFloat(param);
		} catch (Exception ex) {
			return defValue;
		}
	}
	
	public double optDoubleParam(RoutingContext context, String key, double defValue) {
		try {
			String param = context.request().getParam(key);
			return Double.parseDouble(param);
		} catch (Exception ex) {
			return defValue;
		}
	}
	
	public boolean containsParam(RoutingContext context, String key) {
		return context.request().params().contains(key);
	}
	
	
	public String getParam(RoutingContext context, String key) {
		return context.request().getParam(key);
	}
	
	public String getStringParam(RoutingContext context, String key) {
		return context.request().getParam(key);
	}
	
	public Integer getIntegerParam(RoutingContext context, String key) {
		String param = context.request().getParam(key);
		if (param == null) {
			return null;
		}
		
		try {
			return Integer.valueOf(param); 
		} catch (Throwable t) {
			return null;
		}
	}
	
	public Long getLongParam(RoutingContext context, String key) {
		String param = context.request().getParam(key);
		if (param == null) {
			return null;
		}
		
		try {
			return Long.valueOf(param); 
		} catch (Throwable t) {
			return null;
		}
	}
	
	public Boolean getBooleanParam(RoutingContext context, String key) {
		String param = context.request().getParam(key);
		if (param == null) {
			return null;
		}
		
		try {
			return Boolean.valueOf(param); 
		} catch (Throwable t) {
			return null;
		}
	}
	
	public Float getFloatParam(RoutingContext context, String key) {
		String param = context.request().getParam(key);
		if (param == null) {
			return null;
		}
		
		try {
			return Float.valueOf(param); 
		} catch (Throwable t) {
			return null;
		}
	}
	
	public Double getDoubleParam(RoutingContext context, String key) {
		String param = context.request().getParam(key);
		if (param == null) {
			return null;
		}
		
		try {
			return Double.valueOf(param); 
		} catch (Throwable t) {
			return null;
		}
	}
	
	
	
	
}
