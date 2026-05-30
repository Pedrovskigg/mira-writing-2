#include "TimelineEventItem.h"

#include <QAction>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QLinearGradient>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>

namespace {

constexpr qreal kPinR    = 5.0;
constexpr qreal kGripH   = 8.0;  // altura do grip de resize
constexpr qreal kGripW   = 40.0; // largura do grip
constexpr qreal kDescTop = 50.0; // onde começa a area de descricao (y local)
constexpr qreal kDescBot = 20.0; // padding inferior (acima do grip)

QRectF pinRect(const QPointF& c)
{
    return QRectF(c.x() - kPinR, c.y() - kPinR, kPinR * 2, kPinR * 2);
}

QBrush fillBrush(const QColor& base, const QRectF& r)
{
    QLinearGradient g(r.topLeft(), r.bottomLeft());
    g.setColorAt(0.0, base.lighter(125));
    g.setColorAt(0.5, base);
    g.setColorAt(1.0, base.darker(120));
    return QBrush(g);
}

QPainterPath roundedRect(const QRectF& r, qreal radius)
{
    QPainterPath p;
    p.addRoundedRect(r, radius, radius);
    return p;
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────

TimelineEventItem::TimelineEventItem(const TimelineEvent& data,
                                     QGraphicsItem* parent)
    : QGraphicsObject(parent), m_data(data)
{
    setPos(data.x, data.y);
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsGeometryChanges |
             QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
}

// ── Getters / setters ────────────────────────────────────────────────────────

qreal TimelineEventItem::currentH() const
{
    return m_data.expanded ? m_data.expandedH : kH;
}

void TimelineEventItem::setEventData(const TimelineEvent& d)
{
    prepareGeometryChange();
    m_data = d;
    m_scrollOffset = 0.0; // reset ao carregar novo conteudo
    setPos(d.x, d.y);
    update();
}

void TimelineEventItem::setTimelineColor(const QColor& c)
{
    m_timelineColor = c;
    update();
}

QPointF TimelineEventItem::pinPos() const
{
    return QPointF(kW + kShadow + kPinR, currentH() / 2.0);
}

QColor TimelineEventItem::effectiveColor() const
{
    return m_data.color.isValid() ? m_data.color : m_timelineColor;
}

QColor TimelineEventItem::textColor() const
{
    const QColor bg = effectiveColor();
    const qreal lum = 0.299 * bg.redF() + 0.587 * bg.greenF() + 0.114 * bg.blueF();
    return lum > 0.5 ? QColor(30, 30, 30) : QColor(240, 240, 240);
}

bool TimelineEventItem::isOnPin(const QPointF& p) const
{
    return pinRect(pinPos()).contains(p);
}

QRectF TimelineEventItem::resizeGripRect() const
{
    const qreal cx = kW / 2.0;
    const qreal y  = currentH() - kGripH - 4.0;
    return QRectF(cx - kGripW / 2.0, y, kGripW, kGripH);
}

bool TimelineEventItem::isOnResizeGrip(const QPointF& p) const
{
    return m_data.expanded && resizeGripRect().adjusted(-4, -4, 4, 4).contains(p);
}

void TimelineEventItem::setExpandedH(qreal h)
{
    const qreal clamped = qMax(kHexpMin, h);
    if (qFuzzyCompare(clamped, m_data.expandedH)) return;
    prepareGeometryChange();
    m_data.expandedH = clamped;
    // clamp scroll para nao ultrapassar o novo maximo
    const qreal maxS = qMax(0.0, descContentH() - descVisH());
    m_scrollOffset = qBound(0.0, m_scrollOffset, maxS);
    update();
}

qreal TimelineEventItem::descVisH() const
{
    return qMax(0.0, currentH() - kDescTop - kDescBot);
}

qreal TimelineEventItem::descContentH() const
{
    if (m_data.description.isEmpty()) return 0.0;
    QFont f;
    f.setPointSizeF(8.5);
    const QFontMetrics fm(f);
    return fm.boundingRect(QRect(0, 0, qRound(kW - 22), 100000),
                           Qt::TextWordWrap, m_data.description).height();
}

bool TimelineEventItem::wheelScroll(int angleDeltaY)
{
    if (!m_data.expanded) return false;
    const qreal visH    = descVisH();
    const qreal contH   = descContentH();
    const qreal maxScroll = qMax(0.0, contH - visH);
    if (maxScroll < 1.0) return false; // sem overflow → deja zoom agir

    const qreal delta = (angleDeltaY / 120.0) * 36.0;
    m_scrollOffset = qBound(0.0, m_scrollOffset - delta, maxScroll);
    update();
    return true;
}

void TimelineEventItem::paintScrollbar(QPainter* p) const
{
    const qreal visH    = descVisH();
    const qreal contH   = descContentH();
    if (contH <= visH + 1.0 || visH <= 0.0) return;

    const qreal maxScroll = contH - visH;
    const qreal thumbH    = qMax(14.0, visH * visH / contH);
    const qreal trackX    = kW - 5.0;
    const qreal thumbY    = kDescTop + (maxScroll > 0.0 ? (m_scrollOffset / maxScroll) : 0.0)
                                        * (visH - thumbH);
    p->save();
    p->setPen(Qt::NoPen);
    // track
    p->setBrush(QColor(255, 255, 255, 28));
    p->drawRoundedRect(QRectF(trackX, kDescTop, 3.0, visH), 1.5, 1.5);
    // thumb
    p->setBrush(QColor(255, 255, 255, 110));
    p->drawRoundedRect(QRectF(trackX, thumbY, 3.0, thumbH), 1.5, 1.5);
    p->restore();
}

// ── Bounding / shape ─────────────────────────────────────────────────────────

QRectF TimelineEventItem::boundingRect() const
{
    return QRectF(-kShadow, -kShadow,
                  kW + kShadow * 2 + kPinR * 2 + 2,
                  currentH() + kShadow * 2);
}

QPainterPath TimelineEventItem::shape() const
{
    QPainterPath p = roundedRect(QRectF(0, 0, kW, currentH()), kRadius);
    QPainterPath pin;
    pin.addEllipse(pinRect(pinPos()));
    return p.united(pin);
}

// ── Paint ────────────────────────────────────────────────────────────────────

void TimelineEventItem::paint(QPainter* p,
                               const QStyleOptionGraphicsItem*,
                               QWidget*)
{
    p->setRenderHint(QPainter::Antialiasing);

    const qreal  h    = currentH();
    const QRectF body(0, 0, kW, h);
    const QColor fill   = effectiveColor();
    const QColor border = fill.darker(155);
    const QColor txt    = textColor();

    // ── Sombra suave ────────────────────────────────────────────────────────
    for (int i = 3; i >= 1; --i) {
        const QRectF sh = body.adjusted(i, i, i, i);
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(0, 0, 0, 18 * i));
        p->drawPath(roundedRect(sh, kRadius));
    }

    // ── Corpo ───────────────────────────────────────────────────────────────
    p->setBrush(fillBrush(fill, body));
    p->setPen(QPen(border, isSelected() ? 2.0 : 1.0));
    p->drawPath(roundedRect(body, kRadius));

    // ── Anel de seleção ─────────────────────────────────────────────────────
    if (isSelected()) {
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(QColor(255, 255, 255, 200), 1.5, Qt::DashLine));
        p->drawPath(roundedRect(body.adjusted(-3, -3, 3, 3), kRadius + 3));
    }

