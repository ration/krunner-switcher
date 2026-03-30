/*  -*- c++ -*-
 * Copyright 2026 by Tatu Lahtela <lahtela@iki.fi>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "switcher.h"

#include "switcher_debug.h"

#include <KLocalizedString>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QIcon>
#include <QImage>
#include <QPixmap>

K_PLUGIN_CLASS_WITH_JSON(Switcher, "plasma-runner-switcher.json")

static const QString KWIN_SERVICE = QStringLiteral("org.kde.KWin");
static const QString KWIN_WINDOWS_PATH = QStringLiteral("/WindowsRunner");
static const QString KRUNNER_IFACE = QStringLiteral("org.kde.krunner1");

Switcher::Switcher(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args)
    : AbstractRunner(parent, metaData)
{
    Q_UNUSED(args);
    setObjectName(QLatin1String("Switcher"));

    addSyntax(QStringLiteral(":q:"),
              i18n("Switch to application by typing a dot and the app "
                   "name, e.g. '.emacs'"));
}

Switcher::~Switcher() = default;

static QIcon iconFromProperties(const QVariantMap &properties, const QString &iconName)
{
    if (!iconName.isEmpty()) {
        return QIcon::fromTheme(iconName);
    }

    const auto it = properties.find(QStringLiteral("icon-data"));
    if (it == properties.end()) {
        return {};
    }

    const auto iconData = it->value<QDBusArgument>();
    int width = 0, height = 0, rowStride = 0, bitsPerSample = 0, channels = 0;
    bool hasAlpha = false;
    QByteArray data;

    iconData.beginStructure();
    iconData >> width >> height >> rowStride >> hasAlpha >> bitsPerSample >> channels >> data;
    iconData.endStructure();

    if (width <= 0 || height <= 0 || data.isEmpty()) {
        return {};
    }

    // D-Bus icon-data is big-endian ARGB; convert to QImage::Format_ARGB32 (native ARGB)
    QImage img(width, height, QImage::Format_ARGB32);
    const auto *src = reinterpret_cast<const uchar *>(data.constData());
    auto *dst = reinterpret_cast<QRgb *>(img.bits());
    for (int i = 0; i < width * height; ++i) {
        const uchar a = src[i * 4 + 0];
        const uchar r = src[i * 4 + 1];
        const uchar g = src[i * 4 + 2];
        const uchar b = src[i * 4 + 3];
        dst[i] = qRgba(r, g, b, a);
    }

    return QIcon(QPixmap::fromImage(std::move(img)));
}

void Switcher::match(KRunner::RunnerContext &context)
{
    const QString query = context.query();
    if (query.isEmpty() || !context.isValid()) {
        return;
    }

    if (query[0] != QLatin1Char('.')) {
        return;
    }

    const QString kwinQuery = query.mid(1);
    if (kwinQuery.isEmpty()) {
        return;
    }

    QDBusMessage msg =
        QDBusMessage::createMethodCall(KWIN_SERVICE, KWIN_WINDOWS_PATH, KRUNNER_IFACE, QStringLiteral("Match"));
    msg.setArguments({kwinQuery});

    const QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCWarning(KRUNNER_SWITCHER) << "KWin D-Bus call failed:" << reply.errorMessage();
        return;
    }

    if (reply.arguments().isEmpty()) {
        return;
    }

    QList<KRunner::QueryMatch> matches;
    const QDBusArgument matchList = reply.arguments().constFirst().value<QDBusArgument>();

    matchList.beginArray();
    while (!matchList.atEnd()) {
        QString id, text, iconName;
        int type = 0;
        double relevance = 100.0;
        QVariantMap properties;

        matchList.beginStructure();
        matchList >> id >> text >> iconName >> type >> relevance >> properties;
        matchList.endStructure();

        KRunner::QueryMatch match(this);
        match.setData(id);
        match.setText(text);
        match.setIcon(iconFromProperties(properties, iconName));
        match.setRelevance(1.0);
	match.setCategoryRelevance(KRunner::QueryMatch::CategoryRelevance::Highest);

        const auto subtextIt = properties.constFind(QStringLiteral("subtext"));
        if (subtextIt != properties.constEnd()) {
            match.setSubtext(subtextIt->toString());
        }

        qCDebug(KRUNNER_SWITCHER) << "Adding <window> match:" << text;
        matches.append(match);
    }
    matchList.endArray();

    context.addMatches(matches);
}

void Switcher::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match)
{
    Q_UNUSED(context)

    QDBusMessage msg =
        QDBusMessage::createMethodCall(KWIN_SERVICE, KWIN_WINDOWS_PATH, KRUNNER_IFACE, QStringLiteral("Run"));
    msg.setArguments({match.data().toString(), QString()});
    QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
}

#include "switcher.moc"
