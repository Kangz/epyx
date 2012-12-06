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
 * @file input.h
 * @brief User input management
 */
#ifndef EPYX_INPUT_H
#define EPYX_INPUT_H

#include <condition_variable>

namespace Epyx
{
    /**
     * @class Input
     * @brief Epyx input management
     */
    class Input
    {
    public:
        /**
         * @brief Wait for interrupt signal (SIGINT)
         */
        static void waitForInt();

    private:
        static bool waiting;
        static std::condition_variable waitingCondition;

        static void sighandler(int signal);
    };
}

#endif /* EPYX_INPUT_H */
