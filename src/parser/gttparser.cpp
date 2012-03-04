#include "gttparser.h"
#include "../core/common.h"
#include <string.h>

namespace Epyx
{
    GTTParser::GTTParser()
    :currentPkt(NULL), reminded(NULL), remindedSize(0), hasError(false)
    {
        this->reset();
    }

    GTTParser::~GTTParser()
    {
        this->reset();
    }

    void GTTParser::reset()
    {
        // Discard previous data
        if (reminded != NULL) {
            delete[] reminded;
            reminded = NULL;
        }
        remindedSize = 0;
        this->startPacket();
    }

    void GTTParser::startPacket()
    {
        // Set up inernal state
        if (currentPkt != NULL) {
            delete currentPkt;
            currentPkt = NULL;
        }
        currentType = start;
        currentString = "";
        hasError = false;
        errorMessage = "";
    }

    bool GTTParser::getError(std::string& userErr)
    {
        if (!hasError) {
            userErr.assign("");
            return false;
        } else {
            userErr.assign(errorMessage);
            return true;
        }
    }

    void GTTParser::eat(const char *data, long size)
    {
        EPYX_ASSERT(data != NULL && size > 0);

        // Append data to reminded
        if (reminded != NULL) {
            char *tmp = new char[remindedSize + size + 1];
            memcpy(tmp, reminded, remindedSize);
            memcpy(tmp + remindedSize, data, size);
            delete[] reminded;
            reminded = tmp;
            remindedSize += size;
        } else {
            reminded = new char[size + 1];
            remindedSize = size;
            memcpy(reminded, data, size);
        }
        // Add a zero at the end to be sure string terminates
        reminded[remindedSize] = 0;
    }

    GTTPacket* GTTParser::getPacket()
    {
        bool endedPacket;
        char *peek = reminded;
        while (remindedSize > 0) {
            try {
                endedPacket = this->processChar(*peek);
            } catch (ParserException e) {
                log::error << "An error occured in GTTParser while processing '" << *peek << "':\n"
                        << e.message;
                errorMessage = e.message;
                hasError = true;
                return NULL;
            }
            remindedSize--;
            peek++;
            if (endedPacket) {
                // Remember the remainder
                if (reminded != NULL) {
                    char *tmp = new char[remindedSize + 1];
                    memcpy(tmp, peek, remindedSize);
                    delete[] reminded;
                    reminded = tmp;
                } else {
                    delete[] reminded;
                    reminded = NULL;
                }
                GTTPacket* pkt = currentPkt;
                currentPkt = NULL;
                this->startPacket();
                return pkt;
            }
        }

        // We have eaten everything we have
        if (reminded != NULL) {
            delete[] reminded;
            reminded = NULL;
        }
        return NULL;
    }

    /**
     * @brief sInternal function, only called by GTTParser::getPacket
     *
     * @throw ParserException on errors
     */
    bool GTTParser::processChar(char c)
    {
        // Create a nw packet if we does not have anyone yet
        if (currentPkt == NULL)
            currentPkt = new GTTPacket();

        switch(currentType) {
            case start:
                // Start a new packet
                if (!isupper(c))
                    throw ParserException("protocol name should begin with capital letters");
                currentString = c;
                currentType = protocol_name;
                flagType = other;
                break;
            case protocol_name:
                if(isupper(c) || isdigit(c) || (c == '_')) {
                    currentString += c;
                } else if(c == ' ') {
                    currentPkt->protocol = currentString;
                    currentString = "";
                    currentType = method;
                    flagType = start;
                }
                else
                    throw ParserException("protocol name should continue with [A-Z0-9]* or end with a space");
                break;
            case method:
                if (flagType == start){
                    if (!isupper(c))
                        throw ParserException("method should begin with capital letters");
                    currentString = c;
                    flagType = other;
                }
                else if (isupper(c) || isdigit(c) || (c == '_'))
                    currentString += c;
                else if (c == '\r') {
                    currentPkt->method = currentString;
                    currentString = "";
                    currentType = method_line;
                } else
                    throw ParserException("method should continue with [A-Z0-9_] or end with CRLF");
                break;
            case method_line:
                if (c != '\n')
                    throw ParserException("newline should be CRLF");
                currentType = flag_name;
                flagType = start;
                break;
            case flag_name:
                if(flagType == start){
                    if (isalpha(c))
                        currentString = c;
                    else if (c == '\r') {
                        // New line at the end of the headers
                        currentType = newline;
                    } else
                        throw ParserException("flag_name should begin with [a-zA-Z] or we should add newline to end the header");
                    flagType = other;
                } else if(isalnum(c) || (c == '_') || (c == '-')) {
                    currentString += c;
                } else if(c == ':') {
                    currentFlagName = currentString;
                    currentString = "";
                    currentType = flag_indicator;
                    /*
                    if(!strcasecmp(currentString.c_str(), "content-length")) {
                        if(currentPkt->size > 0)
                            throw FailException("Parser", "content-length flag has already appeared");
                        else
                            hasBody=true;

                    }*/
                } else
                    throw ParserException("flag_name should continue with [a-zA-Z0-9_-] or end with :");
                break;
            case flag_indicator:
                // Skip spaces
                if(c == ' ')
                    break;
                /* Walk through */
            case flag_value:
                if (32 <= c && c < 126) {
                    // Printable ASCII characters
                    currentString += c;
                } else if(c == '\r') {
                    if(!strcasecmp(currentFlagName.c_str(), "content-length")) {
                        if(currentPkt->size > 0)
                            throw FailException("Parser", "content-length flag has already appeared");
                        currentPkt->size = String::toInt(currentString.c_str());
                        if (currentPkt->size <= 0)
                            throw FailException("Parser", "not valid body size, body size should be a positive integer");
                        currentPkt->body = new char[currentPkt->size];
                    } else
                        currentPkt->headers[currentFlagName] = currentString;
                    currentString = "";
                    currentFlagName = "";
                    currentType = flag_line;
                } else
                    throw ParserException("flag_value should consist of printable characters or just end with CRLF");
                break;
            case flag_line:
                if (c != '\n')
                    throw ParserException("newline should be CRLF");
                currentType = flag_name;
                flagType = start;
                break;
            case newline:
                if (c != '\n')
                    throw ParserException("newline should be CRLF");
                if (currentPkt->size == 0) {
                    // No body
                    return true;
                }
                bodyCounter = 0;
                currentType = body;
                break;
            case body:
                EPYX_ASSERT(bodyCounter < currentPkt->size);
                currentPkt->body[bodyCounter] = c;
                bodyCounter ++;
                if (bodyCounter == currentPkt->size) {
                    // Return there is a new packet
                    return true;
                }
                break;
            case other:
                throw ParserException("Unreachable state");
        }
        return false;
    }
}
