#include "serverconnection.h"

ServerConnection::ServerConnection(QTcpSocket* socket, QObject* parent)
		: Connection(socket, SERVER, parent) {
	connect(socket);
}
QString ServerConnection::id() const {
	return "ServerConnection<" + m_description + ">";
}

