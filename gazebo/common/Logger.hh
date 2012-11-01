/*
 * Copyright 2011 Nate Koenig
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
/* Desc: A class to log data
 * Author: Nate Koenig
 * Date: 1 Jun 2010
 */

#ifndef _LOGGER_HH_
#define _LOGGER_HH_

#include <vector>
#include <fstream>
#include <string>
#include <boost/thread.hpp>

#include "common/Event.hh"
#include "common/SingletonT.hh"

#define GZ_LOG_VERSION "1.0"

namespace gazebo
{
  namespace common
  {
    /// \addtogroup gazebo_common
    /// \{

    /// \class Logger Logger.hh physics/Logger.hh
    /// \brief Handles logging of data to disk
    ///
    /// The Logger class is a Singleton that manages data logging of any
    /// entity within a running simulation. An entity may be a World, Model,
    /// or any of their child entities. This class only writes log files,
    /// see LogPlay for playback functionality.
    ///
    /// State information for an entity may be logged through the Logger::Add
    /// function, and stopped through the Logger::Remove function. Data may
    /// be logged into a single file, or split into many separate files by
    /// specifying different filenames for the Logger::Add function.
    ///
    /// The Logger is updated at the start of each simulation step. This
    /// guarantees that all data is stored.
    ///
    /// \sa Logplay, State
    class Logger : public SingletonT<Logger>
    {
      /// \brief Constructor
      private: Logger();

      /// \brief Destructor
      private: virtual ~Logger();

      public: bool Init(const std::string &_subdir);

      /// \brief Add an object to a log file.
      ///
      /// Add a new object to a log. An object can be any valid named object
      /// in simulation, including the world itself. Duplicate additions are
      /// ignored. Objects can be added to the same file by passing
      /// specifying the same _filename.
      /// \param[in] _name Name of the object to log.
      /// \param[in] _filename Filename of the log file.
      /// \param[in] _logCallback Function used to log data for the object.
      /// Typically an object will have a log function that outputs data to
      /// the provided ofstream.
      /// \throws Exception
      public: void Add(const std::string &_name, const std::string &_filename,
                    boost::function<bool (std::ostringstream &)> _logCallback);

      /// \brief Remove an entity from a log
      ///
      /// Removes an entity from the logger. The stops data recording for
      /// the entity and all its children. For example, specifying a world
      /// will stop all data logging.
      /// \param[in] _name Name of the log
      /// \return True if the entity existed and was removed. False if the
      /// entity was not registered with the logger.
      public: bool Remove(const std::string &_name);

      /// \brief Stop the logger.
      public: void Stop();

      /// \brief Start the logger.
      public: void Start();

      /// \brief Update the log files
      ///
      /// Captures the current state of all registered entities, and outputs
      /// the data to their respective log files.
      private: void Update();

      /// \brief Run the Write loop.
      private: void Run();

      /// \brief Write the header to file.
      // private: void WriteHeader();

      /// \brief A logger for an entitiy
      private: class Log
      {
        public: Log(const std::string &_filename,
                    boost::function<bool (std::ostringstream &)> _logCB);

        public: virtual ~Log();

        public: void Write();

        public: void Update();

        public: void ClearBuffer();

        public: boost::function<bool (std::ostringstream &)> logCB;
        public: std::string buffer;
        public: std::fstream logFile;
      };

      private: typedef std::map<std::string, Log*> Log_M;
      private: Log_M logs;
      private: Log_M::iterator updateIter;
      private: Log_M::iterator logsEnd;

      private: event::ConnectionPtr updateConnection;

      private: bool stop;
      private: boost::thread *writeThread;
      private: boost::mutex writeMutex;
      private: boost::mutex controlMutex;

      private: boost::condition_variable dataAvailableCondition;

      private: std::string logPathname;

      // private: std::string buffer;

      private: friend class SingletonT<Logger>;
    };
    /// \}
  }
}
#endif
