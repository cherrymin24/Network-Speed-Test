#!/bin/bash

# Variabile pentru port și timp cu valori implicite
DEFAULT_PORT=8000
DEFAULT_TIME=10

# Funcție pentru afișarea modului de utilizare corectă
usage() {
  echo "Utilizare: ./script.sh -s [-p PORT]         # Rulează serverul cu portul specificat sau default 8000"
  echo "           ./script.sh -c IP [-p PORT] [-t TIMP]  # Rulează clientul cu IP-ul serverului, portul și timpul specificate sau default 8000 și 10"
  exit 1
}

# Verificăm numărul corect de argumente
if [ "$#" -lt 1 ]; then
  echo "Număr insuficient de argumente."
  usage
fi

# Verificăm tipul de program de rulat
if [ "$1" == "-s" ]; then
  shift
  PORT=$DEFAULT_PORT
  # Verificăm opțiunea pentru port
  while getopts "p:" opt; do
    case $opt in
      p)
        PORT="$OPTARG"
        ;;
      \?)
        echo "Opțiune invalidă pentru port."
        usage
        ;;
    esac
  done
  # Rulează serverul cu portul specificat sau default
  ./server "$PORT"

elif [ "$1" == "-c" ]; then
  shift
  # Verificăm IP-ul serverului
  if [ "$#" -lt 1 ]; then
    echo "Lipsește adresa IP a serverului pentru client."
    usage
  fi
  SERVER_IP="$1"
  shift
  PORT=$DEFAULT_PORT
  TIME=$DEFAULT_TIME
  # Verificăm opțiunile pentru port și timp
  while getopts "p:t:" opt; do
    case $opt in
      p)
        PORT="$OPTARG"
        ;;
      t)
        TIME="$OPTARG"
        ;;
      \?)
        echo "Opțiune invalidă."
        usage
        ;;
    esac
  done
  # Rulează clientul cu IP-ul serverului, portul și timpul specificate sau default
  ./client "$SERVER_IP" "$PORT" "$TIME"

else
  echo "Argument necunoscut: $1"
  usage
fi

