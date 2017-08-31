package wits.model;


public class AgnssFrameHeader {

	public int		tag;		/// 主类型 @see AgnssFrameTag
	public int		subtype;	/// 子类型
	public int		deviceType;	/// 设备类型
	public String	device;		/// 设备ID
	public long  	time;		/// 时间戳
	public int		length;		/// 数据长度
	
}
