#include "CardItem.h"

#include <QBuffer>
#include <QColorDialog>
#include <QFileDialog>
#include <QImage>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QRadialGradient>
#include <QStyleOption>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QTextDocument>
#include <QTextOption>

namespace {

const QColor kNotePalette[] = {
    QColor(QStringLiteral("#ffd060")), QColor(QStringLiteral("#ffb347")),
    QColor(QStringLiteral("#ff8fab")), QColor(QStringLiteral("#ff6b6b")),
    QColor(QStringLiteral("#a8e6cf")), QColor(QStringLiteral("#87ceeb")),
    QColor(QStringLiteral("#c9b1ff")), QColor(QStringLiteral("#f8f8f8")),
};
const QColor kCommentPalette[] = {
    QColor(QStringLiteral("#a78bfa")), QColor(QStringLiteral("#60a5fa")),
    QColor(QStringLiteral("#f472b6")), QColor(QStringLiteral("#34d399")),
    QColor(QStringLiteral("#fb923c")), QColor(QStringLiteral("#94a3b8")),
    QColor(QStringLiteral("#fbbf24")), QColor(QStringLiteral("#f87171")),
};

// Subclasse com boundingRect fixo — garante que toda a área do card captura
// cliques para edição, independente do tamanho real do texto.
class BodyTextItem : public QGraphicsTextItem {
public:
    qreal bodyW = 180;
    qreal bodyH = 100;

    explicit BodyTextItem(QGraphicsItem* p) : QGraphicsTextItem(p) {}

