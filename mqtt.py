import paho.mqtt.client as mqtt
import datetime

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {str(rc)}.")
    client.subscribe("temperature/#")
    client.subscribe("humidity")

def on_message(client, userdata, msg):
    time = datetime.datetime.now()
    print(f"{time}: {msg.topic}: {str(msg.payload)}")


if __name__ == "__main__":
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("192.168.178.33", 1883, 60)
    client.loop_forever()


