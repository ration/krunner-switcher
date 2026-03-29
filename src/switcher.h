/*  -*- c++ -*-
 * Copyright 2018 by Tatu Lahtela <lahtela@iki.fi>
 * SPDX-FileCopyrightText: 2026 Bjorn Kettunen <bjorn.kettunen@thaodan.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef SWITCHER_H
#define SWITCHER_H

#include <KRunner/AbstractRunner>
#include <QtGui/qwindowdefs.h>

class KWindowInfo;

class Switcher : public KRunner::AbstractRunner
{
    Q_OBJECT

public:
    Switcher(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args);

    ~Switcher() override;

    void match(KRunner::RunnerContext &context) override;

    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;

private Q_SLOTS:

    void prepareForMatchSession();

    void matchSessionComplete();

    void gatherInfo();

private:
    KRunner::QueryMatch windowMatch(const KWindowInfo &info, const KRunner::QueryMatch::CategoryRelevance categoryRelevance, const qreal relevance = 1.0);

    QHash<WId, KWindowInfo> m_windows;
    QHash<WId, QIcon> m_icons;
    QStringList m_desktopNames;
};

#endif