    QRectF boundingRect() const override {
        QRectF r = QGraphicsTextItem::boundingRect();
        r.setWidth(qMax(r.width(),  bodyW));
        r.setHeight(qMax(r.height(), bodyH));
        return r;
    }
    QPainterPath shape() const override {
        QPainterPath p;
        p.addRect(boundingRect());
        return p;
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
        QStyleOptionGraphicsItem opt = *option;
        opt.state &= ~QStyle::State_Selected;
        opt.state &= ~QStyle::State_HasFocus;
        QGraphicsTextItem::paint(painter, &opt, widget);
    }
};

bool calcIsDark(const QColor& c)
{
    return (c.red() * 299 + c.green() * 587 + c.blue() * 114) / 1000 < 128;
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────

CardItem::CardItem(const CanvasCard& data, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_data(data)
{
    setFlag(ItemIsMovable, false);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsFocusable);
    setAcceptHoverEvents(true);
    setPos(m_data.x, m_data.y);

    auto* bti  = new BodyTextItem(this);
    m_textItem = bti;
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_textItem->setAcceptHoverEvents(false); // hover fica no CardItem
    const QString initialText = (m_data.type == QStringLiteral("image"))
        ? m_data.description : m_data.content;
    m_textItem->document()->setPlainText(initialText);

    QTextOption opt;
    opt.setAlignment(Qt::AlignLeft);
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_textItem->document()->setDefaultTextOption(opt);
    m_textItem->document()->setDocumentMargin(0);

    updateTextItem();
    applyTextColor();

    if (m_data.type == QStringLiteral("image")) {
        loadPixmapFromContent();
        m_textItem->setVisible(false); // começa escondido; aparece no duplo-clique
        connect(m_textItem->document(), &QTextDocument::contentsChanged, this, [this]() {
            m_data.description = m_textItem->document()->toPlainText();
            emit dataChanged(m_data);
        });
    } else {
        connect(m_textItem->document(), &QTextDocument::contentsChanged, this, [this]() {
            m_data.content = m_textItem->document()->toPlainText();
            emit dataChanged(m_data);
        });
    }
}

CanvasCard CardItem::cardData() const
{
    CanvasCard d = m_data;
    const QPointF p = pos();
    d.x = p.x();
    d.y = p.y();
    return d;
}

void CardItem::syncFromData()
{
    setPos(m_data.x, m_data.y);
    prepareGeometryChange();
    if (m_data.type == QStringLiteral("image")) loadPixmapFromContent();
    updateTextItem();
    applyTextColor();
    update();
}

// ── Geometria ──────────────────────────────────────────────────────────────

QRectF CardItem::boundingRect() const
{
    const qreal extraH = (m_data.type == QStringLiteral("comment")) ? kTailH : 0.0;
    return QRectF(-kShadow, -kShadow,
                  m_data.width  + kShadow * 2,
                  m_data.height + extraH + kShadow * 2);
}

QPainterPath CardItem::shape() const
{
    QPainterPath p;
    p.addRoundedRect(QRectF(0, 0, m_data.width, m_data.height), kRadius, kRadius);
    return p;
}

void CardItem::updateTextItem()
{
    if (!m_textItem) return;
    const bool isImg = (m_data.type == QStringLiteral("image"));
    // Imagem: área de texto começa abaixo do overlay do header (34px) e cobre o card
    const qreal padL   = 10.0;
    const qreal padTop = isImg ? 34.0 : (kHeaderH + 4.0);
    const qreal padBot = 17.0;
    const qreal tw = qMax(10.0, m_data.width - 2.0 * padL);
    const qreal th = qMax(10.0, m_data.height - padTop - padBot);
    if (auto* bti = static_cast<BodyTextItem*>(m_textItem)) {
        bti->bodyW = tw;
        bti->bodyH = th;
    }
    m_textItem->setTextWidth(tw);
    m_textItem->setPos(padL, padTop);
    if (isImg) m_textItem->setPlainText(m_data.description);
}

// ── Cores ──────────────────────────────────────────────────────────────────

bool CardItem::isDark() const { return calcIsDark(m_data.color); }

QColor CardItem::bodyColor() const
{
    // comment: corpo tem cor levemente escurecida (igual Mira 1: darkenHex 0.28)
    return (m_data.type == QStringLiteral("comment"))
        ? m_data.color.darker(140)
        : m_data.color;
}

QColor CardItem::contrastColor() const
{
    // Contraste calculado sobre bodyColor (onde o texto está)
    const QColor bg = bodyColor();
    return calcIsDark(bg) ? QColor(255, 255, 255, 220) : QColor(0, 0, 0, 180);
}

void CardItem::applyTextColor()
{
    if (!m_textItem) return;
    const QColor tc = (m_data.type == QStringLiteral("image"))
        ? QColor(255, 255, 255, 220)
        : contrastColor();
    m_textItem->setDefaultTextColor(tc);
}

void CardItem::loadPixmapFromContent()
{
    if (m_data.content.isEmpty()) { m_pixmap = QPixmap(); return; }
    const QByteArray ba = QByteArray::fromBase64(m_data.content.toLatin1());
    m_pixmap.loadFromData(ba);
}

void CardItem::openImagePicker()
{
    const QString path = QFileDialog::getOpenFileName(
        nullptr, tr("Escolher imagem"), QString(),
        tr("Imagens (*.png *.jpg *.jpeg *.bmp *.gif *.webp)"));
    if (path.isEmpty()) return;

    QImage img(path);
    if (img.isNull()) return;
    // Comprime: máximo 900px no lado maior, JPEG 82
    if (img.width() > 900 || img.height() > 900)
        img = img.scaled(900, 900, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "JPEG", 82);

    m_data.content = QString::fromLatin1(ba.toBase64());
    loadPixmapFromContent();
    update();
    emit dataChanged(m_data);
}

void CardItem::toggleImageDesc(bool focus)
{
    m_showDesc = !m_showDesc;
    if (m_textItem) {
        m_textItem->setVisible(m_showDesc);
        if (m_showDesc && focus) m_textItem->setFocus();
    }
    update();
}

// ── Pintura ────────────────────────────────────────────────────────────────

void CardItem::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
    const qreal w = m_data.width;
    const qreal h = m_data.height;
    const QColor& bg = m_data.color;

    p->setRenderHint(QPainter::Antialiasing);

    // ── Card de imagem: layout completamente diferente ──────────────────────
    if (m_data.type == QStringLiteral("image")) {
        // Sombra
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(0, 0, 0, 40));
        p->drawRoundedRect(QRectF(3, 5, w, h), 10, 10);

        // Fundo escuro
        p->setPen(QPen(QColor(255, 255, 255, 25), 1));
        p->setBrush(QColor(QStringLiteral("#111111")));
        p->drawRoundedRect(QRectF(0, 0, w, h), 10, 10);

        // Imagem (objectFit: cover)
        if (!m_pixmap.isNull()) {
            p->save();
            QPainterPath clip;
            clip.addRoundedRect(QRectF(0, 0, w, h), 10, 10);
            p->setClipPath(clip);
            p->setOpacity(m_showDesc ? 0.41 : 1.0);
            // Calcula rect de crop centralizado (cover)
            const qreal iw = m_pixmap.width(), ih = m_pixmap.height();
            const qreal scale = qMax(w / iw, h / ih);
            const qreal sw = iw * scale, sh = ih * scale;
            p->drawPixmap(QRectF((w-sw)/2, (h-sh)/2, sw, sh), m_pixmap,
                          QRectF(0, 0, iw, ih));
            p->setOpacity(1.0);
            p->restore();
        } else {
            // Placeholder
            p->setPen(QColor(255, 255, 255, 60));
            p->setFont(QFont(QStringLiteral("Segoe UI"), 10));
            p->drawText(QRectF(0, 0, w, h), Qt::AlignCenter,
                        tr("Clique direito → Escolher imagem"));
        }

        // Header overlay (gradiente escuro no topo, até right-28 para o ×)
        QLinearGradient hg(0, 0, 0, 32);
        hg.setColorAt(0, QColor(0, 0, 0, 107));
        hg.setColorAt(1, QColor(0, 0, 0, 0));
        p->setPen(Qt::NoPen);
        p->setBrush(hg);
        p->drawRect(QRectF(0, 0, w, 32));

        // × no canto superior direito (badge semi-transparente)
        const QColor xBadge = m_hoverDelete ? QColor(0,0,0,180) : QColor(0,0,0,115);
        p->setBrush(xBadge);
        p->setPen(Qt::NoPen);
        p->drawRoundedRect(QRectF(w-24, 4, 20, 20), 4, 4);
        const QColor xc(255, 255, 255, m_hoverDelete ? 220 : 127);
        p->setPen(QPen(xc, 1.2, Qt::SolidLine, Qt::RoundCap));
        const qreal xxc = w-14, xyc = 14;
        p->drawLine(QPointF(xxc-4, xyc-4), QPointF(xxc+4, xyc+4));
        p->drawLine(QPointF(xxc+4, xyc-4), QPointF(xxc-4, xyc+4));

        // Resize handle
        const QColor rhCol(255, 255, 255, int((m_hoverResize ? 0.7 : 0.35) * 255));
        p->setPen(QPen(rhCol, 1.5, Qt::SolidLine, Qt::RoundCap));
        const qreal ox = w-3-10, oy = h-3-10;
        p->drawLine(QPointF(ox+2,oy+9), QPointF(ox+9,oy+2));
        p->drawLine(QPointF(ox+5,oy+9), QPointF(ox+9,oy+5));
        p->drawLine(QPointF(ox+8,oy+9), QPointF(ox+9,oy+8));

        // Pin de conexão
        const qreal px = w/2.0, py = 0.0, pr = 5.0;
        QRadialGradient pinGrad(px - pr*0.3, py - pr*0.3, pr*2.2);
        pinGrad.setColorAt(0.0, QColor(200, 200, 200, 180));
        pinGrad.setColorAt(1.0, QColor(80, 80, 80, 180));
        p->setPen(QPen(QColor(40, 40, 40, 140), 2));
        p->setBrush(pinGrad);
        p->setOpacity(0.55);
        p->drawEllipse(QPointF(px, py), pr, pr);
        p->setOpacity(1.0);
        return; // para aqui — não desenha o resto do paint()
    }

