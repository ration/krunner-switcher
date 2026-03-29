/*  -*- c++ -*-
 * Copyright 2018 by Tatu Lahtela <lahtela@iki.fi>
 * SPDX-FileCopyrightText: 2026 Bjorn Kettunen <bjorn.kettunen@thaodan.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "switcher.h"

#include "switcher_debug.h"

#include <KLocalizedString>
#include <KWindowInfo>
#include <KWindowSystem>
#include <KX11Extras>
#include <QDebug>
#include <QIcon>
#include <QTimer>

K_PLUGIN_CLASS_WITH_JSON(Switcher, "plasma-runner-switcher.json")

Switcher::Switcher(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args)
    : AbstractRunner(parent, metaData)
{
    Q_UNUSED(args);
    setObjectName(QLatin1String("Switcher"));

    addSyntax(QStringLiteral(":q:"),
              i18n("Switch to application by typing a dot and the app "
                   "name, e.g. '.emacs'"));

    connect(this, &KRunner::AbstractRunner::prepare, this, &Switcher::prepareForMatchSession);
    connect(this, &KRunner::AbstractRunner::teardown, this, &Switcher::matchSessionComplete);
}

Switcher::~Switcher() = default;

// Called in the main thread
void Switcher::gatherInfo()
{
    for (const WId &w : KX11Extras::windows()) {
        KWindowInfo info(w,
                         NET::WMWindowType | NET::WMDesktop | NET::WMState | NET::XAWMState | NET::WMName,
                         NET::WM2WindowClass | NET::WM2WindowRole | NET::WM2AllowedActions);
        if (info.valid() && info.name() != "KRunner — krunner") {
            // ignore NET::Tool and other special window types
            NET::WindowType wType = info.windowType(NET::NormalMask | NET::DesktopMask | NET::DockMask | NET::ToolbarMask | NET::MenuMask | NET::DialogMask
                                                    | NET::OverrideMask | NET::TopMenuMask | NET::UtilityMask | NET::SplashMask);

            if (wType != NET::Normal && wType != NET::Override && wType != NET::Unknown && wType != NET::Dialog && wType != NET::Utility) {
                continue;
            }
            m_windows.insert(w, info);
            m_icons.insert(w, QIcon(KX11Extras::icon(w)));
        }
    }

    const int desktopCount = KX11Extras::numberOfDesktops();
    for (int i = 1; i <= desktopCount; ++i) {
        m_desktopNames.append(KX11Extras::desktopName(i));
    }
}

// Called in the main thread
void Switcher::prepareForMatchSession()
{
    gatherInfo();
}

// Called in the main thread
void Switcher::matchSessionComplete()
{
    m_desktopNames.clear();
    m_icons.clear();
    m_windows.clear();
}

// Called in the secondary thread
void Switcher::match(KRunner::RunnerContext &context)
{
    if (!context.isValid() && context.query().size() < 3) {
        return;
    }

    QList<KRunner::QueryMatch> matches;
    qreal relevance = 0.75;

    // keyword match: when term starts with "window" we list all windows
    // the list can be restricted to windows matching a given name, class, role or
    // desktop

    KRunner::QueryMatch::CategoryRelevance matchCategoryRelevance;
    QString termsUnsplit;

    // Maybe change to increase priority instead of making it the highest
    if (context.query()[0] == '.') {
        termsUnsplit = context.query().mid(1, -1);
        relevance = relevance + 0.1;
    } else {
        termsUnsplit = context.query();
    }

    QStringList terms = termsUnsplit.split(' ', Qt::SkipEmptyParts);
    QList<WId> skippedWindows;

    QHashIterator<WId, KWindowInfo> it(m_windows);

    /* First remove all windows which don't match. */
    while (it.hasNext()) {
        it.next();
        const WId w = it.key();
        const KWindowInfo info = it.value();
        const QString windowClass = QString::fromUtf8(info.windowClassName());

        for (auto &term : terms) {
            if (!KX11Extras::hasWId(w) || (!info.name().contains(term, Qt::CaseInsensitive) && !windowClass.contains(term, Qt::CaseInsensitive))) {
                qCDebug(KRUNNER_SWITCHER) << QStringLiteral("Removing window: %1").arg(info.name());
                skippedWindows.append(w);
            }
        }
    }

    /* Then iterate over them again and sort them based on their relevance. */
    it.toFront();
    while (it.hasNext()) {
        it.next();
        const WId w = it.key();
        const KWindowInfo info = it.value();
        const QString windowClass = QString::fromUtf8(info.windowClassName());

        if (skippedWindows.contains(w))
            continue;

        for (auto &term : terms) {
            // // exclude not matching windows

            static ushort highestMatchSize = 5;
            static int indexOfTermWindowName = info.name().indexOf(term, 0, Qt::CaseInsensitive);

            /* Match either by windowName or windowClass.
               Categorize by what matches best.*/

            if (indexOfTermWindowName != -1) {
                // check fullname match or term fully contains match
                if (term.size() > highestMatchSize && indexOfTermWindowName == 0) {
                    matchCategoryRelevance = KRunner::QueryMatch::CategoryRelevance::Highest;
                    relevance = relevance + 0.1;
                } else if (indexOfTermWindowName
                           == 0 /*startswith, but not equals => smaller relevance
                                  boost*/) {
                    relevance = relevance + 0.1;
                    matchCategoryRelevance = KRunner::QueryMatch::CategoryRelevance::High;
                } else {
                    matchCategoryRelevance = KRunner::QueryMatch::CategoryRelevance::Moderate;
                }
            }

            static int indexOfTermWindowClass = windowClass.indexOf(term, 0, Qt::CaseInsensitive);

            if (indexOfTermWindowClass != -1) {
                if (term.size() > highestMatchSize && indexOfTermWindowClass == 0) {
                    matchCategoryRelevance = KRunner::QueryMatch::CategoryRelevance::Highest;
                } else if (indexOfTermWindowClass == 0) {
                    relevance = relevance * 0.1;
                    if (qToUnderlying(matchCategoryRelevance) < qToUnderlying(KRunner::QueryMatch::CategoryRelevance::High)) {
                        relevance = relevance + 0.1;
                        matchCategoryRelevance = KRunner::QueryMatch::CategoryRelevance::High;
                    }
                } else {
                    if (qToUnderlying(matchCategoryRelevance) < qToUnderlying(KRunner::QueryMatch::CategoryRelevance::Moderate)) {
                        relevance = relevance + 0.1;
                        matchCategoryRelevance = KRunner::QueryMatch::CategoryRelevance::Moderate;
                    }
                }
            }

            if (indexOfTermWindowClass != -1 || indexOfTermWindowName != -1) {
                qCDebug(KRUNNER_SWITCHER) << QStringLiteral("Addding info.name %1").arg(info.name());
                matches.append(windowMatch(info, matchCategoryRelevance, relevance));
                break;
            } else {
                qCDebug(KRUNNER_SWITCHER) << QStringLiteral("Not ading window %1").arg(info.name());
            }
        }
    }

    context.addMatches(matches);
}

// Called in the main thread
void Switcher::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match)
{
    Q_UNUSED(context)
    // check if it's a desktop
    if (match.id().startsWith(QLatin1String("windows_desktop"))) {
        KX11Extras::setCurrentDesktop(match.data().toInt());
        return;
    }

    WId w(match.data().toString().toULong());
    KX11Extras::forceActiveWindow(w);
}

KRunner::QueryMatch Switcher::windowMatch(const KWindowInfo &info, const KRunner::QueryMatch::CategoryRelevance categoryRelevance, const qreal relevance)
{
    KRunner::QueryMatch match(this);
    match.setCategoryRelevance(categoryRelevance);
    match.setData(QString::number(info.win()));
    match.setIcon(m_icons[info.win()]);
    match.setText(info.name());
    QString desktopName;
    int desktop = info.desktop();
    if (desktop == NET::OnAllDesktops) {
        desktop = KX11Extras::currentDesktop();
    }
    if (desktop <= m_desktopNames.size()) {
        desktopName = m_desktopNames[desktop - 1];
    } else {
        desktopName = KX11Extras::desktopName(desktop);
    }

    match.setSubtext(i18n("Activate running window on %1", desktopName));
    match.setRelevance(relevance);
    return match;
}

#include "switcher.moc"
