/*
 * Copyright 2015 - 2021, GIBIS-Unifesp and the wiRedPanda contributors
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SERIALIZATIONFUNCTIONS_H
#define SERIALIZATIONFUNCTIONS_H

#include "qneport.h"

class QGraphicsItem;
class Editor;
class Scene;

class SerializationFunctions
{
public:
    static bool update(const QString &fileName, const QString &icDirName);
    static void serialize(const QList<QGraphicsItem *> &items, QDataStream &ds);
    static QList<QGraphicsItem *>
    deserialize(QDataStream &ds, double version, const QString &parentFile, QMap<quint64, QNEPort *> portMap = QMap<quint64, QNEPort *>());
    static QList<QGraphicsItem *> load(QDataStream &ds, const QString &parentFile, Scene *scene = nullptr);
    static double loadVersion(QDataStream &ds);
    static QRectF loadRect(QDataStream &ds, double version);
    static QString loadDolphinFilename(QDataStream &ds, double version);

private:
    static QList<QGraphicsItem *> loadMoveData(const QString &icDirName, QDataStream &ds, double version, QMap<quint64, QNEPort *> portMap);
};

#endif /* SERIALIZATIONFUNCTIONS_H */