    // ── Marcador de tempo (topo) ─────────────────────────────────────────────
    if (!m_data.timeMarker.isEmpty()) {
        QFont f;
        f.setPointSizeF(7.0);
        p->setFont(f);
        p->setPen(QColor(txt.red(), txt.green(), txt.blue(), 150));
        p->drawText(body.adjusted(8, 5, -8, 0),
                    Qt::AlignTop | Qt::AlignHCenter,
                    m_data.timeMarker);
    }

    // ── Título ──────────────────────────────────────────────────────────────
    QFont fTitle;
    fTitle.setPointSizeF(9.0);
    fTitle.setBold(true);
    p->setFont(fTitle);
    p->setPen(txt);

    if (!m_data.expanded) {
        // Estado colapsado: título ocupa o centro vertical
        const QRectF textRect = body.adjusted(8, 16, -8 - kPinR * 2 - kShadow, -6);
        p->drawText(textRect,
                    Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,
                    m_data.title.isEmpty() ? tr("Evento") : m_data.title);
    } else {
        // Estado expandido: título no terço superior, descrição abaixo
        const QRectF titleRect(8, 18, kW - 16 - kPinR * 2 - kShadow, 22);
        p->drawText(titleRect,
                    Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap,
                    m_data.title.isEmpty() ? tr("Evento") : m_data.title);

        // ── Separador ───────────────────────────────────────────────────────
        p->setPen(QColor(txt.red(), txt.green(), txt.blue(), 50));
        p->drawLine(QPointF(10, 44), QPointF(kW - 10, 44));

        // ── Descrição (com scroll) ───────────────────────────────────────────
        if (!m_data.description.isEmpty()) {
            QFont fDesc;
            fDesc.setPointSizeF(8.5);
            p->setFont(fDesc);
            p->setPen(QColor(txt.red(), txt.green(), txt.blue(), 200));

            // Clip para a área visível; texto arranca de (y = kDescTop - scrollOffset)
            const QRectF visArea(10, kDescTop, kW - 22, descVisH());
            const QRectF fullArea(10, kDescTop - m_scrollOffset, kW - 22, 100000);
            p->setClipRect(visArea);
            p->drawText(fullArea, Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap,
                        m_data.description);
            p->setClipping(false);

            // Scrollbar discreta na direita
            paintScrollbar(p);
        }

        // ── Grip de resize ──────────────────────────────────────────────────
        const QRectF grip = resizeGripRect();
        const QColor gripColor = m_hoverGrip
            ? QColor(255, 255, 255, 100)
            : QColor(255, 255, 255, 45);
        p->setBrush(gripColor);
        p->setPen(Qt::NoPen);
        p->drawRoundedRect(grip, 3, 3);
    }

