#pragma once

#include "TimelineTypes.h"

#include <QColor>
#include <QGraphicsObject>
#include <QPointF>

class TimelineEventItem : public QGraphicsObject {
    Q_OBJECT
public:
    explicit TimelineEventItem(const TimelineEvent& data,
                               QGraphicsItem* parent = nullptr);

    const TimelineEvent& eventData() const { return m_data; }
    void setEventData(const TimelineEvent& d);
    void setTimelineColor(const QColor& c);

    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

    QPointF pinPos() const;
    qreal   currentH() const; // altura real (collapsed ou expanded)

    // Rola a descricao quando expandido. Retorna true se o scroll foi consumido.
    bool wheelScroll(int angleDeltaY);

    static constexpr qreal kW        = 120.0;
    static constexpr qreal kH        =  64.0; // altura colapsada
    static constexpr qreal kHexpMin  = 120.0; // altura expandida mínima
    static constexpr qreal kRadius   =   8.0;
    static constexpr qreal kShadow   =   3.0;

signals:
    void dataChanged(const TimelineEvent& data);
    void deleteRequested(const QString& id);
    void positionChanged(const QString& id);
    void geometryChanged(const QString& id); // expand/resize
    void editRequested(const QString& id);
    void pinDragStarted(const QString& fromId, const QPointF& pinScenePos);
    void exportAsDocRequested(TimelineEvent data);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e)        override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e)         override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e)      override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e)  override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* e)         override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* e)        override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* e) override;
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value)               override;

private:
    QColor effectiveColor() const;
    QColor textColor() const;
    bool   isOnPin(const QPointF& localPos) const;
    bool   isOnResizeGrip(const QPointF& localPos) const;
    QRectF resizeGripRect() const;
    void   setExpandedH(qreal h);
    qreal  descContentH() const; // altura total do texto da descricao
    qreal  descVisH()     const; // altura visivel da area de descricao
    void   paintScrollbar(QPainter* p) const;

    TimelineEvent m_data;
    QColor        m_timelineColor{QStringLiteral("#6c8ebf")};

    bool   m_draggingPin      = false;
    bool   m_hoverPin         = false;
    bool   m_hoverGrip        = false;
    bool   m_resizing         = false;
    QPointF m_resizeDragStart;
    qreal  m_resizeDragStartH = 0.0;
    qreal  m_scrollOffset     = 0.0;
};
