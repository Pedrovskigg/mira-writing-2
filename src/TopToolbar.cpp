#include "TopToolbar.h"

#include <QAction>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPair>
#include <QToolButton>
#include <QWidgetAction>

#include "FontPickerPopup.h"
#include "IconUtils.h"

namespace {

constexpr auto kIconNormal   = "#8a857a";
constexpr auto kIconHover    = "#d8d3c6";
constexpr auto kIconSelected = "#f0e8d8";

constexpr int kBarHeight = 48;
constexpr int kIconButtonSize = 32;
constexpr int kIconSize = 20;

QToolButton *makeFlatButton(QWidget *parent)
{
    auto *b = new QToolButton(parent);
    b->setToolButtonStyle(Qt::ToolButtonTextOnly);
    b->setAutoRaise(true);
    b->setCursor(Qt::PointingHandCursor);
    return b;
}

QToolButton *makeIconButton(QWidget *parent)
{
    auto *b = new QToolButton(parent);
    b->setToolButtonStyle(Qt::ToolButtonIconOnly);
    b->setAutoRaise(true);
    b->setCursor(Qt::PointingHandCursor);
    b->setFixedSize(kIconButtonSize, kIconButtonSize);
    b->setIconSize(QSize(kIconSize, kIconSize));
    return b;
}

QIcon loadIcon(const QString &name)
{
    return IconUtils::loadToolbarIcon(
        QStringLiteral(":/icons/%1").arg(name),
        QColor(kIconNormal),
        QColor(kIconHover),
        QColor(kIconSelected));
}

QFrame *makeVSeparator(QWidget *parent)
{
    auto *line = new QFrame(parent);
    line->setObjectName(QStringLiteral("ttbVSep"));
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Plain);
    line->setFixedSize(1, 22);
    return line;
}

int fontButtonPointSize(int editorPt)
{
    return qBound(10, editorPt * 2 / 3, 18);
}

}

