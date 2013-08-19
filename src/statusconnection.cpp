#include "statusconnection.h"
#include "connection.h"

StatusConnection::StatusConnection(Contact* contact, QObject* parent)
		: Connection(contact, STATUS, parent) {
	connect(contact);
}
StatusConnection::StatusConnection(QHostAddress host, quint16 port, QObject* parent)
		: Connection(host, port, STATUS, parent) {
	connect(host, port);
}
QString StatusConnection::id() const {
	return "StatusConnection<" + m_description + ">";
}
