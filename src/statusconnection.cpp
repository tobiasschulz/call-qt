#include "statusconnection.h"
#include "connection.h"

StatusConnection::StatusConnection(const Contact& contact, QObject* parent)
		: Connection(contact, STATUS, parent) {
	connect(contact);
	QObject::connect(this, &Connection::connected, this, &Connection::close);
}
StatusConnection::StatusConnection(QHostAddress host, quint16 port, QObject* parent)
		: Connection(host, port, STATUS, parent) {
	connect(host, port);
	QObject::connect(this, &Connection::connected, this, &Connection::close);
}
QString StatusConnection::id() const {
	return "StatusConnection<" + m_description + ">";
}
