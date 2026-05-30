#pragma once

#include "TimelineTypes.h"

#include <QDialog>

class QLineEdit;
class QPlainTextEdit;
class QToolButton;

class TimelineEventPopup : public QDialog {
    Q_OBJECT
public:
    explicit TimelineEventPopup(const QList<TimelineDef>& timelines,
                                QWidget* parent = nullptr);

    void setEventData(const TimelineEvent& e);
    TimelineEvent eventData() const;

private slots:
    void pickColor();
    void applyTheme();

private:
    void buildUi(const QList<TimelineDef>& timelines);
    void updateColorBtn();

    QLineEdit*     m_title          = nullptr;
    QLineEdit*     m_marker         = nullptr;
    QPlainTextEdit* m_desc          = nullptr;
    QToolButton*   m_colorBtn       = nullptr;
    QColor         m_color;

    class QComboBox* m_timelineCombo = nullptr;
    QList<TimelineDef> m_timelines;

    QString m_eventId;
};
