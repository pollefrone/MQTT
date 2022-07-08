# Para instalar o paho-mqtt use o comando pip install paho-mqtt

import paho.mqtt.client as mqtt
#import datetime 

nomes = ["b'Pollefra'","b'Pollefra2'"]

# O retorno de chamada para quando o cliente recebe uma resposta CONNACK do servidor.
def on_connect(client, userdata, flags, rc):

    # O subscribe fica no on_connect pois, caso perca a conexão ele a renova
    # Lembrando que quando usado o #, você está falando que tudo que chegar após a barra do topico, será recebido
    client.subscribe("v3r_nome_senai")
# Callback responável por receber uma mensagem publicada no tópico acima
def on_message(client, userdata, msg):
    #print(msg.topic + " -  " + str(msg.payload))
    informacacao_esp32 = str(msg.payload)
    print(informacacao_esp32)
    for nome in nomes:
        #print(nome)
        if nome == informacacao_esp32:
            print("Hora registrada")
            # if informacacao_esp32 ==

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conecta no MQTT Broker, no meu caso
client.connect("137.135.83.217", 1883, 60)

# Inicia o loop
client.loop_forever()
