/*
  Скетч к проекту "Многофункциональный RGB светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/GyverLamp/
  Исходники на GitHub: https://github.com/AlexGyver/GyverLamp/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver, AlexGyver Technologies, 2019
  https://AlexGyver.ru/
*/

/*
  Версия 1.4:
  - Исправлен баг при смене режимов
  - Исправлены тормоза в режиме точки доступа
  --- 08.07.2019
  - Исправлены параметры и процесс подключения к WiFi сети (таймаут 7 секунд) и развёртываия WiFi точки доступа (параметры имени/пароля)
  - Добавлено "#define USE_NTP" - позволяет запретить обращаться в интернет
  - Добавлено "#define ESP_USE_BUTTON - позволяет собирать лампу без физической кнопки, иначе яркость эффектов самопроизвольно растёт до максимальной
  - Переработаны параметры IP адресов, STA_STATIC_IP теперь пустой по умолчанию - избавляет от путаницы с IP адресами из неправильных диапазонов
  - Добавлено "#define GENERAL_DEBUG" - выводит в Serial некоторые отладочные сообщения
  - Добавлено "#define WIFIMAN_DEBUG (true)" - выводит в Serial отладочные сообщения библиотеки WiFiManager
  - Добавлена таблица с тест кейсами
  - Форматирование кода, комментарии
  --- 11.07.2019
  - Исправлена ошибка невыключения матрицы после срабатывания будильника, если до будильника матрица была выключенной
  - Дополнена таблица с тест кейсами
  --- 14.07.2019
  - Исправлена ошибка выключения будильника, если перед его срабатыванием был активен эффект "матрица" (или другой эффект, где задействовано мало светодиодов)
  - Добавлено управление по воздуху:
  -- работает только в режиме WiFi клиента
  -- работает при подключенной кнопке (потому что режим прошивки активируется кнопкой)
  --- 16.07.2019
  - Исправлено регулярное подвисание матрицы на 1-2 секунды при отсутствии подключения к интернету (но при успешном подключении к WiFi)
  --- 28.07.2019
  - Доработано взаимодействие с android приложением (отправка состояния после каждой операции)
  --- 01.08.2019
  - Возврат к стандартной библиотеке GyverButton (изменениё из неё перенесено в button.ino
  - Добавлены 2 эффекта: Светлячки со шлейфом и Белый свет
  - При запросе обновления по воздуху (2 четверных касания к кнопке) лампа переключается в режим "Матрица" для визуального подтверждения готовности к прошивке
  - В android приложение добавлена функция сканирования сети и добавления ламп с помощью multicast пакетов, доработка прошивки под это
  --- 03.08.2019
  - Исправлены ошибки взаимодействия android приложения с лампой, в вывод команды CURR добавлено текущее время (или millis(), если время не синхронизировано)
  --- 10.08.2019
  - Добавлена точная настройка яркости, скорости и масштаба эффектов
  - Добавлено взаимодействие с android приложением по управлению будильниками
  --- 14.08.2019
  - Добавлена функция таймера отключения
  --- 26.08.2019
  - Добавлен режим автоматического переключения избранных эффектов
  - Реорганизован код, исправлены ошибки
  --- 28.08.2019
  - Добавлен вызов режима обновления модуля esp из android приложения
  --- 30.08.2019
  - Эффект "Светлячки со шлейфами" переименован в "Угасающие пиксели"
  - Добавлены 5 новых эффекта: "Радуга диагональная", "Метель", "Звездопад", "Светлячки со шлейфами" (новый) и "Блуждающий кубик"
  - Исправлены ошибки
  --- 04.09.2019
  - Большая часть определений (констант) перенесена в файл Constants.h
  - Большая оптимизация использования памяти
  - Исправлена ошибка невключения эффекта "Белый свет" приложением и кнопкой
  - Исправлена ошибка неправильного выбора интервала в режиме Избранное в android приложении
  --- 16.09.2019
  - Добавлено сохранение состояния (вкл/выкл) лампы в EEPROM память
  - Добавлен новый эффект белого света (с горизонтальной полосой)
  - Реорганизован код, исправлены ошибки
  --- 20.09.2019
  - Добавлена возможность сохранять состояние (вкл/выкл) режима "Избранное"; не сбрасывается выключением матрицы, не сбрасывается перезапуском модуля esp
  - Убрана очистка параметров WiFi при старте с зажатой кнопкой; регулируется директивой ESP_RESET_ON_STASRT, которая определена как false по умолчанию
  --- 24.09.2019
  - Добавлены изменения из прошивка от Alex Gyver v1.5: бегущая строка с IP адресом лампы по пятикратному клику на кнопку
*/

