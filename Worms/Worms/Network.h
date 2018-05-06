#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <queue>
#include <fstream>
#include <iostream>
#include "Packet.h"
#include "Parser.h"
#include "GameSettings.h"
#include "Timer.h"

enum { READY_RECEIVED, ACK_RECEIVED, MOVE_REQUEST_RECEIVED, NET_ERROR, QUIT_REQUEST_RECEIVED, TIMEOUT1, TIMEOUT2 }; //FSM EVENTS
enum { READYTOCONNECT, WAIT_READY, WAIT_REQUEST, WAIT_ACK, SHUTDOWN }; //FSM STATES

class Network
{
public:
	//Constructors & destructors
	Network(std::string port);
	~Network();  

	//Networking Protocols
	void networkProtocol(); //Se encarga de las comunicaciones entre maquinas
	Packet run(int ev); //Dependiendo del evento, afecta acorde a la FSM y realiza una accion
	Packet listen(); //Corre FSM hasta volver a esperando evento
	void say(Packet packet);	//Modifica el estado de la FSM mandando un evento

	//Networking Protocol auxiliary functions
	Packet waitReady(); //Espera un I_AM_READY
	Packet waitRequest(); //Espera un evento
	Packet waitAck();	//Espera un ACK

	//Connection functions
	void acceptOrResolve(std::string port); //Se conecta dependiendo si uno es host o client
	void createLineServer();//Crea un server
	void createLineClient(std::string host, std::string port);//Crea un client
	void loadOwnIP(Parser& Parser);//Se fija que ip te pasaron por parser.
	void loadOtherIP();//Se fija cual de las dos ips en el .txt no es la que me pasaron por parser y la devuelvo.
	void sendInfo(std::string msg); //Manda informacion 
	std::string getInfoTimed(int limitInMs); //Recibe informacion timeado
	std::string getInfo();

	//Packet queue functions
	Packet fetchToSend();	//Toma un paquete para mandar
	Packet fetchRecieved(); //Toma un paquete de los recibidos
	void pushToSend(Packet);
	void pushToRecieved(Packet);

	//Getters & Setters
	int getLastEvent();
	int getState();
	void setLastEvent(int ev);
	Packet getPacket(); //Devuelve el packete auxiliar
	void setIfHost(int imhost); //Setea si uno es server o no
	std::string getOwnIP(); //Devuelve propia ip
	std::string getOtherIP(); //Devuelve la ip ajena
	int getIfHost(); //Devuelve el modo de conexion
	int getOtherWormPos();

	//Callbacks
	Packet sendReady();
	Packet sendAckr();
	Packet rest();
	Packet errorComm();
	Packet sendAck();
	Packet reSend();

private:

	//Connection tools
	boost::asio::io_service* IO_handler;
	boost::asio::ip::tcp::socket* socket;
	boost::asio::ip::tcp::resolver* resolver;
	boost::asio::ip::tcp::resolver::iterator endpoint;
	boost::asio::ip::tcp::acceptor* acceptor;
	std::string myIP; //La direccion IP de la computadora actual.
	std::string OtherIP;
	int iAmHost;
	int otherWormPos;

	//Packet queues
	std::queue<Packet> toSend;
	std::queue<Packet> Recieved;


	//FSM data
	int estado = READYTOCONNECT;
	int lastEvent;

	//Auxiliary packet for storage
	Packet packet;

	//Constants
	const std::string TIMEOUT = "timeout";
	const std::string IPFILE = "DireccionesIP.txt";
};