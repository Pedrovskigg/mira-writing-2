#include "ConstrutorStore.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QUuid>

// ── Categoria definitions ─────────────────────────────────────────────────────

static QList<ConstrutorStore::Category> buildCategories()
{
    using W = ConstrutorStore::CategoryWaypoint;
    using C = ConstrutorStore::Category;
    QList<C> cats;

    cats << C{QStringLiteral("magic"), QObject::tr("Magia"), {
        {QObject::tr("Soft"),         QObject::tr("A magia é atmosférica e misteriosa. Sem regras explícitas — funciona por sentimento e narrativa.")},
        {QObject::tr("Branda"),       QObject::tr("Existem algumas regras gerais, mas exceções são aceitas quando servem à história.")},
        {QObject::tr("Equilibrada"),  QObject::tr("A magia tem regras claras, mas há margem para o inesperado e o inexplicável.")},
        {QObject::tr("Estruturada"),  QObject::tr("As regras são bem definidas e consistentes. Exceções são raras e precisam de justificativa.")},
        {QObject::tr("Hard"),         QObject::tr("A magia funciona como uma ciência. Regras rígidas, custos claros e consequências previsíveis.")},
    }};

    cats << C{QStringLiteral("politics"), QObject::tr("Política"), {
        {QObject::tr("Anarquia"),       QObject::tr("Ausência de governo central. Poder distribuído ou inexistente; organização emerge do coletivo.")},
        {QObject::tr("Democracia"),     QObject::tr("O poder emana do povo. Eleições, pluralismo e direitos civis são os pilares.")},
        {QObject::tr("República"),      QObject::tr("Governo representativo com instituições formais e separação de poderes.")},
        {QObject::tr("Oligarquia"),     QObject::tr("Poder concentrado em um grupo seleto: elite econômica, nobre ou militar.")},
        {QObject::tr("Monarquia"),      QObject::tr("Poder centralizado em um governante hereditário, com ou sem representação popular.")},
        {QObject::tr("Totalitarismo"),  QObject::tr("Controle absoluto do Estado sobre todos os aspectos da vida pública e privada.")},
    }};

    cats << C{QStringLiteral("religion"), QObject::tr("Religião"), {
        {QObject::tr("Animista"),     QObject::tr("Espiritualidade difusa. Forças da natureza e ancestrais sem estrutura formal.")},
        {QObject::tr("Popular"),      QObject::tr("Fé prática e cultural, mistura de crenças. Sem ortodoxia rígida.")},
        {QObject::tr("Tradicional"),  QObject::tr("Práticas e textos estabelecidos. Alguma hierarquia, abertura à interpretação local.")},
        {QObject::tr("Conservadora"), QObject::tr("Apego forte à doutrina e tradição. Mudanças são lentas e contestadas.")},
        {QObject::tr("Dogmática"),    QObject::tr("Doutrina imutável e absoluta. Heresia é punida. Autoridade religiosa centralizada.")},
    }};

    cats << C{QStringLiteral("social"), QObject::tr("Social"), {
        {QObject::tr("Igualitário"),    QObject::tr("Todos têm direitos e oportunidades equivalentes. Hierarquias são mínimas.")},
        {QObject::tr("Meritocrático"),  QObject::tr("Status conquistado por desempenho e talento, não por nascimento.")},
        {QObject::tr("Aristocrático"),  QObject::tr("Prestígio determinado por família, linhagem ou títulos. Mobilidade social limitada.")},
        {QObject::tr("Castas"),         QObject::tr("Divisão social rígida e hereditária. Mobilidade entre grupos é praticamente impossível.")},
    }};

    cats << C{QStringLiteral("economy"), QObject::tr("Econômico"), {
        {QObject::tr("Livre Mercado"),  QObject::tr("Economia autorregulada. Preços, produção e distribuição pela oferta e demanda.")},
        {QObject::tr("Liberal"),        QObject::tr("Mercado livre com alguma regulação estatal para garantir concorrência e direitos básicos.")},
        {QObject::tr("Misto"),          QObject::tr("Equilíbrio entre iniciativa privada e participação do Estado em setores estratégicos.")},
        {QObject::tr("Dirigido"),       QObject::tr("O Estado orienta fortemente a economia, definindo prioridades e controlando setores-chave.")},
        {QObject::tr("Planificado"),    QObject::tr("Economia totalmente controlada pelo Estado. Produção, distribuição e preços centralizados.")},
    }};

    cats << C{QStringLiteral("military"), QObject::tr("Militar"), {
        {QObject::tr("Pacifista"),        QObject::tr("Rejeição ativa da guerra. Conflitos resolvidos por diplomacia. Forças armadas mínimas.")},
        {QObject::tr("Defensivo"),        QObject::tr("Exército existe para proteger o território. Sem ambição expansionista.")},
        {QObject::tr("Intervencionista"), QObject::tr("Disposto a usar força militar fora de seu território por causas políticas ou humanitárias.")},
        {QObject::tr("Expansionista"),    QObject::tr("A conquista territorial e o domínio militar são objetivos centrais.")},
    }};

    cats << C{QStringLiteral("technology"), QObject::tr("Tecnologia"), {
        {QObject::tr("Primitivo"),   QObject::tr("Ferramentas simples, sem escrita ou metalurgia avançada. Sociedade de subsistência.")},
        {QObject::tr("Medieval"),    QObject::tr("Metalurgia, agricultura organizada, primeiras cidades. Tecnologia limitada pela tradição.")},
        {QObject::tr("Industrial"),  QObject::tr("Máquinas a vapor, produção em massa, urbanização acelerada.")},
        {QObject::tr("Moderno"),     QObject::tr("Computação, energia elétrica, comunicação global e medicina avançada.")},
        {QObject::tr("Pós-humano"), QObject::tr("IA, engenharia genética, colonização espacial. Tecnologia transcende os limites biológicos.")},
    }};

    cats << C{QStringLiteral("cosmology"), QObject::tr("Cosmologia"), {
        {QObject::tr("Caótico"),        QObject::tr("O universo não tem ordem ou propósito. Forças imprevisíveis dominam a existência.")},
        {QObject::tr("Emergente"),      QObject::tr("Padrões surgem do caos, mas sem um plano divino ou determinismo rígido.")},
        {QObject::tr("Ordenado"),       QObject::tr("O universo segue leis naturais ou divinas consistentes e compreensíveis.")},
        {QObject::tr("Determinístico"), QObject::tr("Tudo é predeterminado. O livre-arbítrio é ilusório; o cosmos segue um roteiro fixo.")},
    }};

    cats << C{QStringLiteral("faction"), QObject::tr("Organização/Facção"), {
        {QObject::tr("Horizontal"),   QObject::tr("Sem hierarquia formal. Decisões por consenso ou rotatividade.")},
        {QObject::tr("Democrática"),  QObject::tr("Liderança eleita pelos membros. Prestação de contas e representação.")},
        {QObject::tr("Hierárquica"),  QObject::tr("Cadeia de comando clara. Autoridade flui de cima para baixo.")},
        {QObject::tr("Autocrática"),  QObject::tr("Poder absoluto nas mãos de um líder ou núcleo muito restrito.")},
    }};

    cats << C{QStringLiteral("lineage"), QObject::tr("Linhagem"), {
        {QObject::tr("Meritocrática"),    QObject::tr("Herdeiro escolhido por competência e realizações, não por parentesco.")},
        {QObject::tr("Eletiva"),          QObject::tr("Sucessores escolhidos por um conselho ou grupo de pares dentro da linhagem.")},
        {QObject::tr("Aristocrática"),    QObject::tr("Primogenitura ou títulos determinam a herança, mas o mérito ainda tem peso.")},
        {QObject::tr("Hereditária Pura"), QObject::tr("Sangue é lei. A linhagem determina tudo; desvios são ilegítimos.")},
    }};

    cats << C{QStringLiteral("mythology"), QObject::tr("Mitologia"), {
        {QObject::tr("Filosófica"),  QObject::tr("Mitos como metáforas para verdades humanas. Sem crença literal em deuses ou eventos.")},
        {QObject::tr("Simbólica"),   QObject::tr("Histórias sagradas interpretadas como símbolos culturais, não como fatos históricos.")},
        {QObject::tr("Literal"),     QObject::tr("Os mitos são história real. Deuses, heróis e eventos são tratados como fatos.")},
        {QObject::tr("Dogmática"),   QObject::tr("Textos sagrados são a verdade absoluta. Questioná-los é heresia.")},
    }};

    cats << C{QStringLiteral("other"), QObject::tr("Outro"), {
        {QObject::tr("Aberto"),       QObject::tr("Estrutura flexível, sem regras rígidas. Adaptável ao contexto.")},
        {QObject::tr("Semiflexível"), QObject::tr("Algumas diretrizes existem, mas há espaço para variação e interpretação.")},
        {QObject::tr("Estruturado"),  QObject::tr("Regras claras e estabelecidas que guiam o funcionamento do sistema.")},
        {QObject::tr("Rígido"),       QObject::tr("Sistema inflexível com regras absolutas. Desvios não são tolerados.")},
    }};

    return cats;
}

