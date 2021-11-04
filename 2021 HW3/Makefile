CC = g++
CFLAGS = -O3 -Wall

all: hw3

hw3: hw3.o Car.o Street.o TrafficLight.o TrafficIntersection.o AlertEvent.o
	$(CC) $(CFLAGS) -o hw3 *.o

hw3.o: hw3.cpp csv.hpp constants.hpp Car.hpp Street.hpp TrafficLight.hpp TrafficIntersection.hpp AlertEvent.hpp
	$(CC) $(CFLAGS) -c hw3.cpp

Car.o: Car.hpp Car.cpp constants.hpp csv.hpp TrafficIntersection.hpp
	$(CC) $(CFLAGS) -c Car.cpp

Street.o: Street.hpp Street.cpp constants.hpp
	$(CC) $(CFLAGS) -c Street.cpp

TrafficLight.o: TrafficLight.hpp TrafficLight.cpp constants.hpp
	$(CC) $(CFLAGS) -c TrafficLight.cpp

TrafficIntersection.o: TrafficIntersection.hpp TrafficIntersection.cpp constants.hpp AlertEvent.hpp csv.hpp TrafficLight.hpp
	$(CC) $(CFLAGS) -c TrafficIntersection.cpp

AlertEvent.o: AlertEvent.hpp AlertEvent.cpp constants.hpp
	$(CC) $(CFLAGS) -c AlertEvent.cpp

clean:
	rm -f *.o hw3
