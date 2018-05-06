#include "Network.h"

using namespace std;


Network::Network(std::string port)
{
	IO_handler = new boost::asio::io_service();
	socket = new boost::asio::ip::tcp::socket(*IO_handler);
}


Network::~Network()
{
	socket->close();
	if (getIfHost() == gameSettings::HOST)
	{
		delete acceptor;
	}
	else if (getIfHost() == gameSettings::CLIENT)
	{
		delete resolver;
	}
	delete socket;
	delete IO_handler;
}

void Network::networkProtocol()
{
	Packet Packet;
	cout << "Network LISTEN" << endl; //DEBUG
	Packet = listen(); //corre la fsm hasta que vuelva al estado inicial.
	pushToRecieved(Packet);
	if (!toSend.empty()) { //si hay algo para decir, lo manda
		Packet = fetchToSend();
		cout << "Network SAY" << endl; //DEBUG
		say(Packet);
	} 
}

Packet Network::fetchToSend()
{
	Packet returnVal;
	returnVal.header = 0;

	if (!toSend.empty()) {
		returnVal = toSend.front();
		toSend.pop();
	}
	return returnVal;
}

Packet Network::fetchRecieved()
{
	Packet returnVal;
	returnVal.header = 0;

	if (!Recieved.empty()) {
		returnVal = Recieved.front();
		Recieved.pop();
	}

	return returnVal;
}

void Network::pushToSend(Packet packet)
{
	toSend.push(packet);
}

void Network::pushToRecieved(Packet packet)
{
	Recieved.push(packet);
}

void Network::createLineServer()
{
	acceptor->accept(*socket);
}

void Network::createLineClient(std::string host, std::string port)
{
	try {

		endpoint = resolver->resolve(boost::asio::ip::tcp::resolver::query(host, port));

		boost::asio::connect(*socket, endpoint);
	}

	catch (std::exception & e)
	{
		std::cout << "Error al intentar conectar" << std::endl;
		setIfHost(gameSettings::QUITTER);
	}
}

std::string Network::getInfoTimed(int limitInMs)
{
	Timer timer;
	char buffer[100];
	size_t lenght = 0;
	boost::system::error_code error;

	timer.start();

	bool timeout = false;

	do
	{
		lenght = this->socket->read_some(boost::asio::buffer(buffer), error);
		timer.stop();

		if (timer.getTime() > limitInMs && lenght == 0)
		{														// Pido que lenght == 0 asi no lo paro mientras esta mandando
			timeout = true;
		}

	} while (error && !timeout);

	std::string retValue;

	if (!timeout)
	{
		buffer[lenght] = 0;
		retValue = buffer;
	}
	else
		retValue = TIMEOUT;

	return retValue;
}

std::string Network::getInfo()
{
	char buffer[100]; //usado solo para recibir iamready o ackr
	size_t lenght = 0;
	boost::system::error_code error;
	std::string retValue;

	lenght = this->socket->read_some(boost::asio::buffer(buffer), error);

	if (!error) {
		buffer[lenght] = 0;
		for (int i = 0; i < lenght; i++) {
			retValue += buffer[i];
		}
	}

	return retValue;
}
void Network::sendInfo(std::string msg)
{																//&0 hacer  que funque con server y no client
	size_t lenght = 0;
	boost::system::error_code error;

	do {

		lenght = this->socket->write_some(boost::asio::buffer(msg, msg.size()), error);

	} while (error);

}

void Network::acceptOrResolve(std::string port)
{
	if (getIfHost() == gameSettings::HOST)
	{
		acceptor = new boost::asio::ip::tcp::acceptor(*IO_handler,
			boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(port)));
	}
	else
	{
		resolver = new boost::asio::ip::tcp::resolver(*IO_handler);
	}
}

using namespace std;

Packet Network::listen()
{
	Packet Packet;
	std::string string;
	bool check = false, good = false;
	int timeoutcount = 0;

	do {
		if (estado == READYTOCONNECT) {
			if (getIfHost() == gameSettings::HOST) {
				sendInfo(Packet.makePacket(IAMRDY, 0, 0, gameSettings::WormInitialPosition));
				this->estado = WAIT_READY;
				cout << "ESTADO = WAIT_READY" << endl; //DEBUG
				Packet = waitReady();
				if (Packet.header == IAMRDY) {
					otherWormPos = Packet.pos;
					run(READY_RECEIVED);
					estado = WAIT_REQUEST;
					cout << "ESTADO = WAIT_REQUEST" << endl; //DEBUG
				}
				else {
					run(NET_ERROR);
				}
			}
			else if (getIfHost() == gameSettings::CLIENT) {
				Packet = waitReady();
				if (Packet.header == IAMRDY) {
					otherWormPos = Packet.pos;
					run(READY_RECEIVED);
				}
				else {
					run(NET_ERROR);
				}
			}
		}
		else if (estado == WAIT_REQUEST) {

			Packet = waitRequest();

		}
		else if (estado == WAIT_ACK) {

			Packet = waitAck();

		}
	} while (estado != WAIT_REQUEST && estado != SHUTDOWN);

	socket->non_blocking(true);

	if (estado == SHUTDOWN) {
		Packet.header = ERROR_;
		Packet.id = 0;
	}

	return Packet;
}