// ── Helpers — JSON ────────────────────────────────────────────────────────────

static QJsonObject nodeToJson(const ConstrutorStore::Node& node)
{
    QJsonObject o;
    o.insert(QStringLiteral("id"),   node.id);
    o.insert(QStringLiteral("name"), node.name);
    o.insert(QStringLiteral("type"), node.type == ConstrutorStore::NodeType::Rule
                                         ? QStringLiteral("rule")
                                         : QStringLiteral("section"));
    if (!node.content.isEmpty())
        o.insert(QStringLiteral("content"), node.content);
    if (!node.children.isEmpty()) {
        QJsonArray children;
        for (const auto& c : node.children)
            children.append(nodeToJson(c));
        o.insert(QStringLiteral("children"), children);
    }
    return o;
}

static ConstrutorStore::Node nodeFromJson(const QJsonObject& o)
{
    ConstrutorStore::Node node;
    node.id      = o.value(QStringLiteral("id")).toString();
    node.name    = o.value(QStringLiteral("name")).toString();
    node.type    = o.value(QStringLiteral("type")).toString() == QLatin1String("rule")
                       ? ConstrutorStore::NodeType::Rule
                       : ConstrutorStore::NodeType::Section;
    node.content = o.value(QStringLiteral("content")).toString();
    for (const auto& v : o.value(QStringLiteral("children")).toArray())
        node.children.append(nodeFromJson(v.toObject()));
    return node;
}

