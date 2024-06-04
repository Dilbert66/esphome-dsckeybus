import time

import paho.mqtt.client as mqtt

mqtt_host = "192.168.1.xx"
mqtt_port = 1883
mqtt_username = "mqtt"
mqtt_password = "xxxx"

device_name = "dscalarm"  # Whatever name you gave it in your yaml
sensor_name = "/sensor/msg_partition_1/state" 
start_code = 0
end_code = 9999
delay = 0

user_data = []


def on_subscribe(client, userdata, mid, reason_code_list, properties):
    # Since we subscribed only for a single channel, reason_code_list contains
    # a single entry
    if reason_code_list[0].is_failure:
        print(f"Broker rejected you subscription: {reason_code_list[0]}")
    else:
        print(f"Broker granted the following QoS: {reason_code_list[0].value}")


def on_unsubscribe(client, userdata, mid, reason_code_list, properties):
    if len(reason_code_list) == 0 or not reason_code_list[0].is_failure:
        print("unsubscribe succeeded (if SUBACK is received in MQTTv3 it success)")
    else:
        print(f"Broker replied with failure: {reason_code_list[0]}")
    client.disconnect()


def on_message(client, userdata, message):
    userdata.append(message.payload)


def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        client.subscribe(device_name + sensor_name)


def wait_for_data(timeout=10):
    start_time = time.time()
    while start_time + timeout > time.time():
        time.sleep(0.01)
        if len(user_data) > 0:
            return True
    return False


def main():
    global start_code

    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.username_pw_set(mqtt_username, mqtt_password)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    mqttc.on_subscribe = on_subscribe
    mqttc.on_unsubscribe = on_unsubscribe

    mqttc.user_data_set(user_data)
    mqttc.connect(mqtt_host, mqtt_port, 60)

    mqttc.loop_start()

    time.sleep(1)
    for msg in user_data:
        print(msg)
    mqttc.publish(device_name + "/alarm/set", "{\"keys\":\"##\",\"partition\":1}")
    wait_for_data()           
    user_data.clear()

    start_batch_time = time.time()

    while True:
        if start_code > end_code:
            print("!!! ALL CODES TESTED !!!")
            break

        print("sending #*8")
        mqttc.publish(device_name + "/alarm/set", "{\"keys\":\"*8\",\"partition\":1}")
        if not wait_for_data():
            print("no response")
        else:
            print(user_data[0])

        if len(user_data) != 0 and user_data[0] == b"B7: Installer code":
            user_data.clear()

            test_code = '{num:04d}'.format(num=start_code)
            start_code = start_code + 1

            print("sending code", test_code)
            mqttc.publish(device_name + "/alarm/set", "{\"keys\":\"" + test_code + "\",\"partition\":1}")
            if not wait_for_data():
                break

            print(user_data[0])
            # Log code and result to a file
            with open("codes.txt", 'a') as file1:
                file1.write(test_code + "\t" + str(user_data[0]) + "\n")

            if user_data[0] == b"E4: Installer menu":
                print("!!!! CODE FOUND !!!!")
                print("====    " + test_code + "    ====")
                mqttc.publish(device_name + "/alarm/set", "{\"keys\":\"##\",\"partition\":1}")                
                break

            if user_data[0] == b"8F: Invalid code":
                # Expected response for invalid code, so continue
                pass
            elif user_data[0] == b"10: Keypad lockout":
                # Lockout, wait for it to clear
                print("Keypad lockout, waiting to clear")
                user_data.clear()
                start_wait_time = time.time()
                if not wait_for_data(600):  # wait max 5 minutes for lockout to clear
                    break
                else:
                    batch_run_time = start_wait_time - start_batch_time
                    waited_time = time.time() - start_wait_time
                    print("Ran {}s Waited {}s Total {}s".
                          format(batch_run_time, waited_time, waited_time + batch_run_time))
                    user_data.clear()
                    start_batch_time = time.time()
                    continue
            else:
                print("unknown response, retrying")
                start_code = start_code - 1
                user_data.clear()
                
        mqttc.publish(device_name + "/alarm/set", "{\"keys\":\"##\",\"partition\":1}")
        wait_for_data()        
        user_data.clear()
        time.sleep(delay)                
"""
        while len(user_data) == 0 or user_data[0] != b"03: Zones open":
            user_data.clear()

            print("sending ##")
            mqttc.publish(device_name + "/alarm/set", "{\"keys\":\"##\",\"partition\":1}")
            if not wait_for_data():
                break
            print(user_data[0])

        user_data.clear()
        time.sleep(delay)
"""       

    mqttc.loop_stop()


if __name__ == '__main__':
    main()
