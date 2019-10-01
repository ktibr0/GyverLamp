

const String server  = "api.openweathermap.org";
const String lat     = "51.31";
const String lon     = "37.89";
const String city     = "Moscow, RU";
const String city_id     = "524901";


const String appid   = ""; //здесь вставьте ключ, полученный на openweathermap 
const String url     = "http://" + server + "/data/2.5/weather?id=" + city_id + "&lang=ru" + "&units=metric&appid=" + appid;
//const String url     = "http://" + server + "/data/2.5/weather?lat=" + lat + "&lon=" + lon + "&lang=ru" +"&units=metric&appid=" + appid; //вызов погоды возможен разными путями, как по коду города (у меня) так и по координатам и т.д.


unsigned long lastConnectionTime = 0;
unsigned long postingInterval = 0;

String httpData;

struct weather_structure {
  unsigned int id;
  const char* main;
  const char* icon;
  const char* descript;
  float temp;
  float pressure;
  byte  humidity;
  float speed;
  float deg;
};
weather_structure weather;


void loop_weather() {


  if (WiFi.status() == WL_CONNECTED) {
    if (millis() < lastConnectionTime) lastConnectionTime = 0;
    if (millis() - lastConnectionTime > postingInterval or lastConnectionTime == 0) {

      HTTPClient client;
      bool find = false;
      //client.setTimeout(1000);
      //Serial.print("Connecting ");
      client.begin(url);
      int httpCode = client.GET();

      if (httpCode > 0) {
        //    Serial.printf("successfully, code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
          httpData = client.getString();
          if (httpData.indexOf(F("\"main\":{\"temp\":")) > -1) {
            lastConnectionTime = millis();
            find = true;
          }
          else Serial.println("Failed, json string is not found");
        }
      }
      else Serial.printf("failed, error: %s\n", client.errorToString(httpCode).c_str());

      //  postingInterval = find ? 600L * 1000L : 60L * 1000L;

      client.end();

      //   Serial.println(httpData);

      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(httpData);

      weather.temp     = root["main"]["temp"];
      weather.descript = root["weather"][0]["description"];

      /* есть возможность получения других реквизитов от погоды - см.API openweathermap
        weather.id       = root["weather"][0]["id"];
        weather.main     = root["weather"][0]["main"];
        weather.icon     = root["weather"][0]["icon"];
        weather.humidity = root["main"]["humidity"];
        weather.pressure = root["main"]["pressure"];
        weather.speed    = root["wind"]["speed"];
        weather.deg      = root["wind"]["deg"];
      */
      httpData = "";
     
      
      /* задел на дальнейшее - картинка или анимация при определенной погоде. пока не реаботает 
      int descript ;
      if (String(weather.descript) = "дождь")
      {
        descript = 1;
      }

      switch (descript) {
        case 1:
          animation (1); // выполнить, если значение 1
          break;
        /*
          case 1:
          // выполнить, если значение 1
          break;
          case 2:
          case 3:
          case 4:
          // выполнить, если значение 2, 3 или 4
          break;
        
        default:
          // выполнить, если значение не совпадает ни с одним case
          descript =  0;
                      break;
      }

   */

      
      String temperature = String(weather.temp); //игры с преобразованием инфы из запроса - некрасиво, но работает
      int temp_leng = temperature.length();
      temperature.remove(temp_leng - 1);

      String text = temperature + "'C" + " " + String(weather.descript);
      const char *text2 = text.c_str();
      while (!fillString(text2)) delay(1);




      /* при отладке раскомментируйте - вывод в монитор порта
            Serial.println("\nWeather");
            Serial.printf("id: %d\n", weather.id);
            Serial.printf("main: %s\n", weather.main);
            Serial.printf("description: %s\n", weather.descript);
            Serial.printf("icon: %s\n", weather.icon);
            Serial.printf("temp: %d celsius\n", round(weather.temp));
            Serial.printf("humidity: %d %\n", round(weather.humidity));
            Serial.printf("pressure: %d hPa or %d mmHg\n", round(weather.pressure), round(weather.pressure * 0.75));
            Serial.printf("wind's speed: %d\n", round(weather.speed));
            Serial.printf("wind's direction: %d\n", round(weather.deg));
            Serial.println();
      */
    }
  }
}