// ── ConstrutorStore ───────────────────────────────────────────────────────────

ConstrutorStore::ConstrutorStore(QObject* parent)
    : QObject(parent)
{
}

void ConstrutorStore::setProjectRoot(const QString& root)
{
    if (m_root == root) return;
    m_root = root;
    m_systems.clear();
}

QString ConstrutorStore::sidecarPath() const
{
    if (m_root.isEmpty()) return {};
    return QDir::cleanPath(m_root + QStringLiteral("/construtor.json"));
}

bool ConstrutorStore::load()
{
    m_systems.clear();
    const QString path = sidecarPath();
    if (path.isEmpty()) return false;
    QFile f(path);
    if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly)) return false;
    const QByteArray data = f.readAll();
    f.close();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return false;

    for (const auto& v : doc.object().value(QStringLiteral("systems")).toArray()) {
        const QJsonObject o = v.toObject();
        System sys;
        sys.id          = o.value(QStringLiteral("id")).toString();
        sys.name        = o.value(QStringLiteral("name")).toString();
        sys.categoryId  = o.value(QStringLiteral("categoryId")).toString();
        sys.sliderIndex = o.value(QStringLiteral("sliderIndex")).toInt(0);
        sys.createdAt   = o.value(QStringLiteral("createdAt")).toVariant().toLongLong();
        for (const auto& nv : o.value(QStringLiteral("nodes")).toArray())
            sys.nodes.append(nodeFromJson(nv.toObject()));
        if (!sys.id.isEmpty())
            m_systems.append(std::move(sys));
    }
    return true;
}

bool ConstrutorStore::save() const
{
    const QString path = sidecarPath();
    if (path.isEmpty()) return false;

    QJsonArray systems;
    for (const System& sys : m_systems) {
        QJsonObject o;
        o.insert(QStringLiteral("id"),          sys.id);
        o.insert(QStringLiteral("name"),         sys.name);
        o.insert(QStringLiteral("categoryId"),   sys.categoryId);
        o.insert(QStringLiteral("sliderIndex"),  sys.sliderIndex);
        o.insert(QStringLiteral("createdAt"),    sys.createdAt);
        QJsonArray nodes;
        for (const Node& n : sys.nodes)
            nodes.append(nodeToJson(n));
        o.insert(QStringLiteral("nodes"), nodes);
        systems.append(o);
    }
    QJsonObject root;
    root.insert(QStringLiteral("systems"), systems);

    QSaveFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return f.commit();
}

