#include "command.h"
#include "../../core/common.h"
#include "../../net/tcpsocket.h"
#include <list>
#include <sstream>

#define SOAPPREFIX "s"
#define SERVICEPREFIX "u"

namespace Epyx
{
    namespace UPNP
    {

        Command::Command()
        :NetQuery(new TCPSocket()) {
        }

        Command::Command(const SockAddress& addr, const std::string& service,
            const std::string& path)
        :NetQuery(new TCPSocket()) {
            this->setRemote(addr, path, service);
        }

        void Command::setRemote(const SockAddress& addr, const std::string& service,
            const std::string& path) {
            this->address = addr;
            this->socket().setAddress(addr);
            this->path = path;
            this->service = service;
        }

        void Command::setAction(const ActionType action) {
            this->action = action;
        }

        void Command::setArguments(const std::map<std::string, std::string>& args) {
            this->args = args;
        }

        void Command::addArgument(const std::string& name, const std::string& value) {
            args[name] = value;
        }

        bool Command::query() {
            if (!action) {
                log::error << "UPnP Command: No action specified" << log::endl;
                return false;
            }
            std::string actiontype = Action::toString(action);
            if (Action::needArgs(action) && args.empty()) {
                log::error << "UPnP Command: You must specify arguments for command "
                    << actiontype << log::endl;
                return false;
            }

            // XML header
            TiXmlDocument dom_command;
            TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
            dom_command.LinkEndChild(decl);

            // Create Envelope
            TiXmlElement *envelope = new TiXmlElement(SOAPPREFIX":Envelope");
            envelope->SetAttribute("xmlns:" SOAPPREFIX, "http://schemas.xmlsoap.org/soap/envelope/");
            envelope->SetAttribute(SOAPPREFIX":encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
            dom_command.LinkEndChild(envelope);

            // Envelope/Body
            TiXmlElement *body = new TiXmlElement(SOAPPREFIX":Body");
            envelope->LinkEndChild(body);

            // Insert command
            std::string prefixedActiontype = std::string(SERVICEPREFIX":") + actiontype;
            TiXmlElement *action_command = new TiXmlElement(prefixedActiontype.c_str());
            action_command->SetAttribute("xmlns:" SERVICEPREFIX, service.c_str());
            body->LinkEndChild(action_command);
            std::list<TiXmlElement *> list_args;
            for (std::map<std::string, std::string>::iterator it = args.begin(); it != args.end(); ++it) {
                list_args.push_back(new TiXmlElement(it->first.c_str()));
                list_args.back()->LinkEndChild(new TiXmlText(it->second.c_str()));
                action_command->LinkEndChild(list_args.back());
            }
            action_command->LinkEndChild(new TiXmlText(""));

            // XML Data Built. Now we export it into command
            TiXmlPrinter printer = TiXmlPrinter();
            printer.SetIndent("    ");
            printer.SetLineBreak(String::crlf);
            dom_command.Accept(&printer);
            std::string command = printer.CStr();

            // Prepend headers
            std::stringstream httpcommand;
            httpcommand << "POST " << path << " HTTP/1.1" << String::crlf;
            httpcommand << "Host: " << address << String::crlf;
            httpcommand << "User-Agent: Epyx Natpunching FTW" << String::crlf;
            httpcommand << "Content-Length: " << command.length() << String::crlf;
            httpcommand << "Content-Type: text/xml" << String::crlf;
            httpcommand << "SOAPAction: \"" << service << "#" << actiontype << "\"" << String::crlf;
            httpcommand << "Connection: Close" << String::crlf;
            httpcommand << "Cache-Control: no-cache" << String::crlf;
            httpcommand << "Pragma: no-cache" << String::crlf;
            httpcommand << String::crlf;
            httpcommand << command << String::crlf;

            log::debug << "UPnP::cmd::send " << actiontype << " to "
                << address << log::endl;
            //log::debug << httpcommand.str() << log::endl;

            return socket().write(httpcommand.str());
        }

        CommandResult* Command::eat(const char *data, long size) {
            // Eat data with HTTP parser
            htpars.eat(data, size);
            GTTPacket *pkt = htpars.getPacket();
            if (pkt == NULL) {
                // Incomplete packet, there may be an error
                std::string error;
                if (htpars.getError(error)) {
                    log::error << "HTTP Parser error in UPnP::command:\n"
                        << error << log::endl;
                }
                return NULL;
            }

            TiXmlDocument dom_answer;
            dom_answer.Parse(pkt->body, 0, TIXML_DEFAULT_ENCODING);

            // DEBUG
            TiXmlPrinter printer = TiXmlPrinter();
            printer.SetIndent("    ");
            printer.SetLineBreak(String::crlf);
            dom_answer.Accept(&printer);
            log::debug << "UPnP::cmd::answer has code " << pkt->method << log::endl;
            //log::debug << printer.CStr() << log::endl;

            // Create result
            CommandResult *res = new CommandResult();
            res->headers = pkt->headers;
            res->http_status = String::toInt(pkt->method);

            TiXmlNode *node = dom_answer.RootElement();
            while (!(strstr(node->Value(), "Body"))) {
                // Each Parent of <s:Body> has only an only Child
                node = node->FirstChild();
            }
            node = node->FirstChild();
            for (TiXmlNode *child = node->FirstChild(); child; child = child->NextSibling()) {
                // Normally, child is of form <Name>Value</Name>
                res->vars[child->Value()] = child->FirstChild()->Value();
            }

            delete pkt;
            return res;
        }
    }
}
