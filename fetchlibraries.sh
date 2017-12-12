cd Firmware/Arduino/libraries

if [ -d jeelib ]; then
  cd jeelib
  echo "git pull"
  git status
  git pull
  cd ..
else
  git clone https://github.com/jcw/jeelib.git
fi

if [ -d EmonLib ]; then
  cd EmonLib
  echo "git pull"
  git status
  git pull
  cd ..
else
  git clone https://github.com/openenergymonitor/EmonLib.git
fi

if [ -d ElsterMeterReader ]; then
  cd ElsterMeterReader
  echo "git pull"
  git status
  git pull
  cd ..
else
  git clone https://github.com/openenergymonitor/ElsterMeterReader.git
fi

if [ -d CustomSoftwareSerial ]; then
  cd CustomSoftwareSerial
  rm CustomSoftwareSerial.cpp
  rm CustomSoftwareSerial.h
  echo "git pull"
  git status
  git pull
  cp src/CustomSoftwareSerial.cpp CustomSoftwareSerial.cpp
  cp src/CustomSoftwareSerial.h CustomSoftwareSerial.h
  cd ..
else
  git clone https://github.com/ledongthuc/CustomSoftwareSerial.git
  cd CustomSoftwareSerial
  cp src/CustomSoftwareSerial.cpp CustomSoftwareSerial.cpp
  cp src/CustomSoftwareSerial.h CustomSoftwareSerial.h
  cd ..
fi

if [ -d pubsubclient ]; then
  cd pubsubclient
  echo "git pull"
  git status
  git pull
  cd ..
else
  git clone https://github.com/knolleary/pubsubclient.git
fi

if [ -d OneWire ]; then
  cd OneWire
  echo "git pull"
  git status
  git pull
  cd ..
else
  git clone https://github.com/PaulStoffregen/OneWire.git
fi
