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
	cout << "Network LISTEN" << endl; //DEBUG
	lastPacketRecieved = listen(); //corre la fsm hasta que vuelva al estado inicial.
	pushToRecieved(lastPacketRecieved);
	if (!toSend.empty()) { //si hay algo para decir, lo manda
		lastPacketSent = fetchToSend();
		cout << "Network SAY" << endl; //DEBUG
		say(lastPacketSent);
	} 
}

Packet Network::fetchToSend()
{
	Packet returnVal;
	returnVal.header = 0;

	if (!toSend.empty()) {
		returnVal = toSend.front();
		this->lastPacketSent = returnVal;
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
	std::string string;
	bool check = false, good = false;
	int timeoutcount = 0;

	do {
		if (estado == READYTOCONNECT) {
			if (getIfHost() == gameSettings::HOST) {
				sendInfo(lastPacketSent.makePacket(IAMRDY, 0, 0, myWormPos.X));
				this->estado = WAIT_READY;
				cout << "ESTADO = WAIT_READY" << endl; //DEBUG
				lastPacketRecieved = waitReady();
				if (lastPacketRecieved.header == IAMRDY) {
					otherWormPos = lastPacketRecieved.pos;
					run(READY_RECEIVED);
					estado = WAIT_REQUEST;
					cout << "ESTADO = WAIT_REQUEST" << endl; //DEBUG
				}
				else {
					run(NET_ERROR);
				}
			}
			else if (getIfHost() == gameSettings::CLIENT) {
				lastPacketRecieved = waitReady();
				if (lastPacketRecieved.header == IAMRDY) {
					otherWormPos = lastPacketRecieved.pos;
					run(READY_RECEIVED);
				}
				else {
					run(NET_ERROR);
				}
			}
		}
		else if (estado == WAIT_REQUEST) {

			lastPacketRecieved = waitRequest();

		}
		else if (estado == WAIT_ACK) {

			lastPacketRecieved = waitAck();

		}
	} while (estado != WAIT_REQUEST && estado != SHUTDOWN);

	socket->non_blocking(true);

	if (estado == SHUTDOWN) {
		lastPacketRecieved.header = ERROR_;
		lastPacketRecieved.id = 0;
	}

	return lastPacketRecieved;
}

void Network::say(Packet packet)
{
	estado = WAIT_ACK;
	cout << "ESTADO = WAIT_ACK" << endl; //DEBUG
	sendInfo(lastPacketSent.makePacket(packet.header, packet.action, packet.id, packet.pos));
	cout << "NETWORK SAID SOMETHING" << endl; //DEBUG
	if (lastPacketSent.header == QUIT_) {
		setLastEvent(QUIT_REQUEST_RECEIVED);
	}

}

Packet Network::run(int ev)
{
	setLastEvent(ev);

	if (ev == TIMEOUT1) {
		estado = WAIT_REQUEST;
		cout << "ESTADO = WAIT_REQUEST" << endl; //DEBUG
		lastPacketSent = reSend();
	}

	switch (estado) {
	case READYTOCONNECT:
		switch (ev) {
		case READY_RECEIVED:
			estado = WAIT_ACK;
			cout << "ESTADO = WAIT_ACK" << endl; //DEBUG
			lastPacketSent = sendReady();
			break;
		default:
			estado = SHUTDOWN;
			cout << "ESTADO = SHUTDOWN" << endl; //DEBUG
			lastPacketRecieved = errorComm();
		}
		break;
	case WAIT_READY:
		switch (ev) {
		case READY_RECEIVED:
			lastPacketSent = sendAckr();
			break;
		default:
			estado = SHUTDOWN;
			cout << "ESTADO = SHUTDOWN" << endl; //DEBUG
			lastPacketRecieved = errorComm();
		}
		break;
	case WAIT_REQUEST:
		switch (ev) {
		case MOVE_REQUEST_RECEIVED:
			estado = WAIT_REQUEST;
			cout << "ESTADO = WAIT_REQUEST" << endl; //DEBUG
			lastPacketSent = sendAck();
			break;
		}
	default:
		estado = SHUTDOWN;
		cout << "ESTADO = SHUTDOWN" << endl; //DEBUG
		lastPacketRecieved = errorComm();
		break;
	case WAIT_ACK:
		switch (ev) {
		case ACK_RECEIVED:
			estado = WAIT_REQUEST;
			cout << "ESTADO = WAIT_REQUEST" << endl; //DEBUG
			lastPacketRecieved = rest();
			break;
		default:
			estado = SHUTDOWN;
			cout << "ESTADO = SHUTDOWN" << endl; //DEBUG
			lastPacketRecieved = errorComm();
		}
		break;
	}

	return lastPacketRecieved;
};

Packet Network::waitReady() {

	int i = 0;
	bool good = false;
	std::string string, aux;
	uint8_t* pointer;
	lastPacketRecieved.header = 0;

	cout << "WAITING IAMREADY" << endl; //DEBUG
		string = getInfo();
		if ((string.c_str())[0] == (char)(IAMRDY)) {
			cout << "GOT IAMREADY" << endl; //DEBUG
			lastPacketRecieved.header = IAMRDY;
		}
		aux += string[1];
		aux += string[2];
		pointer = (uint8_t*)(&(lastPacketRecieved.pos));
		pointer[0] = aux[1];
		pointer[1] = aux[0];
	return lastPacketRecieved;
}

Packet Network::waitRequest() {

	int i = 0, pos = 0;
	bool check = false, good = false;
	std::string string, aux;
	uint8_t* pointer;
	lastPacketRecieved.header = 0;


	cout << "WAITING REQUEST" << endl; //DEBUG
	string = getInfo();
	if (string[0] == (char)(MOVE_)) {
		cout << "GOT REQUEST" << endl; //DEBUG
		good = true;
		lastPacketRecieved.header = MOVE_;
		lastPacketRecieved.action = string[1];
		pointer = (uint8_t*)(&lastPacketRecieved.id);
		for (int j = 0; j < 4; j++) {
			pointer[j] = string[5 - j];
		}
		if (lastPacketRecieved.id == (0xFFFFFFFE)) {
			lastPacketRecieved = run(MOVE_REQUEST_RECEIVED);
		}
		else {
			lastPacketRecieved = run(NET_ERROR);
		}
	}
	else if ((string.c_str())[0] == (char)(IAMRDY) || (string.c_str())[0] == (char)(ACK_) || (string.c_str())[0] == (char)(ERROR_)) {
		good = true;
		lastPacketRecieved = run(NET_ERROR);
	}
	else if ((string.c_str())[0] == (char)(QUIT_)) {
		good = true;
		lastPacketRecieved = run(QUIT_REQUEST_RECEIVED);
	}

	if (!good) {
		lastPacketRecieved.header = 0;
	}

	return lastPacketRecieved;
}

Packet Network::waitAck() {

	int i = 0, pos = 0;
	bool check = false, good = false;
	std::string string, aux;
	uint8_t* pointer;
	lastPacketRecieved.header = 0;
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
					lastPacketRecieved.header = ACK_;
				}
				else {
					lastPacketRecieved.header = QUIT_;
				}

				if (good) {
					lastPacketRecieved.id = 0;
					run(ACK_RECEIVED);
				}
				else {
					lastPacketRecieved = run(NET_ERROR);
				}
			}
			else {
				lastPacketRecieved = run(NET_ERROR);
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
				pointer = (uint8_t*)(&(lastPacketRecieved.id));
				for (int j = 1; j < 5; j++) {
					pointer[j - 1] = string[5 - j];
				}
				if (lastPacketRecieved.id == (0xFFFFFFFE)) {
					lastPacketRecieved = run(ACK_RECEIVED);
				}
				else {
					lastPacketRecieved = run(NET_ERROR);
				}
			}
			else {
				good = true;
				lastPacketRecieved =  run(NET_ERROR);
			}
			i++;
		} while (i < 5 && !good);
		if (!good) {
			lastPacketRecieved.header = 0;
		}
	}
	return lastPacketRecieved;
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

