TP IoT 2023
===========

# Membres du groupe

Les membres de notre groupe sont Lou Lécrivain et Edgar Croüs.

# Architecture
L'architecture de la maquette est la suivante (donnée en séance):
- connection au broker MQTT. Un groupe doit être sub au topic. un groupe doit être pub sur le topic.
- echange des donnees de configuration LoRa via MQTT
- poursuite de l'echange en LoRa

Le sketch présent dans le dépôt correspond au sub MQTT, le groupe en collaboration avec nous est composé de Samuel Guérin, Victor Clairgeaux, Vincent Flageul. Ci-après [le dépôt correspondant](https://github.com/GuerinSamuel/Tp_IoT_2022_LoRa_Bluetooth).

# Étapes de realisation
Les étapes de réalisation effectuées/à faire sont les suivantes:
- [x] connexion à un AP wifi et sub ou pub sur le topic MQTT srt/LE
- [x] mise en place de la logique d'échange d'informations structurées de paramétrage LoRa
- [x] configuration de LoRa
- [x] envoi de messages sur LoRa