TopToolbar::TopToolbar(QWidget *parent)
    : QWidget(parent)
    , newProjectButton(makeIconButton(this))
    , openProjectButton(makeIconButton(this))
    , saveProjectButton(makeIconButton(this))
    , boldButton(makeIconButton(this))
    , italicButton(makeIconButton(this))
    , glossaryButton(makeIconButton(this))
    , readModeButton(makeIconButton(this))
    , focusButton(makeIconButton(this))
    , searchButton(makeIconButton(this))
    , fontButton(makeFlatButton(this))
    , sizeButton(makeFlatButton(this))
    , lineHeightButton(makeFlatButton(this))
    , indentButton(makeFlatButton(this))
    , imageButton(makeIconButton(this))
    , reminderButton(makeIconButton(this))
    , immersiveSoundButton(makeIconButton(this))
    , themePanelButton(makeIconButton(this))
    , settingsButton(makeIconButton(this))
    , fullscreenButton(makeIconButton(this))
    , refMenuButton(makeIconButton(this))
    , fontPicker(nullptr)
    , sizeStepperValueLabel(nullptr)
    , currentFontFamily(QStringLiteral("Alegreya"))
    , currentFontSize(16)
    , currentLineHeightPercent(170)
{
    setObjectName(QStringLiteral("topToolbar"));
    setFixedHeight(kBarHeight);

    focusOffIcon = loadIcon(QStringLiteral("focusmode-off.svg"));
    focusOnIcon  = loadIcon(QStringLiteral("focusmode-on.svg"));

    // ---------------- Grupo A: Projeto ----------------
    newProjectButton->setObjectName(QStringLiteral("ttbProject"));
    newProjectButton->setIcon(loadIcon(QStringLiteral("newproject.svg")));
    newProjectButton->setToolTip(tr("Novo projeto"));
    connect(newProjectButton, &QToolButton::clicked, this, &TopToolbar::newProjectRequested);

    openProjectButton->setObjectName(QStringLiteral("ttbProject"));
    openProjectButton->setIcon(loadIcon(QStringLiteral("loadproject.svg")));
    openProjectButton->setToolTip(tr("Abrir projeto"));
    connect(openProjectButton, &QToolButton::clicked, this, &TopToolbar::openProjectRequested);

    saveProjectButton->setObjectName(QStringLiteral("ttbProject"));
    saveProjectButton->setIcon(loadIcon(QStringLiteral("save-project.svg")));
    saveProjectButton->setToolTip(tr("Salvar projeto (Ctrl+S)"));
    connect(saveProjectButton, &QToolButton::clicked, this, &TopToolbar::saveProjectRequested);

    // ---------------- Grupo B: Formatação inline ----------------
    boldButton->setObjectName(QStringLiteral("ttbInline"));
    boldButton->setIcon(loadIcon(QStringLiteral("bold.svg")));
    boldButton->setCheckable(true);
    boldButton->setToolTip(tr("Negrito (Ctrl+B)"));
    connect(boldButton, &QToolButton::toggled, this, &TopToolbar::boldToggled);

    italicButton->setObjectName(QStringLiteral("ttbInline"));
    italicButton->setIcon(loadIcon(QStringLiteral("italic.svg")));
    italicButton->setCheckable(true);
    italicButton->setToolTip(tr("Itálico (Ctrl+I)"));
    connect(italicButton, &QToolButton::toggled, this, &TopToolbar::italicToggled);

    // ---------------- Grupo C: Ferramentas ----------------
    glossaryButton->setObjectName(QStringLiteral("ttbTool"));
    glossaryButton->setIcon(loadIcon(QStringLiteral("glossary.svg")));
    glossaryButton->setToolTip(tr("Glossário"));
    connect(glossaryButton, &QToolButton::clicked, this, &TopToolbar::glossaryRequested);

    readModeButton->setObjectName(QStringLiteral("ttbTool"));
    readModeButton->setIcon(loadIcon(QStringLiteral("readmode.svg")));
    readModeButton->setCheckable(true);
    readModeButton->setToolTip(tr("Modo leitura"));
    connect(readModeButton, &QToolButton::toggled, this, &TopToolbar::readModeToggled);

    focusButton->setObjectName(QStringLiteral("ttbTool"));
    focusButton->setIcon(focusOffIcon);
    focusButton->setCheckable(true);
    focusButton->setToolTip(tr("Modo foco"));
    connect(focusButton, &QToolButton::toggled, this, [this](bool on) {
        focusButton->setIcon(on ? focusOnIcon : focusOffIcon);
        emit focusModeToggled(on);
    });

    searchButton->setObjectName(QStringLiteral("ttbTool"));
    searchButton->setIcon(loadIcon(QStringLiteral("search.svg")));
    searchButton->setToolTip(tr("Buscar"));
    connect(searchButton, &QToolButton::clicked, this, &TopToolbar::searchRequested);

    // ---------------- Grupo D: Tipografia ----------------
    fontButton->setObjectName(QStringLiteral("ttbFont"));
    fontButton->setText(currentFontFamily);
    applyFontButtonStyle();

    fontPicker = new FontPickerPopup(this);
    connect(fontPicker, &FontPickerPopup::fontSelected, this, [this](const QString &family) {
        currentFontFamily = family;
        fontButton->setText(family);
        applyFontButtonStyle();
        emit fontFamilyChanged(family);
    });
    connect(fontButton, &QToolButton::clicked, this, [this]() {
        if (!fontPicker) return;
        fontPicker->setFontFamilies(fontFamilies, currentFontFamily);
        const QPoint anchor = fontButton->mapToGlobal(QPoint(0, fontButton->height()));
        fontPicker->showAtBelow(anchor);
    });

    const QSize iconSize(kIconSize, kIconSize);

    sizeButton->setObjectName(QStringLiteral("ttbSize"));
    sizeButton->setPopupMode(QToolButton::InstantPopup);
    sizeButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    sizeButton->setIcon(loadIcon(QStringLiteral("font-size.svg")));
    sizeButton->setIconSize(iconSize);
    sizeButton->setText(QString::number(currentFontSize));
    sizeButton->setToolTip(tr("Tamanho da fonte"));

    lineHeightButton->setObjectName(QStringLiteral("ttbLineHeight"));
    lineHeightButton->setPopupMode(QToolButton::InstantPopup);
    lineHeightButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    lineHeightButton->setIcon(loadIcon(QStringLiteral("text-spacing.svg")));
    lineHeightButton->setIconSize(iconSize);
    lineHeightButton->setText(QString::number(currentLineHeightPercent / 100.0, 'f', 1));
    lineHeightButton->setToolTip(tr("Espaçamento da linha"));

    indentButton->setObjectName(QStringLiteral("ttbIndent"));
    indentButton->setText(QStringLiteral("¶"));
    indentButton->setCheckable(true);
    indentButton->setChecked(true);
    indentButton->setToolTip(tr("Identação de parágrafo"));
    connect(indentButton, &QToolButton::toggled, this, &TopToolbar::firstLineIndentToggled);

    // ---------------- Grupo E: Mídia ----------------
    imageButton->setObjectName(QStringLiteral("ttbMedia"));
    imageButton->setIcon(loadIcon(QStringLiteral("add-image.svg")));
    imageButton->setToolTip(tr("Adicionar imagem"));
    connect(imageButton, &QToolButton::clicked, this, &TopToolbar::addImageRequested);

    reminderButton->setObjectName(QStringLiteral("ttbMedia"));
    reminderButton->setIcon(loadIcon(QStringLiteral("reminder.svg")));
    reminderButton->setToolTip(tr("Lembretes"));
    connect(reminderButton, &QToolButton::clicked, this, &TopToolbar::reminderRequested);

    immersiveSoundButton->setObjectName(QStringLiteral("ttbMedia"));
    immersiveSoundButton->setIcon(loadIcon(QStringLiteral("immersive-sound.svg")));
    immersiveSoundButton->setToolTip(tr("Som imersivo"));
    connect(immersiveSoundButton, &QToolButton::clicked, this, &TopToolbar::immersiveSoundRequested);

    // ---------------- Grupo F: Sistema ----------------
    themePanelButton->setObjectName(QStringLiteral("ttbSystem"));
    themePanelButton->setIcon(loadIcon(QStringLiteral("theme-panel.svg")));
    themePanelButton->setToolTip(tr("Temas"));
    connect(themePanelButton, &QToolButton::clicked, this, &TopToolbar::themePanelRequested);

    settingsButton->setObjectName(QStringLiteral("ttbSystem"));
    settingsButton->setIcon(loadIcon(QStringLiteral("settings.svg")));
    settingsButton->setToolTip(tr("Configurações"));
    connect(settingsButton, &QToolButton::clicked, this, &TopToolbar::settingsRequested);

    fullscreenButton->setObjectName(QStringLiteral("ttbSystem"));
    fullscreenButton->setIcon(loadIcon(QStringLiteral("fullscreen.svg")));
    fullscreenButton->setCheckable(true);
    fullscreenButton->setToolTip(tr("Tela cheia"));
    connect(fullscreenButton, &QToolButton::toggled, this, &TopToolbar::fullscreenToggled);

    refMenuButton->setObjectName(QStringLiteral("ttbSystem"));
    refMenuButton->setIcon(loadIcon(QStringLiteral("refmenu.svg")));
    refMenuButton->setToolTip(tr("Painel de Referência"));
    connect(refMenuButton, &QToolButton::clicked, this, &TopToolbar::refMenuToggleRequested);

    // ---------------- Layout ----------------
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 4, 14, 4);
    layout->setSpacing(6);

    // Projeto
    layout->addWidget(newProjectButton);
    layout->addWidget(openProjectButton);
    layout->addWidget(saveProjectButton);
    layout->addWidget(makeVSeparator(this));

    // Ferramentas
    layout->addWidget(glossaryButton);
    layout->addWidget(readModeButton);
    layout->addWidget(focusButton);
    layout->addWidget(searchButton);

    layout->addStretch();

    // Tipografia
    layout->addWidget(fontButton);
    layout->addWidget(sizeButton);
    layout->addWidget(lineHeightButton);
    layout->addWidget(indentButton);
    layout->addWidget(boldButton);
    layout->addWidget(italicButton);

    layout->addStretch();

    // Mídia
    layout->addWidget(imageButton);
    layout->addWidget(reminderButton);
    layout->addWidget(immersiveSoundButton);
    layout->addWidget(makeVSeparator(this));

    // Sistema
    layout->addWidget(themePanelButton);
    layout->addWidget(settingsButton);
    layout->addWidget(fullscreenButton);
    layout->addWidget(makeVSeparator(this));
    layout->addWidget(refMenuButton);

    buildSizeMenu();
    buildLineHeightMenu();
}

