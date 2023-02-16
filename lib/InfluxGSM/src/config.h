#ifndef CONFIG_H
#define CONFIG_H

#define TINY_GSM_MODEM_SIM800
// Set serial for debug console
#define SerialMon Serial

// buffer to store incoming serial data
#define TINY_GSM_RX_BUFFER 100

#define GSM_BAUD_RATE 57600

// Uncomment this if you want to use SSL i.e., HTTPS
// #define USE_SSL

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM SIM Card PIN, if any
#define GSM_PIN ""

//String msisdn = "01717018376";
//String device_id = "1OQakeBxYd";

// GPRS credentials, if any
const char apn[] = "gpinternet";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server details
const char server[] = "ssh.bahon.info";
const char path[] = "/api/v2/write?org=DIU&bucket=dehonker&precision=ns";

const char auth_header[] = "Token Dg8v_kFW7ER_saI4wupvxpi_Aba1wANZND5K9DSBKXlAcUr_ODI9RaN7Hy9875HFBXIzK6574yM2vsxfQgNwXA==";
const char content_type[] = "text/plain; charset=utf-8";
const char accept[] = "application/json";

#endif