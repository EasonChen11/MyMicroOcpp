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
#include "LEDOption.h"
#include "SWITCH.h"
#define STASSID "TCEJ-Home"
#define STAPSK "22435503"

#define OCPP_BACKEND_URL "ws://192.168.0.166:8180/steve/websocket/CentralSystemService/" //"ws://192.168.0.166:9000" //"ws://echo.websocket.events"
#define OCPP_CHARGE_BOX_ID "CP_1"

//
//  Settings which worked for my SteVe instance:
//
// #define OCPP_BACKEND_URL   "ws://192.168.178.100:8180/steve/websocket/CentralSystemService"
// #define OCPP_CHARGE_BOX_ID "esp-charger"

// setups the LED pin
LEDOption permitsCharge(PermitsCharge);
SWITCH RFIDTouch(RFID_PIN), connectorPlugged(ConnectorPlugged), evseReady(EvseReady), evReady(EvReady);
enum RFIDState
{
    RFID_IDLE,
    RFID_FIRST_TOUCHED,
    RFID_FIRST_TOUCHED_IDLE,
    RFID_SECOND_TOUCHED
};
RFIDState RFIDstate = RFID_IDLE;

void HeartBeatEvent(esp32m::Event *event)
{ // handle custom event
    if (event->is("heartbeat"))
    { // check if event is heartbeat
        auto customEvent = (esp32m::HeartBeatEvent *)event;
        customEvent->Blink();
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
    /* init LED */
    // pinMode(ConnectorPlugged, INPUT);pinMode(EvseReady, INPUT);pinMode(EvReady, INPUT);

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
        static ulong lastSampled = millis();
static float energyMeter = 0.f;
if (getTransactionIdTag() > 0)
    energyMeter += ((float) (millis() - lastSampled)) * 0.003f; //increase by 0.003Wh per ms (~ 10.8kWh per h)
lastSampled = millis();
return energyMeter; });

    addMeterValueInput([]
                       { return 120; },
                       "Voltage", "V");
    addMeterValueInput([]
                       { return 10; },
                       "Current.import",
                       "A");
    addMeterValueInput([]
                       { return 100; },
                       "Energy.Active.Import.Register", "Wh");
    addMeterValueInput([]
                       { return 50; },
                       "Frequency", "Hz");
    addMeterValueInput([]
                       { return 10; },
                       "Power.Active.Import",
                       "kW");
    addMeterValueInput([]
                       { return 20; },
                       "Temperature",
                       "Celsius");

    setSmartChargingCurrentOutput([](float limit)
                                  {
        //set the SAE J1772 Control Pilot value here
        Serial.printf("[main] Smart Charging allows maximum charge rate: %.0f\n", limit); });
    setConnectorPluggedInput([]()
                             // return true if an EV is plugged to this EVSE
                             { return connectorPlugged.Is_Ready(); });
    setEvReadyInput([]()
                    // return true if the EV is ready to charge
                    { return evReady.Is_Ready(); });
    setEvseReadyInput([]()
                      // return true if the EVSE is ready to charge
                      { return evseReady.Is_Ready(); });
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
        // Serial.println(F("[main] Energize EV plug"));

        permitsCharge.LED_On();
        // OCPP set up and transaction running. Energize the EV plug here
    }
    else
    {
        permitsCharge.LED_Off();
        // Serial.println(F("[main] De-energize EV plug"));
        // No transaction running at the moment. De-energize EV plug
    }

    /*
     * Use NFC reader to start and stop transactions
     */
    if (/* RFID chip detected? */RFIDTouch.Is_Ready()) // RFID card touched
    {
        String idTag = "ABC"; // e.g. idTag = RFID.readIdTag();

        if (!getTransaction() && RFIDstate == RFID_IDLE)
        {

            // no transaction running or preparing. Begin a new transaction
            Serial.printf("[main] Begin Transaction with idTag %s\n", idTag.c_str());

            /*
             * Begin Transaction. The OCPP lib will prepare transaction by checking the Authorization
             * and listen to the ConnectorPlugged Input. When the Authorization succeeds and an EV
             * is plugged, the OCPP lib will send the StartTransaction
             */
            auto ret = beginTransaction(idTag.c_str());
            // if transaction is authorized, then the transaction is initiated
            RFIDstate = RFID_FIRST_TOUCHED;
            // auto ret = beginTransaction_authorized(idTag.c_str());
            if (ret)
            {
                Serial.println(F("[main] Transaction initiated. OCPP lib will send a StartTransaction when"
                                 "ConnectorPlugged Input becomes true and if the Authorization succeeds"));
                // Serial.printf("[main] transaction %d", getTransaction());
                // Serial.printf("[main] trasaction active %d", getTransaction()->isActive());
                // Serial.printf("[main] trasaction Authorized %d", getTransaction()->isAuthorized());
            }
            else
            {
                Serial.println(F("[main] No transaction initiated"));
            }
        }
        else
        {
            if (RFIDstate == RFID_FIRST_TOUCHED_IDLE)
                // Transaction already initiated. Check if to stop current Tx by RFID card
                if (idTag.equals(getTransactionIdTag()))
                {
                    // card matches -> user can stop Tx
                    Serial.println(F("[main] End transaction by RFID card"));
                    endTransaction(idTag.c_str());
                    endTransaction(getTransaction()->getIdTag());
                    if (RFIDstate == RFID_FIRST_TOUCHED_IDLE)
                        RFIDstate = RFID_SECOND_TOUCHED;
                }
                else
                {
                    Serial.println(F("[main] Cannot end transaction by RFID card (different card?)"));
                }
        }
    }
    else
    {
        if (RFIDstate == RFID_FIRST_TOUCHED)
        {
            RFIDstate = RFID_FIRST_TOUCHED_IDLE;
        }
        else if (RFIDstate == RFID_SECOND_TOUCHED)
        {
            RFIDstate = RFID_IDLE;
        }
    }

    //... see MicroOcpp.h for more possibilities
}
