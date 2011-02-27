#ifndef JuickPlugin_H
#define JuickPlugin_H

#include <interfaces/ipluginmanager.h>
#include <interfaces/istanzaprocessor.h>
#include <interfaces/ixmppstreams.h>

#include "juickprotocolpatcher.h"

#define JUICK_UUID "{a1ba3111-a550-452a-99b5-4f795387ac61}"

class JuickPlugin : public QObject, public IPlugin
{
  Q_OBJECT;
  Q_INTERFACES(IPlugin);
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
private slots:
  void onStreamOpened(IXmppStream *AXmppStream);
  void onStreamClosed(IXmppStream *AXmppStream);
private:
  IStanzaProcessor *FStanzaProcessor;
  IStanzaHandler *FInHandler;
  IStanzaHandler *FOutHandler;
};

#endif // JuickPlugin_H
