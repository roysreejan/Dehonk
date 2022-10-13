#include "InfluxGSM.hpp"
#include "config.h"

#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

SoftwareSerial SerialAT(8, 7); // RX, TX
TinyGsm modem(SerialAT);

TinyGsmClient client(modem);
HttpClient InfluxHttpClient(client, server, 8086);

bool InfluxGSM::bootstrap()
{
    // initialize serial
    SerialAT.begin(57600);
    delay(2000);

    SerialMon.println("Initializing modem...");
    // modem.restart();
    modem.init();

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

    // unlock SIM card if necessary
    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork())
    {
        SerialMon.println(" fail");
        return false;
    }

    SerialMon.println(" success");

    if (modem.isNetworkConnected())
    {
        SerialMon.println("Network connected");
        if (beginGPRS())
            return true;
    }
    return false;
}

bool InfluxGSM::beginGPRS()
{
    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        SerialMon.println(" fail");
        return false;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected())
    {
        SerialMon.println("GPRS connected");
        return true;
    }
    return false;
}

bool InfluxGSM::closeGPRS()
{
    SerialMon.println("Disconnecting GPRS...");
    if (!modem.gprsDisconnect())
    {
        SerialMon.println(" fail");
        return false;
    }
    SerialMon.println(" success");
    return true;
}

bool InfluxGSM::send(const int data)
{

    // check if gprs is connected
    if (!modem.isGprsConnected())
    {
        if (!beginGPRS()){
            return false;
        }
    }

    String PostData = "carhonk,msisdn=01717018376,device_id=ad4aefd544asda duration=" + String(data);

    InfluxHttpClient.beginRequest();
    InfluxHttpClient.post(path);
    InfluxHttpClient.sendHeader("Authorization", auth_header);
    InfluxHttpClient.sendHeader("Content-Type", content_type);
    InfluxHttpClient.sendHeader("Accept", accept);
    InfluxHttpClient.sendHeader("Content-Length", PostData.length());
    InfluxHttpClient.beginBody();
    InfluxHttpClient.print(PostData);
    InfluxHttpClient.endRequest();

    // check response
    int ResponseCode = InfluxHttpClient.responseStatusCode();
    SerialMon.print("Response code: ");
    SerialMon.println(ResponseCode);

    if (ResponseCode == 204)
    {
        // close connection
        InfluxHttpClient.stop();
        return true;
    }
    else
    {
        // close connection
        InfluxHttpClient.stop();
        return false;
    }
}