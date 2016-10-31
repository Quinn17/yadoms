/**
 * Created by Nicolas on 01/03/14.
 */

/**
 * Create an enum parameter handler
 * @param i18NContext
 * @param paramName
 * @param content
 * @param currentValue
 * @constructor
 */
function EnumParameterHandler(i18NContext, paramName, content, currentValue) {
   assert(i18NContext !== undefined, "i18nContext must contain path of i18n");
   assert(paramName !== undefined, "paramName must be defined");
   assert(content !== undefined, "content must be defined");

   //values can be empty
   this.values = content.values;
   this.value = currentValue;
   this.sort = parseBool(content.sort);

   this.name = content.name;
   this.uuid = createUUID();
   this.paramName = paramName;
   this.description = isNullOrUndefined(content.description)?"":content.description;
   this.i18nContext = i18NContext;
   this.content = content;
}

/**
 * Get the DOM Object to insert
 * @returns {string}
 */
EnumParameterHandler.prototype.getDOMObject = function () {
   //Now values can't be empty
   assert(this.values !== undefined, "values field must be defined");

   if ((this.value === undefined) || (this.value == null) || (this.value === "")) {
      //we set the default value
      this.value = this.content.defaultValue;
   }

   //if it is still not defined we take the first item in the list
   if ((this.value === undefined) || (this.value == null) || (this.value === "")) {
      this.value = this.content.values[0];
   }

   var input = "<select " +
                        "class=\"form-control enable-validation\" " +
                        "id=\"" + this.uuid + "\" " +
                        "data-content=\"" + this.description + "\"" +
                        "required ";
   var i18NData = " data-i18n=\"";

   var self = this;
   i18NData += "[data-content]" + self.i18nContext + self.paramName + ".description";

   i18NData += "\" ";
   input += i18NData + " >";

   input += "</select>";
   return ConfigurationHelper.createControlGroup(self, input);
};

EnumParameterHandler.prototype.setValues = function (values) {
   this.values = values;
   this.updateValues();
};


EnumParameterHandler.arrangeCollection = function (collection, alphabeticallySort) {
   var sortable = [];
   $.each(collection, function (key, value) {
      if (value !== undefined && key && key !== "undefined")
         sortable.push([key, value]);
   });

   if (alphabeticallySort === true) {
      sortable.sort(function(a, b) {
          if (a !== undefined && b !== undefined)
              return a[1].localeCompare(b[1]);
          return 1;
      });
   }
   return sortable;
}

EnumParameterHandler.prototype.updateValues = function () {
   var self = this;
   var $select = self.locateInDOM();
   $select.empty();

    //we fill a new object with the values transalted if possible
   var translatedValues = {};
   $.each(self.values, function (key, value) {
       var newValue = "";
       if (i18n.exists(self.i18nContext + self.paramName + ".values." + key)) {
           newValue = $.t(self.i18nContext + self.paramName + ".values." + key);
       }
       else { //if the precedent line doesn't exist into the i18n we are in the case of binding. So we have to display the "value"
           newValue = value;
       }
       translatedValues[key] = newValue;
   });

    //we iterate through the values collection
   $.each(EnumParameterHandler.arrangeCollection(translatedValues, self.sort), function (key, value) {
      //key contains the index in array
      //value contains [languageCode, languageDisplayName]
      var languageCode = value[0];
      var languagelanguageDisplayNameCode = value[1];

      var line = "<option value=\"" + languageCode + "\"";
      if (languageCode === self.value || languagelanguageDisplayNameCode === self.value)
         line += " selected";
	 
      line += " >" + languagelanguageDisplayNameCode + "</option>";
	  
      $select.append(line);
   });
};

EnumParameterHandler.prototype.applyScript = function () {
   this.updateValues();
};

EnumParameterHandler.prototype.locateInDOM = function () {
   return $("select#" + this.uuid);
};

/**
 * Get the param name
 * @returns {string}
 */
EnumParameterHandler.prototype.getParamName = function() {
  return this.paramName;
};

/**
 * Get the current configuration in the form
 * @returns {string}
 */
EnumParameterHandler.prototype.getCurrentConfiguration = function () {
   this.value = $("select#" + this.uuid).val();
   return this.value;
};