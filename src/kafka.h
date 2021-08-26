// Header-only library dealing with kafka cluster connections 
// and consuming of data

#ifndef KAFKA_H_
#define KAFKA_H_

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <cppkafka/consumer.h>
#include <cppkafka/configuration.h>

#include "logger.h"
#include "util.h"

namespace kafka {

    using cppkafka::Consumer;
    using cppkafka::Configuration;
    using cppkafka::Message;
    using cppkafka::TopicPartitionList;
    using std::string;

    inline void consume(string brokers, std::vector<std::string> topics, int maximum, std::shared_ptr<bool> running, std::function<void(std::shared_ptr<string>, std::shared_ptr<std::string>)> callback) {
        string group_id = "group-id"; // todo: need to be user-unique

        // Construct the configuration
        Configuration config = {
            { "metadata.broker.list", brokers },
            { "group.id", group_id },
            // auto commit
            { "enable.auto.commit", false },
            //{"auto.commit.interval.ms", 250}
        };

        // Create the consumer
        Consumer consumer(config);

        // Print the assigned partitions on assignment
        consumer.set_assignment_callback([](const TopicPartitionList& partitions) {
            logger_trace << "Got assigned: " << partitions;
        });

        // Print the revoked partitions on revocation
        consumer.set_revocation_callback([](const TopicPartitionList& partitions) {
            logger_trace << "Got revoked: " << partitions;
        });

        // Subscribe to the topic
        consumer.subscribe(topics);

        for (auto topic : topics) {
            logger_info << "Consuming messages from topic " << topic;
        }
        
        // Now read lines and write them into kafka
        int count = 0;
        while (*running && (count < maximum || maximum == 0)) {
            // Try to consume a message
            Message msg = consumer.poll();
            if (msg) {
                // If we managed to get a message
                if (msg.get_error()) {
                    // Ignore EOF notifications from rdkafka
                    if (!msg.is_eof()) {
                        //logger_err << "[+] Received error notification: " << msg.get_error();
                    }
                }
                else {
                    ++count;

                    auto topic_ptr = std::make_shared<std::string>(msg.get_topic());
                    auto msg_ptr = std::make_shared<std::string>(msg.get_payload());

                    if (topic_ptr->find("arrays") != std::string::npos) {
                        // todo: run a test on the json and check if it's an array (now just checking topic name but should test the json itself)
                        msg_ptr = util::try_json_array(msg_ptr);
                    }

                    callback(msg_ptr, topic_ptr);
                }
            }
        }
    }
}

#endif // KAFKA_H_