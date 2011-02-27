#ifndef JUICKPROTOCOLPATCHER_H
#define JUICKPROTOCOLPATCHER_H

#include <QObject>
#include <interfaces/istanzaprocessor.h>

class JuickProtocolPatcherIn : public QObject, public IStanzaHandler
{
  Q_OBJECT
  Q_INTERFACES(IStanzaHandler);
public:
  explicit JuickProtocolPatcherIn(QObject *parent = 0);

  // IStanzaHandler
  virtual QObject* instance() { return this; };
  virtual bool stanzaReadWrite(int AHandleId, const Jid &AStreamJid, Stanza &AStanza, bool &AAccept);
};


class JuickProtocolPatcherOut : public QObject, public IStanzaHandler
{
  Q_OBJECT
  Q_INTERFACES(IStanzaHandler);
public:
  explicit JuickProtocolPatcherOut(QObject *parent = 0);

  // IStanzaHandler
  virtual QObject* instance() { return this; };
  virtual bool stanzaReadWrite(int AHandleId, const Jid &AStreamJid, Stanza &AStanza, bool &AAccept);
};


#endif // JUICKPROTOCOLPATCHER_H
