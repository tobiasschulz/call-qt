#include "serverconnection.h"

ServerConnection::ServerConnection(QTcpSocket* socket, QObject* parent)
		: Connection(SERVER, parent)
{
}
QString ServerConnection::id() const
{
	return "ServerConnection<" + m_description + ">";
}

