#pragma once

#include "LousaTypes.h"

#include <QColor>
#include <QGraphicsScene>
#include <QList>
#include <QString>

class CardItem;
class ConnectionItem;
class QGraphicsLineItem;
class QTimer;

class LousaScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit LousaScene(QObject* parent = nullptr);

    void setCanvasColor(const QColor& color);
    QColor canvasColor() const { return m_color; }

    // ── Cards ────────────────────────────────────────────────────────────────
    CardItem* addCard(const CanvasCard& data);
    void      removeCard(const QString& id);
    void      clearCards();
    QList<CanvasCard> allCardData() const;
    const QList<CardItem*>& cardItems() const { return m_cards; }
    CardItem* findCard(const QString& id) const;

    // ── Conexões ─────────────────────────────────────────────────────────────
    ConnectionItem* addConnection(const CanvasConnection& data);
    void            removeConnection(const QString& id);
    void            clearConnections();
    QList<CanvasConnection> allConnectionData() const;
    ConnectionItem* findConnection(const QString& id) const;

    // ── Pin drag (chamado por CardItem) ──────────────────────────────────────
    void startPinDrag(const QString& fromCardId, const QPointF& fromScene);
    void updatePinDrag(const QPointF& cursorScene);
    void endPinDrag(const QPointF& cursorScene);

    // ── Snap de waypoint (chamado quando card se move) ───────────────────────
    void checkSnapForCard(const QString& cardId, const QPointF& cardTopCenter);

signals:
    void cardDataChanged();
    void connectionDataChanged();
    void pendingConnection(const QString& fromId, const QString& toId);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private slots:
    void onCardPositionChanged(const QString& cardId);
    void onSnapTimerFired();

private:
    void cancelSnap();

    QColor m_color{QStringLiteral("#1a1a2e")};
    QList<CardItem*>       m_cards;
    QList<ConnectionItem*> m_connections;

    // Pin drag state
    QString            m_dragFromId;
    QGraphicsLineItem* m_ghostLine = nullptr;
    QPointF            m_ghostFrom;

    // Snap timer
    QTimer*  m_snapTimer   = nullptr;
    QString  m_snapCardId;
    QString  m_snapConnId;
};
