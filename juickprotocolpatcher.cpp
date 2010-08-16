#include "juickprotocolpatcher.h"

#include <QDomElement>
#include <QRegExp>
#include <QStringList>
#include <QDebug>

JuickProtocolPatcherIn::JuickProtocolPatcherIn(QObject *parent) :
    QObject(parent)
{
}

bool JuickProtocolPatcherIn::stanzaEdit(int AHandleId, const Jid &AStreamJid, Stanza &AStanza, bool &AAccept)
{
	Q_UNUSED(AHandleId);
	Q_UNUSED(AStreamJid);
	Q_UNUSED(AStanza);
	Q_UNUSED(AAccept);

	QDomElement body = AStanza.element().firstChildElement("body");
	QDomElement juick = AStanza.element().firstChildElement("juick");

	//qDebug() << QDomNode(AStanza.element());
	qDebug() << "Gotcha!";

	if (!body.isNull() && !juick.isNull())
	{
		if (juick.hasAttribute("mid")) // In thread
		{
			QString mid = juick.attribute("mid");
			QString from = AStanza.from() + "#" + mid;
			qDebug() << "InThread:" << from;
			AStanza.setFrom(from);
		}
		else // PM
		{
			QDomElement user = juick.firstChildElement("user");
			QString username = user.attribute("uname");
			QString from = AStanza.from() + "#users/" + username;
			qDebug() << "PM:" << from;
			AStanza.setFrom(from);
		}
	}

	return false;
}

bool JuickProtocolPatcherIn::stanzaRead(int AHandleId, const Jid &AStreamJid, const Stanza &AStanza, bool &AAccept)
{
	Q_UNUSED(AHandleId);
	Q_UNUSED(AStreamJid);
	Q_UNUSED(AStanza);
	Q_UNUSED(AAccept);

	return false;
}

//==================

JuickProtocolPatcherOut::JuickProtocolPatcherOut(QObject *parent) :
	QObject(parent)
{
}

bool JuickProtocolPatcherOut::stanzaEdit(int AHandleId, const Jid &AStreamJid, Stanza &AStanza, bool &AAccept)
{
	Q_UNUSED(AHandleId);
	Q_UNUSED(AStreamJid);
	Q_UNUSED(AAccept);

	bool isJuick = false;

	QRegExp userDest("juick@juick.com/Juick#users/(.+)");
	QRegExp threadDest("juick@juick.com/Juick#(.+)");
	QRegExp replyRef("/\\d+ .*");

	if (userDest.exactMatch(AStanza.to()))
	{
		isJuick = true;
		qDebug() << "PMOut" << userDest.capturedTexts();
		QString username = userDest.capturedTexts().at(1);

		AStanza.setTo("juick@juick.com/Juick");
		QDomElement body = AStanza.element().firstChildElement("body");
		QDomCharacterData bodyText = body.firstChild().toCharacterData();
		bodyText.insertData(0, QString("PM @%1 ").arg(username));
	}
	else if (threadDest.exactMatch(AStanza.to()))
	{
		isJuick = true;
		qDebug() << "ThreadOut" << threadDest.capturedTexts();
		QString thread = threadDest.capturedTexts().at(1);

		AStanza.setTo("juick@juick.com/Juick");
		QDomElement body = AStanza.element().firstChildElement("body");
		QDomCharacterData bodyText = body.firstChild().toCharacterData();

		if (replyRef.exactMatch(bodyText.data()))
			bodyText.insertData(0, QString("#%1").arg(thread));
		else
			bodyText.insertData(0, QString("#%1 ").arg(thread));
	}

	if (isJuick)
		AStanza.addElement("juick", "http://juick.com/message");

	return false;
}

bool JuickProtocolPatcherOut::stanzaRead(int AHandleId, const Jid &AStreamJid, const Stanza &AStanza, bool &AAccept)
{
	Q_UNUSED(AHandleId);
	Q_UNUSED(AStreamJid);
	Q_UNUSED(AStanza);
	Q_UNUSED(AAccept);

	return false;
}
