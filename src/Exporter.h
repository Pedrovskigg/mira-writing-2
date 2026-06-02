#pragma once

#include <QSet>
#include <QString>

class ProjectModel;
class QWidget;
struct Chapter;
struct DrawerItem;

// Exporta capítulos e documentos de gaveta selecionados. Etapa 1: ODT.
// O conteúdo é lido do disco/modelo — a MainWindow salva o projeto antes de
// chamar, garantindo que o disco reflita as edições atuais.
class Exporter {
public:
    enum class Format { Odt };
    enum class ManuscriptMode { SingleDocument, SeparateChapters };

    struct Selection {
        QSet<QString> chapterIds;
        QSet<QString> itemIds;
        Format format = Format::Odt;
        ManuscriptMode manuscriptMode = ManuscriptMode::SingleDocument;
        bool includeMarkers = true;   // marca-textos saem no documento?
    };

    Exporter(ProjectModel* model, const QString& projectRoot);

    // Abre um diálogo de destino e grava. true em sucesso; *error em falha.
    // *nothingExported vira true se a seleção não produziu nenhum arquivo.
    bool run(const Selection& sel, QWidget* dialogParent,
             QString* error = nullptr, bool* nothingExported = nullptr);

private:
    struct OutFile {
        QString path;     // caminho relativo dentro do zip (usa "/")
        QByteArray bytes; // conteúdo já no formato final (.odt)
    };

    // HTML do capítulo com a variação PRIMÁRIA de cada cena.
    QString chapterHtmlPrimary(const Chapter& ch) const;
    QString itemHtml(const DrawerItem& it) const;

    QByteArray htmlToOdt(const QString& html, bool includeMarkers) const;
    QByteArray chaptersToSingleOdt(const QList<const Chapter*>& chapters, bool includeMarkers) const;

    QList<OutFile> buildFiles(const Selection& sel) const;

    static QString safeName(const QString& s);

    ProjectModel* m_model;
    QString m_root;
};
