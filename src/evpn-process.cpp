#include <evpn-process.h>
#include <config.h>

#include <iostream>
#include <cctype>


EvpnProcess::EvpnProcess(boost::asio::io_context & ctx, uint32_t statusUpdateInterval):
    EvpnBase(ctx, statusUpdateInterval),
    Logger("evpn-process"),
    _ctx{ctx}
{}


void EvpnProcess::getLocations(std::function<void (std::list<Location>)> callback)
{
    auto lambda = [this, callback] (short exitCode, std::list<std::string> stdout, std::list<std::string> stderr)
                  {
                      if (0 != exitCode)
                      {
                          throw std::runtime_error("EvpnProcess::getLocations() failed. expressvpn returned exit code " + std::to_string(exitCode));
                      }

                      auto it = std::find_if(stdout.begin(), stdout.end(), [] (std::string const & line) { return line.substr(0, 5) == "ALIAS"; });
                      if (it == stdout.end()) throw std::runtime_error("EvpnProcess::getLocations() failed. Couldn't find \"ALIAS\" in the output");

                      std::string & header = *it;
                      auto findPosOf = [&header] (std::string const & text)
                                       {
                                           std::size_t pos = header.find(text);
                                           if (std::string::npos == pos) throw std::runtime_error("EvpnProcess::getLocations() failed. Couldn't find \"" + text + "\" in the output");
                                           return pos;
                                       };
                      std::size_t countryPos = findPosOf("COUNTRY");
                      std::size_t locationPos = findPosOf("LOCATION");
                      std::size_t recommendedPos = findPosOf("RECOMMENDED");
                      ++++it;
                      std::list<Location> locations;

                      std::string prevValidCountry;
                      for (; it!= stdout.end(); ++it)
                      {
                          auto rtrim = [] (std::string s)
                                       {
                                           s.erase(std::find_if(s.rbegin(),
                                                                s.rend(),
                                                                [] (unsigned char c)
                                                                {
                                                                    return !std::isspace(c);
                                                                }).base(),
                                                   s.end());
                                           return s;
                                           
                                       };
                          std::string & line = *it;
                          std::string shortCode = rtrim(line.substr(0, countryPos));
                          std::string country = rtrim(line.substr(countryPos, locationPos - countryPos));
                          std::string location = rtrim(line.substr(locationPos, recommendedPos - locationPos));
                          std::string recommended = rtrim(recommendedPos + 1 <= line.size() ? line.substr(recommendedPos) : "");
                          if (country.empty())
                          {
                              // 'expressvpn list all' prints empty string for country if the current location is in the same country as the previous one.
                              country = prevValidCountry;
                          }
                          else
                          {
                              prevValidCountry = country;
                          }
                          locations.push_back({ .shortCode = shortCode, .text = location, .country = country, .preferred = recommended == "Y" ? true : false });
                      }
                      debug("Command execution finished");
                      callback(locations);
                  };
    debug("Running command: expressvpn list all");
    std::shared_ptr<Process> obj = Process::execute(context(), lambda, Config::instance().executable(), "list", "all");

}


void EvpnProcess::getStatus(std::function<void (Status, std::optional<std::string>)> callback)
{
    static std::shared_ptr<Process> process;

    if (process.get())
    {
        process->terminate();
    }

    auto parser = [this, callback] (short exitCode, std::list<std::string> stdout, std::list<std::string> stderr)
                  {
                      process.reset();
                      
                      if (exitCode)
                      {
                          throw std::runtime_error("EvpnBase::periodicStatusQuery() failed. expressvpn returned exit code " + std::to_string(exitCode));
                      }
                      // debug("Command execution finished");
                      for (auto const & line: stdout)
                      {
                          if (std::string::npos != line.find("Not connected"))
                          {
                              callback(Status::DISCONNECTED, {});
                              return;
                          }
                          if (std::string::npos != line.find("Connecting..."))
                          {
                              callback(Status::CONNECTING, {});
                              return;
                          }
                          std::string connectedTo{"Connected to"};
                          if (std::size_t pos = line.find(connectedTo); std::string::npos != pos)
                          {
                              std::string locationText = line.substr(pos + connectedTo.size()+1);
                              callback(Status::CONNECTED, locationText);
                              return;
                          }
                      }
                  };
        
    // debug("Running command: expressvpn status");

    process = Process::execute(context(), parser, Config::instance().executable(), "status");
}


void EvpnProcess::connect(std::string const & shortCode, std::function<void ()> callback)
{
    cancel();

    auto parser = [this, callback] (short exitCode, std::list<std::string> stdout, std::list<std::string> stderr)
                  {
                      _connectProcess.reset();
                      debug("Command execution finished");

                      for (auto const & line: stdout)
                      {
                          std::string connectedTo{"Connected to"};
                          if (std::size_t pos = line.find(connectedTo); std::string::npos != pos)
                          {
                              callback();
                              return;
                          }
                      }
                  };
        
    debug("Running command: expressvpn connect", shortCode);
    _connectProcess = Process::execute(context(), parser, Config::instance().executable(), "connect", shortCode);
}

void EvpnProcess::disconnect(std::function<void ()> callback)
{
    cancel();
    
    auto parser = [this, callback] (short exitCode, std::list<std::string> stdout, std::list<std::string> stderr)
                  {
                      _connectProcess.reset();
                      debug("Command execution finished");

                      if (exitCode)
                      {
                          throw std::runtime_error("EvpnBase::disconnect() failed. expressvpn returned exit code " + std::to_string(exitCode));
                      }
                      callback();
                  };
        
    debug("Running command: expressvpn disconnect");
    _connectProcess = Process::execute(context(), parser, Config::instance().executable(), "disconnect");
    
}


void EvpnProcess::cancel()
{
    if (_connectProcess.get())
    {
        _connectProcess->terminate();
        _connectProcess.reset();
    }
}