    // Sombra (note/comment)
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(0, 0, 0, 40));
    p->drawRoundedRect(QRectF(3, 5, w, h), kRadius, kRadius);

    // Fundo
    p->setBrush(bg);
    p->drawRoundedRect(QRectF(0, 0, w, h), kRadius, kRadius);

    // Comment: área de corpo com cor escurecida + rabinho abaixo
    if (m_data.type == QStringLiteral("comment")) {
        p->save();
        p->setClipPath(shape()); // clips ao contorno arredondado do card
        p->fillRect(QRectF(0, kHeaderH, w, h - kHeaderH), m_data.color.darker(140));
        p->restore();

        // Rabinho (CSS border trick → triângulo apontando para baixo)
        const qreal tx = 22.0, tw = kTailH * 1.2, th = kTailH;
        QPolygonF tail;
        tail << QPointF(tx, h) << QPointF(tx + tw, h) << QPointF(tx + tw / 2.0, h + th);
        p->setPen(Qt::NoPen);
        p->setBrush(m_data.color);
        p->drawPolygon(tail);
    }

    // Separador do header
    const QColor sep = isDark() ? QColor(255,255,255,30) : QColor(0,0,0,18);
    p->setPen(QPen(sep, 1));
    p->drawLine(QPointF(1, kHeaderH), QPointF(w - 1, kHeaderH));

    // Dobra de canto
    const qreal f = kFoldSize;
    p->setPen(Qt::NoPen);
    p->setBrush(isDark() ? QColor(0,0,0,70) : QColor(0,0,0,36));
    p->drawPolygon(QPolygonF() << QPointF(w-f,h) << QPointF(w,h) << QPointF(w,h-f));
    p->setBrush(isDark() ? QColor(255,255,255,18) : QColor(255,255,255,60));
    p->drawPolygon(QPolygonF() << QPointF(w-f,h) << QPointF(w,h-f) << QPointF(w-f,h-f));
    p->setPen(QPen(isDark() ? QColor(255,255,255,30) : QColor(0,0,0,30), 0.8));
    p->drawLine(QPointF(w-f, h), QPointF(w, h-f));

    // ── Header ──
    const QColor tc    = contrastColor();
    const QColor muted = QColor(tc.red(), tc.green(), tc.blue(), 90);

    // Grip (6 pontos)
    p->setPen(Qt::NoPen);
    p->setBrush(muted);
    const qreal gx = 8.0, gy = (kHeaderH - 9.0) / 2.0;
    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 2; ++col)
            p->drawEllipse(QPointF(gx + col*4.5, gy + row*4.5), 1.2, 1.2);

    // Doc+ button (SVG: rect + cruz, igual Mira 1)
    {
        const qreal dbx = w - 43, dby = kHeaderH / 2.0;
        const QColor dcol = m_hoverDoc ? tc : muted;
        p->setPen(QPen(dcol, 1.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p->setBrush(Qt::NoBrush);
        p->drawRoundedRect(QRectF(dbx - 4.5, dby - 5.5, 9, 11), 1, 1);
        p->drawLine(QPointF(dbx - 1.5, dby - 1), QPointF(dbx - 1.5, dby + 3));
        p->drawLine(QPointF(dbx - 3.5, dby + 1), QPointF(dbx + 0.5, dby + 1));
    }

    // Color dot (círculo com a cor do card, igual Mira 1)
    {
        const qreal cdx = w - 28, cdy = kHeaderH / 2.0;
        p->setPen(QPen(QColor(0, 0, 0, 76), 1.5));
        p->setBrush(m_data.color);
        p->drawEllipse(QPointF(cdx, cdy), 5.5, 5.5);
        if (m_hoverColor) {  // anel de hover
            p->setPen(QPen(tc, 1.5));
            p->setBrush(Qt::NoBrush);
            p->drawEllipse(QPointF(cdx, cdy), 7, 7);
        }
    }

    // Botão ×
    const QColor xCol = m_hoverDelete ? QColor(220, 60, 60) : muted;
    p->setPen(QPen(xCol, 1.4, Qt::SolidLine, Qt::RoundCap));
    const qreal xx = w - 12.0, xy = kHeaderH / 2.0, xr = 4.0;
    p->drawLine(QPointF(xx-xr, xy-xr), QPointF(xx+xr, xy+xr));
    p->drawLine(QPointF(xx+xr, xy-xr), QPointF(xx-xr, xy+xr));

    // ── Pin de conexão no topo central (igual Mira 1 getCardCenter) ──
    {
        const qreal px = w / 2.0, py = 0.0, pr = 5.0;
        QRadialGradient pinGrad(px - pr * 0.3, py - pr * 0.3, pr * 2.2);
        pinGrad.setColorAt(0.0, m_data.color.lighter(130));
        pinGrad.setColorAt(1.0, m_data.color.darker(150));
        p->setPen(QPen(m_data.color.darker(145), 2));
        p->setBrush(pinGrad);
        p->setOpacity(0.55);
        p->drawEllipse(QPointF(px, py), pr, pr);
        // Anel externo de luz (boxShadow simulado)
        p->setPen(QPen(QColor(255, 255, 255, 20), 1.5));
        p->setBrush(Qt::NoBrush);
        p->drawEllipse(QPointF(px, py), pr + 1.5, pr + 1.5);
        p->setOpacity(1.0);
    }

    // Placeholder quando vazio
    if (m_textItem && m_textItem->document()->toPlainText().isEmpty()) {
        constexpr qreal padL = 10.0, padTop = 4.0;
        const QColor ph(tc.red(), tc.green(), tc.blue(), 80);
        p->setPen(ph);
        p->setFont(QFont(QStringLiteral("Segoe UI"), 12));
        const QRectF phRect(padL, kHeaderH + padTop,
                            w - 2*padL, h - kHeaderH - padTop - kFoldSize);
        p->drawText(phRect, Qt::AlignLeft | Qt::AlignTop, tr("Escreva aqui..."));
    }

    // Resize handle (3 linhas diagonais — igual Mira 1)
    const qreal opacity = isDark() ? 0.35 : 0.25;
    const QColor rhCol(tc.red(), tc.green(), tc.blue(),
                       int((m_hoverResize ? 0.7 : opacity) * 255));
    p->setPen(QPen(rhCol, 1.5, Qt::SolidLine, Qt::RoundCap));
    const qreal ox = w-3-10, oy = h-3-10;
    p->drawLine(QPointF(ox+2,oy+9), QPointF(ox+9,oy+2));
    p->drawLine(QPointF(ox+5,oy+9), QPointF(ox+9,oy+5));
    p->drawLine(QPointF(ox+8,oy+9), QPointF(ox+9,oy+8));
}

// ── Hit-test ────────────────────────────────────────────────────────────────

// Layout do header (da direita): [×@w-12] [colorDot@w-27] [doc+@w-42] [...] [grip]
bool CardItem::isOnDeleteBtn(const QPointF& p) const
{
    if (m_data.type == QStringLiteral("image"))
        return QRectF(m_data.width - 24, 4, 20, 20).contains(p);
    return QRectF(m_data.width - 20, 0, 20, kHeaderH).contains(p);
}
bool CardItem::isOnColorDot(const QPointF& p) const
{
    return QRectF(m_data.width - 36, 0, 16, kHeaderH).contains(p);
}
bool CardItem::isOnDocBtn(const QPointF& p) const
{
    return QRectF(m_data.width - 52, 0, 16, kHeaderH).contains(p);
}
bool CardItem::isOnResizeZone(const QPointF& p) const
{
    return QRectF(m_data.width - 17, m_data.height - 17, 17, 17).contains(p);
}

void CardItem::showColorMenu(const QPoint& screenPos)
{
    const bool isComment = (m_data.type == QStringLiteral("comment"));
    const QColor* palette = isComment ? kCommentPalette : kNotePalette;
    QMenu menu;
    for (int i = 0; i < 8; ++i) {
        const QColor& c = palette[i];
        QPixmap px(16, 16); px.fill(c);
        auto* act = menu.addAction(QIcon(px), QString());
        act->setData(c.name());
    }
    menu.addSeparator();
    auto* custom = menu.addAction(tr("Personalizada..."));

    QAction* chosen = menu.exec(screenPos);
    if (!chosen) return;
    QColor nc = (chosen == custom)
        ? QColorDialog::getColor(m_data.color, nullptr, tr("Cor do card"))
        : QColor(chosen->data().toString());
    if (!nc.isValid()) return;
    prepareGeometryChange();
    m_data.color = nc;
    applyTextColor();
    update();
    emit dataChanged(m_data);
}

// ── Mouse events ────────────────────────────────────────────────────────────

void CardItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) { e->ignore(); return; }

    // Imagem: interação própria
    if (m_data.type == QStringLiteral("image")) {
        if (isOnDeleteBtn(e->pos())) { emit deleteRequested(m_data.id); e->accept(); return; }
        if (isOnResizeZone(e->pos())) {
            m_resizing = true; m_pressScene = e->scenePos();
            m_pressSize = QSizeF(m_data.width, m_data.height);
            e->accept(); return;
        }
        if (!m_showDesc) { // drag de qualquer ponto quando descrição não está aberta
            m_dragging = true; m_pressScene = e->scenePos(); m_pressItemOrigin = pos();
            setCursor(Qt::ClosedHandCursor); e->accept(); return;
        }
        e->ignore(); return;
    }

    if (isOnDeleteBtn(e->pos())) {
        emit deleteRequested(m_data.id);
        e->accept();
        return;
    }
    if (isOnColorDot(e->pos())) {
        // Converte posição local → tela para posicionar o menu
        QPoint screenPos = e->screenPos();
        showColorMenu(screenPos);
        e->accept();
        return;
    }
    if (isOnDocBtn(e->pos())) {
        emit createDocRequested(m_data.id);
        e->accept();
        return;
    }
    if (isOnResizeZone(e->pos())) {
        m_resizing   = true;
        m_pressScene = e->scenePos();
        m_pressSize  = QSizeF(m_data.width, m_data.height);
        e->accept();
        return;
    }
    // Drag pelo header
    if (e->pos().y() < kHeaderH) {
        m_dragging        = true;
        m_pressScene      = e->scenePos();
        m_pressItemOrigin = pos();
        setCursor(Qt::ClosedHandCursor);
        e->accept();
        return;
    }
    e->ignore();
}