void TopToolbar::setFontFamilies(const QStringList &families, const QString &current)
{
    fontFamilies = families;
    currentFontFamily = current;
    fontButton->setText(currentFontFamily);
    applyFontButtonStyle();
}

void TopToolbar::setFontSize(int pt)
{
    currentFontSize = pt;
    updateSizeMenuState();
}

void TopToolbar::setLineHeightPercent(int percent)
{
    currentLineHeightPercent = percent;
    lineHeightButton->setText(QString::number(percent / 100.0, 'f', 1));
    buildLineHeightMenu();
}

void TopToolbar::setFirstLineIndentEnabled(bool enabled)
{
    indentButton->setChecked(enabled);
}

void TopToolbar::buildSizeMenu()
{
    auto *menu = new QMenu(sizeButton);
    menu->setObjectName(QStringLiteral("ttbSizeMenu"));

    auto *stepperWidget = new QWidget(menu);
    stepperWidget->setObjectName(QStringLiteral("ttbSizeStepper"));
    auto *stepperLayout = new QHBoxLayout(stepperWidget);
    stepperLayout->setContentsMargins(10, 6, 10, 6);
    stepperLayout->setSpacing(6);

    auto *minus = new QToolButton(stepperWidget);
    minus->setObjectName(QStringLiteral("ttbSizeStep"));
    minus->setText(QStringLiteral("−"));
    minus->setFixedSize(28, 28);
    minus->setCursor(Qt::PointingHandCursor);
    minus->setAutoRaise(true);

    sizeStepperValueLabel = new QLabel(stepperWidget);
    sizeStepperValueLabel->setObjectName(QStringLiteral("ttbSizeValue"));
    sizeStepperValueLabel->setText(QString::number(currentFontSize));
    sizeStepperValueLabel->setAlignment(Qt::AlignCenter);
    sizeStepperValueLabel->setFixedWidth(40);

    auto *plus = new QToolButton(stepperWidget);
    plus->setObjectName(QStringLiteral("ttbSizeStep"));
    plus->setText(QStringLiteral("+"));
    plus->setFixedSize(28, 28);
    plus->setCursor(Qt::PointingHandCursor);
    plus->setAutoRaise(true);

    stepperLayout->addWidget(minus);
    stepperLayout->addWidget(sizeStepperValueLabel);
    stepperLayout->addWidget(plus);

    connect(minus, &QToolButton::clicked, this, [this]() { applySize(currentFontSize - 2); });
    connect(plus, &QToolButton::clicked, this, [this]() { applySize(currentFontSize + 2); });

    auto *stepperAction = new QWidgetAction(menu);
    stepperAction->setDefaultWidget(stepperWidget);
    menu->addAction(stepperAction);

    menu->addSeparator();

    sizePresetActions.clear();
    const QList<int> presets = {12, 14, 16, 18, 20, 24, 28};
    for (int s : presets) {
        QAction *a = menu->addAction(QString::number(s) + tr(" pt"));
        a->setCheckable(true);
        a->setChecked(s == currentFontSize);
        a->setData(s);
        connect(a, &QAction::triggered, this, [this, s]() { applySize(s); });
        sizePresetActions.append(a);
    }

    sizeButton->setMenu(menu);
}

