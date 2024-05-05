/*
 * SignalMultiplexer.h
 *
 *  Created on: May 29, 2011
 *      Author: sven
 *
 */

#ifndef SIGNALMULTIPLEXER_H_
#define SIGNALMULTIPLEXER_H_
#include <QObject>
#include <QPointer>
#include <list>
#include <map>

/**
 * Signal multiplexer idea from:
 * http://doc.trolltech.com/qq/qq08-action-multiplexer.html
 *
 * Despite to this idea this multiplexer works a quite bit different.
 * It saves all connections made for a specific active object. So this
 * object has no need of having the foreground object to be the object
 * responsible for incoming / outgoing signals. Furthermore any object
 * can be used to connect any object.
 */
class SignalMultiplexer
    : public QObject
{
    Q_OBJECT
public:
    SignalMultiplexer(QObject *parent = nullptr);

    void connect(QObject *sender, const char *signal, QObject *receiver, const char *slot);
    bool disconnect(QObject *sender, const char *signal, QObject *receiver, const char *slot);

    /**
     * Returns the actual active frontend object.
     */
    QObject *getObject() const { return m_object; }

public slots:
    void setObject(QObject *obj);

    void deleteObject( QObject *obj );

private:
    struct connectionState
    {
        connectionState( QObject *src, const char *signalName, QObject *dest, const char *slotName):
            src( src ), signalName( signalName), dest( dest ), slotName( slotName ) {}

        QPointer<QObject> src;
        const char *signalName;

        QPointer<QObject> dest;
        const char *slotName;

        bool operator==(const connectionState &o )const
        {
            return src == o.src && signalName == o.signalName && dest == o.dest && slotName == o.slotName;
        }
    };

    void connect(const connectionState &);
    void disconnect(const connectionState &);

    /**
     * This is the object being the actual active frontside object of the multiplexer.
     */
    QObject *m_object;

    typedef std::list<connectionState> TConnectionList;

    typedef std::map< QObject*, TConnectionList > StateConnectionsForObjects;

    StateConnectionsForObjects m_connectionStates;
};

#endif /* SIGNALMULTIPLEXER_H_ */
