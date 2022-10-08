//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 


package org.car2x.veins.modules.application.traci;
import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;

simple TraCIDataDissemination extends BaseWaveApplLayer
{

    @class(TraCIDataDissemination);
    @display("i=block/app2");
    

    @signal[delay](type="double");
	@statistic[delay](title="delay"; source="delay"; record=stats, vector; interpolationmode=none);   
	@signal[emrgencySent](type="double");
	@statistic[emrgencySent](title="emrgencySent"; source="emrgencySent"; record=stats, vector; interpolationmode=none);   
	@signal[emrgencyRcvd](type="double");
	@statistic[emrgencyRcvd](title="emrgencyRcvd"; source="emrgencyRcvd"; record=stats, vector; interpolationmode=none);   
	@signal[emrgencyFrwrd](type="double");
	@statistic[emrgencyFrwrd](title="emrgencyFrwrd"; source="emrgencyFrwrd"; record=stats, vector; interpolationmode=none);   
	

}