Packet Network::getLastPacketRecieved()
{
	return lastPacketRecieved;
}

Packet Network::getLastPacketSent()
{
	return lastPacketSent;
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
	sendInfo(lastPacketSent.makePacket(IAMRDY, 0, 0, myWormPos.X));
	cout << "SENT IAMREADY" << endl; //DEBUG
	return lastPacketSent;
}

Packet Network::sendAckr()
{
	sendInfo(lastPacketSent.makePacket(ACK_, 0, 0, 0));
	cout << "SENT ACK IAMREADY" << endl; //DEBUG
	return lastPacketSent;
}

Packet Network::rest()
{
	setLastEvent(ACK_RECEIVED);
	lastPacketRecieved.header = ACK_;
	return lastPacketRecieved;
}

Packet Network::errorComm()
{
	lastPacketRecieved.header = ERROR_;
	lastPacketRecieved.id = 0;
	cout << "PACKET ERROR" << endl; //DEBUG
	return lastPacketRecieved;
}

Packet Network::sendAck()
{
	if (getLastEvent() == MOVE_REQUEST_RECEIVED) {
		sendInfo(lastPacketSent.makePacket(ACK_, 0, (getLastPacketSent()).id, 0));
		cout << "SENT ACK MOVE" << endl; //DEBUG
	}
	else if (getLastEvent() == QUIT_REQUEST_RECEIVED) {
		sendInfo(lastPacketSent.makePacket(ACK_, 0, 0, 0));
		cout << "SENT ACK QUIT" << endl; //DEBUG
		lastPacketSent.header = QUIT_;
		lastPacketSent.id = 0;
	}
	return lastPacketSent;
}

Packet Network::reSend()
{
	sendInfo(lastPacketSent.makePacket(lastPacketSent.header, lastPacketSent.action, lastPacketSent.id, lastPacketSent.pos));
	return lastPacketSent;
}
