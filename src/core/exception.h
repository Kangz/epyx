/**
 * Epyx exceptions
 */
#ifndef EPYX_EXCEPTION_H
#define EPYX_EXCEPTION_H

#include <string>
#include <iostream>

namespace Epyx
{
    class Exception
    {
    public:// FIXME: private ?
        std::string type;

        /**
         * Module where the exception was thrown
         */
        std::string module;

        std::string message;

    public:
        Exception(const char *type_, const char *module_, const char *message_);

        /**
         * Append to message
         */
        void append(const std::string& txt);

        /**
         * Print an exception
         */
        friend std::ostream& operator<<(std::ostream& os, Exception& e);
    };

    /**
     * Epyx failure
     */
    class FailException : public Exception
    {
    public:
        FailException(const char *module_, const char *message_);
    };

    /**
     * View errno (standard errors)
     */
    class ErrException : public FailException
    {
    public:
        ErrException(const char *module_, const char *fct);
    };
}

#endif /* EPYX_EXCEPTION_H */
