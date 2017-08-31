package wits.api;

import java.time.LocalDateTime;
import java.util.Map;

import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.core.logging.Logger;
import io.vertx.core.logging.LoggerFactory;
import io.vertx.ext.web.RoutingContext;
import wits.web.CodeMessage;

public class AgnssHandler extends ApiHandler {
	
	private final Logger logger = LoggerFactory.getLogger(AgnssHandler.class);

	protected Map<String, JsonObject>	dataMap;
	protected Map<String, JsonObject>	latestDataMap;
	
	public AgnssHandler() {
		super();
	}
	
	public AgnssHandler setDataStore(Map<String, JsonObject> dataMap) {
		this.dataMap = dataMap;
		return this;
	}
	
	public AgnssHandler setLatestDataStore(Map<String, JsonObject> dataMap) {
		this.latestDataMap = dataMap;
		return this;
	}
	
	
	protected void setupRouter() {
		router.route("/agnss").handler(this::handleGetData);
		router.route("/list").handler(this::handleListDevice);
	}
	
	public void handleGetData(RoutingContext context) {
		
		logger.info(context.request().absoluteURI());
		
		String client_id = optStringParam(context, "client_id", "");
		String device_id = optStringParam(context, "device_id", "");
		String protocol = optStringParam(context, "protocol", "");
		String data_type = optStringParam(context, "data_type", "");
		String gnss  = optStringParam(context, "gnss", "");
		//String pos = optStringParam(context, "pos", "");
		
		if (client_id == null || client_id.isEmpty()) {
			endResponse(context, CodeMessage.INVALID_PARAM, "need client_id");
			return;
		}
		
		if (device_id == null || device_id.isEmpty()) {
			endResponse(context, CodeMessage.INVALID_PARAM, "need device_id");
			return;
		}

		if (protocol == null || protocol.isEmpty()) {
			endResponse(context, CodeMessage.INVALID_PARAM, "need protocol");
			return;
		}
		
		if (data_type == null || data_type.isEmpty()) {
			endResponse(context, CodeMessage.INVALID_PARAM, "need data_type");
			return;
		}
		
		if (gnss == null || gnss.isEmpty()) {
			endResponse(context, CodeMessage.INVALID_PARAM, "need gnss");
			return;
		}
		
		
		if (!protocol.equals("whmx")) {
			endResponse(context, CodeMessage.INVALID_PARAM, "bad protocol");
			return;
		}
		
		String[] dataTypes = new String[] {"eph", "alm"};
		if (!checkCombine(data_type, dataTypes)) {
			endResponse(context, CodeMessage.INVALID_PARAM, "bad data_type");
			return;
		}
		
		String[] gnssList = new String[] {"gps", "bds", "glo"};
		if (!checkCombine(gnss, gnssList)) {
			endResponse(context, CodeMessage.INVALID_PARAM, "bad gnss");
			return;
		}
		
		JsonObject jsonData = getLatestData();
		
		if (jsonData == null || jsonData.isEmpty()) {
			endResponse(context, 500, CodeMessage.INVALID_DATA, "无效星历数据");
			return;
		}
		
		//JsonObject json = new JsonObject();
		
		StringBuilder agnss = new StringBuilder();
		
		String aidini = makeAidini(jsonData);
		agnss.append(aidini);
		
		boolean gps = contains(gnssList, "gps");
		boolean bds = contains(gnssList, "bds");
		
		if (gps && bds) {
			String aidini_gpsbds = jsonData.getString("aidini_gpsbds");
			String aideph_gpsbds = jsonData.getString("aideph_gpsbds");
			if (aidini_gpsbds != null) {
				agnss.append(aidini_gpsbds);
			}
			
			if (aideph_gpsbds != null) {
				agnss.append(aideph_gpsbds);
			}
		} else if (gps) {
			String aidini_gps = jsonData.getString("aidini_gps");
			String aideph_gps = jsonData.getString("aideph_gps");
			if (aidini_gps != null) {
				agnss.append(aidini_gps);
			}
			
			if (aideph_gps != null) {
				agnss.append(aideph_gps);
			}
		} else if (bds) {
			String aidini_bds = jsonData.getString("aidini_bds");
			String aideph_bds = jsonData.getString("aideph_bds");
			if (aidini_bds != null) {
				agnss.append(aidini_bds);
			}
			
			if (aideph_bds != null) {
				agnss.append(aideph_bds);
			}
		}
		
		//json.put("agnss", agnss.toString());
		
		endResponse(context, agnss.toString());
	}
	
	protected boolean checkCombine(String value, String[] valueList) {
		String[] values = value.split(",");
		for (String v: values) {
			int idx = indexOf(v, valueList);
			if (idx == -1) {
				return false;
			}
		}
		return true;
	}
	
