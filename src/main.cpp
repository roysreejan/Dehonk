#include <Arduino.h>
#include <ArduinoSTL.h>
#include <queue>
#include "InfluxGSM.hpp"

using namespace std;

// global variables
const byte horn_interrupt_pin = 2;

int last_horn_engaged_time = 0;
int last_horn_disengaged_time = 0;

// integer queue for storing the last 100 horn events
queue<int> horn_queue;

// a global state variable to keep track of the current state of the horn
bool horn_already_engaged = false;

// function prototypes
void horn_interrupt_handler();
void horn_queue_handler();
void print_status(String msg, int data, int queue_size);

// initialize influxdb
InfluxGSM influxGSM;

// one time setup
void setup()
{
    // initialize pins
    pinMode(horn_interrupt_pin, INPUT);
    // attach interrupt
    attachInterrupt(digitalPinToInterrupt(horn_interrupt_pin), horn_interrupt_handler, CHANGE);

    // initialize serial
    Serial.begin(115200);

    // print welcome message
    Serial.println("System successfully booted");

    // bootstrap influxdb
    if (!influxGSM.bootstrap())
    {
        Serial.println("Failed to establish server connection");
        while (1)
            ;
    }
}

// main loop
void loop()
{
    horn_queue_handler();
}

void horn_queue_handler()
{
    // loop through the queue
    while (!horn_queue.empty())
    {
        // print status
        print_status("Trying to send data to server", horn_queue.front(), horn_queue.size());

        // send data to server
        if (influxGSM.send(horn_queue.front()))
        {
            // pop the queue
            horn_queue.pop();
            // print status
            print_status("Data sent successfully", horn_queue.front(), horn_queue.size());
        }
        else
        {
            // print status
            print_status("Data sending failed", horn_queue.front(), horn_queue.size());
            delay(100);
        }
    }
}

// interrupt handler for horn
void horn_interrupt_handler()
{
    // read pin state
    bool state = digitalRead(horn_interrupt_pin);
    // print pin state

    if (state)
    {
        if (!horn_already_engaged)
        {
            // print horn sound
            Serial.println("> Horn engaged");
            // set the time
            last_horn_engaged_time = millis();
            // set the state
            horn_already_engaged = true;
            // set last horn engaged time
            // last_horn_disengaged_time = 0;
        }
    }

    else
    {
        if (horn_already_engaged)
        {
            // set the time
            last_horn_disengaged_time = millis();
            // set the state
            horn_already_engaged = false;

            // set duration
            int duration = last_horn_disengaged_time - last_horn_engaged_time;

            // reset last horn engaged time
            last_horn_engaged_time = 0;

            // add duration to queue
            horn_queue.push(duration);

            // print status
            print_status("> Horn disengaged", duration, horn_queue.size());
        }
    }
}

void print_status(String msg, int data, int queue_size)
{
    Serial.print(msg);
    Serial.print(" | Duration: ");
    Serial.print(data);
    Serial.print(" | Queue Size: ");
    Serial.println(queue_size);
}