// Ссылка для менеджера плат:
// https://arduino.esp8266.com/stable/package_esp8266com_index.json


#include "pgmspace.h"
#include "Constants.h"
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "Types.h"
#include "timerMinim.h"
#ifdef ESP_USE_BUTTON
#include <GyverButton.h>
#endif
#include "fonts.h"
#ifdef USE_NTP
#include <NTPClient.h>
#endif
#ifdef OTA
#include "OtaManager.h"
#endif
#include "TimerManager.h"
#include "FavoritesManager.h"
#include "EepromManager.h"

#include "bitmap1.h"
#include "bitmap2.h"
#include "bitmap3.h"
#include "bitmap4.h"
#include "bitmap5.h"

// --- ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ ----------
CRGB leds[NUM_LEDS];
WiFiManager wifiManager;
WiFiServer wifiServer(ESP_HTTP_PORT);
WiFiUDP Udp;

#ifdef USE_NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, GMT * 3600, NTP_INTERVAL);
#endif


int8_t hrs = 0, mins = 0, secs = 0, aday = 1, amnth = 1;
int16_t ayear = 1970;
byte frameNum; 


timerMinim timeTimer(3000);

#ifdef ESP_USE_BUTTON
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);
#endif
#ifdef OTA
OtaManager otaManager;
OtaPhase OtaManager::OtaFlag = OtaPhase::None;
#endif

// --- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ -------
uint16_t localPort = ESP_UDP_PORT;
char packetBuffer[MAX_UDP_BUFFER_SIZE];                     // buffer to hold incoming packet
char inputBuffer[MAX_UDP_BUFFER_SIZE];
static const uint8_t maxDim = max(WIDTH, HEIGHT);

ModeType modes[MODE_AMOUNT];
AlarmType alarms[7];

static const uint8_t dawnOffsets[] PROGMEM = {5, 10, 15, 20, 25, 30, 40, 50, 60};   // опции для выпадающего списка параметра "время перед 'рассветом'" (будильник); синхронизировано с android приложением
uint8_t dawnMode;
bool dawnFlag = false;
long thisTime;
bool manualOff = false;

int8_t currentMode = 0;
bool loadingFlag = true;
bool ONflag = false;
uint32_t eepromTimeout;
bool settChanged = false;

unsigned char matrixValue[8][16];

bool TimerManager::TimerRunning = false;
bool TimerManager::TimerHasFired = false;
uint8_t TimerManager::TimerOption = 1U;
uint64_t TimerManager::TimeToFire = 0ULL;

