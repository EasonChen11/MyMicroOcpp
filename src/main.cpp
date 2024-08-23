// matth-x/MicroOcpp
// Copyright Matthias Akstaller 2019 - 2024
// MIT License
#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;
#elif defined(ESP32)
#include <WiFi.h>
#else
#error only ESP32 or ESP8266 supported at the moment
#endif

#include <MicroOcpp.h>
#include "myevent.h"

#define STASSID "TCEJ-Home"
#define STAPSK "22435503"

#define OCPP_BACKEND_URL "ws://192.168.0.166:9000" //"ws://echo.websocket.events"
#define OCPP_CHARGE_BOX_ID "CP_1"

//
//  Settings which worked for my SteVe instance:
//
// #define OCPP_BACKEND_URL   "ws://192.168.178.100:8180/steve/websocket/CentralSystemService"
// #define OCPP_CHARGE_BOX_ID "esp-charger"
// #define LED_PIN 2
static int ledPin;
void LED_Blink(void *pvParameters)
{
    digitalWrite(ledPin, HIGH);      // Turn on the LED
    vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 1000 ms
    digitalWrite(ledPin, LOW);       // Turn off the LED
    vTaskDelete(NULL);               // Delete the task
}

void HeartBeatEvent(esp32m::Event *event)
{ // handle custom event
    if (event->is("heartbeat"))
    { // check if event is heartbeat
        auto customEvent = (esp32m::HeartBeatEvent *)event;
        ledPin = customEvent->getLedPin(); // 从事件对象获取 LED 引脚号
        xTaskCreatePinnedToCore(
            LED_Blink,   // Function to implement the task
            "LED_Blink", // Name of the task
            10000,       // Stack size in words
            NULL,        // Task input parameter
            1,           // Priority of the task
            NULL,        // Task handle
            1            // Core where the task should run
        );
    }
}

void setup()
{

    /*
     * Initialize Serial and WiFi
     */

    Serial.begin(115200);

    Serial.print(F("[main] Wait for WiFi: "));

#if defined(ESP8266)
    WiFiMulti.addAP(STASSID, STAPSK);
    while (WiFiMulti.run() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
#elif defined(ESP32)
    WiFi.begin(STASSID, STAPSK);
    while (!WiFi.isConnected())
    {
        Serial.print('.');
        delay(1000);
    }
#else
#error only ESP32 or ESP8266 supported at the moment
#endif

    Serial.println(F(" connected!"));

    // subscribe to custom event
    auto &manager = esp32m::EventManager::instance();
    manager.subscribe(HeartBeatEvent);
    /*
     * Initialize the OCPP library
     */

    mocpp_initialize(OCPP_BACKEND_URL, OCPP_CHARGE_BOX_ID, "My Charging Station", "My company name");
    // call class MOcppMongooseClient

    /*
     * Integrate OCPP functionality. You can leave out the following part if your EVSE doesn't need it.
     */
    setEnergyMeterInput([]()
                        {
        //take the energy register of the main electricity meter and return the value in watt-hours
        return 0.f; });

    setSmartChargingCurrentOutput([](float limit)
                                  {
        //set the SAE J1772 Control Pilot value here
        Serial.printf("[main] Smart Charging allows maximum charge rate: %.0f\n", limit); });

    setConnectorPluggedInput([]()
                             {
        //return true if an EV is plugged to this EVSE
        return false; });

    //... see MicroOcpp.h for more settings
}

void loop()
{

    /*
     * Do all OCPP stuff (process WebSocket input, send recorded meter values to Central System, etc.)
     */
    mocpp_loop();

    /*
     * Energize EV plug if OCPP transaction is up and running
     */
    if (ocppPermitsCharge())
    {
        Serial.println(F("[main] Energize EV plug"));
        // OCPP set up and transaction running. Energize the EV plug here
    }
    else
    {
        // Serial.println(F("[main] De-energize EV plug"));
        // No transaction running at the moment. De-energize EV plug
    }

    /*
     * Use NFC reader to start and stop transactions
     */
    if (/* RFID chip detected? */ false)
    {
        String idTag = "0123456789ABCD"; // e.g. idTag = RFID.readIdTag();
        if (!getTransaction())
        {
            // no transaction running or preparing. Begin a new transaction
            Serial.printf("[main] Begin Transaction with idTag %s\n", idTag.c_str());

            /*
             * Begin Transaction. The OCPP lib will prepare transaction by checking the Authorization
             * and listen to the ConnectorPlugged Input. When the Authorization succeeds and an EV
             * is plugged, the OCPP lib will send the StartTransaction
             */
            auto ret = beginTransaction(idTag.c_str());

            if (ret)
            {
                Serial.println(F("[main] Transaction initiated. OCPP lib will send a StartTransaction when"
                                 "ConnectorPlugged Input becomes true and if the Authorization succeeds"));
                Serial.printf("[main] transaction %d", getTransaction());
                Serial.printf("[main] trasaction active %d", getTransaction()->isActive());
                Serial.printf("[main] trasaction Authorized %d", getTransaction()->isAuthorized());
            }
            else
            {
                Serial.println(F("[main] No transaction initiated"));
            }
        }
        else
        {
            // Transaction already initiated. Check if to stop current Tx by RFID card
            if (idTag.equals(getTransactionIdTag()))
            {
                // card matches -> user can stop Tx
                Serial.println(F("[main] End transaction by RFID card"));

                endTransaction(idTag.c_str());
            }
            else
            {
                Serial.println(F("[main] Cannot end transaction by RFID card (different card?)"));
            }
        }
    }

    //... see MicroOcpp.h for more possibilities
}
