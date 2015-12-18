/**
 * Define the python generation function for yadoms_get_info block
 * @param block The block
  */
Blockly.Python["yadoms_get_info"] = function (block) {
    var val = block.getFieldValue(block.dropdownValueName_);
    var code = "yApi.getInfo(yApi." + val + ")";

    if (val === block.informationIdList_[0][1] || val === block.informationIdList_[1][1]) {
        code = "scriptUtilities.strToTime(" + code + ")";
    }

    return [code, Blockly.Python.ORDER_ATOMIC];
};
