/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
/**
 * @file dht/consts.h
 * @brief constants used by the current DHT, it might change for a more flexible system some day
 */
#ifndef EPYX_DHT_CONSTS_H
#define EPYX_DHT_CONSTS_H

namespace Epyx
{
namespace DHT
{
    //Define the timeouts in milliesconds
    static const int FIND_CALLBACK_TIMEOUT = 5000;
    static const int GET_CALLBACK_TIMEOUT = 10000;
    static const int SET_CALLBACK_TIMEOUT = 10000;
    static const int SINGLE_REQUEST_TIMEOUT = 2000;
    static const int REPLICATION_TIMEOUT = 60*1000; //very low for tests

    //Parameters for the FIND process
    static const int FIND_NB_NODE_REQUESTED = 5;
    static const int FIND_PARALLEL_QUERIES = 3;

    //Parameters for the GET process
    static const int GET_REDUNDANCY = 10;

    //Parameters for the SET process
    static const int SET_REDUNDANCY = 10;
    static const int SET_ERROR_THRESHOLD = 3;
}
}
#endif /*EPYX_DHT_PEER_H*/
