import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {str(rc)}.")
    client.subscribe("temperature/#")
    client.subscribe("test")

def on_message(client, userdata, msg):
    print("Received message:")
    print(f"{msg.topic}: {msg.payload}")


if __name__ == "__main__":
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("localhost", 1883, 60)
    client.loop_forever()


