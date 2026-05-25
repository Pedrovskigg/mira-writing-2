#pragma once

#include <QList>
#include <QObject>
#include <QString>

namespace Theme {

// Bundle de propriedades visuais de um tema. Cores em #rrggbb ou rgba(...).
// Layout da página (largura/margens) NÃO é responsabilidade do tema — fica
// em EditorLayout.
struct MiraTheme {
    QString id;
    QString name;
    bool bundled = true;

    // Cores principais (painéis, app)
    QString appBackground;
    QString panelBackground;
    QString panelBorder;
    QString textPrimary;
    QString textMuted;
    QString textBright;
    QString hoverOverlay;
    QString pressedOverlay;
    QString subtleBorder;
    QString accentDefault;

    // Hover/focus mais fortes — usados em botões e inputs.
    // Em tema escuro, `rgba(255,255,255,0.12+)`; em tema claro, `rgba(0,0,0,0.08+)`.
    QString hoverStrong;
    QString borderStrong;
    QString focusBorder;

    // Background de campos "inset" (textareas, combos): em tema escuro afunda
    // pra `rgba(0,0,0,0.30)`; em tema claro sobe pra um cinza neutro.
    QString inputBackground;

    // Texto e borda de elementos desabilitados.
    QString disabledText;

    // Cor de "ring" pra selecionar itens em grids/swatches — em tema escuro é
    // branco, em tema claro é preto suave.
    QString selectionRing;

    // Cores semânticas: confirm/success, danger/delete, warning, info.
    // Cada uma vem com a cor cheia + um background hover suave.
    QString accentSuccess;
    QString accentSuccessSoft;
    QString accentSuccessBorderSoft;
    QString accentDanger;
    QString accentDangerSoft;
    QString accentDangerBorderSoft;
    QString accentWarning;
    QString accentInfo;
    QString accentInfoSoft;
    QString accentInfoBorderSoft;

    // Editor — "página" de escrita (apenas cor; largura/margens vivem em EditorLayout)
    QString editorBackground;
    QString editorTextColor;

    // Sombra projetada da página (estilo FocusWriter). Quando enabled, o
    // editorColumn ganha QGraphicsDropShadowEffect.
    bool pageShadowEnabled = false;
    QString pageShadowColor = QStringLiteral("rgba(0,0,0,140)");
    int pageShadowRadius = 24;
    int pageShadowOffset = 6;
};

// Singleton observável. Quem precisa reagir a troca de tema deve conectar
// ao sinal themeChanged() e reaplicar suas stylesheets.
class Manager : public QObject {
    Q_OBJECT
public:
    static Manager* instance();

    const MiraTheme& current() const;
    const QList<MiraTheme>& available() const;
    void setCurrent(const QString& id);

signals:
    void themeChanged();

private:
    Manager();
    void loadBundled();
    void loadFromSettings();
    void saveToSettings() const;

    QList<MiraTheme> m_themes;
    int m_currentIndex = 0;
};

// API legada — chamadas existentes (`Theme::appBackground()` etc.) seguem
// funcionando e passam a refletir o tema atual.
QString appBackground();
QString panelBackground();
QString panelBorder();
QString panelBorderRadius();
QString textPrimary();
QString textMuted();
QString textBright();
QString hoverOverlay();
QString pressedOverlay();
QString subtleBorder();
QString accentDefault();
QString panelQss(const QString& objectName);

// Hover/focus fortes e inputs.
QString hoverStrong();
QString borderStrong();
QString focusBorder();
QString inputBackground();
QString disabledText();
QString selectionRing();

// Cores semânticas (acentos).
QString accentSuccess();
QString accentSuccessSoft();
QString accentSuccessBorderSoft();
QString accentDanger();
QString accentDangerSoft();
QString accentDangerBorderSoft();
QString accentWarning();
QString accentInfo();
QString accentInfoSoft();
QString accentInfoBorderSoft();

// Novos acessores específicos do editor.
QString editorBackground();
QString editorTextColor();
bool pageShadowEnabled();
QString pageShadowColor();
int pageShadowRadius();
int pageShadowOffset();

// Stylesheet global do app — substitui o bloco hard-coded que vivia no main.cpp.
// Tem QMenu, QScrollBar, TopToolbar, FontPickerPopup, ImageInsertDialog,
// ImageOverlay etc. Reaplicar em themeChanged().
QString globalStyleSheet();

} // namespace Theme
