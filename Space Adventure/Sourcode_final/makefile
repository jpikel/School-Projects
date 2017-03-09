#Program Filename: makefile
#Author: Johannes Pikel
#Date: 2016.05.03
#Description: makefile for the Space Station adventure
#
#
#
#
#
#
#
#
#
CXX = g++
#
#
SRCS = main.cpp
SRCS += Item.cpp
SRCS += SpaceSuit.cpp
SRCS += OxygenTank.cpp
SRCS += Notepad.cpp
SRCS += LaserScalpel.cpp
SRCS += EnergyCell.cpp
SRCS += Ragdoll.cpp
SRCS += Room.cpp
SRCS += SleepingQuarters.cpp
SRCS += CargoBay.cpp
SRCS += Airlock.cpp
SRCS += Truss.cpp
SRCS += Bridge.cpp
SRCS += Laboratory.cpp
SRCS += EquipBay.cpp
SRCS += Game.cpp
#
HEADERS = Item.hpp
HEADERS += SpaceSuit.hpp
HEADERS += OxygenTank.hpp
HEADERS += Notepad.hpp
HEADERS += LaserScalpel.hpp
HEADERS += EnergyCell.hpp
HEADERS += Ragdoll.hpp
HEADERS += Room.hpp
HEADERS += SleepingQuarters.hpp
HEADERS += CargoBay.hpp
HEADERS += Airlock.hpp
HEADERS += Truss.hpp
HEADERS += Bridge.hpp
HEADERS += Laboratory.hpp
HEADERS += EquipBay.hpp
HEADERS += Game.hpp
#
# NOTES: Item.cpp include Item.hpp
# 		 SpaceSuit.cpp includes SpaceSuit.hpp; SpaceSuit child of Item
# 		 OxygenTank.cpp includes OxygenTank.hpp; OxygenTank child of Item
#		 Notepad.cpp includes Notepad.hpp; child of Item
#		 LaserScalpel.cpp includes LaserScalpel.hpp; child of Item
#		 EnergyCell.cpp includes Energycell.hpp; child of Item
#		 Ragdoll.cpp include Ragdoll.hpp and Item.hpp
#		 Room.cpp includes Room.cpp; and all Item and Item derived
#		 SleepingQuaraters.cpp includes SleepingQuarters.hpp; child of Room
#		 CargoBay.cpp includes CargoBay.hpp; child of Room
#		 Airlock.cpp includes Airlock.hpp; chile of Room
#		 Truss.cpp includes Truss.hpp; child of Room
#		 Bridge.cpp includes Bridge.hpp; child of Room
#		 Laboratory.cpp includes Laboratory.hpp; child of Room
#		 EquipBay.cpp includes EquipBay.hpp; child of Room
#CXXFLAGS = -std=c++0X
CXXFLAGS += -g
CXXFLAGS += -O2
# -O2 flag should provide optimization for the tail recursive function
CXXFLAGS += -pedantic-errors
CXXFLAGS += -Wall
#
#
#

main:  ${SRCS} ${HEADERS}
	${CXX} ${CXXFLAGS} ${SRCS} -o main

clean:
	rm -f *.o main 
