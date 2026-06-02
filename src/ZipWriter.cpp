#include "ZipWriter.h"

namespace {

quint32 crc32(const QByteArray& data) {
    static quint32 table[256];
    static bool built = false;
    if (!built) {
        for (quint32 i = 0; i < 256; ++i) {
            quint32 c = i;
            for (int k = 0; k < 8; ++k)
                c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            table[i] = c;
        }
        built = true;
    }
    quint32 c = 0xFFFFFFFFu;
    const uchar* p = reinterpret_cast<const uchar*>(data.constData());
    const int n = data.size();
    for (int i = 0; i < n; ++i)
        c = table[(c ^ p[i]) & 0xFF] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

void putU16(QByteArray& b, quint16 v) {
    b.append(char(v & 0xFF));
    b.append(char((v >> 8) & 0xFF));
}

void putU32(QByteArray& b, quint32 v) {
    b.append(char(v & 0xFF));
    b.append(char((v >> 8) & 0xFF));
    b.append(char((v >> 16) & 0xFF));
    b.append(char((v >> 24) & 0xFF));
}

// Flag bit 11 = nome do arquivo em UTF-8.
constexpr quint16 kFlagUtf8 = 0x0800;
// Hora/data DOS fixa (1º jan 2020, 00:00) — válida e estável.
constexpr quint16 kDosTime = 0;
constexpr quint16 kDosDate = ((2020 - 1980) << 9) | (1 << 5) | 1;

} // namespace

void ZipWriter::addFile(const QString& path, const QByteArray& data) {
    Entry e;
    e.name = path.toUtf8();
    e.crc = crc32(data);
    e.size = quint32(data.size());
    e.offset = quint32(m_local.size());

    // Local file header.
    putU32(m_local, 0x04034b50);
    putU16(m_local, 20);            // version needed
    putU16(m_local, kFlagUtf8);     // general purpose flag
    putU16(m_local, 0);             // method: stored
    putU16(m_local, kDosTime);
    putU16(m_local, kDosDate);
    putU32(m_local, e.crc);
    putU32(m_local, e.size);        // compressed == uncompressed (stored)
    putU32(m_local, e.size);
    putU16(m_local, quint16(e.name.size()));
    putU16(m_local, 0);             // extra field length
    m_local.append(e.name);
    m_local.append(data);

    m_entries.append(e);
}

QByteArray ZipWriter::finish() {
    QByteArray central;
    for (const Entry& e : m_entries) {
        putU32(central, 0x02014b50);
        putU16(central, 20);            // version made by
        putU16(central, 20);            // version needed
        putU16(central, kFlagUtf8);
        putU16(central, 0);             // method: stored
        putU16(central, kDosTime);
        putU16(central, kDosDate);
        putU32(central, e.crc);
        putU32(central, e.size);
        putU32(central, e.size);
        putU16(central, quint16(e.name.size()));
        putU16(central, 0);             // extra length
        putU16(central, 0);             // comment length
        putU16(central, 0);             // disk number start
        putU16(central, 0);             // internal attrs
        putU32(central, 0);             // external attrs
        putU32(central, e.offset);      // offset of local header
        central.append(e.name);
    }

    QByteArray out = m_local;
    const quint32 centralOffset = quint32(out.size());
    out.append(central);

    // End of central directory record.
    putU32(out, 0x06054b50);
    putU16(out, 0);                                  // disk number
    putU16(out, 0);                                  // disk with central dir
    putU16(out, quint16(m_entries.size()));          // entries on this disk
    putU16(out, quint16(m_entries.size()));          // total entries
    putU32(out, quint32(central.size()));            // central dir size
    putU32(out, centralOffset);                      // central dir offset
    putU16(out, 0);                                  // comment length
    return out;
}