    // ── Pin de conexão ───────────────────────────────────────────────────────
    const QPointF pin = pinPos();
    const QColor pinFill = m_hoverPin ? fill.lighter(160) : fill.darker(110);
    p->setBrush(pinFill);
    p->setPen(QPen(border, 1.0));
    p->drawEllipse(pinRect(pin));
}

// ── Eventos de mouse ─────────────────────────────────────────────────────────

void TimelineEventItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if (e->button() == Qt::LeftButton && isOnPin(e->pos())) {
        m_draggingPin = true;
        emit pinDragStarted(m_data.id, mapToScene(pinPos()));
        e->accept();
        return;
    }
    if (e->button() == Qt::LeftButton && isOnResizeGrip(e->pos())) {
        m_resizing         = true;
        m_resizeDragStart  = e->pos();
        m_resizeDragStartH = m_data.expandedH;
        e->accept();
        return; // NÃO chama super → Qt não inicia o drag de movimento
    }
    QGraphicsObject::mousePressEvent(e);
}

void TimelineEventItem::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_resizing) {
        const qreal delta = e->pos().y() - m_resizeDragStart.y();
        setExpandedH(m_resizeDragStartH + delta);
        e->accept();
        return;
    }
    QGraphicsObject::mouseMoveEvent(e);
}

void TimelineEventItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_draggingPin) {
        m_draggingPin = false;
        e->accept();
        return;
    }
    if (m_resizing) {
        m_resizing = false;
        m_data.expandedH = qMax(kHexpMin, m_data.expandedH);
        emit geometryChanged(m_data.id);
        e->accept();
        return;
    }
    QGraphicsObject::mouseReleaseEvent(e);
}

void TimelineEventItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e)
{
    if (e->button() == Qt::LeftButton && !isOnPin(e->pos()) && !isOnResizeGrip(e->pos())) {
        prepareGeometryChange();
        m_data.expanded = !m_data.expanded;
        if (!m_data.expanded) m_scrollOffset = 0.0; // reset ao colapsar
        update();
        emit geometryChanged(m_data.id);
        e->accept();
        return;
    }
    QGraphicsObject::mouseDoubleClickEvent(e);
}

void TimelineEventItem::hoverMoveEvent(QGraphicsSceneHoverEvent* e)
{
    const bool onPin  = isOnPin(e->pos());
    const bool onGrip = isOnResizeGrip(e->pos());

    if (onPin != m_hoverPin) {
        m_hoverPin = onPin;
        update();
    }
    if (onGrip != m_hoverGrip) {
        m_hoverGrip = onGrip;
        update();
    }

    if (onPin)       setCursor(Qt::CrossCursor);
    else if (onGrip) setCursor(Qt::SizeVerCursor);
    else             setCursor(Qt::ArrowCursor);
}

void TimelineEventItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    if (m_hoverPin || m_hoverGrip) {
        m_hoverPin  = false;
        m_hoverGrip = false;
        setCursor(Qt::ArrowCursor);
        update();
    }
}

void TimelineEventItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* e)
{
    QMenu menu;
    auto* actEdit   = menu.addAction(tr("Editar evento"));
    menu.addSeparator();
    auto* actExport = menu.addAction(tr("Exportar como documento"));
    menu.addSeparator();
    auto* actDelete = menu.addAction(tr("Remover"));

    const QAction* chosen = menu.exec(e->screenPos());
    if (chosen == actEdit)   emit editRequested(m_data.id);
    if (chosen == actExport) emit exportAsDocRequested(m_data);
    if (chosen == actDelete) emit deleteRequested(m_data.id);
    e->accept();
}

QVariant TimelineEventItem::itemChange(GraphicsItemChange change,
                                       const QVariant& value)
{
    if (change == ItemPositionHasChanged) {
        m_data.x = pos().x();
        m_data.y = pos().y();
        emit positionChanged(m_data.id);
    }
    return QGraphicsObject::itemChange(change, value);
}
