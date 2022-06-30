/***********************************************************************************************************************
*                                                                                                                      *
* scpi-server-tools                                                                                                    *
*                                                                                                                      *
* Copyright (c) 2012-2022 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

#include "BridgeSCPIServer.h"
#include <stdexcept>
#include "../log/log.h"

#define FS_PER_SECOND 1e15
#define SECONDS_PER_FS 1e-15

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

BridgeSCPIServer::BridgeSCPIServer(ZSOCKET sock)
	: SCPIServer(sock)
{
}

BridgeSCPIServer::~BridgeSCPIServer()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers

bool BridgeSCPIServer::ParseDouble(const std::string& s, double& v)
{
	try
	{
		v = stod(s);
		return true;
	}
	catch (const std::invalid_argument& ia)
	{
		LogWarning("Invalid double: %s\n", s.c_str());
		return false;
	}
}

bool BridgeSCPIServer::ParseUint64(const std::string& s, uint64_t& v)
{
	try
	{
		v = stoull(s);
		return true;
	}
	catch (const std::invalid_argument& ia)
	{
		LogWarning("Invalid u64: %s\n", s.c_str());
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command processing

bool BridgeSCPIServer::OnCommand(const std::string& line, const std::string& subject,
                                 const std::string& cmd, const std::vector<std::string>& args)
{
	(void) line;

	if (subject == "")
	{
		// Device commands

		if (cmd == "START")
			AcquisitionStart(false);
		else if (cmd == "SINGLE")
			AcquisitionStart(true);
		else if (cmd == "FORCE")
			AcquisitionForceTrigger();
		else if (cmd == "STOP")
			AcquisitionStop();
		else if (cmd == "RATE" && args.size() == 1)
		{
			uint64_t arg;
			if (ParseUint64(args[0], arg))
				SetSampleRate(arg);
			else
				return false;
		}
		else if (cmd == "DEPTH" && args.size() == 1)
		{
			uint64_t arg;
			if (ParseUint64(args[0], arg))
				SetSampleDepth(arg);
			else
				return false;
		}
		else
			return false;
	}
	else
	{
		if (subject == "TRIG")
		{
			// Trigger commands

			if (cmd == "DELAY" && args.size() == 1)
			{
				uint64_t arg;
				if (ParseUint64(args[0], arg))
					SetTriggerDelay(arg);
				else
					return false;
			}
			else if (cmd == "SOU" && args.size() == 1)
			{
				size_t arg;
				if (GetChannelID(args[0], arg))
					SetTriggerSource(arg);
				else
					return false;
			}
			else if (cmd == "MODE" && args.size() == 1)
			{
				if (args[0] == "EDGE")
					SetTriggerTypeEdge();
				else
					return false;
			}
			else if (cmd == "LEV" && args.size() == 1)
			{
				double arg;
				if (ParseDouble(args[0], arg))
					SetTriggerLevel(arg);
				else
					return false;
			}
			else if (cmd == "EDGE:DIR" && args.size() == 1)
				SetEdgeTriggerEdge(args[0]);
			else
				return false;
		}
		else
		{
			// Channel commands (probably)

			size_t channelId;

			if (!GetChannelID(subject, channelId)) return false;

			ChannelType channelType = GetChannelType(channelId);

			if (cmd == "ON")
				SetChannelEnabled(channelId, true);
			else if (cmd == "OFF")
				SetChannelEnabled(channelId, false);
			else if (cmd == "COUP" && channelType == CH_ANALOG && args.size() == 1)
				SetAnalogCoupling(channelId, args[0]);
			else if (cmd == "RANGE" && channelType == CH_ANALOG && args.size() == 1)
			{
				double arg;
				if (ParseDouble(args[0], arg))
					SetAnalogRange(channelId, arg);
				else
					return false;
			}
			else if (cmd == "OFFS" && channelType == CH_ANALOG && args.size() == 1)
			{
				double arg;
				if (ParseDouble(args[0], arg))
					SetAnalogOffset(channelId, arg);
				else
					return false;
			}
			else if (cmd == "THRESH" && channelType == CH_DIGITAL && args.size() == 1)
			{
				double arg;
				if (ParseDouble(args[0], arg))
					SetDigitalThreshold(channelId, arg);
				else
					return false;
			}
			else if (cmd == "HYS" && channelType == CH_DIGITAL && args.size() == 1)
			{
				double arg;
				if (ParseDouble(args[0], arg))
					SetDigitalHysteresis(channelId, arg);
				else
					return false;
			}
			else
				return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Query processing

bool BridgeSCPIServer::OnQuery(const string& line, const string& subject, const string& cmd)
{
	(void) line; (void) subject;

	//Read ID code
	if(cmd == "*IDN")
		SendReply(GetMake() + "," + GetModel() + "," + GetSerial() + "," + GetFirmwareVersion());

	//Get number of channels
	else if(cmd == "CHANS")
		SendReply(to_string(GetAnalogChannelCount()));

	//Checks if we're armed
	else if(cmd == "ARMED")
	{
		if(IsTriggerArmed())
			SendReply("1");
		else
			SendReply("0");
	}

	//Get legal sample rates for the current configuration
	else if(cmd == "RATES")
	{
		auto rates = GetSampleRates();
		string ret = "";
		for(auto rate : rates)
		{
			double interval = FS_PER_SECOND / rate;
			ret += to_string(interval) + ",";
		}
		SendReply(ret);
	}

	//Get memory depths
	else if(cmd == "DEPTHS")
	{
		auto depths = GetSampleDepths();
		string ret = "";
		for(auto d : depths)
			ret += to_string(d) + ",";
		SendReply(ret);
	}

	//Nope, invalid command or something handled by the derived class
	else
		return false;

	return true;
}
