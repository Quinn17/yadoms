﻿/**
 * Created by nicolasHILAIRE on 09/02/2016.
 */

/**
 * Creates an instance of WidgetApi
 * @constructor
 */
function WidgetApi(widget) {
   assert(!isNullOrUndefined(widget), "widget must be defined");

   this.widget = widget;
   this.widgetBatteryClass = "widget-toolbar-battery";
}

/**
 * Jquery find method into widget instance
 */
WidgetApi.prototype.find = function (pattern) {
   return this.widget.$gridWidget.find(pattern);
}

/**
 * Obtain information about a keyword
 * @param {} keywordId to query
 * @returns {} a promise that's return done when information grabbed from server
 */
WidgetApi.prototype.getKeywordInformation = function (keywordId) {
   assert(!isNullOrUndefined(keywordId), "keywordId must be defined");
   return KeywordManager.get(keywordId);
};

/**
 * Register keywords to receive notifications when a new acquisition triggers
 * @param {} keywordIds to register (can be a single value or an array of values)
 * @returns {} a promise that's return done when information grabbed from server
 */
WidgetApi.prototype.registerKeywordAcquisitions = function (keywordIds) {
   assert(!isNullOrUndefined(keywordIds), "keywordIds must be defined");

   var self = this;

   if (!self.widget.listenedKeywords)
      self.widget.listenedKeywords = [];

   if (Array.isArray(keywordIds)) {
      $.each(keywordIds, function (index, value) {
         self.widget.listenedKeywords.push(value);
      });
   } else {
      self.widget.listenedKeywords.push(keywordIds);
   }
};

/**
 * Lazy load required libraries
 * @param {} librayNames to load (can be a single value or an array of values)
 * @returns {} a promise that's return done when libraries are loaded
 */
WidgetApi.prototype.loadLibrary = function (librayNames) {
   assert(!isNullOrUndefined(librayNames), "librayNames must be defined");

   if (Array.isArray(librayNames)) {
      return asyncLoadJSLibs(librayNames);
   } else {
      return asyncLoadJSLib(librayNames);
   }
};

/**
 * Lazy load required CSS stylesheet
 * @param {} librayNames to load (can be a single value or an array of values)
 * @returns {} a promise that's return done when libraries are loaded
 */
WidgetApi.prototype.loadCss = function (cssFiles) {
   assert(!isNullOrUndefined(cssFiles), "cssFiles must be defined");

   if (Array.isArray(cssFiles)) {
      return asyncLoadManyCss(cssFiles);
   } else {
      return asyncLoadCss(cssFiles);
   }
};

/**
 * Configure toolbar
 * @param {} options : json options (see wiki for help)
 */
WidgetApi.prototype.toolbar = function (options) {
    assert(!isNullOrUndefined(options), "options must be defined");

    var self = this;

    //we define default values
    options.activated = options.activated || false;
    options.displayTitle = options.displayTitle || true;
    options.items = options.items || [];
    options.batteryItem = options.batteryItem || false;

    //we manage options
    self.widget.toolbarActivated = options.activated;

    if (self.widget.toolbarActivated) {
        self.find(".panel-widget-header").removeClass("hidden");
        self.widget.displayTitle = options.displayTitle;

        //we manage title
        if (self.widget.displayTitle)
            self.find(".panel-widget-title").removeClass("hidden");
        else
            self.find(".panel-widget-title").addClass("hidden");

        //battery
        if (options.batteryItem) {
            self.widget.$toolbar.append("<div class=\"" + self.widgetBatteryClass + "\" deviceId=\"\"></div>");
        }

        //all other items
        $.each(options.items, function(index, value) {
            if (value.hasOwnProperty("custom")) {
                self.widget.$toolbar.append(value["custom"]);
            } else if (value.hasOwnProperty("separator")) {
                //for the moment value has no effect. Reserved for future use with options like margin, ...
                self.widget.$toolbar.append("<div class=\"widget-toolbar-separator\"></div>");

            } else {
                console.error("Unknown item type: " + index);
            }
        });
    } else {
        self.find(".panel-widget-header").addClass("hidden");
    }
};

/**
 * Check if the widget use battery item and configure it
 */
WidgetApi.prototype.manageBatteryConfiguration = function () {

    var self = this;
    var $battery = self.widget.$toolbar.find("." + self.widgetBatteryClass);
    if ($battery.length > 0) {
        //we clear the div that will contain the battery indicator
        $battery.empty();
        var deviceId = $battery.attr("deviceId");
        if (deviceId) {
            //we check for the device to look if it has battery keyword
            DeviceManager.getKeywordsByDeviceId(deviceId)
            .done(function (keywords) {
                var batteryLevel = keywords.find(function (element) { return element.capacityName === "batteryLevel"; });
                if (batteryLevel) {
                    //it has capacity
                    $battery.append("<span class=\"\"/>");
                    $battery.attr("keywordId", batteryLevel.id);
                    //we add it to the filter of keyword for websockets
                    self.widget.viewModel.widgetApi.registerKeywordAcquisitions(batteryLevel.id);

                    //we ask immediately for the battery value
                    AcquisitionManager.getLastValue(batteryLevel.id)
                    .done(function (lastValue) {
                        self.widget.viewModel.widgetApi.updateBatteryLevel(lastValue.value);
                    })
                    .fail(function (error) {
                        notifyError($.t("objects.generic.errorGetting", { objectName: "Acquisition KeywordId = " + batteryLevel.id }), error);
                    });
                }
                else {
                    //we can hide the div to prevent margin spaces before the title
                    $battery.hide();
                }
            })
            .fail(function (error) {
                notifyError($.t("objects.generic.errorGetting", { objectName: "keyword for device = " + deviceId }), error);
            });
        }
    }
}

