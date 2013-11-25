#ifndef TCPLISTEN_H
#define TCPLISTEN_H

#include <glm/glm.hpp>
#include <QTcpSocket>
#include <QThread>
#include <QNetworkSession>
#include "GLScene.hpp"
#include <string>
#include <sstream>

class GLScene;

using namespace std;

class TcpListen : public QThread{
	Q_OBJECT

	public:
		TcpListen(GLScene *scene);
		void run();	

	private:
		QTcpSocket *tcpSocket;
		GLScene *scene;
		QNetworkSession *networkSession;

	private slots:
		void onConnected();
		void onDisconnected();
		void onReadyRead();
};

#endif