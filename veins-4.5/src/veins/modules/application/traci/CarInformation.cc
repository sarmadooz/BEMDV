

#ifndef Car
#define Car
#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"



class CarInformation {

private:
    int carID;
    double distance;
    double direction;
    double speed;
    int transmissionRange;
    simtime_t lastUpdate;
public:
    CarInformation() {
        // TODO Auto-generated constructor stub

    }
    CarInformation(int id, double dis) {
            carID=id;
            distance=dis;

        }

    ~CarInformation() {
        // TODO Auto-generated destructor stub
    }

    int getCarId() const {
        return carID;
    }

    void setCarId(int carId) {
        carID = carId;
    }

    double getDistance() const {
        return distance;
    }

    void setDistance(double distance) {
        this->distance = distance;
    }

    double getDirection() const {
        return direction;
    }

    void setDirection(double direction) {
        this->direction = direction;
    }

    double getSpeed() const {
        return speed;
    }

    void setSpeed(double speed) {
        this->speed = speed;
    }

    int getTransmissionRange() const {
        return transmissionRange;
    }

    void setTransmissionRange(int transmissionRange) {
        this->transmissionRange = transmissionRange;
    }

    const simtime_t& getLastUpdate() const {
        return lastUpdate;
    }

    void setLastUpdate(const simtime_t& lastUpdate) {
        this->lastUpdate = lastUpdate;
    }
};


#endif



