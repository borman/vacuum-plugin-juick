#include "juickplugin.h"

#include <definitions/namespaces.h>

JuickPlugin::JuickPlugin()
{
  FStanzaProcessor = NULL;
}

JuickPlugin::~JuickPlugin()
{
}

void JuickPlugin::pluginInfo(IPluginInfo *APluginInfo)
{
  APluginInfo->name = tr("Juick");
  APluginInfo->description = tr("Adds a user-friendly interface for juick.com microblogging service");
  APluginInfo->version = "0.1";
  APluginInfo->author = "Mikhail Borisov";
  APluginInfo->homePage = "http://www.vacuum-im.org";
  APluginInfo->dependences.append(STANZAPROCESSOR_UUID);
  APluginInfo->dependences.append(XMPPSTREAMS_UUID);
}

bool JuickPlugin::initConnections(IPluginManager *APluginManager, int &AInitOrder)
{
  Q_UNUSED(AInitOrder);

  IPlugin *plugin = APluginManager->pluginInterface("IStanzaProcessor").value(0,NULL);
  if (plugin)
  {
    FStanzaProcessor = qobject_cast<IStanzaProcessor *>(plugin->instance());
  }

  plugin = APluginManager->pluginInterface("IXmppStreams").value(0,NULL);
  if (plugin)
  {
    IXmppStreams *FXmppStreams = qobject_cast<IXmppStreams *>(plugin->instance());
    if (FXmppStreams)
    {
      connect(FXmppStreams->instance(),
          SIGNAL(opened(IXmppStream *)),
          SLOT(onStreamOpened(IXmppStream *)));
      connect(FXmppStreams->instance(),
          SIGNAL(closed(IXmppStream *)),
          SLOT(onStreamClosed(IXmppStream *)));
    }
  }

  return (FStanzaProcessor != NULL);
}

void JuickPlugin::onStreamOpened(IXmppStream *AXmppStream)
{
  IStanzaHandle handle;
  handle.handler = this;
  handle.streamJid = AXmppStream->streamJid();

  handle.order = 100;
  handle.direction = IStanzaHandle::DirectionIn;
  handle.conditions.append("/message[@from='juick@juick.com/Juick']");
  FSHIMessageIn.insert(handle.streamJid, 
		  FStanzaProcessor->insertStanzaHandle(handle));

  handle.order = 1001;
  handle.direction = IStanzaHandle::DirectionOut;
  handle.conditions.clear();
  handle.conditions.append("/message");
  FSHIMessageOut.insert(handle.streamJid,
		  FStanzaProcessor->insertStanzaHandle(handle));
}

bool JuickPlugin::stanzaReadWrite(int AHandleId, const Jid &AStreamJid, 
		Stanza &AStanza, bool&AAccept)
{
	if (FSHIMessageIn.value(AStreamJid) == AHandleId)
		return stanzaIn(AStanza);
	else if (FSHIMessageOut.value(AStreamJid) == AHandleId)
		return stanzaOut(AStanza);
	else
		return false;
}

bool JuickPlugin::stanzaIn(Stanza &AStanza)
{
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
        return true; // Drop post announcements
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

bool JuickPlugin::stanzaOut(Stanza &AStanza)
{
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

void JuickPlugin::onStreamClosed(IXmppStream *AXmppStream)
{
  Q_UNUSED(AXmppStream);
  if (FStanzaProcessor)
  {
	  FStanzaProcessor->removeStanzaHandle(
			  FSHIMessageIn.take(AXmppStream->streamJid()));
	  FStanzaProcessor->removeStanzaHandle(
			  FSHIMessageOut.take(AXmppStream->streamJid()));
  }
}

bool JuickPlugin::initObjects()
{
  return true;
}

bool JuickPlugin::initSettings()
{
  return true;
}

bool JuickPlugin::startPlugin()
{
  return true;
}

Q_EXPORT_PLUGIN2(plg_juick, JuickPlugin)
