#ifndef MODEL_USERS_H
#define MODEL_USERS_H

#include "contact.h"
#include "model-abstract.h"

class ContactList;
class ContactScanner;

namespace Model
{
	class Users: public Abstract
	{
	Q_OBJECT
	public:
		explicit Users(Abstract* parentmodel, QObject* parent = 0);
		QString id() const;

		int internalSize() const;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		Contact getContact(const QModelIndex& index) const;
		User getUser(const QModelIndex& index) const;

	public slots:
		void setUsernameFormat(QString format);

	protected:
		virtual QString formatUser(const User& user) const = 0;
		QString formatUserName(const User& user) const;

	private:
		enum UsernameFormat
		{
			FORMAT_SYSTEM, FORMAT_FIRST, FORMAT_FULL
		};
		UsernameFormat m_usernameFormat;
	};

	class UsersWithComputername: public Users
	{
	Q_OBJECT
	public:
		explicit UsersWithComputername(Abstract* parentmodel, QObject* parent = 0);
		QString formatUser(const User& user) const;
	};

	class UsersWithoutComputername: public Users
	{
	Q_OBJECT
	public:
		explicit UsersWithoutComputername(Abstract* parentmodel, QObject* parent = 0);
		QString formatUser(const User& user) const;
	};
}

#endif // MODEL_USERS_H
