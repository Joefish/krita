/*
 * Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _RULER_ASSISTANT_H_
#define _RULER_ASSISTANT_H_

#include "kis_painting_assistant.h"

class Ruler;

class RulerAssistant : public KisPaintingAssistant
{
public:
    RulerAssistant();
    virtual QPointF adjustPosition(const QPointF& point, const QPointF& strokeBegin);
    virtual QPointF buttonPosition() const;
    virtual int numHandles() const { return 2; }
protected:
    virtual void drawAssistant(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter* converter, bool cached, KisCanvas2* canvas, bool assistantVisible=true, bool previewVisible=true);
    virtual void drawCache(QPainter& gc, const KisCoordinatesConverter *converter,  bool assistantVisible=true);
private:
    QPointF project(const QPointF& pt) const;
};

class RulerAssistantFactory : public KisPaintingAssistantFactory
{
public:
    RulerAssistantFactory();
    virtual ~RulerAssistantFactory();
    virtual QString id() const;
    virtual QString name() const;
    virtual KisPaintingAssistant* createPaintingAssistant() const;
};

#endif
