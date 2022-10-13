#ifndef INFLUXDB_HPP
#define INFLUXDB_HPP
class InfluxGSM
{
public:
    bool bootstrap();
    bool send(const int data);

private:
    bool beginGPRS();
    bool closeGPRS();
};

#endif