void TopToolbar::updateSizeMenuState()
{
    sizeButton->setText(QString::number(currentFontSize));
    applyFontButtonStyle();
    if (sizeStepperValueLabel) {
        sizeStepperValueLabel->setText(QString::number(currentFontSize));
    }
    for (QAction *a : std::as_const(sizePresetActions)) {
        a->setChecked(a->data().toInt() == currentFontSize);
    }
}

void TopToolbar::applyFontButtonStyle()
{
    QFont f(currentFontFamily, fontButtonPointSize(currentFontSize));
    f.setBold(true);
    fontButton->setFont(f);
}

void TopToolbar::buildLineHeightMenu()
{
    const QList<QPair<int, QString>> spacings = {
        { 130, tr("Compacto (1.3)") },
        { 150, tr("Confortável (1.5)") },
        { 170, tr("Padrão (1.7)") },
        { 190, tr("Amplo (1.9)") },
        { 220, tr("Espaçoso (2.2)") },
    };

    auto *menu = new QMenu(lineHeightButton);
    for (const auto &sp : spacings) {
        const int percent = sp.first;
        QAction *a = menu->addAction(sp.second);
        a->setCheckable(true);
        a->setChecked(percent == currentLineHeightPercent);
        connect(a, &QAction::triggered, this, [this, percent]() {
            currentLineHeightPercent = percent;
            lineHeightButton->setText(QStringLiteral("AA  %1").arg(percent / 100.0, 0, 'f', 1));
            emit lineHeightChanged(percent);
            buildLineHeightMenu();
        });
    }
    lineHeightButton->setMenu(menu);
}

void TopToolbar::applySize(int pt)
{
    const int minSize = 10;
    const int maxSize = 48;
    int next = qBound(minSize, pt, maxSize);
    if (next == currentFontSize) return;
    currentFontSize = next;
    updateSizeMenuState();
    emit fontSizeChanged(next);
}
