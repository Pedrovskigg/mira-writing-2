#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include <QStringList>
#include <QWidget>

class QLabel;
class QToolButton;
class FontPickerPopup;
class QWidget;

class TopToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit TopToolbar(QWidget *parent = nullptr);

    void setFontFamilies(const QStringList &families, const QString &current);
    void setFontSize(int pt);
    void setLineHeightPercent(int percent);
    void setFirstLineIndentEnabled(bool enabled);

signals:
    void fontFamilyChanged(const QString &family);
    void fontSizeChanged(int pt);
    void lineHeightChanged(int percent);
    void firstLineIndentToggled(bool enabled);
    void addImageRequested();
    void focusModeToggled(bool enabled);
    void newProjectRequested();
    void openProjectRequested();
    void saveProjectRequested();
    void refMenuToggleRequested();
    // Placeholders — ainda sem implementação
    void boldToggled(bool enabled);
    void italicToggled(bool enabled);
    void glossaryRequested();
    void readModeToggled(bool enabled);
    void searchRequested();
    void reminderRequested();
    void immersiveSoundRequested();
    void settingsRequested();
    void fullscreenToggled(bool enabled);
    void themePanelRequested();

private:
    QToolButton *newProjectButton;
    QToolButton *openProjectButton;
    QToolButton *saveProjectButton;
    QToolButton *boldButton;
    QToolButton *italicButton;
    QToolButton *glossaryButton;
    QToolButton *readModeButton;
    QToolButton *focusButton;
    QToolButton *searchButton;
    QToolButton *fontButton;
    QToolButton *sizeButton;
    QToolButton *lineHeightButton;
    QToolButton *indentButton;
    QToolButton *imageButton;
    QToolButton *reminderButton;
    QToolButton *immersiveSoundButton;
    QToolButton *themePanelButton;
    QToolButton *settingsButton;
    QToolButton *fullscreenButton;
    QToolButton *refMenuButton;

    QIcon focusOffIcon;
    QIcon focusOnIcon;

    FontPickerPopup *fontPicker;

    QLabel *sizeStepperValueLabel;
    QList<QAction*> sizePresetActions;

    QStringList fontFamilies;
    QString currentFontFamily;
    int currentFontSize;
    int currentLineHeightPercent;

    void buildSizeMenu();
    void buildLineHeightMenu();
    void updateSizeMenuState();
    void applySize(int pt);
    void applyFontButtonStyle();
};

#endif
