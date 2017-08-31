package wits.web;

import io.vertx.core.json.JsonObject;

public class CodeMessage {

	private int code;
	private String message;
	private Throwable cause;
	
	public CodeMessage() {
		this.setCode(SUCCESS);
		this.setMessage(findMessage(code));
	}
	
	public CodeMessage(int code) {
		this.setCode(code);
		this.setMessage(findMessage(code));
	}
	
	public CodeMessage(int code, String message) {
		this.setCode(code);
		this.setMessage(message);
	}
	
	public CodeMessage(int code, Throwable t) {
		this.setCode(code);
		this.setCause(t);
		if (t != null) {
			this.setMessage(t.toString());
		} else {
			this.setMessage(findMessage(code));
		}
	}
	
	public CodeMessage(int code, String message, Throwable t) {
		this.setCode(code);
		this.setMessage(message);
		this.setCause(t);
	}
	
	
	@Override
	public String toString() {
		return toJson().toString();
	}
	
	public JsonObject toJson() {
		JsonObject obj = new JsonObject();
		obj.put("code", getCode());
		obj.put("desc", getMessage());
		return obj;
	}


	public int getCode() {
		return code;
	}

	public void setCode(int code) {
		this.code = code;
	}

	public String getMessage() {
		return message;
	}

	public void setMessage(String message) {
		this.message = message;
	}
	
	public Throwable cause() {
		return cause;
	}

	public void setCause(Throwable cause) {
		this.cause = cause;
	}
	
	
	
	public static final int SUCCESS = 0;		/// 成功
	public static final int FAIL = 1;			/// 失败
	public static final int ALREADY_EXIST = 2;	/// 对象已经存在
	public static final int NO_ENTRY = 3;		/// 对象不存在
	
	public static final int INVALID_PARAM = 400; 	/// 无效参数
	public static final int FORBIDDEN = 403;
	
	public static final int INVALID_DATA = 411;
	
	public static final int SERVER_ERROR = 500;		/// 服务器错误
	
	public static final int AUTH_FAILED = 3001;		/// 认证失败
	public static final int NO_AUTH = 3002;			/// 缺少认证
	public static final int CALL_SPEED_LIMIT = 3003;	/// 调用太频繁
	
	
	public static final int DB_NO_CONNECTION = 3100;	/// 无效数据连接
	public static final int DB_ERROR	= 3101;		/// 数据库操作错误
	
	public static final int INVALID_SMS = 3102;		/// 无效的验证码
	
	public static final int OPERATION_FAILED = 3200;	/// 操作失败
	
	/// device
	public static final int NO_SUCH_DEVICE = 3300;		/// 设备不存在
	public static final int DEVICE_OFFLINE = 3301;		/// 设备不在线
	
	/// user
	public static final int NO_SUCH_USER = 3400;		/// 用户不存在
	public static final int NAME_PASSWORD_ERROR = 3401;	/// 用户名或者密码错误
	public static final int PASSWORD_ERROR = 3402;	/// 用户名或者密码错误
	public static final int SESSION_EXPIRED = 3403;	/// 会话过期
	public static final int SESSION_KICKOFF = 3404;	/// 账号在别处登录
	
	
	
	private static final CodeMessage messages[] = new CodeMessage[] {
		new CodeMessage(SUCCESS, "成功"),
		new CodeMessage(FAIL, "失败"),
		new CodeMessage(ALREADY_EXIST, "已经存在"),
		new CodeMessage(NO_ENTRY, "对象不存在"),
		
		new CodeMessage(INVALID_PARAM, "无效参数"),
		new CodeMessage(FORBIDDEN, "权限不够"),
		new CodeMessage(INVALID_DATA, "无效数据"),
		
		new CodeMessage(SERVER_ERROR, "服务端错误"),
		
		new CodeMessage(AUTH_FAILED, "认证失败"),
		new CodeMessage(NO_AUTH, "缺少认证信息"),
		new CodeMessage(CALL_SPEED_LIMIT, "调用太频繁"),
		
		
		new CodeMessage(DB_NO_CONNECTION, "无数据库连接"),
		new CodeMessage(DB_ERROR, "数据库操作错误"),
		
		new CodeMessage(INVALID_SMS, "无效验证码"),
		new CodeMessage(OPERATION_FAILED, "操作失败"),
		
		new CodeMessage(NO_SUCH_DEVICE, "不存在该设备"),
		new CodeMessage(DEVICE_OFFLINE, "设备不在线"),
		
		new CodeMessage(NO_SUCH_USER, "不存在该用户"),
		new CodeMessage(NAME_PASSWORD_ERROR, "错误的用户名或密码"),
		new CodeMessage(PASSWORD_ERROR, "错误的密码"),
		new CodeMessage(SESSION_EXPIRED, "会话过期"),
		new CodeMessage(SESSION_KICKOFF, "账号在别处登录"),
			
	}; 
	
	public static String findMessage(int code) {
		for (CodeMessage msg: messages) {
			if (msg.code == code) {
				return msg.getMessage();
			}
		}
		return "";
	}


	
	
}
