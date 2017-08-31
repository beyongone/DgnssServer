package wits.model;

public class AgnssData {
	
	public AgnssFrameHeader header;
	public AgnssParam param;
	public AgnssAssistData assistData;
	
	public AgnssData() {
		header = new AgnssFrameHeader();
		param = new AgnssParam();
		assistData = new AgnssAssistData();
	}
	
	
}
