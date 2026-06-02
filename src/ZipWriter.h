#pragma once

#include <QByteArray>
#include <QList>
#include <QString>

// Escritor de ZIP mínimo, método "stored" (sem compressão). Suficiente para
// empacotar arquivos que já são comprimidos por dentro (ODT/EPUB/DOCX são zips),
// onde recomprimir renderia pouco. Nomes de arquivo em UTF-8 (bit 11 do flag),
// então acentos no título dos capítulos/documentos saem corretos.
class ZipWriter {
public:
    // path usa "/" como separador de pastas. data é o conteúdo bruto do arquivo.
    void addFile(const QString& path, const QByteArray& data);

    // Monta e devolve o arquivo .zip completo.
    QByteArray finish();

private:
    struct Entry {
        QByteArray name;   // UTF-8
        quint32 crc = 0;
        quint32 size = 0;
        quint32 offset = 0;
    };
    QByteArray m_local;        // local headers + dados, na ordem de inserção
    QList<Entry> m_entries;
};
