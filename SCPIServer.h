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

#ifndef SCPIServer_h
#define SCPIServer_h

#include "../../lib/xptools/Socket.h"
#include <string>
#include <vector>

/**
	@brief Server class for managing a single SCPI client connection
 */
class SCPIServer
{
public:
	SCPIServer(ZSOCKET sock);
	virtual ~SCPIServer();

	void MainLoop();

protected:
	bool RecvCommand(std::string& str);
	bool SendReply(const std::string& cmd);

	void ParseLine(
		const std::string& line,
		std::string& subject,
		std::string& cmd,
		bool& query,
		std::vector<std::string>& args);

	/**
		@brief Process a command

		@return True if the command was recognized and processed, false if unknown or invalid.
	 */
	virtual bool OnCommand(
		const std::string& line,
		const std::string& subject,
		const std::string& cmd,
		const std::vector<std::string>& args) =0;

	/**
		@brief Process a query command

		@param line		Full SCPI line (for display in error messages or logs)
		@param subject	Subject of the SCPI command (for example "C2" in "C2:OFFS?")
		@param cmd		Command (for example "OFFS" in "C2:OFFS?")

		@return True if the command was recognized and processed, false if unknown or invalid.
	 */
	virtual bool OnQuery(
		const std::string& line,
		const std::string& subject,
		const std::string& cmd) =0;

protected:
	Socket m_socket;
};

#endif