void CardItem::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_dragging) {
        setPos(m_pressItemOrigin + (e->scenePos() - m_pressScene));
        e->accept();
        return;
    }
    if (m_resizing) {
        const QPointF d = e->scenePos() - m_pressScene;
        prepareGeometryChange();
        m_data.width  = qMax(kMinW, m_pressSize.width()  + d.x());
        m_data.height = qMax(kMinH, m_pressSize.height() + d.y());
        updateTextItem();
        update();
        e->accept();
        return;
    }
    e->ignore();
}

void CardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_dragging || m_resizing) {
        m_dragging = m_resizing = false;
        setCursor(Qt::ArrowCursor);
        const QPointF p = pos();
        m_data.x = p.x(); m_data.y = p.y();
        emit dataChanged(m_data);
        e->accept();
        return;
    }
    e->ignore();
}

// ── Hover ────────────────────────────────────────────────────────────────────

void CardItem::hoverMoveEvent(QGraphicsSceneHoverEvent* e)
{
    const QPointF lp = e->pos();
    const bool onDel    = isOnDeleteBtn(lp);
    const bool onColor  = isOnColorDot(lp);
    const bool onDoc    = isOnDocBtn(lp);
    const bool onResize = isOnResizeZone(lp);
    const bool onHeader = lp.y() < kHeaderH;

    if (onDel != m_hoverDelete || onColor != m_hoverColor ||
        onDoc != m_hoverDoc || onResize != m_hoverResize) {
        m_hoverDelete = onDel;
        m_hoverColor  = onColor;
        m_hoverDoc    = onDoc;
        m_hoverResize = onResize;
        update();
    }
    if (onDel || onColor || onDoc) setCursor(Qt::PointingHandCursor);
    else if (onResize)             setCursor(Qt::SizeFDiagCursor);
    else if (onHeader)             setCursor(Qt::OpenHandCursor);
    else                           setCursor(Qt::ArrowCursor);
}

void CardItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    const bool needUpdate = m_hoverDelete || m_hoverColor || m_hoverDoc || m_hoverResize;
    m_hoverDelete = m_hoverColor = m_hoverDoc = m_hoverResize = false;
    if (needUpdate) update();
    setCursor(Qt::ArrowCursor);
}

// ── Context menu ─────────────────────────────────────────────────────────────

void CardItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e)
{
    if (m_data.type == QStringLiteral("image")) {
        toggleImageDesc(true);
        e->accept();
        return;
    }
    QGraphicsObject::mouseDoubleClickEvent(e);
}

void CardItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* e)
{
    if (m_data.type == QStringLiteral("image")) {
        QMenu menu;
        menu.addAction(tr("Escolher imagem..."), this, &CardItem::openImagePicker);
        menu.addSeparator();
        menu.addAction(tr("Remover card"), this, [this]() {
            emit deleteRequested(m_data.id);
        });
        menu.exec(e->screenPos());
        return;
    }
    showColorMenu(e->screenPos());
}
