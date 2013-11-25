#include "TcpListen.hpp"
#include <iostream>

TcpListen::TcpListen(GLScene *scene): networkSession(0){
	this-> scene = scene;
}

void TcpListen::run()
{
	// Create new socket
	tcpSocket = new QTcpSocket();

	// Setup socket signals
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

	// connect to ADB
	tcpSocket->connectToHost("127.0.0.1", 38300);

	// Start event loop
	exec();	
}

void TcpListen::onReadyRead()
{
	// Read data from socket as a text stream
	QTextStream in(tcpSocket);
	QString response;
	response = in.readLine();

	if(!response.isEmpty())
	{
		string responseString = response.toStdString();

		istringstream iss(responseString);

		float x, y, z;

		iss >> x >> y >> z;
		

		scene->tcpDataRcvd(glm::vec3(x,y,z));



	}
}

void TcpListen::onConnected() {
	cout << "\n CONNECTED TO ADB \n";
}
void TcpListen::onDisconnected() {
	cout << "\n DISCONNECTED FROM ADB \n";
	// shutdown or reconnect
}