/**
 * Define the python generation function for yadoms_wait_for_event_result block
 * @param block The block
 * @return {*[]}
 */
Blockly.Python["yadoms_wait_for_event_result"] = function (block) {
	var code = "";
	
	switch(block.getChoice()) {
		case "value":
			//use Blockly.Python.lastWaitForEventResultKeywordId and not Blockly.Python.lastWaitForEventResultVar + ".getKeywordId() because
			//method Blockly.Yadoms.Python.castToPython must be determined at generation time and .getKeywordId() is python method at runtime
			code = Blockly.Yadoms.Python.castToPython(Blockly.Python.lastWaitForEventResultKeywordId, Blockly.Python.lastWaitForEventResultVar + ".getValue()");
			break;
		case "keywordName":
			code = "yApi.getKeywordName(" + Blockly.Python.lastWaitForEventResultVar + ".getKeywordId())";
			break;
		case "deviceName":
			code = "yApi.getKeywordDeviceName(" + Blockly.Python.lastWaitForEventResultVar + ".getKeywordId())";
			break;
		case "fullName":
			code = "yApi.getKeywordDeviceName(" + Blockly.Python.lastWaitForEventResultVar + ".getKeywordId()) + '.' + yApi.getKeywordName(" + Blockly.Python.lastWaitForEventResultVar + ".getKeywordId())";
			break;
	}
	return [code, Blockly.Python.ORDER_ATOMIC];
};

