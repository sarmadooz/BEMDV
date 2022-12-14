//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef TraCIDemo11p_H
#define TraCIDemo11p_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "CarInformation.cc"
#include "GatewayModel.cc"

/**
 * @brief
 * A tutorial demo for TraCI. When the car is stopped for longer than 10 seconds
 * it will send a message out to other cars containing the blocked road id.
 * Receiving cars will then trigger a reroute via TraCI.
 * When channel switching between SCH and CCH is enabled on the MAC, the message is
 * instead send out on a service channel following a WAVE Service Advertisement
 * on the CCH.
 *
 * @author Christoph Sommer : initial DemoApp
 * @author David Eckhoff : rewriting, moving functionality to BaseWaveApplLayer, adding WSA
 *
 */

class TraCIDemo11p : public BaseWaveApplLayer {
public:
    bool hasScanned;
    int clusterHead;
    bool isClusterHead;
    int electionTime;
    bool tempUpdate; //temp var to make sure if car distance is updated or new car is aded into the table
    std::vector<int> carsInRange;
    std::vector<double> carsInRangeDistances;
    std::vector<CarInformation*> clusterMembers;
    std::vector<GatewayModel*> gateways;
    simtime_t lastUpdate;
    int electionDelay;
    simtime_t lastAdvertize;
    simtime_t lastBSMSent;

    static simsignal_t delaySignal;
    static simsignal_t emrgencySentSignal;
    static simsignal_t emrgencyRcvdSignal;
    static simsignal_t emrgencyFrwrdSignal;

    virtual void initialize(int stage);
protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;
protected:
    virtual void onBSM(BasicSafetyMessage* bsm);
    virtual void onWSM(WaveShortMessage* wsm);
    virtual void onWSA(WaveServiceAdvertisment* wsa);
    virtual bool compareDirection(double dir1, double dir2);
    virtual void handleSelfMsg(cMessage* msg);
    virtual void handlePositionUpdate(cObject* obj);
    virtual void advertize(int clusterHeadID);
};

#endif
