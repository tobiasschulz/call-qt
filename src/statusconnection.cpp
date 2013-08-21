#include "statusconnection.h"
#include "connection.h"

StatusConnection::StatusConnection(const Contact& contact, QObject* parent)
		: Connection(STATUS, parent) {
	connect(contact.host());
	QObject::connect(this, &Connection::connected, this, &Connection::close);
}
StatusConnection::StatusConnection(const Host& host, QObject* parent)
		: Connection(STATUS, parent) {
	connect(host);
	QObject::connect(this, &Connection::connected, this, &Connection::close);
}
QString StatusConnection::id() const {
	return "StatusConnection<" + m_description + ">";
}
