#ifndef JuickPlugin_H
#define JuickPlugin_H

#include <QMap>
#include <interfaces/ipluginmanager.h>
#include <interfaces/istanzaprocessor.h>
#include <interfaces/ixmppstreams.h>

#define JUICK_UUID "{a1ba3111-a550-452a-99b5-4f795387ac61}"

class JuickPlugin : public QObject, public IPlugin, public IStanzaHandler
{
  Q_OBJECT;
  Q_INTERFACES(IPlugin IStanzaHandler);
public:
  JuickPlugin();
  ~JuickPlugin();

  // IPlugin
  virtual QObject *instance() { return this; }
  virtual QUuid pluginUuid() const { return JUICK_UUID; }
  virtual void pluginInfo(IPluginInfo *APluginInfo);
  virtual bool initConnections(IPluginManager *APluginManager, int &AInitOrder);
  virtual bool initObjects();
  virtual bool initSettings();
  virtual bool startPlugin();
  // IStanzaHandler
  virtual bool stanzaReadWrite(int AHandleId, const Jid &AStreamJid, 
		  Stanza &AStanza, bool &AAccept);
private slots:
  void onStreamOpened(IXmppStream *AXmppStream);
  void onStreamClosed(IXmppStream *AXmppStream);
private:
  bool stanzaIn(Stanza &AStanza);
  bool stanzaOut(Stanza &AStanza);

  IStanzaProcessor *FStanzaProcessor;
  QMap<Jid, int> FSHIMessageIn;
  QMap<Jid, int> FSHIMessageOut;
};

#endif // JuickPlugin_H
