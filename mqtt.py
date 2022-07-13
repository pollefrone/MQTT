# Para instalar o paho-mqtt use o comando pip install paho-mqtt

import paho.mqtt.client as mqtt
import datetime 

# entrei_agora = datetime.datetime.now() #data e horario atual
# sai_agora = datetime.datetime.now() #data e horario atual

pollefra = 0
pollefra2 = 0
entrou = 0

# O retorno de chamada para quando o cliente recebe uma resposta CONNACK do servidor.
def on_connect(client, userdata, flags, rc):

    # O subscribe fica no on_connect pois, caso perca a conexão ele a renova
    # Lembrando que quando usado o #, você está falando que tudo que chegar após a barra do topico, será recebido
    client.subscribe("v3r_nome_senai")

# Callback responável por receber uma mensagem publicada no tópico acima
def on_message(client, userdata,msg,):
    #print(msg.topic + " -  " + str(msg.payload))
    mensagem_esp32=str(msg.payload)
   # print(msg.payload)
    # for nome in nomes:
    
    if mensagem_esp32 == "b'Pollefra'":
        global pollefra
        global entrei_agora
        if pollefra == entrou:
            entrei_agora = datetime.datetime.now() #data e horario atual
            pollefra = 1
            print("entrou o {} as {}" .format(mensagem_esp32,entrei_agora))
        else:
            sai_agora = datetime.datetime.now() #data e horario atual
            pollefra = 0
            print("saiu o {} as {}".format(mensagem_esp32, sai_agora)) 
            carga_horaria = sai_agora  - entrei_agora  
            carga_horaria_texto = str(carga_horaria)
            print("carga horaria do {} é de {}".format(mensagem_esp32, carga_horaria_texto))

    if mensagem_esp32 == "b'Pollefra2'":
        global pollefra2
        if pollefra2 == entrou:
            entrei_agora = datetime.datetime.now() #data e horario atual
            pollefra2 = 1
            print("entrou o {} as {}" .format(mensagem_esp32,entrei_agora))
        else:
            sai_agora = datetime.datetime.now() #data e horario atual
            pollefra2 = 0
            print("saiu o {} as {}".format(mensagem_esp32, sai_agora)) 
            carga_horaria = sai_agora  - entrei_agora  
            carga_horaria_texto = str(carga_horaria)
            print("carga horaria do {} é de {}".format(mensagem_esp32, carga_horaria_texto))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conecta no MQTT Broker, no meu caso
client.connect("137.135.83.217", 1883, 60)

# Inicia o loop
client.loop_forever() 