	protected int indexOf(String value, String[] valueList) {
		int idx = 0;
		for (String v: valueList) {
			if (v.equals(value)) {
				return idx;
			}
			
			idx ++;
		}
		return -1;
	}
	
	protected boolean contains(String[] valueList, String value) {
		return indexOf(value, valueList) >= 0;
	}
	
	protected void handleListDevice(RoutingContext context) {
		JsonArray devices = new JsonArray();
		for (Map.Entry<String, JsonObject> entry: dataMap.entrySet()) {
			JsonObject json = entry.getValue().copy();
			json.put("id", entry.getKey());
			devices.add(json);
		}
		
		endResponse(context, devices.toString());
	}
	
	/**
	 * 生成 AIDINI 数据
	 * @param jsonData
	 * @return
	 */
	public String makeAidini(JsonObject jsonData) {
		StringBuilder sb = new StringBuilder();
		sb.append('$');
		
		String aidini = makeAidiniContent(jsonData);
		sb.append(aidini);
		
		int checksum = mxtChkSum_Cal(aidini);
		sb.append('*');
		String hex = Integer.toHexString(checksum);
		sb.append(hex);
		
		sb.append("\r\n");
		return sb.toString();
	}
	
	protected String makeAidiniContent(JsonObject jsonData) {
		//StringJoiner joiner = new StringJoiner(",");
		StringBuilder sb = new StringBuilder();
		sb.append("AIDINI,");
		
		sb.append(jsonData.getValue("basetime"));
		sb.append(',');
		
		sb.append(jsonData.getValue("flag"));
		sb.append(',');
		
		boolean useDataTime = false;
		if (useDataTime) {
			sb.append(jsonData.getValue("year"));
			sb.append(',');
			sb.append(jsonData.getValue("month"));
			sb.append(',');
			sb.append(jsonData.getValue("day"));
			sb.append(',');
			
			sb.append(jsonData.getValue("hour"));
			sb.append(',');
			sb.append(jsonData.getValue("minute"));
			sb.append(',');
			sb.append(jsonData.getValue("sec"));
			sb.append(',');
			
			int MilliSec = 0;
			sb.append(MilliSec);
			sb.append(',');
		} else {
			LocalDateTime dt = LocalDateTime.now();
			sb.append(dt.getYear());
			sb.append(',');
			sb.append(dt.getMonthValue());
			sb.append(',');
			sb.append(dt.getDayOfMonth());
			sb.append(',');
			
			sb.append(dt.getHour());
			sb.append(',');
			sb.append(dt.getMinute());
			sb.append(',');
			sb.append(dt.getSecond());
			sb.append(',');
			
			int MilliSec = (int)(System.currentTimeMillis() % 1000);
			sb.append(MilliSec);
			sb.append(',');
		}
		

		double secacc = 5;	//jsonData.getValue("secacc", 5);
		sb.append(secacc);
		sb.append(',');
		
		double lat = 0;	//jsonData.getValue("lat", 0);
		sb.append(lat);
		sb.append(',');
		
		double lon = 0; // jsonData.getValue("lon", 0);
		sb.append(lon);
		sb.append(',');
		
		double alt = 0; // jsonData.getValue("alt", 0); 
		sb.append(alt);
		sb.append(',');
		double posacc = 0; // jsonData.getValue("posacc", 0); 
		sb.append(posacc);
		sb.append(',');
		
		int bdsmask_low = jsonData.getInteger("bdsmask_low", 0);
		int bdsmask_high = jsonData.getInteger("bdsmask_high", 0);
		int gpsmask = jsonData.getInteger("gpsmask", 0);
		
		sb.append(toHexString(bdsmask_low));
		sb.append(',');
		sb.append(toHexString(bdsmask_high));
		sb.append(',');
		sb.append(toHexString(gpsmask));
		
		return sb.toString();
	}
	
	protected String toHexString(int value) {
		return "H" + Integer.toHexString(value);
	}
	
	public static int mxtChkSum_Cal(String str) {
		int checksum = 0;
		for (int i = 0; i < str.length(); i ++) {
			checksum = checksum ^ str.charAt(i);
		}
		return checksum;
	}
	
	
	protected JsonObject getLatestData() {
		JsonObject jsonData = null;
		try {
			if (latestDataMap.entrySet().iterator().hasNext()) {
				Map.Entry<String, JsonObject> entry = latestDataMap.entrySet().iterator().next();
				jsonData = entry.getValue();
			}
		} catch (Throwable t) {
			logger.warn(t);
		}
		return jsonData;
	}
	
	
}
