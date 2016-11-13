/**
 * Created by jeanmichelDECORET on 26/05/14.
 */

/**
 * This associative array based on plugin Type contains all plugin type information with their package
 * @type {Array}
 */
PluginInstanceManager.packageList = [];

/**
 * Factory which create objects from json data
 * @param json The json data
 * @returns {Plugin} List of plugins
 */
PluginManager.factory = function (json) {
   assert(!isNullOrUndefined(json), "json must be defined");
   return new Plugin(json);
};

/**
 * Function which list all available plugins with their packages
 */
PluginManager.getAll = function () {

   var d = new $.Deferred();

   RestEngine.getJson("rest/plugin")
   .done(function(data) {
      //we've got a list of plugin type. For each of one we download the package.json
      PluginManager.packageList = [];
      i18n.options.resGetPath = '__ns__/locales/__lng__.json';
	  
      var arrayOfDeffered = [];
      $.each(data.plugins, function (index, pluginType) {
         i18n.loadNamespace("plugins/" + pluginType);

         var deffered = PluginManager.downloadPackage(pluginType);
		 
		 arrayOfDeffered.push(deffered);
         deffered.done(function (package) {
			
			var defferedDownload = new $.Deferred();
            PluginManager.packageList[pluginType] = PluginManager.factory(package);
			defferedDownload.resolve();
         })
         .fail(function (error) {
			defferedDownload.reject();
            notifyError($.t("objects.pluginInstance.errorGettingPackage", { pluginName: pluginType }), error);
         });
      });

      $.whenAll(arrayOfDeffered)
	     .done(function () {
            i18n.options.resGetPath = "locales/__lng__.json";
            d.resolve(PluginManager.packageList);
         })
		 .fail(function (error) {
			d.reject();
         });
   })
   .fail(function (error) {
      notifyError($.t("objects.plugin.errorListing"), error);
      d.reject();
   });
   
   return d.promise();
};

/**
 * Download a plugin package for an instance
 * @param pluginType The plugin type to download
 */
PluginManager.downloadPackage = function (pluginType) {
   assert(!isNullOrUndefined(pluginType), "plugin Type must be defined");

   var d = new $.Deferred();
   RestEngine.get("plugins/" + pluginType + "/package.json", { dataType: "json" })
   .done(function (data) {
      i18n.loadNamespace("plugins/" + pluginType, function () {
         d.resolve(data);
      });
   })
   .fail(d.reject);
   return d.promise();
};

/**
 *
 * @constructor
 */
function PluginManager() {
}

