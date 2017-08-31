package wits.common;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

import io.vertx.core.json.DecodeException;
import io.vertx.core.json.JsonObject;

public class ConfigLoader {

	/**
	 * 加载json配置
	 * @param filepath	配置文件路径
	 * @param optPath	可选的配置文件路径
	 * @return
	 */
	public static JsonObject load(String filepath, String optPath) {
		JsonObject config = loadConfig(filepath);
		if (config == null) {
			config = loadConfig(optPath);
		}
		return config;
	}
	
	/**
	 * 加载 json 配置
	 * @param filename	文件名称, 可以是相对路径
	 * @return
	 */
	public static JsonObject loadConfig(String filename) {
		JsonObject conf = new JsonObject();
		try (Scanner scanner = new Scanner(new File(filename)).useDelimiter("\\A")) {
			String sconf = scanner.next();
			try {
				conf = new JsonObject(sconf);
			} catch (DecodeException e) {
				//
			}
		} catch (FileNotFoundException e) {
			//
		}
		return conf;
	}
	
		
}