void Network::say(Packet packet)
{
	estado = WAIT_ACK;
	cout << "ESTADO = WAIT_ACK" << endl; //DEBUG
	sendInfo(packet.makePacket(packet.header, packet.action, packet.id, packet.pos));
	cout << "NETWORK SAID SOMETHING" << endl; //DEBUG
	if (packet.header == QUIT_) {
		setLastEvent(QUIT_REQUEST_RECEIVED);
	}

}

Packet Network::run(int ev)
{
	Packet packet;

	setLastEvent(ev);

	if (ev == TIMEOUT1) {
		estado = WAIT_REQUEST;
		cout << "ESTADO = WAIT_REQUEST" << endl; //DEBUG
		packet = reSend();
	}

	switch (estado) {
	case READYTOCONNECT:
		switch (ev) {
		case READY_RECEIVED:
			estado = WAIT_ACK;
			cout << "ESTADO = WAIT_ACK" << endl; //DEBUG
			packet = sendReady();
			break;
		default:
			estado = SHUTDOWN;
			cout << "ESTADO = SHUTDOWN" << endl; //DEBUG
			packet = errorComm();
		}
		break;
	case WAIT_READY:
		switch (ev) {
		case READY_RECEIVED:
			packet = sendAckr();
			break;
		default:
			estado = SHUTDOWN;
			cout << "ESTADO = SHUTDOWN" << endl; //DEBUG
			packet = errorComm();
		}
		break;
	case WAIT_REQUEST:
		switch (ev) {
		case MOVE_REQUEST_RECEIVED:
			estado = WAIT_REQUEST;
			cout << "ESTADO = WAIT_REQUEST" << endl; //DEBUG
			packet = sendAck();
			break;
		}
	default:
		estado = SHUTDOWN;
		cout << "ESTADO = SHUTDOWN" << endl; //DEBUG
		packet = errorComm();
		break;
	case WAIT_ACK:
		switch (ev) {
		case ACK_RECEIVED:
			estado = WAIT_REQUEST;
			cout << "ESTADO = WAIT_REQUEST" << endl; //DEBUG
			packet = rest();
			break;
		default:
			estado = SHUTDOWN;
			cout << "ESTADO = SHUTDOWN" << endl; //DEBUG
			packet = errorComm();
		}
		break;
	}

	return packet;
};

Packet Network::waitReady() {

	int i = 0;
	bool good = false;
	std::string string, aux;
	uint8_t* pointer;
	Packet Packet;
	Packet.header = 0;

	cout << "WAITING IAMREADY" << endl; //DEBUG
		string = getInfo();
		if ((string.c_str())[0] == (char)(IAMRDY)) {
			cout << "GOT IAMREADY" << endl; //DEBUG
			good = true;
			aux += string[1];
			aux += string[2];
			pointer = (uint8_t*)(&(Packet.pos));
			pointer[0] = aux[1];
			pointer[1] = aux[0];
			Packet.header = IAMRDY;
		}
		else {
			Packet = run(NET_ERROR);
		}
	return Packet;
}

Packet Network::waitRequest() {

	int i = 0, pos = 0;
	bool check = false, good = false;
	std::string string, aux;
	uint8_t* pointer;
	Packet Packet;
	Packet.header = 0;


	cout << "WAITING REQUEST" << endl; //DEBUG
	string = getInfo();
	if (string[0] == (char)(MOVE_)) {
		cout << "GOT REQUEST" << endl; //DEBUG
		good = true;
		this->packet.header = MOVE_;
		this->packet.action = string[1];
		pointer = (uint8_t*)(&Packet.id);
		for (int j = 0; j < 4; j++) {
			pointer[j] = string[5 - j];
		}
		run(MOVE_REQUEST_RECEIVED);
		Packet = this->packet;
	}
	else if ((string.c_str())[0] == (char)(IAMRDY) || (string.c_str())[0] == (char)(ACK_) || (string.c_str())[0] == (char)(ERROR_)) {
		good = true;
		Packet = run(NET_ERROR);
	}
	else if ((string.c_str())[0] == (char)(QUIT_)) {
		good = true;
		Packet = run(QUIT_REQUEST_RECEIVED);
	}

	if (!good) {
		Packet.header = 0;
		this->packet.header = 0;
	}

	//string = getInfoTimed(gameSettings::networkTimeLimit);
	//if (string != "timeout") {
	//	check = true;
	//	if ((string.c_str())[0] == (char)(MOVE_)) {
	//		good = true;
	//		std::cout << "Received REQUEST" << std::endl;
	//		this->packet.header = MOVE_;
	//		this->packet.action = (string.c_str())[1];
	//		pointer = (uint8_t*)(&Packet.id);
	//		for (int j = 0; j < 4; j++) {
	//			pointer[j] = string[5 - j];
	//		}
	//		run(MOVE_REQUEST_RECEIVED);
	//		Packet = this->packet;
	//	}
	//	else if ((string.c_str())[0] == (char)(IAMRDY) || (string.c_str())[0] == (char)(ACK_) || (string.c_str())[0] == (char)(ERROR_)) {
	//		good = true;
	//		Packet = run(NET_ERROR);
	//	}
	//	else if ((string.c_str())[0] == (char)(QUIT_)) {
	//		Packet = run(QUIT_REQUEST_RECEIVED);
	//	}
	//}
	//if (!good) {
	//	Packet.header = 0;
	//}

	return Packet;
}

