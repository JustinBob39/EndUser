#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <tinyxml2.h>

using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;
using tinyxml2::XMLText;
using tinyxml2::XMLText;

class Config {
    std::string server_uri = "mqtt://localhost:1883";
    std::string client_id = "test_client";
    std::string topic = "test_topic";
    int connect_timeout = 10;
    int keep_alive_interval = 60;
    bool clean_session = false;
    int qos = 1;

    void parseFile() {
        XMLDocument doc;
        if (doc.LoadFile("config.xml") != tinyxml2::XML_SUCCESS) {
            std::cerr << "Failed to load config.xml" << std::endl;
            return;
        }
        if (XMLElement *attributes_element = doc.FirstChildElement("attributes")) {
            XMLElement *server_uri_element = attributes_element->FirstChildElement("server_uri");
            XMLElement *client_id_element = attributes_element->FirstChildElement("client_id");
            XMLElement *topic_element = attributes_element->FirstChildElement("topic");
            XMLElement *connect_timeout_element =
                    attributes_element->FirstChildElement("connect_timeout");
            XMLElement *keep_alive_interval_element =
                    attributes_element->FirstChildElement("keep_alive_interval");
            XMLElement *clean_session_element = attributes_element->FirstChildElement("clean_session");
            XMLElement *qos_element = attributes_element->FirstChildElement("qos");
            if (server_uri_element) {
                server_uri = server_uri_element->FirstChild()->ToText()->Value();
            }
            if (client_id_element) {
                client_id = client_id_element->FirstChild()->ToText()->Value();
            }
            if (topic_element) {
                topic = topic_element->FirstChild()->ToText()->Value();
            }
            if (connect_timeout_element) {
                const auto val_str = std::string(connect_timeout_element->FirstChild()->ToText()->Value());
                connect_timeout = std::stoi(val_str);
            }
            if (keep_alive_interval_element) {
                const auto val_str = std::string(keep_alive_interval_element->FirstChild()->ToText()->Value());
                keep_alive_interval = std::stoi(val_str);
            }
            if (clean_session_element) {
                if (clean_session_element->FirstChild()->ToText()->Value() == std::string("true")) {
                    clean_session = true;
                }
            }
            if (qos_element) {
                const auto val_str = std::string(qos_element->FirstChild()->ToText()->Value());
                qos = std::stoi(val_str);
            }
        }
    }

public:
    Config() {
        parseFile();
    }

    [[nodiscard]] std::string get_server_uri() const {
        return server_uri;
    }

    [[nodiscard]] std::string get_client_id() const {
        return client_id;
    }

    [[nodiscard]] std::string get_topic() const {
        return topic;
    }

    [[nodiscard]] int get_connect_timeout() const {
        return connect_timeout;
    }

    [[nodiscard]] int get_keep_alive_interval() const {
        return keep_alive_interval;
    }

    [[nodiscard]] bool get_clean_session() const {
        return clean_session;
    }

    [[nodiscard]] int get_qos() const {
        return qos;
    }
};

#endif //CONFIG_H
