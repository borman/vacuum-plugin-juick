#include "juickplugin.h"

#include <definitions/namespaces.h>

JuickPlugin::JuickPlugin()
{
  FStanzaProcessor = NULL;
  FInHandler = new JuickProtocolPatcherIn(this);
  FOutHandler = new JuickProtocolPatcherOut(this);
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

void JuickPlugin::onStreamOpened(IXmppStream *AXmppStream)
{
  IStanzaHandle handle_in;
  handle_in.handler = FInHandler;
  handle_in.order = 200;
  handle_in.direction = IStanzaHandle::DirectionIn;
  handle_in.streamJid = AXmppStream->streamJid();
  handle_in.conditions.append("/message[@from='juick@juick.com/Juick']");

  IStanzaHandle handle_out;
  handle_out.handler = FOutHandler;
  handle_out.order = 100;
  handle_out.direction = IStanzaHandle::DirectionOut;
  handle_out.streamJid = AXmppStream->streamJid();
  handle_out.conditions.append("/message");

  FStanzaProcessor->insertStanzaHandle(handle_in);
  FStanzaProcessor->insertStanzaHandle(handle_out);
}

void JuickPlugin::onStreamClosed(IXmppStream *AXmppStream)
{
  Q_UNUSED(AXmppStream);
  /*
  pepCapableStreams.removeAll(AXmppStream->streamJid());
  unverifiedStreams.removeAll(AXmppStream->streamJid());
  if (stanzaHandles.contains(AXmppStream->streamJid()))
  {
    FStanzaProcessor->removeStanzaHandle(stanzaHandles[AXmppStream->streamJid()]);
    stanzaHandles.remove(AXmppStream->streamJid());
  }
  */
}


Q_EXPORT_PLUGIN2(plg_juick, JuickPlugin)