// ── Static lookup ─────────────────────────────────────────────────────────────

const QList<ConstrutorStore::Category>& ConstrutorStore::categories()
{
    static const QList<Category> cats = buildCategories();
    return cats;
}

const ConstrutorStore::Category* ConstrutorStore::categoryById(const QString& id)
{
    for (const Category& c : categories())
        if (c.id == id) return &c;
    return nullptr;
}

// ── Systems CRUD ──────────────────────────────────────────────────────────────

ConstrutorStore::System* ConstrutorStore::findSystem(const QString& id)
{
    for (System& s : m_systems)
        if (s.id == id) return &s;
    return nullptr;
}

const ConstrutorStore::System* ConstrutorStore::system(const QString& id) const
{
    for (const System& s : m_systems)
        if (s.id == id) return &s;
    return nullptr;
}

QString ConstrutorStore::addSystem(const QString& name, const QString& categoryId,
                                    int sliderIndex)
{
    System sys;
    sys.id          = QUuid::createUuid().toString(QUuid::WithoutBraces);
    sys.name        = name;
    sys.categoryId  = categoryId;
    sys.sliderIndex = sliderIndex;
    sys.createdAt   = QDateTime::currentMSecsSinceEpoch();
    m_systems.append(std::move(sys));
    save();
    emit changed();
    return m_systems.last().id;
}

bool ConstrutorStore::updateSystem(const QString& id, const QString& name, int sliderIndex)
{
    System* sys = findSystem(id);
    if (!sys) return false;
    sys->name        = name;
    sys->sliderIndex = sliderIndex;
    save();
    emit changed();
    return true;
}

bool ConstrutorStore::removeSystem(const QString& id)
{
    const int before = m_systems.size();
    m_systems.erase(std::remove_if(m_systems.begin(), m_systems.end(),
                                   [&](const System& s) { return s.id == id; }),
                    m_systems.end());
    if (m_systems.size() == before) return false;
    save();
    emit changed();
    return true;
}

// ── Nodes CRUD ────────────────────────────────────────────────────────────────

ConstrutorStore::Node* ConstrutorStore::findNode(QList<Node>& nodes, const QString& id)
{
    for (Node& n : nodes) {
        if (n.id == id) return &n;
        Node* c = findNode(n.children, id);
        if (c) return c;
    }
    return nullptr;
}

bool ConstrutorStore::removeNodeRecursive(QList<Node>& nodes, const QString& id)
{
    for (int i = 0; i < nodes.size(); ++i) {
        if (nodes[i].id == id) {
            nodes.removeAt(i);
            return true;
        }
        if (removeNodeRecursive(nodes[i].children, id)) return true;
    }
    return false;
}

QString ConstrutorStore::addNode(const QString& systemId, const QString& parentNodeId,
                                  NodeType type, const QString& name)
{
    System* sys = findSystem(systemId);
    if (!sys) return {};

    Node newNode;
    newNode.id   = QUuid::createUuid().toString(QUuid::WithoutBraces);
    newNode.name = name;
    newNode.type = type;

    if (parentNodeId.isEmpty()) {
        sys->nodes.append(newNode);
    } else {
        Node* parent = findNode(sys->nodes, parentNodeId);
        if (!parent) return {};
        parent->children.append(newNode);
    }

    save();
    emit changed();
    return newNode.id;
}

bool ConstrutorStore::updateNode(const QString& systemId, const QString& nodeId,
                                  const QString& name, const QString& content)
{
    System* sys = findSystem(systemId);
    if (!sys) return false;
    Node* node = findNode(sys->nodes, nodeId);
    if (!node) return false;
    node->name    = name;
    node->content = content;
    save();
    emit changed();
    return true;
}

bool ConstrutorStore::removeNode(const QString& systemId, const QString& nodeId)
{
    System* sys = findSystem(systemId);
    if (!sys) return false;
    if (!removeNodeRecursive(sys->nodes, nodeId)) return false;
    save();
    emit changed();
    return true;
}
