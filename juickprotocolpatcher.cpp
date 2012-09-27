#include "juickprotocolpatcher.h"

#include <QDomElement>
#include <QRegExp>
#include <QStringList>
#include <QDebug>

JuickProtocolPatcherIn::JuickProtocolPatcherIn(QObject *parent) :
    QObject(parent)
{
}

bool JuickProtocolPatcherIn::stanzaReadWrite(int AHandleId, const Jid &AStreamJid, Stanza &AStanza, bool &AAccept)
{
  Q_UNUSED(AHandleId);
  Q_UNUSED(AStreamJid);
  Q_UNUSED(AAccept);

  QDomElement body = AStanza.element().firstChildElement("body");
  QDomElement juick = AStanza.element().firstChildElement("juick");

  if (!body.isNull() && !juick.isNull())
  {
    if (juick.hasAttribute("mid")) // In thread
    {
      if (juick.hasAttribute("rid")) // Redirect replies only
      {
        QString mid = juick.attribute("mid");
        QString from = QString("%1@juick.com").arg(mid);
        AStanza.setFrom(from);
      }
      else
        return true;
    }
    else // PM
    {
      QDomElement user = juick.firstChildElement("user");
      QString username = user.attribute("uname");
      QString from = QString("pm_%1@juick.com").arg(username);
      AStanza.setFrom(from);
    }
  }

  return false;
}

//==================

JuickProtocolPatcherOut::JuickProtocolPatcherOut(QObject *parent) :
  QObject(parent)
{
}

bool JuickProtocolPatcherOut::stanzaReadWrite(int AHandleId, const Jid &AStreamJid, Stanza &AStanza, bool &AAccept)
{
  Q_UNUSED(AHandleId);
  Q_UNUSED(AStreamJid);
  Q_UNUSED(AAccept);

  bool isJuick = false;

  QRegExp userDest("pm_(.+)@juick.com");
  QRegExp threadDest("(.+)@juick.com");
  QRegExp replyRef("/\\d+ .*");

  if (userDest.exactMatch(AStanza.to()))
  {
    isJuick = true;
    QString username = userDest.capturedTexts().at(1);

    AStanza.setTo("juick@juick.com/Juick");
    QDomElement body = AStanza.element().firstChildElement("body");
    QDomCharacterData bodyText = body.firstChild().toCharacterData();
    bodyText.insertData(0, QString("PM @%1 ").arg(username));
  }
  else if (threadDest.exactMatch(AStanza.to()))
  {
    isJuick = true;
    QString thread = threadDest.capturedTexts().at(1);

    AStanza.setTo("juick@juick.com/Juick");
    QDomElement body = AStanza.element().firstChildElement("body");
    QDomCharacterData bodyText = body.firstChild().toCharacterData();
    
    QString message = bodyText.data().trimmed();

    if (message == "U")
      bodyText.setData(QString("U #%1").arg(thread));
    else if (message == "!")
      bodyText.setData(QString("! #%1").arg(thread));
    else if (replyRef.exactMatch(bodyText.data()))
      bodyText.insertData(0, QString("#%1").arg(thread)); // A reply to a reply
    else
      bodyText.insertData(0, QString("#%1 ").arg(thread)); // A reply to a thread
  }

  if (isJuick)
    AStanza.addElement("juick", "http://juick.com/message");

  return false;
}

