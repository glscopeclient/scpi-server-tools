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

#ifndef BridgeSCPIServer_h
#define BridgeSCPIServer_h

#include "SCPIServer.h"

/**
	@brief SCPI server supporting common commands shared by all scopehal bridge servers
 */
class BridgeSCPIServer : public SCPIServer
{
public:
	BridgeSCPIServer(ZSOCKET sock);
	virtual ~BridgeSCPIServer();

protected:

	virtual bool OnCommand(
		const std::string& line,
		const std::string& subject,
		const std::string& cmd,
		const std::vector<std::string>& args);

	virtual bool OnQuery(
		const std::string& line,
		const std::string& subject,
		const std::string& cmd);

	//Accessor methods for queries (must be overridden in derived classes)
protected:

	bool ParseDouble(const std::string& s, double& v);
	bool ParseUint64(const std::string& s, uint64_t& v);

	//-- Version Information Accessors --//
	/**
		@brief Returns the vendor / make of the instrument for *IDN? response
	 */
	virtual std::string GetMake() =0;

	/**
		@brief Returns the model name of the instrument for *IDN? response
	 */
	virtual std::string GetModel() =0;

	/**
		@brief Returns the serial number of the instrument for *IDN? response
	 */
	virtual std::string GetSerial() =0;

	/**
		@brief Returns the firmware version of the instrument for *IDN? response
	 */
	virtual std::string GetFirmwareVersion() =0;

	//-- Hardware Capabilities --//
	/**
		@brief Returns the number of analog channels
	 */
	virtual size_t GetAnalogChannelCount() =0;

	/**
		@brief Returns the set of currently valid sample rates, in Hz
	 */
	virtual std::vector<size_t> GetSampleRates() =0;

	/**
		@brief Returns the set of currently valid memory depths
	 */
	virtual std::vector<size_t> GetSampleDepths() =0;


	//-- Acquisition Commands --//
	/**
		@brief Arm the device for capture. If oneShot, capture only one waveform
	 */
	virtual void AcquisitionStart(bool oneShot = false) =0;

	/**
		@brief Force the device to capture a waveform
	 */
	virtual void AcquisitionForceTrigger() =0;

	/**
		@brief Stop the device from capturing further waveforms
	 */
	virtual void AcquisitionStop() =0;

	/**
		@brief Checks if the trigger is currently armed\
	 */
	virtual bool IsTriggerArmed() =0;


	//-- Probe Configuration --//
	/**
		@brief Enable or disable the probe on channel `chIndex`, enable if `enabled==true`
	 */
	virtual void SetChannelEnabled(size_t chIndex, bool enabled) =0;

	/**
		@brief Set the coupling of the probe on channel `chIndex` to `coupling`
	 */
	virtual void SetAnalogCoupling(size_t chIndex, const std::string& coupling) =0;

	/**
		@brief Set the requested voltage range of the probe on channel `chIndex`
		       to `range` (Volts max-to-min)
	 */
	virtual void SetAnalogRange(size_t chIndex, double range_V) =0;

	/**
		@brief Set the requested voltage offset of the probe on channel `chIndex`
		       to `offset` (Volts)
	 */
	virtual void SetAnalogOffset(size_t chIndex, double offset_V) =0;

	/**
		@brief Set the threshold for a digital HIGH on the channel `chIndex`
	 */
	virtual void SetDigitalThreshold(size_t chIndex, double threshold_V) =0;

	/**
		@brief Set the hysteresis value for the digital channel `chIndex`
	 */
	virtual void SetDigitalHysteresis(size_t chIndex, double hysteresis) =0;

	//-- Sampling Configuration --//
	/**
		@brief Set sample rate in Hz
	 */
	virtual void SetSampleRate(uint64_t rate_hz) =0;

	/**
		@brief Set sample rate in samples
	 */
	virtual void SetSampleDepth(uint64_t depth) =0;

	//-- Trigger Configuration --//
	/**
		@brief Set trigger delay in femptoseconds
	 */
	virtual void SetTriggerDelay(uint64_t delay_fs) =0;

	/**
		@brief Set trigger source to the probe on channel `chIndex`
	 */
	virtual void SetTriggerSource(size_t chIndex) =0;

	//-- (Edge) Trigger Configuration --//
	/**
		@brief Set the trigger's level to `level` in Volts
	 */
	virtual void SetTriggerLevel(double level_V) =0;

	/**
		@brief Configure the device to use an edge trigger
	 */
	virtual void SetTriggerTypeEdge() =0;

	/**
		@brief Set the edge trigger's activation to the edge `edge`
		       ("RISING", "FALLING", ...)
	 */
	virtual void SetEdgeTriggerEdge(const std::string& edge) =0;

	//-- Channel Information --//
	/**
		@brief Converts a string name (for example "C2") to an implementation-specific numeric channel ID.
		       The channel id must uniquely identify the channel across types/banks.

		This function returns true if the conversion succeeds, placing results into id_out.
	 */
	virtual bool GetChannelID(const std::string& subject, size_t& id_out) =0;

	enum ChannelType
	{
		CH_ANALOG,
		CH_DIGITAL,
		CH_EXTERNAL_TRIGGER
	};

	/**
		@brief Given a valid channel ID, return it's type.
	 */
	virtual ChannelType GetChannelType(size_t channel) =0;
};

#endif
