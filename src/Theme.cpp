#include "Theme.h"

#include <QSettings>

namespace Theme {

Manager* Manager::instance()
{
    static Manager* mgr = new Manager();
    return mgr;
}

Manager::Manager()
{
    loadBundled();
    loadFromSettings();
}

void Manager::loadBundled()
{
    {
        MiraTheme t;
        // Mantemos o id "full-black" pra não invalidar QSettings antigos,
        // mas o nome de exibição é "Grafite" — clássico do Mira 1.
        t.id = QStringLiteral("full-black");
        t.name = QStringLiteral("Grafite");
        t.bundled = true;
        t.appBackground    = QStringLiteral("#0a0a0a");  // R10 G10 B10
        t.panelBackground  = QStringLiteral("#0d0d0d");  // R13 G13 B13
        t.panelBorder      = QStringLiteral("#2a2a2a");
        t.textPrimary      = QStringLiteral("#e0e0e0");  // R224 G224 B224
        t.textMuted        = QStringLiteral("#888888");
        t.textBright       = QStringLiteral("#f5f5f5");
        t.hoverOverlay     = QStringLiteral("rgba(255,255,255,0.06)");
        t.pressedOverlay   = QStringLiteral("rgba(255,255,255,0.04)");
        t.subtleBorder     = QStringLiteral("rgba(255,255,255,0.10)");
        t.accentDefault    = QStringLiteral("#3a8c7a");

        t.hoverStrong         = QStringLiteral("rgba(255,255,255,0.12)");
        t.borderStrong        = QStringLiteral("rgba(255,255,255,0.20)");
        t.focusBorder         = QStringLiteral("rgba(255,255,255,0.30)");
        t.inputBackground     = QStringLiteral("rgba(0,0,0,0.30)");
        t.disabledText        = QStringLiteral("rgba(255,255,255,0.30)");
        t.selectionRing       = QStringLiteral("#ffffff");

        t.accentSuccess           = QStringLiteral("#7BC592");
        t.accentSuccessSoft       = QStringLiteral("rgba(120,200,140,0.18)");
        t.accentSuccessBorderSoft = QStringLiteral("rgba(123,197,146,0.50)");
        t.accentDanger            = QStringLiteral("#e05555");
        t.accentDangerSoft        = QStringLiteral("rgba(224,85,85,0.12)");
        t.accentDangerBorderSoft  = QStringLiteral("rgba(224,85,85,0.40)");
        t.accentWarning           = QStringLiteral("#d66060");
        t.accentInfo              = QStringLiteral("#4a9eff");
        t.accentInfoSoft          = QStringLiteral("rgba(74,158,255,0.30)");
        t.accentInfoBorderSoft    = QStringLiteral("rgba(74,158,255,0.60)");

        t.editorBackground = QStringLiteral("#101010");  // R16 G16 B16
        t.editorTextColor  = QStringLiteral("#e0e0e0");  // R224 G224 B224
        // Sombra sutil em fundo escuro: só pra dar volume.
        t.pageShadowEnabled = true;
        t.pageShadowColor = QStringLiteral("rgba(0,0,0,160)");
        t.pageShadowRadius = 22;
        t.pageShadowOffset = 4;
        m_themes.append(t);
    }
    {
        MiraTheme t;
        t.id = QStringLiteral("full-white");
        t.name = QStringLiteral("Full White");
        t.bundled = true;
        // Branco de verdade — sem tons de creme/amarelo. Contraste da página
        // contra os painéis vem da sombra projetada e de uma diferença sutil
        // de cinza, nada de matiz quente.
        t.appBackground    = QStringLiteral("#dcdcdc");
        t.panelBackground  = QStringLiteral("#f4f4f4");
        t.panelBorder      = QStringLiteral("#b8b8b8");
        t.textPrimary      = QStringLiteral("#1a1a1a");
        t.textMuted        = QStringLiteral("#6e6e6e");
        t.textBright       = QStringLiteral("#0a0a0a");
        t.hoverOverlay     = QStringLiteral("rgba(0,0,0,0.06)");
        t.pressedOverlay   = QStringLiteral("rgba(0,0,0,0.04)");
        t.subtleBorder     = QStringLiteral("rgba(0,0,0,0.12)");
        t.accentDefault    = QStringLiteral("#2f7565");

        t.hoverStrong         = QStringLiteral("rgba(0,0,0,0.10)");
        t.borderStrong        = QStringLiteral("rgba(0,0,0,0.22)");
        t.focusBorder         = QStringLiteral("rgba(0,0,0,0.32)");
        t.inputBackground     = QStringLiteral("rgba(0,0,0,0.04)");
        t.disabledText        = QStringLiteral("rgba(0,0,0,0.32)");
        t.selectionRing       = QStringLiteral("#1a1a1a");

        // Verdes/vermelhos mais escuros pra terem contraste sobre o branco.
        t.accentSuccess           = QStringLiteral("#2f8a4f");
        t.accentSuccessSoft       = QStringLiteral("rgba(47,138,79,0.14)");
        t.accentSuccessBorderSoft = QStringLiteral("rgba(47,138,79,0.45)");
        t.accentDanger            = QStringLiteral("#b73030");
        t.accentDangerSoft        = QStringLiteral("rgba(183,48,48,0.10)");
        t.accentDangerBorderSoft  = QStringLiteral("rgba(183,48,48,0.45)");
        t.accentWarning           = QStringLiteral("#b03a3a");
        t.accentInfo              = QStringLiteral("#1f6fd6");
        t.accentInfoSoft          = QStringLiteral("rgba(31,111,214,0.18)");
        t.accentInfoBorderSoft    = QStringLiteral("rgba(31,111,214,0.55)");

        t.editorBackground = QStringLiteral("#ffffff");
        t.editorTextColor  = QStringLiteral("#141414");
        t.pageShadowEnabled = true;
        t.pageShadowColor = QStringLiteral("rgba(0,0,0,90)");
        t.pageShadowRadius = 28;
        t.pageShadowOffset = 6;
        m_themes.append(t);
    }
    {
        // Solarized Dark — paleta clássica do Ethan Schoonover. Fundo
        // azul-petróleo escuro, texto bege esverdeado. Accent em ciano.
        MiraTheme t;
        t.id = QStringLiteral("solarized-dark");
        t.name = QStringLiteral("Solarized Dark");
        t.bundled = true;
        t.appBackground    = QStringLiteral("#002b36");  // base03
        t.panelBackground  = QStringLiteral("#073642");  // base02
        t.panelBorder      = QStringLiteral("#0f4754");
        t.textPrimary      = QStringLiteral("#93a1a1");  // base1
        t.textMuted        = QStringLiteral("#586e75");  // base01
        t.textBright       = QStringLiteral("#eee8d5");  // base2
        t.hoverOverlay     = QStringLiteral("rgba(238,232,213,0.06)");
        t.pressedOverlay   = QStringLiteral("rgba(238,232,213,0.04)");
        t.subtleBorder     = QStringLiteral("rgba(238,232,213,0.10)");
        t.accentDefault    = QStringLiteral("#2aa198");  // cyan

        t.hoverStrong         = QStringLiteral("rgba(238,232,213,0.12)");
        t.borderStrong        = QStringLiteral("rgba(238,232,213,0.20)");
        t.focusBorder         = QStringLiteral("rgba(238,232,213,0.30)");
        t.inputBackground     = QStringLiteral("rgba(0,0,0,0.25)");
        t.disabledText        = QStringLiteral("rgba(238,232,213,0.30)");
        t.selectionRing       = QStringLiteral("#fdf6e3");

        t.accentSuccess           = QStringLiteral("#859900");  // green
        t.accentSuccessSoft       = QStringLiteral("rgba(133,153,0,0.18)");
        t.accentSuccessBorderSoft = QStringLiteral("rgba(133,153,0,0.50)");
        t.accentDanger            = QStringLiteral("#dc322f");  // red
        t.accentDangerSoft        = QStringLiteral("rgba(220,50,47,0.14)");
        t.accentDangerBorderSoft  = QStringLiteral("rgba(220,50,47,0.45)");
        t.accentWarning           = QStringLiteral("#cb4b16");  // orange
        t.accentInfo              = QStringLiteral("#268bd2");  // blue
        t.accentInfoSoft          = QStringLiteral("rgba(38,139,210,0.25)");
        t.accentInfoBorderSoft    = QStringLiteral("rgba(38,139,210,0.55)");

        t.editorBackground = QStringLiteral("#073642");
        t.editorTextColor  = QStringLiteral("#93a1a1");
        t.pageShadowEnabled = true;
        t.pageShadowColor = QStringLiteral("rgba(0,0,0,160)");
        t.pageShadowRadius = 22;
        t.pageShadowOffset = 4;
        m_themes.append(t);
    }
    {
        // Solarized Light — mesma paleta, polos invertidos. Fundo creme
        // claro, texto cinza-azulado. Accents idênticos ao dark.
        MiraTheme t;
        t.id = QStringLiteral("solarized-light");
        t.name = QStringLiteral("Solarized Light");
        t.bundled = true;
        t.appBackground    = QStringLiteral("#eee8d5");  // base2
        t.panelBackground  = QStringLiteral("#fdf6e3");  // base3
        t.panelBorder      = QStringLiteral("#d8d2c0");
        t.textPrimary      = QStringLiteral("#586e75");  // base01
        t.textMuted        = QStringLiteral("#93a1a1");  // base1
        t.textBright       = QStringLiteral("#002b36");  // base03
        t.hoverOverlay     = QStringLiteral("rgba(0,43,54,0.06)");
        t.pressedOverlay   = QStringLiteral("rgba(0,43,54,0.04)");
        t.subtleBorder     = QStringLiteral("rgba(0,43,54,0.12)");
        t.accentDefault    = QStringLiteral("#2aa198");

        t.hoverStrong         = QStringLiteral("rgba(0,43,54,0.10)");
        t.borderStrong        = QStringLiteral("rgba(0,43,54,0.22)");
        t.focusBorder         = QStringLiteral("rgba(0,43,54,0.32)");
        t.inputBackground     = QStringLiteral("rgba(0,43,54,0.04)");
        t.disabledText        = QStringLiteral("rgba(0,43,54,0.32)");
        t.selectionRing       = QStringLiteral("#002b36");

        t.accentSuccess           = QStringLiteral("#5e7300");
        t.accentSuccessSoft       = QStringLiteral("rgba(94,115,0,0.14)");
        t.accentSuccessBorderSoft = QStringLiteral("rgba(94,115,0,0.50)");
        t.accentDanger            = QStringLiteral("#b32421");
        t.accentDangerSoft        = QStringLiteral("rgba(179,36,33,0.10)");
        t.accentDangerBorderSoft  = QStringLiteral("rgba(179,36,33,0.45)");
        t.accentWarning           = QStringLiteral("#a23a10");
        t.accentInfo              = QStringLiteral("#1e6ca8");
        t.accentInfoSoft          = QStringLiteral("rgba(30,108,168,0.16)");
        t.accentInfoBorderSoft    = QStringLiteral("rgba(30,108,168,0.55)");

        t.editorBackground = QStringLiteral("#fdf6e3");
        t.editorTextColor  = QStringLiteral("#073642");
        t.pageShadowEnabled = true;
        t.pageShadowColor = QStringLiteral("rgba(0,43,54,80)");
        t.pageShadowRadius = 26;
        t.pageShadowOffset = 5;
        m_themes.append(t);
    }
    {
        // Sépia — inspirado no FocusWriter "Old Paper". Papel envelhecido,
        // texto marrom escuro. Sensação de máquina de escrever.
        MiraTheme t;
        t.id = QStringLiteral("sepia");
        t.name = QStringLiteral("Sépia");
        t.bundled = true;
        t.appBackground    = QStringLiteral("#e8dcc0");
        t.panelBackground  = QStringLiteral("#f4ecd8");
        t.panelBorder      = QStringLiteral("#c8b896");
        t.textPrimary      = QStringLiteral("#5b4636");
        t.textMuted        = QStringLiteral("#8a7355");
        t.textBright       = QStringLiteral("#2e2118");
        t.hoverOverlay     = QStringLiteral("rgba(91,70,54,0.06)");
        t.pressedOverlay   = QStringLiteral("rgba(91,70,54,0.04)");
        t.subtleBorder     = QStringLiteral("rgba(91,70,54,0.14)");
        t.accentDefault    = QStringLiteral("#9a5a2b");  // terracota

        t.hoverStrong         = QStringLiteral("rgba(91,70,54,0.10)");
        t.borderStrong        = QStringLiteral("rgba(91,70,54,0.24)");
        t.focusBorder         = QStringLiteral("rgba(91,70,54,0.34)");
        t.inputBackground     = QStringLiteral("rgba(91,70,54,0.05)");
        t.disabledText        = QStringLiteral("rgba(91,70,54,0.32)");
        t.selectionRing       = QStringLiteral("#2e2118");

        t.accentSuccess           = QStringLiteral("#5e7a3a");  // verde-oliva
        t.accentSuccessSoft       = QStringLiteral("rgba(94,122,58,0.14)");
        t.accentSuccessBorderSoft = QStringLiteral("rgba(94,122,58,0.45)");
        t.accentDanger            = QStringLiteral("#a83333");
        t.accentDangerSoft        = QStringLiteral("rgba(168,51,51,0.10)");
        t.accentDangerBorderSoft  = QStringLiteral("rgba(168,51,51,0.45)");
        t.accentWarning           = QStringLiteral("#a83333");
        t.accentInfo              = QStringLiteral("#3a6b8a");  // azul-acinzentado
        t.accentInfoSoft          = QStringLiteral("rgba(58,107,138,0.16)");
        t.accentInfoBorderSoft    = QStringLiteral("rgba(58,107,138,0.55)");

        t.editorBackground = QStringLiteral("#f4ecd8");
        t.editorTextColor  = QStringLiteral("#3a2e22");
        t.pageShadowEnabled = true;
        t.pageShadowColor = QStringLiteral("rgba(91,70,54,90)");
        t.pageShadowRadius = 26;
        t.pageShadowOffset = 5;
        m_themes.append(t);
    }
    {
        // Nord — paleta ártica do Arctic Ice Studio. Cinzas azulados frios
        // com accents em ciano-glaciar e auroras.
        MiraTheme t;
        t.id = QStringLiteral("nord");
        t.name = QStringLiteral("Nord");
        t.bundled = true;
        t.appBackground    = QStringLiteral("#2e3440");  // nord0
        t.panelBackground  = QStringLiteral("#3b4252");  // nord1
        t.panelBorder      = QStringLiteral("#4c566a");  // nord3
        t.textPrimary      = QStringLiteral("#d8dee9");  // nord4
        t.textMuted        = QStringLiteral("#7b8394");
        t.textBright       = QStringLiteral("#eceff4");  // nord6
        t.hoverOverlay     = QStringLiteral("rgba(216,222,233,0.06)");
        t.pressedOverlay   = QStringLiteral("rgba(216,222,233,0.04)");
        t.subtleBorder     = QStringLiteral("rgba(216,222,233,0.10)");
        t.accentDefault    = QStringLiteral("#88c0d0");  // nord8 frost ciano

        t.hoverStrong         = QStringLiteral("rgba(216,222,233,0.12)");
        t.borderStrong        = QStringLiteral("rgba(216,222,233,0.22)");
        t.focusBorder         = QStringLiteral("rgba(216,222,233,0.32)");
        t.inputBackground     = QStringLiteral("rgba(0,0,0,0.22)");
        t.disabledText        = QStringLiteral("rgba(216,222,233,0.30)");
        t.selectionRing       = QStringLiteral("#eceff4");

        t.accentSuccess           = QStringLiteral("#a3be8c");  // nord14 aurora green
        t.accentSuccessSoft       = QStringLiteral("rgba(163,190,140,0.20)");
        t.accentSuccessBorderSoft = QStringLiteral("rgba(163,190,140,0.55)");
        t.accentDanger            = QStringLiteral("#bf616a");  // nord11 aurora red
        t.accentDangerSoft        = QStringLiteral("rgba(191,97,106,0.15)");
        t.accentDangerBorderSoft  = QStringLiteral("rgba(191,97,106,0.50)");
        t.accentWarning           = QStringLiteral("#d08770");  // nord12 aurora orange
        t.accentInfo              = QStringLiteral("#81a1c1");  // nord9 frost azul
        t.accentInfoSoft          = QStringLiteral("rgba(129,161,193,0.22)");
        t.accentInfoBorderSoft    = QStringLiteral("rgba(129,161,193,0.55)");

        t.editorBackground = QStringLiteral("#3b4252");
        t.editorTextColor  = QStringLiteral("#d8dee9");
        t.pageShadowEnabled = true;
        t.pageShadowColor = QStringLiteral("rgba(0,0,0,160)");
        t.pageShadowRadius = 24;
        t.pageShadowOffset = 5;
        m_themes.append(t);
    }
    {
        // High Contrast — acessibilidade. Preto puro, texto branco puro,
        // accents saturados ao extremo. Sem sutilezas.
        MiraTheme t;
        t.id = QStringLiteral("high-contrast");
        t.name = QStringLiteral("Alto Contraste");
        t.bundled = true;
        t.appBackground    = QStringLiteral("#000000");
        t.panelBackground  = QStringLiteral("#0a0a0a");
        t.panelBorder      = QStringLiteral("#ffffff");
        t.textPrimary      = QStringLiteral("#ffffff");
        t.textMuted        = QStringLiteral("#bdbdbd");
        t.textBright       = QStringLiteral("#ffffff");
        t.hoverOverlay     = QStringLiteral("rgba(255,255,255,0.15)");
        t.pressedOverlay   = QStringLiteral("rgba(255,255,255,0.08)");
        t.subtleBorder     = QStringLiteral("rgba(255,255,255,0.40)");
        t.accentDefault    = QStringLiteral("#ffff00");  // amarelo puro

        t.hoverStrong         = QStringLiteral("rgba(255,255,255,0.25)");
        t.borderStrong        = QStringLiteral("rgba(255,255,255,0.70)");
        t.focusBorder         = QStringLiteral("#ffff00");
        t.inputBackground     = QStringLiteral("rgba(255,255,255,0.06)");
        t.disabledText        = QStringLiteral("rgba(255,255,255,0.45)");
        t.selectionRing       = QStringLiteral("#ffff00");

        t.accentSuccess           = QStringLiteral("#00ff7f");  // verde saturado
        t.accentSuccessSoft       = QStringLiteral("rgba(0,255,127,0.20)");
        t.accentSuccessBorderSoft = QStringLiteral("rgba(0,255,127,0.70)");
        t.accentDanger            = QStringLiteral("#ff4040");  // vermelho saturado
        t.accentDangerSoft        = QStringLiteral("rgba(255,64,64,0.20)");
        t.accentDangerBorderSoft  = QStringLiteral("rgba(255,64,64,0.70)");
        t.accentWarning           = QStringLiteral("#ffa500");  // laranja
        t.accentInfo              = QStringLiteral("#00d0ff");  // ciano vibrante
        t.accentInfoSoft          = QStringLiteral("rgba(0,208,255,0.25)");
        t.accentInfoBorderSoft    = QStringLiteral("rgba(0,208,255,0.70)");

        t.editorBackground = QStringLiteral("#000000");
        t.editorTextColor  = QStringLiteral("#ffffff");
        t.pageShadowEnabled = false;  // não combina com o estilo limpo
        t.pageShadowColor = QStringLiteral("rgba(0,0,0,0)");
        t.pageShadowRadius = 0;
        t.pageShadowOffset = 0;
        m_themes.append(t);
    }
}

void Manager::loadFromSettings()
{
    QSettings s;
    const QString id = s.value(QStringLiteral("theme/currentId"),
                               QStringLiteral("full-black")).toString();
    for (int i = 0; i < m_themes.size(); ++i) {
        if (m_themes.at(i).id == id) {
            m_currentIndex = i;
            return;
        }
    }
    m_currentIndex = 0;
}

void Manager::saveToSettings() const
{
    QSettings s;
    s.setValue(QStringLiteral("theme/currentId"),
               m_themes.at(m_currentIndex).id);
}

const MiraTheme& Manager::current() const
{
    return m_themes.at(m_currentIndex);
}

const QList<MiraTheme>& Manager::available() const
{
    return m_themes;
}

void Manager::setCurrent(const QString& id)
{
    for (int i = 0; i < m_themes.size(); ++i) {
        if (m_themes.at(i).id == id) {
            if (i == m_currentIndex) return;
            m_currentIndex = i;
            saveToSettings();
            emit themeChanged();
            return;
        }
    }
}

// ---- API legada ----
QString appBackground()     { return Manager::instance()->current().appBackground; }
QString panelBackground()   { return Manager::instance()->current().panelBackground; }
QString panelBorder()       { return Manager::instance()->current().panelBorder; }
QString panelBorderRadius() { return QStringLiteral("10px"); }
QString textPrimary()       { return Manager::instance()->current().textPrimary; }
QString textMuted()         { return Manager::instance()->current().textMuted; }
QString textBright()        { return Manager::instance()->current().textBright; }
QString hoverOverlay()      { return Manager::instance()->current().hoverOverlay; }
QString pressedOverlay()    { return Manager::instance()->current().pressedOverlay; }
QString subtleBorder()      { return Manager::instance()->current().subtleBorder; }
QString accentDefault()     { return Manager::instance()->current().accentDefault; }

QString hoverStrong()              { return Manager::instance()->current().hoverStrong; }
QString borderStrong()             { return Manager::instance()->current().borderStrong; }
QString focusBorder()              { return Manager::instance()->current().focusBorder; }
QString inputBackground()          { return Manager::instance()->current().inputBackground; }
QString disabledText()             { return Manager::instance()->current().disabledText; }
QString selectionRing()            { return Manager::instance()->current().selectionRing; }

QString accentSuccess()            { return Manager::instance()->current().accentSuccess; }
QString accentSuccessSoft()        { return Manager::instance()->current().accentSuccessSoft; }
QString accentSuccessBorderSoft()  { return Manager::instance()->current().accentSuccessBorderSoft; }
QString accentDanger()             { return Manager::instance()->current().accentDanger; }
QString accentDangerSoft()         { return Manager::instance()->current().accentDangerSoft; }
QString accentDangerBorderSoft()   { return Manager::instance()->current().accentDangerBorderSoft; }
QString accentWarning()            { return Manager::instance()->current().accentWarning; }
QString accentInfo()               { return Manager::instance()->current().accentInfo; }
QString accentInfoSoft()           { return Manager::instance()->current().accentInfoSoft; }
QString accentInfoBorderSoft()     { return Manager::instance()->current().accentInfoBorderSoft; }

QString panelQss(const QString& objectName)
{
    return QStringLiteral(R"(
        #%1 {
            background: %2;
            border: 1px solid %3;
            border-radius: %4;
        }
    )").arg(objectName, panelBackground(), panelBorder(), panelBorderRadius());
}

