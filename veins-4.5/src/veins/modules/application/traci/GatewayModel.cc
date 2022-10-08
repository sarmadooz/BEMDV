/*
 * GatewayModel.cc
 *
 *  Created on: Mar 15, 2022
 *      Author: Sarmad
 */




#ifndef GatewayModel
#define Car
#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"



class GatewayModel {

private:
    int gatewayID;
    int numNodes;
    int chID;//clusterhead ID

public:
    GatewayModel() {
        // TODO Auto-generated constructor stub

    }

    ~GatewayModel() {
        // TODO Auto-generated destructor stub
    }

    int getChId() const {
        return chID;
    }

    void setChId(int chId) {
        chID = chId;
    }

    int getGatewayId() const {
        return gatewayID;
    }

    void setGatewayId(int gatewayId) {
        gatewayID = gatewayId;
    }

    int getNumNodes() const {
        return numNodes;
    }

    void setNumNodes(int numNodes) {
        this->numNodes = numNodes;
    }
};


#endif