/**
 * Apply batterylevel onto the icon of the toolbar
 */
WidgetApi.prototype.updateBatteryLevel = function (batteryLevel) {
    assert(!isNullOrUndefined(batteryLevel), "batteryLevel must be defined");
    var self = this;

    //we compute the battery fill
    var fill;
    var lvl = parseInt(batteryLevel);
    if (lvl < 20.0)
        fill = 0;
    else if (lvl < 40.0)
        fill = 1;
    else if (lvl < 60.0)
        fill = 2;
    else if (lvl < 80.0)
        fill = 3;
    else
        fill = 4;

    this.widget.$toolbar.find("div." + self.widgetBatteryClass + " span").removeClass().addClass("fa fa-battery-" + fill);
};

/**
 * Define the keyword Id of the battery toolbar icon. 
 */
WidgetApi.prototype.configureBatteryIcon = function (deviceId) {
    assert(!isNullOrUndefined(deviceId), "deviceId must be defined");
    var $batteryIcon = this.widget.$toolbar.find("." + this.widgetBatteryClass);
    if ($batteryIcon) {
        $batteryIcon.attr("deviceId", deviceId);
    }
}

/**
 * Make the text which has class "textfit" fit into the widget
 */
WidgetApi.prototype.fitText = function () {
    var $texts = this.find(".textfit");
    if ($texts)
        $texts.fitText();
}

/**
 * Notify a message to the window using noty library
 * @param message message to display
 * @param gravity gravity of the message
 * @returns {noty}
 * @param timeout timeout of the noty
 */
WidgetApi.prototype.notify = function(message, gravity, timeout) {
    assert(message != undefined, "Message must be defined");
    gravity = gravity || "information";
    timeout = timeout || defaultNotyTimeout;
    
    return noty({ text: message, timeout: timeout, layout: defaultNotyLayout, type: gravity });
}

/**
 * Configure/Modify the Rolling Title for widgets
 */
 
WidgetApi.prototype.manageRollingTitle = function () {
	var self = this;
	
	if (self.widget.displayTitle)
	{
		 
		if ( $("[widget-id=" + self.widget.id + "]").find(".panel-widget-title-toolbar")[0].scrollWidth < 15 )
			toolbarSize = 0;
		else
			toolbarSize = $("[widget-id=" + self.widget.id + "]").find(".panel-widget-title-toolbar")[0].scrollWidth;
		 
		//Calculate the overflow ! Theses values could be obtain, only after the application !
		var overflow = toolbarSize +
					   $("[widget-id=" + self.widget.id + "]").find(".panel-widget-title")[0].scrollWidth -
					   $("[widget-id=" + self.widget.id + "]").find(".panel-widget-header")[0].scrollWidth; 
					   
		if (overflow > 0) {
			
			if ($("[widget-id=" + self.widget.id + "]").find(".panel-widget-title-" + self.widget.id).length !== 0)
			{ 
				 rule = getRule ( "panel-widget-title-marquee-" + self.widget.id );
                 
				 //Delete the rule containing the overflow to scroll
				 rule[0].deleteRule("50%");
				 
				 //Append the new overflow
				 rule[0].appendRule("50% { text-indent: " + -overflow + "px;}");
			}
			else
			{
				// Create completly the css rule
				$("<style type='text/css'> .panel-widget-title-" + self.widget.id + "{margin: 0 auto; overflow: hidden; white-space: nowrap; box-sizing: border-box; animation: panel-widget-title-marquee-" + self.widget.id +
				  " 10s steps(150) infinite;-webkit-animation-play-state: running; animation-play-state: running;}</style>").appendTo("head");	//html > //ease-in-out
				$("<style type='text/css'> @keyframes panel-widget-title-marquee-" + self.widget.id + " { 0%   { text-indent: 0px; } 50% { text-indent: " + -overflow + "px;}  100%  { text-indent: 0px; } }</style>").appendTo("head");
				$("[widget-id=" + self.widget.id + "]").find(".panel-widget-title").addClass("panel-widget-title-" + self.widget.id);
			}
		}
		else
		{
			// If exist, remove the class associated to the div
			$("[widget-id=" + self.widget.id + "]").find(".panel-widget-title").removeClass("panel-widget-title-" + self.widget.id);
		}		
	}
}