QString editorBackground()   { return Manager::instance()->current().editorBackground; }
QString editorTextColor()    { return Manager::instance()->current().editorTextColor; }
bool pageShadowEnabled()     { return Manager::instance()->current().pageShadowEnabled; }
QString pageShadowColor()    { return Manager::instance()->current().pageShadowColor; }
int pageShadowRadius()       { return Manager::instance()->current().pageShadowRadius; }
int pageShadowOffset()       { return Manager::instance()->current().pageShadowOffset; }

QString globalStyleSheet()
{
    // Vivia em main.cpp como um QString gigante hard-coded em dark. Movido pra
    // cá pra reagir ao tema corrente. Cobre cor de fundo da janela, TopToolbar,
    // menus globais, scrollbar, popups de fonte, dialog/overlay de imagem.
    const QString appBg      = appBackground();
    const QString panelBg    = panelBackground();
    const QString panelBd    = panelBorder();
    const QString txtPrim    = textPrimary();
    const QString txtMuted   = textMuted();
    const QString txtBright  = textBright();
    const QString hover      = hoverOverlay();
    const QString hoverStr   = hoverStrong();
    const QString border     = subtleBorder();
    const QString inputBg    = inputBackground();
    const QString selBg      = pressedOverlay();
    const QString accentBg   = accentDefault();

    return QStringLiteral(R"(
        QMainWindow {
            background-color: %1;
        }
        #editorContainer {
            background-color: %1;
        }
        QTextEdit {
            background-color: %2;
            border: none;
            padding: 80px 100px;
            selection-background-color: %13;
        }
        #topToolbarHolder {
            background: transparent;
        }
        #topToolbar {
            background-color: %2;
            border: 1px solid %3;
            border-radius: 14px;
        }
        #topToolbar QToolButton {
            background: transparent;
            color: %5;
            border: none;
            padding: 4px 6px;
            font-size: 12px;
            border-radius: 6px;
        }
        #topToolbar QToolButton:hover {
            color: %6;
            background-color: %7;
        }
        #topToolbar QToolButton:checked {
            color: %6;
            background-color: %8;
        }
        #topToolbar QToolButton::menu-indicator {
            image: none;
            width: 0;
        }
        #topToolbar QFrame#ttbVSep {
            background-color: %3;
            border: none;
            margin: 0 4px;
        }
        #topToolbar QToolButton#ttbFont {
            min-width: 130px;
            color: %4;
            padding: 4px 8px;
        }
        #topToolbar QToolButton#ttbFont:hover {
            color: %6;
        }
        #topToolbar QToolButton#ttbSize,
        #topToolbar QToolButton#ttbLineHeight {
            min-width: 60px;
            padding: 4px 6px;
        }
        #topToolbar QToolButton#ttbIndent {
            font-size: 14px;
            min-width: 24px;
            padding: 4px 0;
        }
        #ttbSizeStepper QToolButton#ttbSizeStep {
            background: %8;
            color: %4;
            border: none;
            border-radius: 4px;
            font-size: 16px;
            font-weight: bold;
        }
        #ttbSizeStepper QToolButton#ttbSizeStep:hover {
            background: %7;
            color: %6;
        }
        #ttbSizeStepper QLabel#ttbSizeValue {
            color: %6;
            font-size: 15px;
            font-weight: bold;
        }
        QMenu {
            background-color: %2;
            color: %4;
            border: 1px solid %3;
            padding: 4px;
        }
        QMenu::item {
            padding: 6px 24px;
        }
        QMenu::item:selected {
            background-color: %8;
            color: %6;
            border-radius: 3px;
        }
        QMenu::separator {
            height: 1px;
            background: %9;
            margin: 4px 8px;
        }
        #fontPickerPopup {
            background-color: %2;
            border: 1px solid %3;
        }
        #fontPickerList {
            background-color: %2;
            color: %4;
            border: none;
            outline: none;
            padding: 4px;
        }
        #fontPickerList::item {
            padding: 8px 12px;
            border-radius: 3px;
        }
        #fontPickerList::item:hover {
            background-color: %7;
            color: %6;
        }
        #fontPickerList::item:selected {
            background-color: %8;
            color: %6;
        }
        #imageInsertDialog {
            background-color: %2;
        }
        #imageInsertDialog QLabel {
            color: %4;
            font-size: 12px;
        }
        #imageInsertDialog #imagePreview {
            background-color: %10;
            border: 1px solid %3;
            border-radius: 4px;
            color: %5;
        }
        #imageInsertDialog QRadioButton {
            color: %4;
            spacing: 6px;
            font-size: 12px;
        }
        #imageInsertDialog QRadioButton::indicator {
            width: 14px;
            height: 14px;
            border-radius: 7px;
            border: 1px solid %3;
            background: %10;
        }
        #imageInsertDialog QRadioButton::indicator:checked {
            background: %4;
            border-color: %4;
        }
        #imageInsertDialog QSlider::groove:horizontal {
            background: %8;
            height: 4px;
            border-radius: 2px;
        }
        #imageInsertDialog QSlider::handle:horizontal {
            background: %4;
            width: 14px;
            height: 14px;
            margin: -6px 0;
            border-radius: 7px;
        }
        #imageInsertDialog QSpinBox {
            background: %10;
            color: %6;
            border: 1px solid %3;
            border-radius: 4px;
            padding: 4px 6px;
        }
        #imageInsertDialog QPushButton {
            background: %8;
            color: %4;
            border: none;
            padding: 8px 18px;
            border-radius: 4px;
            font-size: 12px;
        }
        #imageInsertDialog QPushButton:hover {
            background: %7;
            color: %6;
        }
        #imageInsertDialog QPushButton:default {
            background: %12;
            color: %6;
        }
        #imageInsertDialog QPushButton:default:hover {
            background: %12;
        }
        #imageOverlay {
            background-color: %2;
            border: 1px solid %3;
            border-radius: 6px;
        }
        #imageOverlay QToolButton#imgOverlayBtn {
            background: transparent;
            color: %4;
            border: none;
            padding: 4px 8px;
            border-radius: 3px;
            font-size: 14px;
            min-width: 22px;
        }
        #imageOverlay QToolButton#imgOverlayBtn:hover {
            background-color: %7;
            color: %6;
        }
        #imageOverlay QToolButton#imgOverlayBtn[active="true"] {
            background-color: %12;
            color: %6;
        }
        #imageOverlay QLabel#imgOverlayWidth {
            color: %6;
            font-size: 12px;
            font-weight: bold;
        }
        #imageOverlay QFrame#imgOverlaySep {
            color: %3;
        }
        QScrollBar:vertical {
            background: %1;
            width: 10px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: %8;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: %7;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }
        QScrollBar:horizontal {
            background: %1;
            height: 10px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background: %8;
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: %7;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: transparent;
        }
    )")
        .arg(appBg,       // %1
             panelBg,     // %2
             panelBd,     // %3
             txtPrim,     // %4
             txtMuted,    // %5
             txtBright,   // %6
             hover,       // %7
             hoverStr,    // %8
             border,      // %9
             inputBg)     // %10
        .arg(selBg,       // %11 (não usado; reservado)
             accentBg,    // %12
             accentBg);   // %13
}

} // namespace Theme
