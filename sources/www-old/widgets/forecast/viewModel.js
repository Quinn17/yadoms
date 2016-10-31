function Convertmstokmh(value) {
   return value * 3.6;
}

widgetViewModelCtor =

/**
 * Create a Forecast ViewModel
 * @constructor
 */
function forecastViewModel() {

   //observable data
   this.data = ko.observable("");
   this.city = ko.observable("");

   //Default value - This value is overwrite after
   this.period = ko.observableArray();

   //Definition of the temporary array
   this.TempPeriod = new Array();

   this.MaxTempVisible = ko.observable(true);
   this.MinTempVisible = ko.observable(true);
   this.MaxWindVisible = ko.observable(true);
   this.AveWindVisible = ko.observable(true);
   this.RainDayVisible = ko.observable(true);

   this.dateformat = "";
   this.lastUpdate = "";

   //Number of day to be displayed
   this.DayNumber = ko.observable(10);

   //Height of the widget.
   this.height = 0;


   /**
    * Initialization method
    * @param widget widget class object
    */
   this.initialize = function () {
       var self = this;
       self.widgetApi.toolbar({
           activated: false
       });
   };

   /**
    * Draw in circle the speed and the direction of the wind
    * @data device identifier which make the values
    * @WindPosition Direction from the Wind
    * @WindSpeed    Speed of the Wind
    */
   this.canvasload = function (data, windPosition, windSpeed) {
      //get a reference to the canvas
      var ctx = this.widgetApi.find("#" + data).get(0).getContext("2d");

      // Refresh the canvas, clear all existing information
      ctx.clearRect(0, 0, 36, 36);

      ctx.fillStyle = "rgb(0,0,0)"; // black

      //draw a circle
      ctx.beginPath();

      ctx.arc(20, 20, 17, 0, Math.PI * 2, true);
      ctx.closePath();

      ctx.lineWidth = 1;
      ctx.strokeStyle = '#000000';
      ctx.stroke();

      ctx.font = "14px Georgia";

      //write the text at the same position as the height of the column
      ctx.fillText(windSpeed, 20 - (7 * String(windSpeed).match(/\d/g).length) / 2, 23);
      ctx.stroke();

      //triangle of the direction
      ctx.beginPath();
      ctx.moveTo(18 - 11 * Math.sin(Math.PI / 180 * parseInt(windPosition)), 18 - 11 * Math.cos(Math.PI / 180 * parseInt(windPosition)));
      ctx.lineTo(18 - 18 * Math.sin(Math.PI / 180 * (parseInt(windPosition) + 10)), 18 - 18 * Math.cos(Math.PI / 180 * (parseInt(windPosition) + 10)));
      ctx.lineTo(18 - 18 * Math.sin(Math.PI / 180 * (parseInt(windPosition) - 10)), 18 - 18 * Math.cos(Math.PI / 180 * (parseInt(windPosition) - 10)));
      ctx.closePath();
      ctx.fill();
      ctx.stroke();
   };

   this.RainCanvasLoad = function (data, rainValue) {
      //get a reference to the canvas
      var ctx = this.widgetApi.find("#" + data).get(0).getContext("2d");

      // Refresh the canvas, clear all existing information
      ctx.clearRect(0, 0, 40, 45);

      ctx.fillStyle = "rgb(116,208,241)"; // blue Azur clair

      var positionY = 0;
      if (rainValue < 40)
         positionY = 40 - parseInt(rainValue);


      ctx.fillRect(0, positionY, 40, rainValue);
      ctx.stroke();

      ctx.fillStyle = "rgb(0,0,255)"; // black

      // We use the same font as in the css file
      ctx.font = $( ".cases" ).css( "font-size" ) + " " + $( ".cases" ).css( "font-family" );

      //write the text at the same position as the height of the column
      ctx.fillText(rainValue, 20 - (7 * String(rainValue).match(/\d/g).length) / 2, 23);
      ctx.stroke();
   };

   /**
    * New acquisition handler
    * @param device Device on which new acquisition was received
    * @param data Acquisition data
    */
   this.onNewAcquisition = function (keywordId, data) {
      var self = this;

      if (keywordId === self.widget.configuration.device.keywordId) {
         if (data.value && data.value !== "") {
            var obj = jQuery.parseJSON(data.value);
			
            //We only keep the city name
            var res = obj.city.split(",");
			
			   if (res.length >=2)
               self.city(res[1]);
            else
               self.city(res[0]);

            //We delete all information already keep in.
            while (self.TempPeriod.length > 0) {
               self.TempPeriod.pop();
            }

            self.DayNumber(obj.forecast.length);

            //Copy of all object into the temporary array
            $.each(obj.forecast, function (i) {
               // create the name for each div where wind canvas will be attached
               var elementId = 'widget-column-' + i;

               // create the name for each div where rain canvas will be attached
               var rainElementId = 'widget-rain-' + i;

               var timeString = "";

               if (self.dateformat === "DateFormat1") {
                  //Ex: Mon. 15
                  timeString = moment(obj.forecast[i].Day + "-" + obj.forecast[i].Month, "DD-MM").format('dddd').substring(0, 3) + ". " + obj.forecast[i].Day;
               } else if (self.dateformat === "DateFormat2") {
                  timeString = moment(obj.forecast[i].Day + "-" + obj.forecast[i].Month, "DD-MM").format('LL');
               }

               self.TempPeriod.push({
                  WeatherCondition: obj.forecast[i].WeatherCondition,
                  TimeDate: timeString,
                  TempMax: obj.forecast[i].TempMax + $.t(obj.Units.temperature), //  � obj.Units.temperature.substring(0, obj.Units.temperature.length - 1),
                  TempMin: obj.forecast[i].TempMin + $.t(obj.Units.temperature), //obj.Units.temperature.substring(0, obj.Units.temperature.length - 1),
                  MaxWind: Convertmstokmh(parseFloat(obj.forecast[i].MaxWind, 10)),
                  AveWind: Convertmstokmh(parseFloat(obj.forecast[i].AveWind, 10)),
                  AveWindDegrees: obj.forecast[i].AveWindDegrees,
                  WindCanvasId: elementId,
                  RainCanvasId: rainElementId,
                  RainDay: obj.forecast[i].RainDay,
                  WeatherIcon: "widgets/forecast/images/Icons1/" + obj.forecast[i].WeatherCondition + ".png"
               });
            }
            );

            //Resize the widget and display the elements automatically
            self.resized();

            //Send update information to the HMI, on if the date is new
            if (self.lastUpdate !== data.date._i) {
               notifyInformation($.t("widgets/forecast:forecastUpdated", { objectName: self.widget.configuration.device.friendlyName }));
               self.lastUpdate = data.date._i;
            }
         }
      }
   };

   this.configurationChanged = function () {
      var self = this;

      if ((isNullOrUndefined(self.widget)) || (isNullOrUndefinedOrEmpty(self.widget.configuration)))
         return;

      //we register keyword new acquisition
      self.widgetApi.registerKeywordAcquisitions(self.widget.configuration.device.keywordId);

      try {
         self.MaxTempVisible(true);
         self.MinTempVisible(true);
         self.MaxWindVisible(parseBool(self.widget.configuration.Information.content.MaxWind));
         self.AveWindVisible(parseBool(self.widget.configuration.Information.content.AveWind));
         self.RainDayVisible(parseBool(self.widget.configuration.Information.content.RainDay));

         //Read the date format
         self.dateformat = self.widget.configuration.DateFormat;
      }
      catch (err) {
         console.debug(err.message);
      }
   }

   this.resized = function () {
      var self = this;

      if (self.widget.getHeight() <= 150) {
         //In one case : we keep only the icon
         self.MaxTempVisible(false);
         self.MinTempVisible(false);
         self.RainDayVisible(false);
         self.AveWindVisible(false);
         self.MaxWindVisible(false);
      }
      else if ((self.widget.getHeight() <= 220) && (self.widget.getHeight() > 190)) {
         self.height = 20; // Location 
         self.height = self.height + 20; // Size of the day
         self.height = self.height + 40; //Size of the icon of the day

         //In two cases : we enable automatically temperatures. All others information are read from the configuration
         self.MaxTempVisible(true);
         self.height = self.height + 20; //Size of the maximum temperature
         self.MinTempVisible(true);
         self.height = self.height + 20; //Size of the minimum temperature

         self.AveWindVisible(parseBool(self.widget.configuration.Information.content.AveWind));
         if (self.AveWindVisible())
            self.height = self.height + 40; //Size of the Wind Canvas

         self.MaxWindVisible(parseBool(self.widget.configuration.Information.content.MaxWind));
         if (self.MaxWindVisible())
            self.height = self.height + 20; //Size of the Maximum Wind

         if (((self.height + 40) <= 200) && parseBool(self.widget.configuration.Information.content.RainDay)) {
            self.RainDayVisible(true);
            self.height = self.height + 40; //Size of the rain canvas
         }
         else
            self.RainDayVisible(false);
      }
      else if ((self.SizePainted !== 3) && (self.widget.getHeight() > 220)) {
         //In three cases : we enable automatically temperatures. All others information are read from the configuration
         self.MaxTempVisible(true);
         self.MinTempVisible(true);
         self.MaxWindVisible(parseBool(self.widget.configuration.Information.content.MaxWind));
         self.AveWindVisible(parseBool(self.widget.configuration.Information.content.AveWind));
         self.RainDayVisible(parseBool(self.widget.configuration.Information.content.RainDay));
      }

      if (self.widget.getWidth() <= 100) {
          self.DayNumber(1);
      }
      else if (self.widget.getWidth() <= 200) // if length = 2 cases -> 2 days
      {
          self.DayNumber(3);
      }
      else if (self.widget.getWidth() <= 300) // if length = 3 cases -> 3 days
      {
          self.DayNumber(4);
      }
      else if (self.widget.getWidth() <= 400) // if length = 4 cases -> 5 days
      {
          self.DayNumber(6);
      }
      else if (self.widget.getWidth() <= 500) // if length = 5 cases -> 6 days
      {
          self.DayNumber(6);
      }
      else if (self.widget.getWidth() <= 600) // if length = 6 cases -> 8 days
      {
          self.DayNumber(8);
      }
      else if (self.DayNumber() !== 10)  // Otherwise 10 days
      {
          self.DayNumber(10);
      }

      self.period.removeAll();
      self.period(self.TempPeriod.slice(0, self.DayNumber()));
   };
};