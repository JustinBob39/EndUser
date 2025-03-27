#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

#include <mqtt/async_client.h>

#include "Config.h"
#include "Deserializer.h"

void print_time() {
    const auto now = std::chrono::system_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    const std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::cout << "Time: " << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count() << std::endl;
}

int main(const int argc, char *argv[]) {
    const Config config;
    mqtt::async_client client(config.get_server_uri(), config.get_client_id());

    const auto connect_options = mqtt::connect_options_builder::v3()
            .connect_timeout(std::chrono::seconds(config.get_connect_timeout()))
            .keep_alive_interval(std::chrono::seconds(config.get_keep_alive_interval()))
            .clean_session(config.get_clean_session())
            .automatic_reconnect()
            .finalize();

    client.set_connected_handler([](const std::string &) {
        std::cout << "\n*** Connected ***" << std::endl;
    });

    try {
        client.start_consuming();

        std::cout << "Connecting to the MQTT server..." << std::endl;
        const auto token_ptr = client.connect(connect_options);
        auto connect_response = token_ptr->get_connect_response();

        if (!connect_response.is_session_present()) {
            std::cout << "No session present on server. Subscribing..." << std::endl;
            client.subscribe(config.get_topic(), config.get_qos())->wait();
        }

        std::cout << "Waiting for messages on topic: '" << config.get_topic() << "'" << std::endl;
        while (true) {
            auto message = client.consume_message();
            Deserializer deserializer = {};
            if (message) {
                print_time();
                std::cout << "Topic: " << message->get_topic() << std::endl;
                std::cout << "Qos: " << message->get_qos() << std::endl;
                std::vector<Deserializer::Entry> entries = deserializer.transform(message->get_payload().data(),
                    static_cast<int>(message->get_payload().size()));
                for (const auto entry: entries) {
                    std::cout << entry.to_string() << std::endl;
                }
            } else {
                std::cout << "*** Connection Lost ***" << std::endl;
            }
        }
    } catch (const mqtt::exception &exc) {
        std::cerr << exc << std::endl;
        return 1;
    }

    return 0;
}