Packet Network::waitAck() {

	int i = 0, pos = 0;
	bool check = false, good = false;
	std::string string, aux;
	uint8_t* pointer;
	Packet Packet;
	Packet.header = 0;
	static bool first = true;

	if (getLastEvent() == READY_RECEIVED || getLastEvent() == QUIT_REQUEST_RECEIVED) {

		do {
			cout << "WAITING ACK READY OR QUIT" << endl; //DEBUG
			if (first) {
				first = false;
				socket->non_blocking(true);
			}
			string = getInfoTimed(gameSettings::networkTimeLimit);
			if ((string.c_str())[0] == (char)(ACK_)) {
				cout << "GOT ACK" << endl; //DEBUG
				good = true;
				if (!(string[1] == 0))
					good = false;
				if (getLastEvent() == READY_RECEIVED) {
					Packet.header = ACK_;
				}
				else {
					Packet.header = QUIT_;
				}

				if (good) {
					Packet.id = 0;
					run(ACK_RECEIVED);
				}
				else {
					run(NET_ERROR);
					Packet.header = ERROR_;
				}
			}
			else {
				run(NET_ERROR);
				Packet.header = ERROR_;
			}
			i++;
		} while (i < 5 && !good);
	}
	else {
		do {
			cout << "WAITING ACK" << endl; //DEBUG
			string = getInfo();
			if ((string.c_str())[0] == (char)(ACK_)) {
				cout << "GOT ACK" << endl; //DEBUG
				good = true;
				pointer = (uint8_t*)(&(Packet.id));
				for (int j = 1; j < 5; j++) {
					pointer[j - 1] = string[5 - j];
				}
			}
			else {
				good = true;
				Packet = run(NET_ERROR);
			}
			i++;
		} while (i < 5 && !good);
		if (!good) {
			Packet.header = 0;
		}
	}
	return Packet;
}

void Network::loadOwnIP(Parser& Parser) {
	this->myIP = Parser.myIP;
}

void Network::loadOtherIP() {
	std::ifstream dirFile;
	dirFile.open(IPFILE); //Abrimos el archivo .txt con las direcciones IP...

	std::string auxString; //...Y creamos un string auxiliar para poder trabajarlas.

	while (dirFile.good()) { // ¿Aún hay direcciones que agarrar?
		getline(dirFile, auxString); //Si las hay, la ponemos en el string auxiliar...
		if (auxString != this->myIP) {
			this->OtherIP = auxString;
		}
	}
}

////////////////////////////Setters & Getters////////////////////////////////////

int Network::getLastEvent()
{
	return lastEvent;
}

Packet Network::getPacket()
{
	return packet;
}

void Network::setLastEvent(int ev)
{
	lastEvent = ev;
}

int Network::getState()
{
	return estado;
}

void Network::setIfHost(int imhost) {
	this->iAmHost = imhost;
}

std::string Network::getOwnIP() {
	return this->myIP;
}

std::string Network::getOtherIP()
{
	return this->OtherIP;
}

int Network::getIfHost()
{
	return this->iAmHost;
}

int Network::getOtherWormPos()
{
	return otherWormPos;
}

Packet Network::sendReady()
{
	Packet packet;
	packet.header = 0;
	sendInfo(packet.makePacket(IAMRDY, 0, 0, gameSettings::WormInitialPosition + 400));
	cout << "SENT IAMREADY" << endl; //DEBUG
	return packet;
}

Packet Network::sendAckr()
{
	Packet packet;
	packet.header = 0;
	sendInfo(packet.makePacket(ACK_, 0, 0, 0));
	cout << "SENT ACK IAMREADY" << endl; //DEBUG
	return packet;
}

Packet Network::rest()
{
	Packet packet;
	setLastEvent(ACK_RECEIVED);
	packet.header = 0;
	return packet;
}

Packet Network::errorComm()
{
	Packet packet;
	packet.header = ERROR_;
	cout << "PACKET ERROR" << endl; //DEBUG
	return packet;
}

Packet Network::sendAck()
{
	Packet packet;
	packet.header = 0;
	if (getLastEvent() == MOVE_REQUEST_RECEIVED) {
		sendInfo(packet.makePacket(ACK_, 0, (getPacket()).id, 0));
		cout << "SENT ACK MOVE" << endl; //DEBUG
	}
	else if (getLastEvent() == QUIT_REQUEST_RECEIVED) {
		sendInfo(packet.makePacket(ACK_, 0, 0, 0));
		cout << "SENT ACK QUIT" << endl; //DEBUG
		packet.header = QUIT_;
	}
	return packet;
}

Packet Network::reSend()
{
	Packet packet;
	packet.header = 0;
	return packet; //pensar
}
