/**
 * Define a block which target a precise hour every day
 * @param block The block
 * @return {*[]}
 */
 Blockly.Blocks['yadoms_date_now'] = {
  init: function() {
    this.appendDummyInput()
        .appendField($.t("blockly.blocks.dates.yadoms_date_now.title", { defaultValue: "Today" }));
    this.setInputsInline(true);
    this.setOutput(true, "time");
    this.setColour(Blockly.Yadoms.Dates.time.HUE);
    this.setTooltip($.t("blockly.blocks.dates.yadoms_date_now.tooltip", { defaultValue: "" }));
    this.setHelpUrl(Blockly.Yadoms.GenerateHelpUrl(this.type));
  }
};