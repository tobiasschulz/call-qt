#include "statusconnection.h"
#include "connection.h"

StatusConnection::StatusConnection(QObject* parent, Contact contact)
		: Connection(parent, contact) {
}

StatusConnection::StatusConnection(QObject* parent, QHostAddress host, quint16 port)
		: Connection(parent, host, port) {
}
