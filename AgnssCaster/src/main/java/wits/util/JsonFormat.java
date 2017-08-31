package wits.util;

import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;

public class JsonFormat {
	
	public static enum FormatFlag {
		IgnoreNull,		/// 忽略null值的字段
		WithNull
	}

	/**
	 * 将json的键串联为字符串, 比如 (a,b,c)
	 * @param json		
	 * @param delimiter		分隔符
	 * @param prefix		前缀, 为null表示没有
	 * @param postfix		后缀, 为null表示没有
	 * @return
	 */
	public static String joinKeys(JsonObject json, String delimiter, String prefix, String postfix) {
		StringBuilder sb = new StringBuilder();
		if (prefix != null) {
			sb.append(prefix);
		}
		
		int index = 0;
		for (String key: json.fieldNames()) {
			if (index > 0 && (delimiter != null)) {
				sb.append(delimiter);
			}
			
			sb.append(key);
			
			index ++;
		}
		
		if (postfix != null) {
			sb.append(postfix);
		}
		
		return sb.toString();
	}
	
	/**
	 * 将json的各个字段值串联起来, 比如: (1,2,3)
	 * @param json
	 * @param delimiter		分隔符
	 * @param prefix		前缀
	 * @param postfix		后缀
	 * @param flag			串联标记, 是否格式化null
	 * @return
	 */
	public static String joinValues(JsonObject json, String delimiter, String prefix, String postfix, FormatFlag flag) {
		StringBuilder sb = new StringBuilder();
		if (prefix != null) {
			sb.append(prefix);
		}
		
		int index = 0;
		for (String key: json.fieldNames()) {
			
			Object value = json.getValue(key);
			if (value == null) {
				if (flag == FormatFlag.IgnoreNull) {
					continue;
				}
			}
			
			if (index > 0 && (delimiter != null)) {
				sb.append(delimiter);
			}
			
			sb.append(value);
			
			index ++;
		}
		
		if (postfix != null) {
			sb.append(postfix);
		}
		
		return sb.toString();
	}
	
	/**
	 * 将json各个键值串联起来, 比如: (a=1,b=2,c=3)
	 * @param json
	 * @param equalSign		等于号, 在键,值之间. 比如"="
	 * @param delimiter		分隔符, 比如","
	 * @param prefix		前缀
	 * @param postfix		后缀
	 * @param flag			格式化标记
	 * @return
	 */
	public static String join(JsonObject json, String equalSign, String delimiter, String prefix, String postfix, FormatFlag flag) {
		StringBuilder sb = new StringBuilder();
		if (prefix != null) {
			sb.append(prefix);
		}
		
		int index = 0;
		for (String key: json.fieldNames()) {
			
			Object value = json.getValue(key);
			if (value == null) {
				if (flag == FormatFlag.IgnoreNull) {
					continue;
				}
			}
			
			if (index > 0 && (delimiter != null)) {
				sb.append(delimiter);
			}
			
			sb.append(key);
			sb.append(equalSign);
			sb.append(value);
			
			index ++;
		}
		
		if (postfix != null) {
			sb.append(postfix);
		}
		
		return sb.toString();
	}
	
	/**
	 * 将json键值串联起来, 比如: a=1,b=2,c=3
	 * @param json
	 * @param equalSign		等于号, 在键,值之间. 比如"="
	 * @param delimiter		分隔符, 比如","
	 * @param flag
	 * @return
	 */
	public static String join(JsonObject json, String equalSign, String delimiter, FormatFlag flag) {
		return join(json, equalSign, delimiter, null, null, flag);
	}
	
	/**
	 * 将json串联为SQL语句中的片段. 比如 (?,?,?)
	 * @param json
	 * @param delimiter
	 * @param prefix
	 * @param postfix
	 * @param flag
	 * @param holder
	 * @param exclude 排除某个键
	 * @param params
	 * @return
	 */
	public static String join(JsonObject json, String delimiter, String prefix, String postfix, 
			FormatFlag flag, String holder, String exclude, JsonArray params) {
		StringBuilder sb = new StringBuilder();
		if (prefix != null) {
			sb.append(prefix);
		}
		
		int index = 0;
		for (String key: json.fieldNames()) {
			
			Object value = json.getValue(key);
			if (value == null) {
				if (flag == FormatFlag.IgnoreNull) {
					continue;
				}
			}
			
			if (exclude != null && exclude.equals(key))
			{
				continue;
			}
			
			if (index > 0 && (delimiter != null)) {
				sb.append(delimiter);
			}
			
			sb.append(holder);
			params.add(value);
			
			index ++;
		}
		
		if (postfix != null) {
			sb.append(postfix);
		}
		
		return sb.toString();
	}
	
	/**
	 * 将json键值串联起来, 比如: a=?,b=?,c=?
	 * @param json
	 * @param equalSign
	 * @param delimiter
	 * @param prefix
	 * @param postfix
	 * @param flag
	 * @param holder
	 * @param exclude	排除某个键 
	 * @param params
	 * @return
	 */
	public static String join(JsonObject json, String equalSign, String delimiter, String prefix, String postfix,
			FormatFlag flag, String holder, String exclude, JsonArray params) {
		StringBuilder sb = new StringBuilder();
		if (prefix != null) {
			sb.append(prefix);
		}
		
		int index = 0;
		for (String key: json.fieldNames()) {
			
			Object value = json.getValue(key);
			if (value == null) {
				if (flag == FormatFlag.IgnoreNull) {
					continue;
				}
			}
			
			if (exclude != null && exclude.equals(key))
			{
				continue;
			}
			
			if (index > 0 && (delimiter != null)) {
				sb.append(delimiter);
			}
			
			sb.append(key);
			sb.append(equalSign);
			sb.append(holder);
			params.add(value);
			
			index ++;
		}
		
		if (postfix != null) {
			sb.append(postfix);
		}
		
		return sb.toString();
	}
	
	public static String joinSqlSet(JsonObject json, String exclude, JsonArray params) {
		return join(json, "=", ",", null, null, FormatFlag.IgnoreNull, "?", exclude, params);
	}
	
	public static String joinSqlValues(JsonObject json, String exclude, JsonArray params) {
		return join(json, ",", "(", ")", FormatFlag.IgnoreNull, "?", exclude, params);
	}
	
	
}
