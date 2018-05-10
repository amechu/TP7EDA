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
	lastPacketRecieved = listen(); //corre la fsm hasta que vuelva al estado de waiting request o shutdown.
	if (lastPacketRecieved.header != 0) {
		pushToRecieved(lastPacketRecieved);
	}
	if (!toSend.empty()) { //si hay algo para decir, lo manda
		lastPacketSent = fetchToSend();
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

std::string Network::getInfo()
{
	char buffer[100];
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
{			
	size_t lenght = 0;
	boost::system::error_code error;

	lenght = this->socket->write_some(boost::asio::buffer(msg, msg.size()), error);

	if (error.value() == boost::asio::error::eof) {
		run(NET_ERROR);
	}
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
		if (estado == READYTOCONNECT) { //Handshake
			if (getIfHost() == gameSettings::HOST) {
				sendInfo(lastPacketSent.makePacket(IAMRDY, 0, 0, myWormPos.X));
				this->estado = WAIT_READY;
				lastPacketRecieved = waitReady();
				if (lastPacketRecieved.header == IAMRDY) {
					otherWormPos = lastPacketRecieved.pos;
					run(READY_RECEIVED);
				}
				else {
					run(NET_ERROR);
				}
			}
			else if (getIfHost() == gameSettings::CLIENT) {
				this->estado = WAIT_READY;
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
		else if (estado == WAIT_REQUEST) { //Espera un move o quit

			lastPacketRecieved = waitRequest();

		}
		else if (estado == WAIT_ACK) { //Espera un ack

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
	sendInfo(lastPacketSent.makePacket(packet.header, packet.action, packet.id, packet.pos));
	timeoutTimer.start(); //Para ver si hay timeout y hay que remandar
	if (lastPacketSent.header == QUIT_) {
		lastPacketRecieved = waitAckForQuit();
		pushToRecieved(lastPacketRecieved);
	}

}

Packet Network::run(int ev) //FSM switch de switches perdón
{
	setLastEvent(ev);

	if (ev == TIMEOUT1) {
		estado = WAIT_ACK;
		lastPacketSent = reSend();
	}

	switch (estado) {
	case READYTOCONNECT:
		switch (ev) {
		case READY_RECEIVED:
			estado = WAIT_ACK;
			lastPacketSent = sendReady();
			break;
		default:
			estado = SHUTDOWN;
			lastPacketRecieved = errorComm();
		}
		break;
	case WAIT_READY:
		switch (ev) {
		case READY_RECEIVED:
			if (getIfHost() == gameSettings::HOST) {
				lastPacketSent = sendAckr();
				this->estado = WAIT_REQUEST;
			}
			else if (getIfHost() == gameSettings::CLIENT) {
				lastPacketSent = sendReady();
				this->estado = WAIT_ACK;
			}
			break;
		default:
			estado = SHUTDOWN;
			lastPacketRecieved = errorComm();
		}
		break;
	case WAIT_REQUEST:
		switch (ev) {
		case MOVE_REQUEST_RECEIVED:
			estado = WAIT_REQUEST;
			lastPacketSent = sendAck();
			break;
		case QUIT_REQUEST_RECEIVED:
			estado = SHUTDOWN;
			lastPacketRecieved.header = QUIT_;
			lastPacketRecieved.id = 0;
			lastPacketSent = sendAck();
			break;
		}
		break;
	default:
		estado = SHUTDOWN;
		lastPacketRecieved = errorComm();
		break;
	case WAIT_ACK:
		switch (ev) {
		case ACK_RECEIVED:
			estado = WAIT_REQUEST;
			lastPacketRecieved = rest();
			break;
		default:
			estado = SHUTDOWN;
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

		string = getInfo();
		if ((string.c_str())[0] == (char)(IAMRDY)) {
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


	string = getInfo();
	if (string[0] == (char)(MOVE_)) {
		good = true;
		lastPacketRecieved.header = MOVE_;
		lastPacketRecieved.action = string[1];
		pointer = (uint8_t*)(&lastPacketRecieved.id);
		for (int j = 0; j < 4; j++) {
			pointer[j] = string[j + 2];
		}
		if (lastPacketRecieved.id == (0x0)) {
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
	std::string string, aux;
	uint8_t* pointer;

		string = getInfo();
		if ((string.c_str())[0] == (char)(ACK_)) {
			pointer = (uint8_t*)(&(lastPacketRecieved.id));
			for (int j = 0; j < 4; j++) {
				pointer[j] = string[j + 2];
			}
			if (lastPacketRecieved.id == (0x0)) {
				timeoutcount = 0;
				lastPacketRecieved = run(ACK_RECEIVED);
			}
			else {
				lastPacketRecieved = run(NET_ERROR);
			}
		}
		timeoutTimer.stop();
		if (timeoutTimer.getTime() > gameSettings::networkTimeLimit && timerIsOn) { //Ve si se debe remandar o si hay timeout
			timeoutcount++;
			if (timeoutcount >= 5) {
				lastPacketRecieved = run(NET_ERROR);
			}
			else {
				lastPacketSent = reSend();
			}
		}

	return lastPacketRecieved;
}

Packet Network::waitAckForQuit()
{
	int i = 0, pos = 0;
	bool check = false, good = false;
	std::string string, aux;
	uint8_t* pointer;
	lastPacketRecieved.header = 0;

	al_rest(0.5);
	do {
		string = getInfo();
		if ((string.c_str())[0] == (char)(ACK_)) {
			good = true;
			pointer = (uint8_t*)(&(lastPacketRecieved.id));
			for (int j = 0; j < 4; j++) {
				pointer[j] = string[j + 2];
			}
			if (lastPacketRecieved.id == (0x0)) {
				lastPacketRecieved.header = QUIT_;
				lastPacketRecieved.id = 0;
			}
			else {
				lastPacketRecieved = run(NET_ERROR);
			}
		}
		i++;
	} while (i < 5 && !good);
	if (!good) {
		lastPacketRecieved.header = 0;
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
	return lastPacketSent;
}

Packet Network::sendAckr()
{
	sendInfo(lastPacketSent.makePacket(ACK_, 0, 0, 0));
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
	return lastPacketRecieved;
}

Packet Network::sendAck()
{
	if (getLastEvent() == MOVE_REQUEST_RECEIVED) {
		sendInfo(lastPacketSent.makePacket(ACK_, 0, 0, 0));
	}
	else if (getLastEvent() == QUIT_REQUEST_RECEIVED) {
		sendInfo(lastPacketSent.makePacket(ACK_, 0, 0, 0));
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
