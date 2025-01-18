/*
  auxp.c - Digital aux port status reporting plugin for grblHAL

  Copyright (c) 2023 CHE
  Version 0.2 - 18 April, 2023

  For use with grblHAL: https://github.com/grblHAL
  grblHAL plugins: https://github.com/grblHAL/plugins

  This software is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This digital Aux port status reporting plugin is distributed in the
  hope that it will be useful, but WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl. If not, see <http://www.gnu.org/licenses/>.

  Copyright reserved by the author.
*/



#include "driver.h"



#if AUXP_ENABLE

#define MAX_AUX_PORTS 8
bool digitalAuxPortOn[MAX_AUX_PORTS];
bool hasChanged = true;

static on_realtime_report_ptr on_realtime_report = NULL;
static on_report_options_ptr on_report_options = NULL;
static driver_reset_ptr driver_reset = NULL;
static digital_out_ptr digital_out = NULL;



static void onRealtimeReport (stream_write_ptr stream_write, report_tracking_flags_t report) {
	char buf[20] = "";
	strcat(buf, "|Dout:");

	if ( (hasChanged) || (report.all) ) {
		for (uint8_t i = 0; i < MAX_AUX_PORTS; i++)
			if (digitalAuxPortOn[i]) strcat(buf, uitoa(i));
		stream_write(buf);
		hasChanged = false;
	}

	// Call previous function in the chain
	if (on_realtime_report)
		on_realtime_report(stream_write, report);
}



static void onReportOptions (bool newopt)  {
	if (on_report_options)
	 	on_report_options(newopt);
	if (!newopt)
		hal.stream.write("[PLUGIN:AUXP v0.2]" ASCII_EOL);
}



static void driverReset (void) {

	// Reset port flags
	for (uint8_t i = 0; i < MAX_AUX_PORTS; i++)
		digitalAuxPortOn[i] = false;

	// Trigger initial report
	hasChanged = true;	

	// Call previous function in the chain
	if (driver_reset)
		driver_reset();
}



static void OnDigitalOut(uint8_t port, bool on) {

	if (port < MAX_AUX_PORTS) {
  	if (digitalAuxPortOn[port] != on) {
			// Save port state and mark as changed
			digitalAuxPortOn[port] = on;
			hasChanged = true;
		}
	}

	// Call original function
	if (digital_out)
		digital_out(port, on);
}



// INIT FUNCTION - CALLED FROM plugins_init.h()
void auxp_init() {

	// Subscribe to driver reset event
	driver_reset = hal.driver_reset;
	hal.driver_reset = driverReset;

	// Subscribe to report options event
	on_report_options = grbl.on_report_options;
	grbl.on_report_options = onReportOptions;

	// Subscribe to realtime report events
	on_realtime_report = grbl.on_realtime_report;
	grbl.on_realtime_report = onRealtimeReport;

	// Subscribe to digital out event
	digital_out = hal.port.digital_out;
	hal.port.digital_out = OnDigitalOut;
}



#endif