uint8_t FavoritesManager::FavoritesRunning = 0;
uint16_t FavoritesManager::Interval = DEFAULT_FAVORITES_INTERVAL;
uint16_t FavoritesManager::Dispersion = DEFAULT_FAVORITES_DISPERSION;
uint8_t FavoritesManager::UseSavedFavoritesRunning = 0;
uint8_t FavoritesManager::FavoriteModes[MODE_AMOUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t FavoritesManager::nextModeAt = 0UL;


void setup()
{
  Serial.begin(115200);
  Serial.println();

  ESP.wdtDisable();
  //ESP.wdtEnable(WDTO_8S);

  #if defined(ESP_USE_BUTTON) && ESP_RESET_ON_STASRT
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  buttonTick();
  if (touch.state())                                        // сброс сохранённых SSID и пароля при старте с зажатой кнопкой, если разрешено
  {
    wifiManager.resetSettings();

    #ifdef GENERAL_DEBUG
    Serial.println(F("Настройки WiFiManager сброшены"));
    #endif
  }
  #endif

  // ЛЕНТА
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)/*.setCorrection(TypicalLEDStrip)*/;
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  FastLED.clear();
  FastLED.show();

  // WI-FI
  wifiManager.setDebugOutput(WIFIMAN_DEBUG);                // вывод отладочных сообщений
  //wifiManager.setMinimumSignalQuality();                  // установка минимально приемлемого уровня сигнала WiFi сетей (8% по умолчанию)
  if (ESP_MODE == 0)                                        // режим WiFi точки доступа
  {
    // wifiManager.setConfigPortalBlocking(false);
    WiFi.softAPConfig(                                      // wifiManager.startConfigPortal использовать нельзя, т.к. он блокирует вычислительный процесс внутри себя, а затем перезагружает ESP, т.е. предназначен только для ввода SSID и пароля
      IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]),        // IP адрес WiFi точки доступа
      IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], 1),                      // первый доступный IP адрес сети
      IPAddress(255, 255, 255, 0));                                                         // маска подсети

    WiFi.softAP(AP_NAME, AP_PASS);

    Serial.println(F("Режим WiFi точки доступа"));
    Serial.print(F("IP адрес: "));
    Serial.println(WiFi.softAPIP());

    wifiServer.begin();
  }
  else                                                      // режим WiFi клиента (подключаемся к роутеру, если есть сохранённые SSID и пароль, иначе создаём WiFi точку доступа и запрашиваем их)
  {
    Serial.println(F("Режим WiFi клиента"));
    if (WiFi.SSID())
    {
      Serial.print(F("Подключение WiFi сети: "));
      Serial.println(WiFi.SSID());
    }
    else
    {
      Serial.println(F("WiFi сеть не определена, запуск WiFi точки доступа для настройки параметров подключения к WiFi сети..."));
    }

    if (STA_STATIC_IP)
    {
      wifiManager.setSTAStaticIPConfig(
        IPAddress(STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], STA_STATIC_IP[3]),  // статический IP адрес ESP в режиме WiFi клиента
        IPAddress(STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], 1),                 // первый доступный IP адрес сети (справедливо для 99,99% случаев; для сетей меньше чем на 255 адресов нужно вынести в константы)
        IPAddress(255, 255, 255, 0));                                                       // маска подсети (справедливо для 99,99% случаев; для сетей меньше чем на 255 адресов нужно вынести в константы)
    }

    wifiManager.setConnectTimeout(ESP_CONN_TIMEOUT);        // установка времени ожидания подключения к WiFi сети, затем старт WiFi точки доступа
    wifiManager.setConfigPortalTimeout(ESP_CONF_TIMEOUT);   // установка времени работы WiFi точки доступа, затем перезагрузка; отключить watchdog?
    wifiManager.autoConnect(AP_NAME, AP_PASS);              // пытаемся подключиться к сохранённой ранее WiFi сети; в случае ошибки, будет развёрнута WiFi точка доступа с указанными AP_NAME и паролем на время ESP_CONN_TIMEOUT секунд; http://AP_STATIC_IP:ESP_HTTP_PORT (обычно http://192.168.0.1:80) - страница для ввода SSID и пароля от WiFi сети роутера

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println(F("Время ожидания ввода SSID и пароля от WiFi сети или подключения к WiFi сети превышено\nПерезагрузка модуля"));

      #if defined(ESP8266)
      ESP.reset();
      #else
      ESP.restart();
      #endif
    }

    Serial.print(F("IP адрес: "));
    Serial.println(WiFi.localIP());
  }

  Serial.printf_P(PSTR("Порт UDP сервера: %u\n"), localPort);
  Udp.begin(localPort);

  EepromManager::InitEepromSettings(                        // инициализация EEPROM; запись начального состояния настроек, если их там ещё нет; инициализация настроек лампы значениями из EEPROM
    modes, alarms, &ONflag, &dawnMode, &currentMode,
    &(FavoritesManager::ReadFavoritesFromEeprom),
    &(FavoritesManager::SaveFavoritesToEeprom));

  #ifdef USE_NTP
  timeClient.begin();
  #endif

  memset(matrixValue, 0, sizeof(matrixValue));
  randomSeed(micros());
  changePower();
  loadingFlag = true;
}


void loop()
{
  parseUDP();
  effectsTick();
  EepromManager::HandleEepromTick(&settChanged, &eepromTimeout, &ONflag, 
  &currentMode, modes, &(FavoritesManager::SaveFavoritesToEeprom));
  #ifdef USE_NTP
  timeTick();
  #endif
  #ifdef ESP_USE_BUTTON
  buttonTick();
  #endif
  #ifdef OTA
  otaManager.HandleOtaUpdate();                             // ожидание и обработка команды на обновление прошивки по воздуху
  #endif
  TimerManager::HandleTimer(&ONflag, &settChanged,          // обработка событий таймера отключения лампы
    &eepromTimeout, &changePower);
  if (FavoritesManager::HandleFavorites(                    // обработка режима избранных эффектов
      &ONflag,
      &currentMode,
      &loadingFlag
      #ifdef USE_NTP
      , &dawnFlag
      #endif
      ))
  {
    FastLED.setBrightness(modes[currentMode].Brightness);
    FastLED.clear();
    delay(1);
  }
  ESP.wdtFeed();                                            // пнуть собаку
  yield();                                                  // обработать все "служебные" задачи: wdt, WiFi подключение и т.д. (?)
}
